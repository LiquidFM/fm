#include "idmroot.h"


FILE_SYSTEM_NS_BEGIN

IdmRoot::IdmRoot(const Info &info, IdmItem *parent) :
	IdmInfoItem(info, parent),
	m_label(QString::fromLatin1(".."))
{}

QVariant IdmRoot::data(qint32 column, qint32 role) const
{
	if (column == 0 && role == Qt::DisplayRole)
		return m_label;
	else
		return QVariant();
}

bool IdmRoot::isRoot() const
{
	return true;
}

FILE_SYSTEM_NS_END
