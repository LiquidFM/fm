#include "../desktopenvironment.h"
#include "../../filesystem/interfaces/filesystemifileinfo.h"
#include "xdgmime/src/xdgmime.h"

#if defined(DESKTOP_ENVIRONMENT_IS_KDE)
#	include "kde/kde_de_p.h"
#elif defined(DESKTOP_ENVIRONMENT_IS_GTK)
#	include "gtk/gtk_de_p.h"
#endif

#include <QtCore/QCache>
#include <QtCore/QReadWriteLock>

#include <stdlib.h>
#include <string.h>
#include <cassert>
#include <X11/Xlib.h>
#include <X11/Xatom.h>


enum X11Atom
{
    DTWM_IS_RUNNING,
    _DT_SAVE_MODE,
    _SGI_DESKS_MANAGER,
    NAtoms
};

static const char *x11_atomnames[NAtoms] =
{
	"DTWM_IS_RUNNING",
	"_DT_SAVE_MODE",
	"_SGI_DESKS_MANAGER"
};


#if defined(DESKTOP_ENVIRONMENT_IS_KDE)
	static int kde_version = 0;
#endif


struct IconIndex
{
	IconIndex()
	{}
	IconIndex(const QString &name, int size, int context) :
		name(name),
		size(size),
		context(context)
	{}

	bool operator==(const IconIndex &other) const
	{
		return context == other.context && size == other.size && name == other.name;
	}

	bool operator<(const IconIndex &other) const
	{
		return context < other.context || size < other.size || name < other.name;
	}

	QString name;
	int size;
	int context;
};

uint qHash(const IconIndex &index)
{
	return qHash(index.name) + index.size + index.context;
}


class IconCache
{
public:
	IconCache()
	{}

	QIcon *iconFromCache(const QString &name, int size, int context)
	{
		QReadLocker lock(&m_cacheLock);
		return m_cache.object(IconIndex(name, size, context));
	}

	QIcon *iconToCache(const QString &name, int size, int context, const QString &fileName)
	{
		IconIndex index(name, size, context);
		QWriteLocker lock(&m_cacheLock);

		if (QIcon *icon = m_cache.object(index))
			return icon;
		else
		{
			QScopedPointer<QIcon> icon(new QIcon());

			icon->addFile(fileName, QSize(size, size));
			m_cache.insert(index, icon.data());

			return icon.take();
		}
	}

private:
	QReadWriteLock m_cacheLock;
	QCache<IconIndex, QIcon> m_cache;
};
static IconCache *iconCache = 0;


inline static char *loadMimeTypeIcon(const char *mimeType, int size, const char *theme)
{
	if (char *icon_path = xdg_mime_type_icon_lookup(mimeType, size, theme))
		return icon_path;
	else
	{
		const XdgArray *apps;

		if (apps = xdg_mime_user_apps_lookup(mimeType))
			for (int i = 0, sz = xdg_mime_array_size(apps); i < sz; ++i)
				if (icon_path = xdg_mime_app_icon_lookup(xdg_mime_array_app_item_at(apps, i), theme, size))
					return icon_path;

		if (apps = xdg_mime_default_apps_lookup(mimeType))
			for (int i = 0, sz = xdg_mime_array_size(apps); i < sz; ++i)
				if (icon_path = xdg_mime_app_icon_lookup(xdg_mime_array_app_item_at(apps, i), theme, size))
					return icon_path;

		if (apps = xdg_mime_known_apps_lookup(mimeType))
			for (int i = 0, sz = xdg_mime_array_size(apps); i < sz; ++i)
				if (icon_path = xdg_mime_app_icon_lookup(xdg_mime_array_app_item_at(apps, i), theme, size))
					return icon_path;
	}

	return 0;
}


