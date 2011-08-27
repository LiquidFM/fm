#ifndef FILESYSTEMINODEVIEW_H_
#define FILESYSTEMINODEVIEW_H_

#include <QtCore/QList>
#include <QtCore/QAbstractItemModel>
#include <QtGui/QAbstractItemDelegate>
#include <QtGui/QAbstractItemView>
#include <QtGui/QAction>
#include "../filesystem_ns.h"


FILE_SYSTEM_NS_BEGIN

class INode;

class INodeView
{
public:
	typedef QList<QAction*> MenuActionList;

public:
	virtual ~INodeView() {}

	virtual void select(const QModelIndex &index) = 0;
	virtual void setNode(INode *node, QAbstractItemModel *model, QAbstractItemDelegate *delegate = 0, const MenuActionList &menuActions = MenuActionList()) = 0;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMINODEVIEW_H_ */
