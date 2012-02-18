#include "../filesysteminfo.h"
#include "../filesystemfile.h"
#include "../../tools/filesystemcommontools.h"
#include "../../../application.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>


FILE_SYSTEM_NS_BEGIN

static QString root = QString::fromLatin1("/");
static uint userId = getuid();
static uint groupId = getgid();


static int translatePermissions(const struct stat &st)
{
	int res = 0;

	if ((st.st_mode & S_IROTH) ||
		(st.st_uid == userId && (st.st_mode & S_IRUSR)) ||
		(st.st_gid == groupId && (st.st_mode & S_IRGRP)))
		res |= IFileInfo::Read;

	if ((st.st_mode & S_IWOTH) ||
		(st.st_uid == userId  && (st.st_mode & S_IWUSR)) ||
		(st.st_gid == groupId && (st.st_mode & S_IWGRP)))
		res |= IFileInfo::Write;

	if ((st.st_mode & S_IXOTH) ||
		(st.st_uid == userId  && (st.st_mode & S_IXUSR)) ||
		(st.st_gid == groupId && (st.st_mode & S_IXGRP)))
		res |= IFileInfo::Exec;

	return res;
}


static QString normalizeFilePath(const QString &filePath, bool &isRoot)
{
	QString res = filePath;

	if (res.startsWith(QChar('/')))
	{
		if (res.size() == 1)
			isRoot = true;

		return res;
	}
	else
	{
		char buff[PATH_MAX] = {};

		if (char *name = getcwd(buff, PATH_MAX))
			res.prepend(QString::fromUtf8(name));
	}

	if (res.endsWith(QChar('/')))
		res.chop(1);

	return res;
}


Info::Info() :
	m_isRoot(false)
{}

Info::Info(const QString &filePath) :
	m_isRoot(false),
	m_filePath(normalizeFilePath(filePath, m_isRoot)),
	m_fileName(filePath.mid(filePath.lastIndexOf(QChar('/')) + 1))
{
	refresh();
}

Info::Info(const QString &filePath, bool identifyType) :
	m_isRoot(false),
	m_filePath(normalizeFilePath(filePath, m_isRoot)),
	m_fileName(filePath.mid(filePath.lastIndexOf(QChar('/')) + 1))
{
	refresh();

	if (identifyType)
		m_info.type = Application::desktopService()->fileTypeInfo(m_filePath, m_info.isDir, 16);
}

FileTypeId Info::id() const
{
	return m_info.type.id;
}

QIcon Info::icon() const
{
	return m_info.type.icon;
}

QString Info::name() const
{
	return m_info.type.name;
}

QString Info::description() const
{
	return m_info.type.descritpion;
}

bool Info::isDir() const
{
	return m_info.isDir;
}

bool Info::isFile() const
{
	return m_info.isFile;
}

bool Info::isLink() const
{
	return m_info.isLink;
}

bool Info::exists() const
{
	struct stat st;
	return stat(m_filePath.toUtf8(), &st) == 0;
}

IFile::size_type Info::fileSize() const
{
	return m_info.size;
}

QString Info::fileName() const
{
	return m_fileName;
}

QString Info::absolutePath() const
{
	return m_filePath.mid(0, m_filePath.lastIndexOf(QChar('/')));
}

QString Info::absoluteFilePath() const
{
	return m_filePath;
}

QString Info::absoluteFilePath(const QString &fileName) const
{
#ifdef Q_OS_WIN
	QString str = m_info.isDir() ? absoluteFilePath() : absolutePath();

	if (str.isEmpty())
		return fileName;
	else
		if (str.endsWith(QChar('/')))
			return str.append(fileName);
		else
			return str.append(QChar('/')).append(fileName);
#else
	QString str = m_info.isDir ? absoluteFilePath() : absolutePath();

	if (fileName == root)
		return fileName;
	else
		return str.append(QChar('/')).append(fileName);
#endif
}

QDateTime Info::lastModified() const
{
	return m_info.lastModified;
}

int Info::permissions() const
{
	return m_info.permissions;
}

void Info::refresh()
{
	int res;
	struct stat st;
	QByteArray name = m_filePath.toUtf8();

	if ((res = lstat(name, &st)) == 0)
		if ((m_info.isFile = S_ISREG(st.st_mode)) || (m_info.isDir = S_ISDIR(st.st_mode)))
		{
			m_info.permissions = translatePermissions(st);
			m_info.size = st.st_size;
			m_info.lastModified = QDateTime::fromTime_t(st.st_mtime);
		}
		else if (m_info.isLink = S_ISLNK(st.st_mode))
		{
			char buff[PATH_MAX] = {};

			if ((res = readlink(name, buff, PATH_MAX)) == 0)
				if (char *realName = canonicalize_file_name(buff))
				{
					if ((res = stat(realName, &st)) == 0)
						if ((m_info.isFile = S_ISREG(st.st_mode)) || (m_info.isDir = S_ISDIR(st.st_mode)))
						{
							m_info.permissions = translatePermissions(st);
							m_info.size = st.st_size;
							m_info.lastModified = QDateTime::fromTime_t(st.st_mtime);
						}

					free(realName);
				}
		}
}

IFile::size_type Info::freeSpace() const
{
	return Tools::freeSpace(m_info.isDir ? absoluteFilePath().toUtf8() : absolutePath().toUtf8());
}

bool Info::contains(const QString &fileName) const
{
	return QDir(absoluteFilePath()).exists(fileName);
}

bool Info::rename(const QString &newFileName, QString &error) const
{
	QFile file(absoluteFilePath());

	if (file.rename(absoluteFilePath(newFileName)))
		return true;
	else
	{
		error = file.errorString();
		return false;
	}
}

IFile *Info::file(IFile::OpenMode mode, QString &error) const
{
	QFile::OpenMode openMode;
	PScopedPointer<FileSystem::File> file(new FileSystem::File(absoluteFilePath()));

	switch (mode)
	{
		case IFile::ReadOnly:
			openMode = QFile::ReadOnly;
			break;

		case IFile::ReadWrite:
			openMode = QFile::ReadWrite | QFile::Truncate;
			break;
	}

	if (file->open(openMode))
		return file.take();
	else
		error = file->lastError();

	return 0;
}

IFileControl *Info::openFile(const QString &fileName, QString &error) const
{
	return new Info(absoluteFilePath(fileName));
}

IFileControl *Info::openFolder(const QString &fileName, bool create, QString &error) const
{
	QDir dir(isDir() ? absoluteFilePath() : absolutePath());

	if (dir.exists(fileName))
		return new Info(dir.absoluteFilePath(fileName));
	else
		if (create)
			if (dir.mkdir(fileName))
				return new Info(dir.absoluteFilePath(fileName));
			else
				error = QString::fromLatin1("Failed to create directory \"%1\".").arg(absoluteFilePath(fileName));

	return 0;
}

FILE_SYSTEM_NS_END
