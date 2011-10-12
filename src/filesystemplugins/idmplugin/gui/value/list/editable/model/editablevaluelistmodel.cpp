#include "editablevaluelistmodel.h"


IDM_PLUGIN_NS_BEGIN

EditableValueListModel::EditableValueListModel(const IdmContainer &container, const Select &query, QObject *parent) :
	ValueListModel(container, query, parent)
{}

void EditableValueListModel::add(const List &list)
{
	ValueListModel::add(list);
}

void EditableValueListModel::add(IdmEntityValue *value)
{
	beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
	m_items.push_back(value);
	endInsertRows();
}

void EditableValueListModel::add(Database::id_type id, const QVariant &value)
{
	beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
	m_items.push_back(IdmValueReader::createValue(m_reader.entity(), id, value));
	endInsertRows();
}

void EditableValueListModel::remove(const QModelIndex &index)
{
	ValueListModel::remove(index);
}

IDM_PLUGIN_NS_END