#ifndef IDMNODEBASE_H_
#define IDMNODEBASE_H_

#include "idmdelegate.h"
#include "idmproxymodel.h"
#include "../items/idmitem.h"
#include "../containeres/idmcontainer.h"
#include "../../../filesystem/folder/base/filesystemfoldernodebase.h"


FILE_SYSTEM_NS_BEGIN

class IdmNodeBase : public FolderNodeBase
{
public:
	IdmNodeBase(IdmContainer *storage, const Info &info, Node *parent = 0);
	virtual ~IdmNodeBase();

    /* QAbstractItemModel */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	/* INode */
	virtual IFileInfo *info(const QModelIndex &idx) const;
	virtual IFileControl *createControl(const QModelIndex &idx, PluginsManager *plugins);

	/* INode::IFileOperations */
	virtual void remove(const QModelIndexList &list);
	virtual void cancel(const QModelIndexList &list);
	virtual void calculateSize(const QModelIndexList &list);
	virtual void pathToClipboard(const QModelIndexList &list);
	virtual void copy(const QModelIndexList &list, INode *destination);
	virtual void move(const QModelIndexList &list, INode *destination);

	/* Node */
	virtual void switchTo(Node *node, const QModelIndex &selected);

protected:
	/* Node */
	virtual QModelIndex rootIndex() const;
	virtual QAbstractItemModel *proxyModel() const { return &((IdmNodeBase *)this)->m_proxy; }
	virtual QAbstractItemDelegate *itemDelegate() const { return &((IdmNodeBase *)this)->m_delegate; }

	virtual Node *viewChild(const QModelIndex &idx, PluginsManager *plugins, QModelIndex &selected);
	virtual Node *viewChild(const QString &fileName, PluginsManager *plugins, QModelIndex &selected);

protected:
	/* FolderNodeBase */
	virtual UpdatesList::Map updateFilesMap() const;
	virtual void updateFilesEvent(const UpdatesList &updates);
	virtual void scanForSizeEvent(bool canceled, PScopedPointer<FileSystemList> &entries);
	virtual void scanForCopyEvent(bool canceled, PScopedPointer<FileSystemList> &entries, PScopedPointer<IFileControl> &destination, bool move);
	virtual void scanForRemoveEvent(bool canceled, PScopedPointer<FileSystemList> &entries);
	virtual void performCopyEvent(bool canceled, PScopedPointer<FileSystemList> &entries, bool move);
	virtual void performRemoveEvent(PScopedPointer<FileSystemList> &entries);

	virtual void updateProgressEvent(const QString &fileName, quint64 progress, quint64 timeElapsed);
	virtual void completedProgressEvent(const QString &fileName, quint64 timeElapsed);

protected:
	IdmItem *rootItem() const { return m_items.at(1); }

protected:
	typedef QList<IdmItem*> ItemsList;

protected:
	ItemsList m_items;
	IdmProxyModel m_proxy;
	IdmDelegate m_delegate;
	IdmContainer *m_storage;
};

FILE_SYSTEM_NS_END

#endif /* IDMNODEBASE_H_ */