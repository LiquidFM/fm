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
#ifndef IDM_QUERYRESULTPROPERTYITEM_H_
#define IDM_QUERYRESULTPROPERTYITEM_H_

#include <liquiddb/EntityValue>
#include "idm_queryresultitem.h"


IDM_PLUGIN_NS_BEGIN
using namespace LiquidDb;


class QueryResultPropertyItem : public QueryResultItem
{
public:
    typedef QList<QueryResultItem::Holder> Container;

public:
    QueryResultPropertyItem(const Entity::Property &property, Base *parent);

    /* Base */
    virtual Base *at(size_type index) const;
    virtual size_type size() const;
    virtual size_type indexOf(Base *item) const;
    virtual QVariant data(qint32 column, qint32 role) const;

    /* QueryResultItem */
    virtual bool isRoot();
    virtual bool isProperty();
    virtual bool isValue();
    virtual bool isPath();
    virtual bool isRootPathValue();

    const Entity::Property &property() const { return m_property; }

    void add(const EntityValue &value);
    void add(const EntityValue::Values &values);
    void remove(size_type index);

protected:
    Entity::Property m_property;
    Container m_items;
};

IDM_PLUGIN_NS_END

#endif /* IDM_QUERYRESULTPROPERTYITEM_H_ */