DesktopEnvironment::DesktopEnvironment() :
	m_type(DE_Unknown)
{
	iconCache = new IconCache();

	if (Display *display = XOpenDisplay(NULL))
    {
        int rc;
        Atom type;
        int format;
        const char *var;
        unsigned long length, after;
        unsigned char *data = 0;
        Atom atoms[NAtoms];

#if defined(XlibSpecificationRelease) && (XlibSpecificationRelease >= 6)
		XInternAtoms(display, const_cast<char **>(x11_atomnames), NAtoms, False, atoms);
#else
		for (int i = 0; i = 0; i < NAtoms; ++i)
			atoms[i] = XInternAtom(display, const_cast<char *>(x11_atomnames[i]), False);
#endif

		do
		{
			if (var = getenv("KDE_FULL_SESSION"))
			{
				m_type = DE_Kde;

#if defined(DESKTOP_ENVIRONMENT_IS_KDE)
				if (var = getenv("KDE_SESSION_VERSION"))
					kde_version = atoi(var);
#endif
				break;
			}

			if (var = getenv("DESKTOP_SESSION"))
				if (!strcmp(var, "gnome"))
				{
					m_type = DE_Gnome;
					break;
				}

			// GNOME_DESKTOP_SESSION_ID is deprecated, but still check it
			if (var = getenv("GNOME_DESKTOP_SESSION_ID"))
			{
				m_type = DE_Gnome;
				break;
			}

			rc = XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
									atoms[_DT_SAVE_MODE],
									0, 2, False, XA_STRING, &type, &format, &length,
									&after, &data);
			if (rc == Success && length)
			{
				if (!strcmp(reinterpret_cast<const char *>(data), "xfce4"))
				{
					m_type = DE_Gnome;
					break;
				}

				XFree(data);
				data = 0;
			}

			rc = XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
									atoms[DTWM_IS_RUNNING],
									0, 1, False, AnyPropertyType, &type, &format, &length,
									&after, &data);
			if (rc == Success && length)
			{
				m_type = DE_Cde;
				break;
			}

			rc = XGetWindowProperty(display, RootWindow(display, DefaultScreen(display)),
									atoms[_SGI_DESKS_MANAGER], 0, 1, False, XA_WINDOW,
									&type, &format, &length, &after, &data);
			if (rc == Success && length)
			{
				m_type = DE_4Dwm;
				break;
			}
		}
		while (true);

		if (data)
			XFree(reinterpret_cast<char *>(data));

		XCloseDisplay(display);
    }

    xdg_mime_init();
}

DesktopEnvironment::~DesktopEnvironment()
{
	xdg_mime_shutdown();
	delete iconCache;
}

QIcon DesktopEnvironment::processingIcon(int iconSize) const
{
	return findIcon("view-refresh", iconSize, Actions);
}

QIcon DesktopEnvironment::cancelingIcon(int iconSize) const
{
	return findIcon("application-exit", iconSize, Actions);
}

FileTypeId DesktopEnvironment::fileTypeId(FileTypes::Audio::Type id) const
{
	FileTypeId typeId;

	switch (id)
	{
		case FileTypes::Audio::M3uFile:
			typeId.mime = QString::fromLatin1("audio/x-mpegurl");
			break;
	}

	return typeId;
}

FileTypeId DesktopEnvironment::fileTypeId(FileTypes::Application::Type id) const
{
	FileTypeId typeId;

	switch (id)
	{
		case FileTypes::Application::GZipFile:
			typeId.mime = QString::fromLatin1("application/x-gzip");
			break;

		case FileTypes::Application::TarFile:
			typeId.mime = QString::fromLatin1("application/x-tar");
			break;

		case FileTypes::Application::CompressedTarFile:
			typeId.mime = QString::fromLatin1("application/x-compressed-tar");
			break;

		case FileTypes::Application::BZipCompressedTarFile:
			typeId.mime = QString::fromLatin1("application/x-bzip-compressed-tar");
			break;

		case FileTypes::Application::ZipFile:
			typeId.mime = QString::fromLatin1("application/zip");
			break;

		case FileTypes::Application::BZipFile:
			typeId.mime = QString::fromLatin1("application/x-bzip");
			break;

		case FileTypes::Application::RarFile:
			typeId.mime = QString::fromLatin1("application/x-rar");
			break;

		case FileTypes::Application::TarzFile:
			typeId.mime = QString::fromLatin1("application/x-tarz");
			break;

		case FileTypes::Application::BZip2File:
			typeId.mime = QString::fromLatin1("application/x-bzip2");
			break;

		case FileTypes::Application::JavaArchiveFile:
			typeId.mime = QString::fromLatin1("application/x-java-archive");
			break;

		case FileTypes::Application::DebFile:
			typeId.mime = QString::fromLatin1("application/x-deb");
			break;

		case FileTypes::Application::Arch7zCompressedFile:
			typeId.mime = QString::fromLatin1("application/x-7z-compressed");
			break;

		case FileTypes::Application::CompressFile:
			typeId.mime = QString::fromLatin1("application/x-compress");
			break;

		case FileTypes::Application::ZipCompressedFile:
			typeId.mime = QString::fromLatin1("application/x-zip-compressed");
			break;

		case FileTypes::Application::LzmaFile:
			typeId.mime = QString::fromLatin1("application/x-lzma");
			break;

		case FileTypes::Application::ServicepackFile:
			typeId.mime = QString::fromLatin1("application/x-servicepack");
			break;

		case FileTypes::Application::XzCompressedTarFile:
			typeId.mime = QString::fromLatin1("application/x-xz-compressed-tar");
			break;

		case FileTypes::Application::LzmaCompressedTarFile:
			typeId.mime = QString::fromLatin1("application/x-lzma-compressed-tar");
			break;

		case FileTypes::Application::CdImageFile:
			typeId.mime = QString::fromLatin1("application/x-cd-image");
			break;
	}

	return typeId;
}

