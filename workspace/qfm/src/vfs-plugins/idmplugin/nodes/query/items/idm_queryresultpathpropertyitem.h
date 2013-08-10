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
#ifndef IDM_QUERYRESULTPATHPROPERTYITEM_H_
#define IDM_QUERYRESULTPATHPROPERTYITEM_H_

#include <vfs/tasks/vfs_tasksnode.h>
#include <vfs/interfaces/vfs_ifilecontainer.h>
#include "idm_queryresultpropertyitem.h"


IDM_PLUGIN_NS_BEGIN

class QueryResultPathPropertyItem : public QueryResultPropertyItem
{
public:
	QueryResultPathPropertyItem(const Entity::Property &property, Base *parent);

	void add(const IFileContainer *container, const EntityValue::Holder &value);
	void add(const IFileContainer *container, const CompositeEntityValue::List &values);
	void add(Snapshot::Files &files, const IFileContainer *container, const CompositeEntityValue::List &values);
};

IDM_PLUGIN_NS_END

#endif /* IDM_QUERYRESULTPATHPROPERTYITEM_H_ */