#include "idmitemslist.h"


FILE_SYSTEM_NS_BEGIN

IdmItemsList::IdmItemsList(IdmItem *parent) :
	IdmItem(parent)
{}

IdmItemsList::~IdmItemsList()
{
	qDeleteAll(m_items);
}

bool IdmItemsList::isRoot() const
{
	return false;
}

bool IdmItemsList::isList() const
{
	return true;
}

bool IdmItemsList::isMenuItem() const
{
	return false;
}

bool IdmItemsList::isEntityItem() const
{
	return false;
}

FILE_SYSTEM_NS_END