FileTypeInfo DesktopEnvironment::fileTypeInfo(const QString &absoluteFilePath, bool isDir, int iconSize) const
{
	if (isDir)
		return fileTypeInfo(iconSize);
	else
	{
		struct stat st;
		QByteArray fileName = absoluteFilePath.toUtf8();
		const char *mimeType = xdg_mime_get_mime_type_from_file_name(fileName);

		if (mimeType == XDG_MIME_TYPE_UNKNOWN)
			mimeType = xdg_mime_get_mime_type_for_file(fileName, &st);

		return fileTypeInfo(mimeType, iconSize);
	}
}

FileTypeInfo DesktopEnvironment::fileTypeInfoFromFileName(const QString &fileName, bool isDir, int iconSize) const
{
	if (isDir)
		return fileTypeInfo(iconSize);
	else
		return fileTypeInfo(xdg_mime_get_mime_type_from_file_name(fileName.toUtf8()), iconSize);
}

QByteArray DesktopEnvironment::themeName() const
{
#if defined(DESKTOP_ENVIRONMENT_IS_KDE)
	return DesktopEnvironmentPrivate::iconThemeName(kde_version).toUtf8();
#else
	return DesktopEnvironmentPrivate::iconThemeName().toUtf8();
#endif
}

FileTypeInfo DesktopEnvironment::fileTypeInfo(int iconSize) const
{
	FileTypeInfo info;

	info.icon = findIcon("folder", iconSize, Places);
	info.name = info.id.mime = QString::fromLatin1("<DIR>");

	return info;
}

FileTypeInfo DesktopEnvironment::fileTypeInfo(const char *mimeType, int iconSize) const
{
	FileTypeInfo info;

	if (strcmp(mimeType, XDG_MIME_TYPE_TEXTPLAIN) == 0 ||
		strcmp(mimeType, XDG_MIME_TYPE_UNKNOWN) == 0 ||
		strcmp(mimeType, XDG_MIME_TYPE_EMPTY) == 0)
	{
		info.icon = findIcon(XDG_MIME_TYPE_TEXTPLAIN, iconSize, MimeTypes);
		info.name = info.id.mime = QString::fromUtf8(XDG_MIME_TYPE_TEXTPLAIN);
	}
	else
	{
		info.icon = findMimeTypeIcon(mimeType, iconSize);
		info.name = info.id.mime = QString::fromUtf8(mimeType);
	}

	return info;
}

QIcon DesktopEnvironment::findIcon(const char *name, int iconSize, int context) const
{
	QString nameString = QString::fromLatin1(name);

	if (QIcon *icon = iconCache->iconFromCache(nameString, iconSize, context))
		return *icon;
	else
	{
		QIcon res;

		if (char *icon_path = xdg_mime_icon_lookup(name, iconSize, static_cast<Context>(context), themeName()))
		{
			res = *iconCache->iconToCache(nameString, iconSize, context, QString::fromUtf8(icon_path));
			free(icon_path);
		}

		return res;
	}
}

QIcon DesktopEnvironment::findMimeTypeIcon(const char *mimeType, int iconSize) const
{
	QString nameString = QString::fromLatin1(mimeType);

	if (QIcon *icon = iconCache->iconFromCache(nameString, iconSize, MimeTypes))
		return *icon;
	else
	{
		QIcon res;
		QByteArray iconThemeName = themeName();

		if (char *icon_path = loadMimeTypeIcon(mimeType, iconSize, iconThemeName))
		{
			res = *iconCache->iconToCache(nameString, iconSize, MimeTypes, QString::fromUtf8(icon_path));
			free(icon_path);
		}
		else
			if (icon_path = xdg_mime_type_icon_lookup(XDG_MIME_TYPE_TEXTPLAIN, iconSize, iconThemeName))
			{
				res = *iconCache->iconToCache(nameString, iconSize, MimeTypes, QString::fromUtf8(icon_path));
				free(icon_path);
			}

		return res;
	}
}
