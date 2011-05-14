#ifndef FILESYSTEMFOLDERNODE_H_
#define FILESYSTEMFOLDERNODE_H_

#include <QtCore/QMap>
#include <QtCore/QList>
#include "filesystemchangeslist.h"
#include "filesystemfolderdelegate.h"
#include "filesystemfolderproxymodel.h"
#include "events/filesystemmodelevent.h"
#include "info/filesystemfoldernodeinfo.h"
#include "items/filesystemfoldernodeitem.h"
#include "items/filesystemfoldernodeentry.h"
#include "functors/filesystemfoldernodefunctors.h"
#include "../filesystem_ns.h"
#include "../filesystemnode.h"
#include "../../tools/metatemplates.h"


FILE_SYSTEM_NS_BEGIN

class FolderNode : public Node
{
	Q_DISABLE_COPY(FolderNode)

public:
	FolderNode(const Info &info, Node *parent = 0);

    virtual bool event(QEvent *e);

    /* QAbstractItemModel */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	/* IFileInfo */
	virtual bool isDir() const;
	virtual bool isFile() const;
	virtual bool exists() const;
	virtual QString fileName() const;
	virtual QString absolutePath() const;
	virtual QString absoluteFilePath() const;
	virtual QDateTime lastModified() const;

	virtual IFile *open(IFile::OpenMode mode, QString &error) const;
	virtual IFileInfo *create(const QString &fileName, FileType type, QString &error) const;

	virtual void refresh();

	/* IFileOperations */
	virtual void remove(const QModelIndexList &list);
	virtual void calculateSize(const QModelIndexList &list);
	virtual void copy(const QModelIndexList &list, Node *destination);
	virtual void move(const QModelIndexList &list, Node *destination);

	/* Node */
	virtual void update();
	virtual Node *subnode(const QModelIndex &idx, PluginsManager *plugins);
	virtual void remove(Node *subnode);
	virtual void view(QAbstractItemView *itemView);
	virtual QModelIndex indexFor(const QString &fileName);

	virtual QModelIndex parentEntryIndex() const { return m_parentEntryIndex; }
	virtual void setParentEntryIndex(const QModelIndex &value) { m_parentEntryIndex = value; }

	virtual QModelIndex rootIndex() const;
	virtual bool isRootIndex(const QModelIndex &index) const;

protected:
	virtual bool isRootNode() const { return m_items.size() == 0 || !m_items.at(0).item->isRootItem(); }
	virtual Node *node(const QString &fileName, PluginsManager *plugins);

protected:
	void processIndexList(const QModelIndexList &list, const Functors::Functor &functor);
	void removeFunctor(FolderNodeItem *entry);
	void calculateSizeFunctor(FolderNodeItem *entry);
	void copyFunctor(FolderNodeItem *entry, Node *destination);
	void moveFunctor(FolderNodeItem *entry, Node *destination);

protected:
	FolderNodeItem *rootItem() const { return m_items.at(0).item; }
	bool isUpdating() const { return m_updating; }
	void setUpdating(bool value) { m_updating = value; }

protected:
	void list(FolderNodeItem *fileSystemTree);
	void listEvent(const ModelEvent::Params *p);

	void updateFiles();
	void updateFilesEvent(const ModelEvent::Params *p);

	void removeEntry(FolderNodeItem *entry);
	void scanForRemove(FolderNodeItem *entry);
	void scanForRemoveEvent(const ModelEvent::Params *p);
	void removeCompleteEvent(const ModelEvent::Params *p);
	void removeCanceledEvent(const ModelEvent::Params *p);

	void scanForSize(FolderNodeItem *entry);
	void scanForSizeEvent(const ModelEvent::Params *p);

	void copyEntry(FolderNodeItem *entry, Node *destination);
	void scanForCopy(FolderNodeItem *entry, Node *destination);
	void moveEntry(FolderNodeItem *entry, Node *destination);
	void scanForMove(FolderNodeItem *entry, Node *destination);
	void scanForCopyEvent(const ModelEvent::Params *p);
	void scanForMoveEvent(const ModelEvent::Params *p);
	void copyCompleteEvent(const ModelEvent::Params *p);
	void copyCanceledEvent(const ModelEvent::Params *p);

	void questionAnswerEvent(const ModelEvent::Params *p);
	void updateProgressEvent(const ModelEvent::Params *p);

protected:
	class Values
	{
	public:
		struct Value
		{
			Value() :
				node(0),
				item(0)
			{}
			Value(FolderNodeItem *item) :
				node(0),
				item(item)
			{
				Q_ASSERT(item);
			}
			Value(FolderNodeItem *item, Node *node) :
				node(node),
				item(item)
			{
				Q_ASSERT(item);
			}

			bool operator==(const Value &other)
			{
				return item == other.item;
			}

			Node *node;
			FolderNodeItem *item;
		};
		typedef QList<Value> 			 ValueList;
		typedef ValueList::size_type     size_type;
		typedef QMap<QString, size_type> ValueMap;
		enum { InvalidIndex = (size_type)-1 };

	public:
		Values()
		{}
		~Values()
		{
			for (ValueList::size_type i = 0, size = m_list.size(); i < size; ++i)
			{
				delete m_list.at(i).node;
				delete m_list.at(i).item;
			}
		}

		Value &operator[](size_type index) { return m_list[index]; }
		const Value &operator[](size_type index) const { return m_list[index]; }
		const Value &at(size_type index) const { return m_list.at(index); }
		const Value &last() const { return m_list.last(); }

		size_type size() const { return m_list.size(); }
		size_type indexOf(FolderNodeItem *item) const { return m_list.indexOf(item); }
		size_type indexOf(const QString &fileName) const { return m_map.value(fileName, InvalidIndex); }

		void add(const Value &value)
		{
			m_list.push_back(value);
			m_map[value.item->fileName()] = m_list.size() - 1;
		}
		void add(FolderNodeItem *item)
		{
			m_list.push_back(item);
			m_map[item->fileName()] = m_list.size() - 1;
		}
		void add(FolderNodeItem *item, Node *node)
		{
			m_list.push_back(Value(item, node));
			m_map[item->fileName()] = m_list.size() - 1;
		}
		void remove(size_type index)
		{
			Q_ASSERT(index != InvalidIndex);
			const Value &value = m_list.at(index);
			m_map.remove(value.item->fileName());
			delete value.item;
			delete value.node;
			m_list.removeAt(index);
		}

	private:
		ValueList m_list;
		ValueMap m_map;
	};

protected:
	Values &items() { return m_items; }

private:
	ChangesList makeChangeSet() const;
	QModelIndex index(int column, FolderNodeItem *item) const;
	Node *createNode(const Info &info, PluginsManager *plugins) const;
	Values::Value createNode(const QString &fileName, PluginsManager *plugins, Node *&node) const;
	Info fileInfo(const QString &fileName) const;

	void updateFirstColumn(FolderNodeItem *entry);
	void updateSecondColumn(FolderNodeItem *entry);
	void updateBothColumns(FolderNodeItem *entry);
	void removeEntry(Values::size_type index);
	void removeEntry(const QModelIndex &index);
	void refresh(FolderNodeItem *fileSystemTree);
	void doRefresh();

private:
	bool m_updating;
	Values m_items;
	FolderProxyModel m_proxy;
	FolderDelegate m_delegate;
	QModelIndex m_parentEntryIndex;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMFOLDERNODE_H_ */
