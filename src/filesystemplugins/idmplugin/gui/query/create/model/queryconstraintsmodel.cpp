#include "queryconstraintsmodel.h"
#include "../../../../storage/constraints/value/idmconstraint.h"


IDM_PLUGIN_NS_BEGIN

QueryConstraintsModel::QueryConstraintsModel(QObject *parent) :
	QAbstractItemModel(parent),
	m_andTypeLabel(tr("(...AND...)")),
	m_orTypeLabel(tr("(...OR...)"))
{}

int QueryConstraintsModel::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		if (static_cast<BaseConstraint*>(parent.internalPointer())->isGroup())
			return static_cast<GroupConstraint*>(parent.internalPointer())->size();
		else
			return 0;
	else
    	return 1;
}

int QueryConstraintsModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant QueryConstraintsModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    	return data(static_cast<BaseConstraint*>(index.internalPointer()), index.column(), role);
    else
    	return data(const_cast<QueryConstraintsRootItem*>(&m_root), index.column(), role);
}

Qt::ItemFlags QueryConstraintsModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

QVariant QueryConstraintsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal)
		switch (section)
		{
			case 0:
				return tr("Constraint");

			case 1:
			{
				switch (role)
				{
					case Qt::DisplayRole:
						return tr("Type");

					case Qt::TextAlignmentRole:
						return Qt::AlignCenter;
				}

				break;
			}
		}

	return QVariant();
}

QModelIndex QueryConstraintsModel::index(int row, int column, const QModelIndex &parent) const
{
	if (hasIndex(row, column, parent))
		if (parent.isValid())
			if (static_cast<BaseConstraint*>(parent.internalPointer())->isGroup())
				return createIndex(row, column, static_cast<GroupConstraint*>(parent.internalPointer())->at(row));
		    else
				return createIndex(row, column, m_root.at(row));
		else
			return createIndex(row, column, const_cast<QueryConstraintsRootItem*>(&m_root));
    else
        return QModelIndex();
}

QModelIndex QueryConstraintsModel::parent(const QModelIndex &child) const
{
    if (child.isValid())
		if (BaseConstraint *parent = static_cast<BaseConstraint*>(child.internalPointer())->parent())
			if (parent->parent())
				return createIndex(static_cast<GroupConstraint*>(parent->parent())->indexOf(parent), 0, parent);
			else
				return createIndex(0, 0, const_cast<QueryConstraintsRootItem*>(&m_root));

    return QModelIndex();
}

void QueryConstraintsModel::add(const QModelIndex &index)
{
	if (!index.isValid())
	{
		beginInsertRows(QModelIndex(), m_root.size(), m_root.size());
		m_root.add(new GroupConstraint(GroupConstraint::And, &m_root));
		endInsertRows();
	}
	else
		if (static_cast<BaseConstraint*>(index.internalPointer())->isGroup())
		{
			beginInsertRows(index, static_cast<GroupConstraint*>(index.internalPointer())->size(), static_cast<GroupConstraint*>(index.internalPointer())->size());
			static_cast<GroupConstraint*>(index.internalPointer())->add(new GroupConstraint(GroupConstraint::And, static_cast<GroupConstraint*>(index.internalPointer())));
			endInsertRows();
		}
}

void QueryConstraintsModel::add(Constraint *constraint, const QModelIndex &index)
{
	if (index.isValid() && constraint)
	{
		beginInsertRows(index, static_cast<GroupConstraint*>(index.internalPointer())->size(), static_cast<GroupConstraint*>(index.internalPointer())->size());
		static_cast<GroupConstraint*>(index.internalPointer())->add(constraint);
		endInsertRows();
	}
}

void QueryConstraintsModel::remove(const QModelIndex &index)
{
//	beginRemoveRows(QModelIndex(), index.row(), index.row());
//	delete m_items.at(index.row());
//	m_items.remove(index.row());
//	endRemoveRows();
}

GroupConstraint *QueryConstraintsModel::take()
{
	GroupConstraint *res = new GroupConstraint(m_root);
	m_root.clear();
	return res;
}

QVariant QueryConstraintsModel::data(BaseConstraint *item, int column, int role) const
{
	switch (column)
	{
		case 0:
		{
			if (role == Qt::DisplayRole)
				if (item->isGroup())
					switch (static_cast<GroupConstraint*>(item)->type())
					{
						case GroupConstraint::And:
							return m_andTypeLabel;

						case GroupConstraint::Or:
							return m_orTypeLabel;
					}
				else
					return QString::fromLatin1("%1 %2 \"%3\"").
							arg(static_cast<Constraint*>(item)->property().name).
							arg(Constraint::operatorToString(static_cast<Constraint*>(item)->op())).
							arg(static_cast<Constraint*>(item)->value()->value().toString());

			break;
		}

		case 1:
		{
			if (item->isGroup())
				switch (role)
				{
					case Qt::DisplayRole:
						return GroupConstraint::typeToString(static_cast<GroupConstraint*>(item)->type());

					case Qt::TextAlignmentRole:
						return Qt::AlignCenter;
				}

			break;
		}

		default:
			break;
	}

	return QVariant();
}

IDM_PLUGIN_NS_END