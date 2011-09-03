#ifndef ENTITIESLISTMODEL_H_
#define ENTITIESLISTMODEL_H_

#include <QtCore/QAbstractItemModel>
#include "../../../items/idmitem.h"
#include "../../../storage/entities/idmentity.h"


IDM_PLUGIN_NS_BEGIN

class EntitiesListModel : public QAbstractItemModel
{
public:
	typedef QList<IdmItem*>       value_type;
	typedef value_type::size_type size_type;

public:
	EntitiesListModel(QObject *parent = 0);
	virtual ~EntitiesListModel();

    /* QAbstractItemModel */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual int columnCount(const QModelIndex &parent) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	virtual QModelIndex parent(const QModelIndex &child) const;

	IdmEntity *entityAt(size_type index) const;
	const QString &nameAt(size_type index) const;
	size_type size() const { return m_items.size(); }

	void add(IdmEntity *entity, const QString &name);
	void remove(const QModelIndex &index);

private:
	value_type m_items;
};

IDM_PLUGIN_NS_END

#endif /* ENTITIESLISTMODEL_H_ */
