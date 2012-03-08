#include "filesysteminfolistitem.h"


FILE_SYSTEM_NS_BEGIN

InfoListItem::InfoListItem(const QString &filePath) :
	InfoItem(filePath),
	m_container(*this),
	m_totalSize(0)
{}

InfoListItem::~InfoListItem()
{
	qDeleteAll(m_items);
}

Info::size_type InfoListItem::totalSize() const
{
	return m_totalSize;
}

FILE_SYSTEM_NS_END
