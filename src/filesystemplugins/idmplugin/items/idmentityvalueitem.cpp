#include "idmentityvalueitem.h"


IDM_PLUGIN_NS_BEGIN

IdmEntityValueItem::IdmEntityValueItem(IdmEntity *entity, const QVariant &value, IdmItem *parent) :
	IdmEntityItem(entity, parent),
	m_value(value)
{}

QVariant IdmEntityValueItem::data(qint32 column, qint32 role) const
{
	if (role == Qt::DisplayRole)
		switch (column)
		{
			case 0: return m_value;
		}

	return QVariant();
}

IDM_PLUGIN_NS_END