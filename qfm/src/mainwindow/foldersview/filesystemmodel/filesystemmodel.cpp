#include "filesystemmodel.h"
#include "items/filesystemtree.h"
#include "items/filesystementry.h"
#include "events/filesystemmodelevents.h"
#include "tasks/listfilestask.h"
#include "tasks/updatefilestask.h"
#include "tasks/scanfilestask.h"
#include "visitor/filesystemlockedentryvisitor.h"
#include "../../../tools/rangeintersection.h"
#include "../../../application.h"
#include <QtCore/QSet>
#include <QtCore/QThreadPool>
#include <QtGui/QClipboard>
#include <QtGui/QMessageBox>


FileSystemModel::FileSystemModel(const QString &currentDirectory, QObject *parent) :
	QAbstractItemModel(parent),
	m_currentFsTree(new FileSystemTree(currentDirectory))
{
	list(m_currentFsTree);
}

FileSystemModel::FileSystemModel(const QFileInfo &fileInfo, QObject *parent) :
	QAbstractItemModel(parent),
	m_currentFsTree(new FileSystemTree(fileInfo))
{
	list(m_currentFsTree);
}

FileSystemModel::~FileSystemModel()
{
	while (m_currentFsTree->parent() != 0)
		m_currentFsTree = m_currentFsTree->parent();

	delete m_currentFsTree;
}

