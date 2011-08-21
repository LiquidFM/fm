#ifndef IDMENTITYITEM_H_
#define IDMENTITYITEM_H_

#include <QtCore/QVariant>
#include "idmitemslist.h"
#include "../storage/entities/idmentity.h"


FILE_SYSTEM_NS_BEGIN

class IdmEntityItem : public IdmItemsList
{
public:
	IdmEntityItem(IdmEntity *entity, IdmItem *parent = 0);

	/* IdmItem */
	virtual QVariant data(qint32 column, qint32 role) const;
	virtual bool isEntityItem() const;

	IdmEntity *entity() const { return m_entity; }
	void add(IdmEntityItem *item) { m_items.push_back(item); }
	void remove(size_type index) { m_items.removeAt(index); }
	void remove(IdmEntityItem *item) { m_items.removeAt(m_items.indexOf(item)); }

protected:
	IdmEntity *m_entity;
};

FILE_SYSTEM_NS_END

#endif /* IDMENTITYITEM_H_ */
