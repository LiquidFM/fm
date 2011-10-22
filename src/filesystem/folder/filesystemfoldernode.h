#ifndef FILESYSTEMFOLDERNODE_H_
#define FILESYSTEMFOLDERNODE_H_

#include "filesystemfolderdelegate.h"
#include "filesystemfolderproxymodel.h"
#include "functors/filesystemfoldernodefunctors.h"
#include "base/filesystemfoldernodebase.h"
#include "containers/filesystemitemscontainer.h"
#include "../../tools/metatemplates.h"
#include "../../tools/rangeintersection.h"


FILE_SYSTEM_NS_BEGIN

class FolderNode : public FolderNodeBase
{
	Q_DISABLE_COPY(FolderNode)

public:
	FolderNode(const Info &info, Node *parent = 0);

    /* FileSystemModel */
	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/* INode */
	virtual IFileInfo *info(const QModelIndex &idx) const;

	/* INode::IFileOperations */
	virtual void menuAction(QAction *action);
	virtual void createFile(const QModelIndex &index);
	virtual void createDirectory(const QModelIndex &index);
	virtual void rename(const QModelIndexList &list);
	virtual void remove(const QModelIndexList &list);
	virtual void cancel(const QModelIndexList &list);
	virtual void calculateSize(const QModelIndexList &list);
	virtual void pathToClipboard(const QModelIndexList &list);
	virtual void copy(const QModelIndexList &list, INode *destination);
	virtual void move(const QModelIndexList &list, INode *destination);

protected:
	/* Node */
	virtual QModelIndex rootIndex() const;
	virtual QAbstractItemModel *proxyModel() const { return &((FolderNode *)this)->m_proxy; }
	virtual QAbstractItemDelegate *itemDelegate() const { return &((FolderNode *)this)->m_delegate; }
	virtual const INodeView::MenuActionList &menuActions() const { return m_menuActions; }

	virtual Node *viewChild(const QModelIndex &idx, PluginsManager *plugins, QModelIndex &selected);
	virtual Node *viewChild(const QString &fileName, PluginsManager *plugins, QModelIndex &selected);

protected:
	/* FolderNodeBase */
	virtual UpdatesList::Map updateFilesMap() const;
	virtual void updateFilesEvent(const UpdatesList &updates);
	virtual void scanForSizeEvent(bool canceled, PScopedPointer<InfoListItem> &entries);
	virtual void scanForCopyEvent(bool canceled, PScopedPointer<InfoListItem> &entries, PScopedPointer<ICopyControl> &control, bool move);
	virtual void scanForRemoveEvent(bool canceled, PScopedPointer<InfoListItem> &entries);
	virtual void performCopyEvent(bool canceled, PScopedPointer<InfoListItem> &entries, bool move);
	virtual void performRemoveEvent(PScopedPointer<InfoListItem> &entries);

	virtual void updateProgressEvent(const QString &fileName, quint64 progress, quint64 timeElapsed);
	virtual void completedProgressEvent(const QString &fileName, quint64 timeElapsed);

protected:
	typedef QPair<Values::size_type, FileSystemBaseItem*> ProcessedValue;
	class ProcessedList : public Functors::Functor, public QList<ProcessedValue>
	{
	protected:
		virtual void call(Values::size_type index, FileSystemBaseItem *entry)
		{
			push_back(ProcessedValue(index, entry));
		}
	};

	class AbsoluteFilePathList : public Functors::Functor, public QStringList
	{
	protected:
		virtual void call(Values::size_type index, FileSystemBaseItem *entry)
		{
			push_back(entry->absoluteFilePath());
		}
	};

	class CancelFunctor : public Functors::Functor
	{
	public:
		CancelFunctor(TasksMap &tasks) :
			m_tasks(tasks)
		{}

	protected:
		virtual void call(Values::size_type index, FileSystemBaseItem *entry);

	private:
		TasksMap &m_tasks;
	};

	class RenameFunctor : public Functors::Functor
	{
	protected:
		virtual void call(Values::size_type index, FileSystemBaseItem *entry);
	};

	void processIndexList(const QModelIndexList &list, Functors::Functor &functor);
	void processLockedIndexList(const QModelIndexList &list, Functors::Functor &functor);

private:
	void scanForRemove(const ProcessedList &entries);
	void scanForSize(const ProcessedList &entries);
	void scanForCopy(const ProcessedList &entries, INode *destination, bool move);

private:
	QModelIndex index(int column, FileSystemBaseItem *item) const;
	Node *createNode(const Info &info, PluginsManager *plugins) const;
	QModelIndex indexForFile(FileSystemBaseItem *item) const;
	QModelIndex indexForFile(FileSystemBaseItem *item, Values::size_type index) const;

	void updateFirstColumn(FileSystemBaseItem *entry);
	void updateFirstColumn(const RangeIntersection &range);
	void updateFirstColumn(Values::size_type index, FileSystemBaseItem *entry);
	void updateSecondColumn(FileSystemBaseItem *entry);
	void updateSecondColumn(const RangeIntersection &range);
	void updateSecondColumn(Values::size_type index, FileSystemBaseItem *entry);
	void updateBothColumns(FileSystemBaseItem *entry);
	void updateBothColumns(const RangeIntersection &range);
	void updateBothColumns(Values::size_type index, FileSystemBaseItem *entry);
	void removeEntry(Values::size_type index);
	void removeEntry(const QModelIndex &index);

private:
	ItemsContainer m_container;
	Values &m_items;
	FolderProxyModel m_proxy;
	FolderDelegate m_delegate;
	INodeView::MenuActionList m_menuActions;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMFOLDERNODE_H_ */