bool FileSystemModel::event(QEvent *e)
{
	switch (static_cast<FileSystemModelEvent::EventType>(e->type()))
	{
		case FileSystemModelEvent::ListFiles:
		{
			e->accept();
			listEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		case FileSystemModelEvent::UpdateFiles:
		{
			e->accept();
			updateEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		case FileSystemModelEvent::ScanFilesForRemove:
		{
			e->accept();
			scanForRemoveEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		case FileSystemModelEvent::ScanFilesForSize:
		{
			e->accept();
			scanForSizeEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		case FileSystemModelEvent::ScanFilesForCopy:
		{
			e->accept();
			scanForCopyEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		case FileSystemModelEvent::ScanFilesForMove:
		{
			e->accept();
			scanForMoveEvent(static_cast<FileSystemModelEvent*>(e)->parameters());
			return true;
		}
		default:
			break;
	}

	return QAbstractItemModel::event(e);
}

int FileSystemModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
        return static_cast<FileSystemItem*>(parent.internalPointer())->size();
	else
    	return m_currentFsTree->size();
}

int FileSystemModel::columnCount(const QModelIndex &parent) const
{
	return 3;
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    	return static_cast<FileSystemItem*>(index.internalPointer())->data(index.column(), role);
    else
    	return m_currentFsTree->child(index.row())->data(index.column(), role);
}

Qt::ItemFlags FileSystemModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
    	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    else
        return Qt::NoItemFlags;
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		switch (section)
		{
			case 0:
			{
				return tr("Name");
				break;
			}
			case 1:
			{
				return tr("Size");
				break;
			}
			case 2:
			{
				return tr("Modified");
				break;
			}
		}

	return QVariant();
}

QModelIndex FileSystemModel::index(int row, int column, const QModelIndex &parent) const
{
	if (hasIndex(row, column, parent))
		if (parent.isValid())
			return index(row, column, static_cast<FileSystemItem*>(parent.internalPointer()));
		else
			return createIndex(row, column, m_currentFsTree->child(row));
    else
        return QModelIndex();
}

QModelIndex FileSystemModel::parent(const QModelIndex &child) const
{
    if (child.isValid())
		if (FileSystemItem *parentItem = static_cast<FileSystemItem*>(child.internalPointer())->parent())
			if (parentItem != m_currentFsTree)
				if (parentItem->parent())
					return createIndex(parentItem->parent()->indexOf(parentItem), 0, parentItem);
				else
					return createIndex(m_currentFsTree->indexOf(parentItem), 0, parentItem);

    return QModelIndex();
}

QModelIndex FileSystemModel::rootIndex() const
{
	if (m_currentFsTree->size() > 0 && m_currentFsTree->child(0)->isRoot())
		return createIndex(0, 0, m_currentFsTree->child(0));
	else
		return QModelIndex();
}

QModelIndex FileSystemModel::parentEntryIndex() const
{
	if (m_currentFsTree->parent() != 0)
	{
		FileSystemTree::size_type index = m_currentFsTree->parent()->indexOf(static_cast<FileSystemTree*>(m_currentFsTree)->parentEntry());

		if (index != FileSystemTree::InvalidIndex)
			return createIndex(index, 0, static_cast<FileSystemTree*>(m_currentFsTree)->parentEntry());
	}

	return QModelIndex();
}

const QFileInfo &FileSystemModel::currentDirectoryInfo() const
{
	return static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo();
}

const QFileInfo &FileSystemModel::fileInfo(const QModelIndex &index) const
{
	if (m_currentFsTree->child(index.row())->isRoot())
		return static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo();
	else
		return static_cast<FileSystemEntry*>(m_currentFsTree->child(index.row()))->fileInfo();
}

void FileSystemModel::pathToClipboard(const QModelIndexList &list) const
{
	QString text;
	QSet<FileSystemItem*> set;
	FileSystemItem *item;
#ifdef Q_OS_WIN32
	QString endl = QString::fromLatin1("\r\n");
#else
	QString endl = QString::fromLatin1("\n");
#endif

	for (QModelIndexList::size_type i = 0, size = list.size(); i < size; ++i)
		if (!set.contains(item = static_cast<FileSystemItem*>(list.at(i).internalPointer())))
		{
			set.insert(item);

			if (!item->isRoot())
				text.append(static_cast<FileSystemEntry*>(item)->fileInfo().absoluteFilePath()).append(endl);
		}

	text.chop(endl.size());
	Application::clipboard()->setText(text);
}

void FileSystemModel::pathToClipboard(const FileSystemModelAdaptor &list) const
{
	QString text;
	QSet<FileSystemItem*> set;
	FileSystemItem *item;
#ifdef Q_OS_WIN32
	QString endl = QString::fromLatin1("\r\n");
#else
	QString endl = QString::fromLatin1("\n");
#endif

	for (FileSystemModelAdaptor::size_type i = 0, size = list.size(); i < size; ++i)
		if (!set.contains(item = static_cast<FileSystemItem*>(list.at(i).internalPointer())))
		{
			set.insert(item);

			if (!item->isRoot())
				text.append(static_cast<FileSystemEntry*>(item)->fileInfo().absoluteFilePath()).append(endl);
		}

	text.chop(endl.size());
	Application::clipboard()->setText(text);
}

QModelIndex FileSystemModel::find(const QString &fileName) const
{
	for (FileSystemItem::size_type i = 0, size = m_currentFsTree->size(); i < size; ++i)
		if (!static_cast<FileSystemItem*>(m_currentFsTree->child(i))->isRoot() &&
			static_cast<FileSystemEntry*>(m_currentFsTree->child(i))->fileInfo().fileName() == fileName)
			return createIndex(i, 0, m_currentFsTree->child(i));

	return QModelIndex();
}

void FileSystemModel::refresh()
{
	if (isLocked())
		return;

	FileSystemTree *tree = static_cast<FileSystemTree*>(m_currentFsTree);
	tree->fileInfo().refresh();

	if (tree->fileInfo().exists())
		update(tree);
	else
	{
		beginRemoveRows(QModelIndex(), 0, tree->size() - 1);

		QDir dir = tree->fileInfo().absoluteDir();
		while (!dir.exists())
			dir.cdUp();

		delete m_currentFsTree;
		m_currentFsTree = new FileSystemTree(dir.absolutePath(), 0);
		list(m_currentFsTree);

		endRemoveRows();
	}
}

void FileSystemModel::refreshSize(const QModelIndex &index)
{
	if (!static_cast<FileSystemItem*>(index.internalPointer())->isRoot() &&
		!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
	{
		FileSystemEntry *entry = static_cast<FileSystemEntry*>(index.internalPointer());
		entry->fileInfo().refresh();

		if (entry->fileInfo().exists())
		{
			if (entry->fileInfo().isDir())
				scanForSize(m_currentFsTree, entry);
		}
		else
			removeEntry(index);
	}
}

void FileSystemModel::activated(const QModelIndex &index)
{
	if (static_cast<FileSystemItem*>(index.internalPointer())->isRoot())
		if (FileSystemTree *parent = static_cast<FileSystemTree*>(static_cast<FileSystemTree*>(m_currentFsTree)->parent()))
		{
			beginRemoveRows(QModelIndex(), 0, m_currentFsTree->size() - 1);
			if (!parent->isUpdating())
				update(parent);
			endRemoveRows();

			beginInsertRows(QModelIndex(), 0, parent->size() - 1);
			m_currentFsTree = parent;
			endInsertRows();
		}
		else
		{
			beginRemoveRows(QModelIndex(), 0, m_currentFsTree->size() - 1);
			static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo().refresh();

			if (static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo().exists())
			{
				parent = new FileSystemTree(static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo().absolutePath(), 0);
				parent->add<FileSystemEntry>(FilesTask::info(static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo()));
				parent->setSubtree(static_cast<FileSystemTree*>(m_currentFsTree));
				static_cast<FileSystemTree*>(m_currentFsTree)->setParentEntry(parent->last());
			}
			else
			{
				QDir dir = static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo().absoluteDir();
				while (!dir.exists())
					dir.cdUp();

				parent = new FileSystemTree(dir.absolutePath(), 0);
				delete m_currentFsTree;
			}
			endRemoveRows();

			beginInsertRows(QModelIndex(), 0, parent->size() - 1);
			update(parent);
			m_currentFsTree = parent;
			endInsertRows();
		}
	else
		if (!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
		{
			FileSystemEntry *entry = static_cast<FileSystemEntry*>(index.internalPointer());
			entry->fileInfo().refresh();

			if (entry->fileInfo().exists())
				if (entry->fileInfo().isDir())
					if (FileSystemTree *tree = static_cast<FileSystemTree*>(static_cast<FileSystemTree*>(m_currentFsTree)->subtree(entry)))
					{
						beginRemoveRows(QModelIndex(), 0, m_currentFsTree->size() - 1);
						if (!tree->isUpdating())
							update(tree);
						endRemoveRows();

						beginInsertRows(QModelIndex(), 0, tree->size() - 1);
						m_currentFsTree = tree;
						endInsertRows();
					}
					else
					{
						beginRemoveRows(QModelIndex(), 0, m_currentFsTree->size() - 1);
						tree = static_cast<FileSystemTree*>(m_currentFsTree);
						tree->setSubtree(entry, m_currentFsTree = new FileSystemTree(entry->fileInfo().absoluteFilePath(), tree));
						static_cast<FileSystemTree*>(m_currentFsTree)->setParentEntry(entry);
						endRemoveRows();

						beginInsertRows(QModelIndex(), 0, m_currentFsTree->size() - 1);
						list(m_currentFsTree);
						endInsertRows();
					}
				else
				{

				}
			else
			{
				FileSystemEntry::size_type index = m_currentFsTree->indexOf(entry);
				beginRemoveRows(QModelIndex(), index, index);
				static_cast<FileSystemTree*>(m_currentFsTree)->remove(index);
				endRemoveRows();
			}
		}
}

void FileSystemModel::setCurrentDirectory(const QString &filePath)
{
	QFileInfo info(filePath);

	if (info.exists())
		setCurrentDirectory(info);
}

void FileSystemModel::setCurrentDirectory(const QFileInfo &info)
{
	if (isLocked())
		return;

	beginRemoveRows(QModelIndex(), 0, m_currentFsTree->size() - 1);

	while (m_currentFsTree->parent() != 0)
		m_currentFsTree = m_currentFsTree->parent();
	delete m_currentFsTree;

	FileSystemTree *tree;

	if (info.isFile())
	{
		tree = new FileSystemTree(info.absolutePath(), 0);
		tree->add<FileSystemEntry>(FilesTask::info(info));
		update(tree);
	}
	else
	{
		tree = new FileSystemTree(info, 0);
		list(tree);
	}

	endRemoveRows();

	beginInsertRows(QModelIndex(), 0, tree->size() - 1);
	m_currentFsTree = tree;
	endInsertRows();
}

void FileSystemModel::rename(const QModelIndex &index, const QString &newFileName)
{
	if (!static_cast<FileSystemItem*>(index.internalPointer())->isRoot() &&
		!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
	{
		QFileInfo &info = static_cast<FileSystemEntry*>(index.internalPointer())->fileInfo();
		info.refresh();

		if (info.exists())
		{
			QDir dir = info.absoluteDir();

			if (dir.rename(info.fileName(), newFileName))
			{
				static_cast<FileSystemEntry*>(index.internalPointer())->update(FilesTask::info(QFileInfo(dir.absoluteFilePath(newFileName))));
				emit dataChanged(index, index);
			}
		}
		else
			removeEntry(index);
	}
}

void FileSystemModel::createDirectory(const QString &dirName)
{
	const QFileInfo &info = static_cast<FileSystemTree*>(m_currentFsTree)->fileInfo();

	if (info.exists())
	{
		QDir dir = info.absoluteDir();

		if (dir.mkdir(dirName))
		{
			beginInsertRows(QModelIndex(), m_currentFsTree->size(), m_currentFsTree->size());
			static_cast<FileSystemTree*>(m_currentFsTree)->add<FileSystemEntry>(FilesTask::info(QFileInfo(dir.absoluteFilePath(dirName))));
			endInsertRows();
		}
	}
}

void FileSystemModel::remove(const QModelIndex &index)
{
	if (!static_cast<FileSystemItem*>(index.internalPointer())->isRoot() &&
		!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
	{
		QFileInfo &info = static_cast<FileSystemEntry*>(index.internalPointer())->fileInfo();
		info.refresh();

		if (info.exists())
		{
			if (info.isDir())
				scanForRemove(static_cast<FileSystemTree*>(m_currentFsTree), static_cast<FileSystemEntry*>(index.internalPointer()));
			else
				if (info.absoluteDir().remove(info.fileName()))
				{
					beginRemoveRows(QModelIndex(), index.row(), index.row());
					static_cast<FileSystemTree*>(m_currentFsTree)->remove(index.row());
					endRemoveRows();
				}
		}
		else
			removeEntry(index);
	}
}

void FileSystemModel::copy(const QModelIndex &index, FileSystemModel *destination)
{
	if (!static_cast<FileSystemItem*>(index.internalPointer())->isRoot() &&
		!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
	{
		FileSystemEntry *entry = static_cast<FileSystemEntry*>(index.internalPointer());
		entry->fileInfo().refresh();

		if (entry->fileInfo().exists())
			scanForCopy(m_currentFsTree, entry, destination);
		else
			removeEntry(index);
	}
}

void FileSystemModel::move(const QModelIndex &index, FileSystemModel *destination)
{
	if (!static_cast<FileSystemItem*>(index.internalPointer())->isRoot() &&
		!static_cast<FileSystemEntry*>(index.internalPointer())->isLocked())
	{
		FileSystemEntry *entry = static_cast<FileSystemEntry*>(index.internalPointer());
		entry->fileInfo().refresh();

		if (entry->fileInfo().exists())
			scanForMove(m_currentFsTree, entry, destination);
		else
			removeEntry(index);
	}
}

void FileSystemModel::list(FileSystemItem *fileSystemTree)
{
	QScopedPointer<ListFilesTask::Params> params(new ListFilesTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);

	Application::instance()->taskPool().handle(new ListFilesTask(params.take()));
	static_cast<FileSystemTree*>(fileSystemTree)->setUpdating(true);
}

void FileSystemModel::listEvent(const FileSystemModelEvent::Params *p)
{
	typedef const ListFilesTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);

	if (!params->updates.isEmpty())
		if (m_currentFsTree == params->fileSystemTree)
		{
			beginInsertRows(QModelIndex(), params->fileSystemTree->size(), params->fileSystemTree->size() + params->updates.size() - 1);
			static_cast<FileSystemTree*>(params->fileSystemTree)->add<FileSystemEntry>(params->updates);
			endInsertRows();
		}
		else
			static_cast<FileSystemTree*>(params->fileSystemTree)->add<FileSystemEntry>(params->updates);

	if (params->isLastEvent)
		static_cast<FileSystemTree*>(params->fileSystemTree)->setUpdating(false);
}

void FileSystemModel::update(FileSystemItem *fileSystemTree)
{
	QScopedPointer<UpdateFilesTask::Params> params(new UpdateFilesTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);
	params->list = static_cast<FileSystemTree*>(fileSystemTree)->makeChangeSet();

	Application::instance()->taskPool().handle(new UpdateFilesTask(params.take()));
	static_cast<FileSystemTree*>(fileSystemTree)->setUpdating(true);
}

void FileSystemModel::updateEvent(const FileSystemModelEvent::Params *p)
{
	typedef const UpdateFilesTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);
	ChangesList list = params->updates;
	RangeIntersection updateRange(1);

	for (ChangesList::size_type i = 0; i < list.size();)
		if (list.at(i).type() == Change::Updated)
		{
			FileSystemEntry *entry = static_cast<FileSystemEntry*>(list.at(i).entry());
			FileSystemEntry::size_type index = params->fileSystemTree->indexOf(entry);
			entry->update(list.at(i).info());
			updateRange.add(index, index);
			list.removeAt(i);
		}
		else
			if (list.at(i).type() == Change::Deleted)
			{
				FileSystemEntry::size_type index = params->fileSystemTree->indexOf(list.at(i).entry());
				beginRemoveRows(QModelIndex(), index, index);
				static_cast<FileSystemTree*>(params->fileSystemTree)->remove(index);
				endRemoveRows();
				list.removeAt(i);
			}
			else
				++i;

	for (RangeIntersection::RangeList::size_type i = 0, size = updateRange.size(); i < size; ++i)
		emit dataChanged(createIndex(updateRange.at(i).top(), 0, params->fileSystemTree),
						 createIndex(updateRange.at(i).bottom(), columnCount(), params->fileSystemTree));

	if (!list.isEmpty())
	{
		beginInsertRows(QModelIndex(), params->fileSystemTree->size(), params->fileSystemTree->size() + list.size() - 1);
		static_cast<FileSystemTree*>(params->fileSystemTree)->add<FileSystemEntry>(list);
		endInsertRows();
	}

	if (params->isLastEvent)
		static_cast<FileSystemTree*>(params->fileSystemTree)->setUpdating(false);
}

void FileSystemModel::scanForRemove(FileSystemItem *fileSystemTree, FileSystemItem *entry)
{
	QScopedPointer<ScanFilesForRemoveTask::Params> params(new ScanFilesForRemoveTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);
	params->entry = static_cast<FileSystemEntry*>(entry);

	Application::instance()->taskPool().handle(new ScanFilesForRemoveTask(params.take()));
	static_cast<FileSystemEntry*>(entry)->setLocked(true);
}

void FileSystemModel::scanForRemoveEvent(const FileSystemModelEvent::Params *p)
{
	typedef const ScanFilesForRemoveTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);

	if (QMessageBox::question(
			&Application::instance()->mainWindow(),
			tr("Remove directory..."),
			tr("Would you like to remove \"%1\" directory?").arg(static_cast<FileSystemEntry*>(params->entry)->fileInfo().absoluteFilePath()),
			QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		if (params->fileSystemTree == m_currentFsTree)
		{
			FileSystemItem::size_type index = m_currentFsTree->indexOf(params->entry);

			beginRemoveRows(QModelIndex(), index, index);
			static_cast<FileSystemTree*>(m_currentFsTree)->remove(index);
			delete params->subtree;
			endRemoveRows();
		}
		else
		{
			params->fileSystemTree->remove(params->fileSystemTree->indexOf(params->entry));
			delete params->subtree;
		}
	else
		static_cast<FileSystemTree*>(params->fileSystemTree)->setSubtree(params->entry, params->subtree);

	params->entry->setLocked(false);
}

void FileSystemModel::scanForSize(FileSystemItem *fileSystemTree, FileSystemItem *entry)
{
	QScopedPointer<ScanFilesForSizeTask::Params> params(new ScanFilesForSizeTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);
	params->entry = static_cast<FileSystemEntry*>(entry);

	Application::instance()->taskPool().handle(new ScanFilesForSizeTask(params.take()));
	static_cast<FileSystemEntry*>(entry)->setLocked(true);
}

void FileSystemModel::scanForSizeEvent(const FileSystemModelEvent::Params *p)
{
	typedef const ScanFilesForSizeTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);
	static_cast<FileSystemEntry*>(params->entry)->setFileSize(params->size);

	if (m_currentFsTree == params->fileSystemTree)
	{
		QModelIndex index = createIndex(m_currentFsTree->indexOf(params->entry), 1, params->entry);
		emit dataChanged(index, index);
	}

	params->entry->setLocked(false);
}

void FileSystemModel::scanForCopy(FileSystemItem *fileSystemTree, FileSystemItem *entry, FileSystemModel *destination)
{
	QScopedPointer<ScanFilesForCopyTask::Params> params(new ScanFilesForCopyTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);
	params->entry = static_cast<FileSystemEntry*>(entry);
	params->destination = destination;

	Application::instance()->taskPool().handle(new ScanFilesForCopyTask(params.take()));
	static_cast<FileSystemEntry*>(entry)->setLocked(true);
}

void FileSystemModel::scanForCopyEvent(const FileSystemModelEvent::Params *p)
{
	typedef const ScanFilesForCopyTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);
	static_cast<FileSystemEntry*>(params->entry)->setFileSize(params->size);

	params->entry->setLocked(false);
}

void FileSystemModel::scanForMove(FileSystemItem *fileSystemTree, FileSystemItem *entry, FileSystemModel *destination)
{
	QScopedPointer<ScanFilesForMoveTask::Params> params(new ScanFilesForMoveTask::Params());
	params->receiver = (QObject*)this;
	params->fileSystemTree = static_cast<FileSystemTree*>(fileSystemTree);
	params->entry = static_cast<FileSystemEntry*>(entry);
	params->destination = destination;

	Application::instance()->taskPool().handle(new ScanFilesForMoveTask(params.take()));
	static_cast<FileSystemEntry*>(entry)->setLocked(true);
}

void FileSystemModel::scanForMoveEvent(const FileSystemModelEvent::Params *p)
{
	typedef const ScanFilesForMoveTask::EventParams *ParamsType;
	ParamsType params = static_cast<ParamsType>(p);
	static_cast<FileSystemEntry*>(params->entry)->setFileSize(params->size);

	params->entry->setLocked(false);
}

bool FileSystemModel::isLocked() const
{
	FileSystemLockedEntryVisitor visitor;
	m_currentFsTree->accept(&visitor);

	if (visitor.entrys().isEmpty())
		return false;
	else
	{
		QMessageBox::information(
				&Application::instance()->mainWindow(),
				tr("Busy directories"),
				tr("There is a busy directories:\n").append(visitor.directories().join(QChar('\n'))));
		return true;
	}
}

QModelIndex FileSystemModel::index(int column, FileSystemItem *item) const
{
	int index = m_currentFsTree->indexOf(item);

	if (index != -1)
		return createIndex(index, column, item);
	else
		return QModelIndex();
}

QModelIndex FileSystemModel::index(int row, int column, FileSystemItem *parentItem) const
{
	FileSystemItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

void FileSystemModel::removeEntry(const QModelIndex &index)
{
	beginRemoveRows(QModelIndex(), index.row(), index.row());
	static_cast<FileSystemTree*>(m_currentFsTree)->remove(index.row());
	endRemoveRows();
}