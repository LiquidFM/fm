#include "libunrarscanner.h"
#include "libunrarfileinfo.h"
#include "../../../../filesystem/tools/filesystempath.h"
#include "../../../../filesystem/containers/filesystemsnapshot.h"

#include <wchar.h>
#include <libunrar/rar.hpp>
#include <libunrar/dll.hpp>
#include <libunrar/timefn.hpp>

#include <QtCore/QMap>


LIBUNRAR_ARC_PLUGIN_NS_BEGIN

inline static IFileInfo::size_type unpackedSize(const struct RARHeaderDataEx &info)
{
	IFileInfo::size_type res = info.UnpSizeHigh;
	return (res << (sizeof(unsigned int) * 8)) | info.UnpSize;
}

inline static QDateTime fromDosTime(unsigned int time)
{
	RarTime dt;
	RarLocalTime lt;

	dt.SetDos(time);
	dt.GetLocal(&lt);

	return QDateTime(QDate(lt.Year, lt.Month, lt.Day), QTime(lt.Hour, lt.Minute, lt.Second));
}


class WrappedNodeItem : public FileSystem::WrappedNodeItem
{
public:
	typedef QMap<QString, FileSystem::WrappedNodeItem *> Container;

public:
	WrappedNodeItem(const IFileContainer *container, const QString &fileName, const struct RARHeaderDataEx &info, WrappedNodeItem *parent) :
		FileSystem::WrappedNodeItem(container, unpackedSize(info), parent)
	{
		m_data.path = QString::fromWCharArray(info.FileNameW);
		m_data.fileName = fileName;
		m_data.fileSize = totalSize();
		m_data.lastModified = fromDosTime(info.FileTime);
	}
	virtual ~WrappedNodeItem()
	{
		qDeleteAll(m_map);
	}

	WrappedNodeItem *find(const QString &fileName) const
	{
		return static_cast<WrappedNodeItem *>(m_map.value(fileName));
	}

	void insert(const QString &fileName, WrappedNodeItem *item)
	{
		incTotalSize((m_map[fileName] = item)->totalSize());
	}

	void populateInfo()
	{
		if (m_map.isEmpty())
			info() = new Info(m_data, false);
		else
		{
			items().reserve(items().size() + m_map.size());

			for (Container::iterator i = m_map.begin(), e = m_map.end(); i != e; i = m_map.erase(i))
			{
				items().append(*i);
				static_cast<WrappedNodeItem *>(*i)->populateInfo();
			}

			m_data.fileSize += totalSize();
			info() = new Info(m_data, true);
		}
	}

private:
	Info::Data m_data;
	Container m_map;
};


Scanner::Scanner(const IFileContainer *container, const QString &fileName) :
	m_container(container),
	m_fileName(fileName)
{}

Scanner::~Scanner()
{}

Scanner::IEnumerator *Scanner::enumerate(QString &error) const
{
	error = tr("LibUnrar does not support enumerating.");
	return NULL;
}

IFileInfo *Scanner::info(const QString &fileName, QString &error) const
{
	return NULL;
}

void Scanner::scan(Snapshot &snapshot, const volatile Flags &aborted, QString &error) const
{
	QByteArray fileName(m_container->location(m_fileName).toUtf8());

	struct RAROpenArchiveDataEx archiveData;
	memset(&archiveData, 0, sizeof(struct RAROpenArchiveDataEx));

	archiveData.ArcName = fileName.data();
	archiveData.OpenMode = RAR_OM_LIST;

	if (void *archive = RAROpenArchiveEx(&archiveData))
	{
		QMap<QString, WrappedNodeItem *> parents;
	    IFileContainer::Holder container;
		struct RARHeaderDataEx fileInfo;
	    WrappedNodeItem *parent;
	    WrappedNodeItem *entry;
	    QString fileName;
	    const wchar_t *path;
	    wchar_t *sep;
		int res;

	    while ((res = RARReadHeaderEx(archive, &fileInfo)) == 0 && !aborted)
	    {
			path = fileInfo.FileNameW;

			if ((sep = wcschr(const_cast<wchar_t *>(path), L'/')) != NULL)
			{
				(*sep) = 0;
				WrappedNodeItem *&p = parents[fileName = QString::fromWCharArray(path)];
				(*sep) = L'/';

				if (p == NULL)
					snapshot.insert(fileName, p = parent = new WrappedNodeItem(m_container, fileName, fileInfo, NULL));
				else
					parent = p;

				path = (++sep);

				while ((sep = wcschr(const_cast<wchar_t *>(path), L'/')) != NULL)
				{
					(*sep) = 0;

					if (entry = parent->find(fileName = QString::fromWCharArray(path)))
						parent = entry;
					else
					{
						parent->insert(fileName, entry = new WrappedNodeItem(m_container, fileName, fileInfo, parent));
						parent = entry;
					}

					(*sep) = L'/';
					path = (++sep);
				}

				if (!(fileName = QString::fromWCharArray(path)).isEmpty() &&
					parent->find(fileName) == NULL)
				{
					parent->insert(fileName, new WrappedNodeItem(m_container, fileName, fileInfo, parent));
				}
			}
			else
			{
				WrappedNodeItem *&p = parents[fileName = QString::fromWCharArray(path)];

				if (p == NULL)
					snapshot.insert(fileName, p = new WrappedNodeItem(m_container, fileName, fileInfo, NULL));
			}

			RARProcessFile(archive, RAR_SKIP, NULL, NULL);
	    }

		if (!aborted)
	    	if (res == ERAR_END_ARCHIVE)
				for (Snapshot::const_iterator i = snapshot.begin(), end = snapshot.end(); i != end; ++i)
					static_cast<WrappedNodeItem *>((*i).second)->populateInfo();
	    	else
	    		error = errorDescription(res);

		RARCloseArchive(archive);
	}
}

void Scanner::refresh(Snapshot &snapshot, const volatile Flags &aborted, QString &error) const
{
	error = tr("LibUnrar does not support refreshing.");
}

QString Scanner::errorDescription(int code)
{
	switch (code)
	{
		case ERAR_NO_MEMORY:
			return tr("ERAR_NO_MEMORY");

		case ERAR_BAD_DATA:
			return tr("ERAR_BAD_DATA");

		case ERAR_BAD_ARCHIVE:
			return tr("ERAR_BAD_ARCHIVE");

		case ERAR_UNKNOWN_FORMAT:
			return tr("ERAR_UNKNOWN_FORMAT");

		case ERAR_EOPEN:
			return tr("ERAR_EOPEN");

		case ERAR_ECREATE:
			return tr("ERAR_ECREATE");

		case ERAR_ECLOSE:
			return tr("ERAR_ECLOSE");

		case ERAR_EREAD:
			return tr("ERAR_EREAD");

		case ERAR_EWRITE:
			return tr("ERAR_EWRITE");

		case ERAR_SMALL_BUF:
			return tr("ERAR_SMALL_BUF");

		case ERAR_UNKNOWN:
			return tr("ERAR_UNKNOWN");

		default:
			return tr("Unknown");
	}
}

LIBUNRAR_ARC_PLUGIN_NS_END
