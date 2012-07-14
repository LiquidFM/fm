#ifndef IDMROOTNODE_H_
#define IDMROOTNODE_H_

#include <vfs/tasks/vfs_tasksnode.h>
#include "idmrootnodedelegate.h"
#include "../../containeres/idmcontainer.h"


IDM_PLUGIN_NS_BEGIN

class IdmRootNode : public TasksNode
{
public:
	IdmRootNode(IFileContainer::Holder &container, Node *parent = 0);
	virtual ~IdmRootNode();

    /* FileSystemModel */
	virtual int columnCount(const QModelIndex &parent) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/* IFileOperations */
	virtual ICopyControl *createControl(INodeView *view) const;
	virtual void contextMenu(const QModelIndexList &list, INodeView *view);
	virtual void createFile(const QModelIndex &index, INodeView *view);
	virtual void createDirectory(const QModelIndex &index, INodeView *view);
	virtual void rename(const QModelIndex &index, INodeView *view);
	virtual void rename(const QModelIndexList &list, INodeView *view);
	virtual void remove(const QModelIndexList &list, INodeView *view);
	virtual void cancel(const QModelIndexList &list, INodeView *view);
	virtual void calculateSize(const QModelIndexList &list, INodeView *view);
	virtual void pathToClipboard(const QModelIndexList &list, INodeView *view);
	virtual void copy(const INodeView *source, INodeView *destination);
	virtual void move(const INodeView *source, INodeView *destination);
	virtual void removeToTrash(const QModelIndexList &list, INodeView *view);
	virtual ::History::Entry *search(const QModelIndex &index, INodeView *view);

	/* INode */
    virtual void refresh();
	virtual QString title() const;
	virtual QString location() const;
	virtual Sorting sorting() const;
	virtual Geometry geometry() const;
	virtual QAbstractItemModel *model() const;
	virtual QAbstractItemDelegate *delegate() const;
	virtual const INodeView::MenuActionList &actions() const;
	virtual QAbstractItemView::SelectionMode selectionMode() const;
	virtual ::History::Entry *menuAction(QAction *action, INodeView *view);

protected:
	/* Node */
	virtual QModelIndex rootIndex() const;
	virtual Node *viewChild(const QModelIndex &idx, QModelIndex &selected);
	virtual Node *viewChild(const QString &fileName, QModelIndex &selected);
	virtual void nodeRemoved(Node *node);

protected:
	/* TasksNode */
	virtual void updateProgressEvent(const NodeItem::Holder &item, quint64 progress, quint64 timeElapsed);
	virtual void completedProgressEvent(const NodeItem::Holder &item, quint64 timeElapsed);
	virtual void performActionEvent(const AsyncFileAction::FilesList &files, const QString &error);

private:
	enum MenuId
	{
		Create,
		Remove,
		AddProperty,
		RemoveProperty,
		Find
	};

	enum Items
	{
		RootItemIndex = 0,
		FilesItemIndex = 1
	};

private:
	class Container : public TasksNode::Container
	{
	public:
		typedef QList<Item*> List;

	public:
		Container();
		virtual ~Container();

		virtual size_type size() const;
		virtual Item *at(size_type index) const;
		virtual size_type indexOf(Item *item) const;

	private:
		friend class IdmRootNode;
		List m_container;
	};

private:
	void createEntity();
	void removeEntity(const QModelIndex &index);
	void addProperty(const QModelIndex &index);
	void removeProperty(const QModelIndex &index);

private:
	void doAdd(IdmEntity *entity);
	void doRemove(IdmEntity *entity);
	void doAdd(const QModelIndex &index, Container::Item *item, IdmEntity *property, const QString &propertyName);
	void doRemove(const QModelIndex &index, Container::Item *item, Container::Item *property);
	void expand(Container::Item *parent);

private:
	typedef QMap<IdmEntity*, Container::List> EntitiesMap;

private:
	INodeView::MenuActionList m_actions;
	Container m_itemsContainer;
	Container::List &m_items;
	EntitiesMap m_entities;
	IdmContainer m_container;
	IdmRootNodeDelegate m_delegate;
};

IDM_PLUGIN_NS_END

#endif /* IDMROOTNODE_H_ */