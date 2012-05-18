#include "arcfilecontainer.h"


ARC_PLUGIN_NS_BEGIN

IFileContainer *FileContainer::create(const IFileContainer *container, const IFileInfo *file, QString &error)
{
	IFileContainer::Holder localContainer(container->open());

	if (localContainer)
	{
		IFileAccessor::Holder reader(localContainer->open(file->fileName(), IFileAccessor::ReadWrite, error));

		if (reader)
			return new FileContainer(localContainer, reader, file->fileName());
		else
			if (reader = localContainer->open(file->fileName(), IFileAccessor::ReadOnly, error))
				return new FileContainer(localContainer, reader, file->fileName());
	}

	return NULL;
}

QString FileContainer::location() const
{
	return m_path;
}

QString FileContainer::location(const QString &fileName) const
{
	return QString(m_path).append(QChar('/')).append(fileName);
}

bool FileContainer::isPhysical() const
{
	return false;
}

IFileInfo::size_type FileContainer::freeSpace() const
{
	return m_data->container->freeSpace();
}

ICopyControl *FileContainer::createControl(INodeView *view) const
{
	return NULL;
}

bool FileContainer::contains(const QString &fileName) const
{
	return false;
}

bool FileContainer::remove(const QString &fileName, QString &error) const
{
	return false;
}

bool FileContainer::rename(const QString &oldName, const QString &newName, QString &error) const
{
	return false;
}

bool FileContainer::move(const IFileContainer *source, const QString &fileName, QString &error) const
{
	return false;
}

IFileContainer *FileContainer::open() const
{
	return new FileContainer(*this);
}

IFileAccessor *FileContainer::open(const QString &fileName, int flags, QString &error) const
{
	return NULL;
}

IFileContainer *FileContainer::open(const QString &fileName, bool create, QString &error) const
{
	return NULL;
}

const IFileContainerScanner *FileContainer::scanner() const
{
	return this;
}

void FileContainer::enumerate(IEnumerator::Holder &enumerator) const
{

}

IFileInfo *FileContainer::info(const QString &fileName, QString &error) const
{
	return NULL;
}

void FileContainer::scan(Snapshot &snapshot, const volatile Flags &aborted) const
{

}

void FileContainer::refresh(Snapshot &snapshot, const volatile Flags &aborted) const
{

}

void FileContainer::scan(WrappedNodeItem *root, const volatile Flags &aborted) const
{

}

FileContainer::FileContainer(IFileContainer::Holder &container, IFileAccessor::Holder &file, const QString &fileName) :
	m_data(new Data(container, file, fileName)),
	m_path(m_data->container->location().append(QChar('/')).append(fileName))
{}

ARC_PLUGIN_NS_END