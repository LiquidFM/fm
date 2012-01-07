#ifndef FILESYSTEMCOPYINFO_H_
#define FILESYSTEMCOPYINFO_H_

#include <QtCore/QCoreApplication>
#include "filesysteminfo.h"
#include "../tasks/interfaces/filesystemicopycontrol.h"


FILE_SYSTEM_NS_BEGIN

class CopyInfo : public ICopyControl
{
	Q_DECLARE_TR_FUNCTIONS(CopyInfo)

public:
	CopyInfo(const Info &info);

	/* IFileType */
	virtual QIcon icon() const;
	virtual QString name() const;
	virtual QString description() const;

	/* IFileInfo */
	virtual bool isDir() const;
	virtual bool isFile() const;
	virtual bool isLink() const;
	virtual bool exists() const;
	virtual qint64 fileSize() const;
	virtual QString fileName() const;
	virtual QString absolutePath() const;
	virtual QString absoluteFilePath() const;
	virtual QString absoluteFilePath(const QString &fileName) const;
	virtual QDateTime lastModified() const;
	virtual int permissions() const;
	virtual void refresh();

	/* IFileControl */
	virtual IFile::size_type freeSpace() const;
	virtual bool contains(IFileControl *info) const;
	virtual bool rename(const QString &newFileName, QString &error) const;

	virtual IFile *open(IFile::OpenMode mode, QString &error) const;
	virtual IFileControl *open(IFileControl *info, QString &error) const;

	virtual IFileControl *create(IFileControl *info, QString &error) const;
	virtual IFileControl *create(const QString &name, FileType type, QString &error) const;

	/* ICopyControl */
	virtual bool physicalCopyIsNecessary() const;
	virtual bool start(const ScanedFiles::Files &files, bool move);
	virtual void done(bool error);
	virtual void canceled();

private:
	Info m_info;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMCOPYINFO_H_ */
