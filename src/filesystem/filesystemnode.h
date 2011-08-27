#ifndef FILESYSTEMNODE_H_
#define FILESYSTEMNODE_H_

#include <QtCore/QSet>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QAbstractItemDelegate>
#include "filesystem_ns.h"
#include "interfaces/filesysteminode.h"


FILE_SYSTEM_NS_BEGIN

class Node : public QAbstractItemModel, public INode
{
	Q_DISABLE_COPY(Node)

public:
	Node(Node *parent = 0);

	/* INode */
	virtual INode *root() const;
	virtual int columnsCount() const;

	/* INode::IFileNavigation */
	virtual void viewClosed(INodeView *nodeView);
	virtual void viewParent(INodeView *nodeView);
	virtual void viewThis(INodeView *nodeView, const QModelIndex &selected);
	virtual void viewChild(INodeView *nodeView, const QModelIndex &idx, PluginsManager *plugins);
	virtual void viewChild(INodeView *nodeView, const Path::Iterator &path, PluginsManager *plugins);
	virtual void viewAbsolute(INodeView *nodeView, const QString &absoluteFilePath, PluginsManager *plugins);

	void setParentEntryIndex(const QModelIndex &value) { m_parentEntryIndex = value; }
	virtual void switchTo(Node *node, const QModelIndex &selected);

protected:
	virtual QModelIndex rootIndex() const = 0;
	virtual QAbstractItemModel *proxyModel() const = 0;
	virtual QAbstractItemDelegate *itemDelegate() const = 0;

	virtual Node *viewChild(const QModelIndex &idx, PluginsManager *plugins, QModelIndex &selected) = 0;
	virtual Node *viewChild(const QString &fileName, PluginsManager *plugins, QModelIndex &selected) = 0;

protected:
	QStringList toFileNameList(const FileInfoList &files) const;
	bool isVisible() const { return !m_view.isEmpty(); }
	void removeThis();

private:
	void addView(INodeView *view) { m_view.insert(view); }
	void removeView(INodeView *view) { m_view.remove(view); }
	void switchTo(Node *node, INodeView *nodeView, const QModelIndex &selected);

private:
	typedef QSet<INodeView*> ViewSet;

private:
	ViewSet m_view;
	QModelIndex m_parentEntryIndex;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMNODE_H_ */
