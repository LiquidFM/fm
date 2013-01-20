/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2013 Dmitriy Vilkov, <dav.daemon@gmail.com>
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
#include "default_tmpnodeitem.h"


DEFAULT_PLUGIN_NS_BEGIN

TmpNodeItem::TmpNodeItem(IFileInfo::Holder &info, Base *parent) :
    NodeItem(info, parent)
{}

TmpNodeItem::TmpNodeItem(IFileInfo::Holder &info, Node *node, Base *parent) :
    NodeItem(info, node, parent)
{}

bool TmpNodeItem::isTmpItem() const
{
    return true;
}

DEFAULT_PLUGIN_NS_END
