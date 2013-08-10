/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2012 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */
#include "idm_queryresultcompositerootitem.h"
#include "idm_queryresultpropertyitem.h"
#include "idm_queryresultpathpropertyitem.h"


IDM_PLUGIN_NS_BEGIN

QueryResultCompositeRootItem::QueryResultCompositeRootItem(Snapshot::Files &files, const IFileContainer *container, const EntityValue::Holder &value, Base *parent) :
	QueryResultRootItem(value, parent),
	m_items(value->entity()->size())
{
	QueryResultPropertyItem::Holder item;

	for (Entity::size_type i = 0, size = value->entity()->size(); i < size; ++i)
	{
		const Entity::Property &poperty = value->entity()->at(i);

		if (poperty.entity->type() == Database::Path)
		{
			item = new QueryResultPathPropertyItem(poperty, this);
			item.as<QueryResultPathPropertyItem>()->add(files, container, value.as<CompositeEntityValue>()->values(poperty.entity));
		}
		else
		{
			item = new QueryResultPropertyItem(poperty, this);
			item.as<QueryResultPropertyItem>()->add(value.as<CompositeEntityValue>()->values(poperty.entity));
		}

		m_items[i] = item;
	}
}

QueryResultCompositeRootItem::Base *QueryResultCompositeRootItem::at(size_type index) const
{
	return m_items.at(index).data();
}

QueryResultCompositeRootItem::size_type QueryResultCompositeRootItem::size() const
{
	return m_items.size();
}

QueryResultCompositeRootItem::size_type QueryResultCompositeRootItem::indexOf(Base *item) const
{
	Holder holder(static_cast<QueryResultItem *>(item));
	return m_items.indexOf(holder);
}

IDM_PLUGIN_NS_END