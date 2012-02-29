/***************************************************************************
 *   Copyright (C) 2011 by Peter Penz <peter.penz19@gmail.com>             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA            *
 ***************************************************************************/

#ifndef KFILEITEMLISTGROUPHEADER_H
#define KFILEITEMLISTGROUPHEADER_H

#include "kitemlistgroupheader.h"
#include <QtGui/QStaticText>


class KFileItemListGroupHeader : public KItemListGroupHeader
{
    Q_OBJECT

public:
    KFileItemListGroupHeader(QGraphicsWidget* parent = 0);
    virtual ~KFileItemListGroupHeader();

    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

protected:
    virtual void dataChanged(const QVariant& current, const QVariant& previous);
    virtual void resizeEvent(QGraphicsSceneResizeEvent* event);

private:
    void updateText();

private:
    QFont m_font;
    QStaticText m_text;
};

#endif
