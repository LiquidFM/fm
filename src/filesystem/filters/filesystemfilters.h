#ifndef FILESYSTEMFILTERS_H_
#define FILESYSTEMFILTERS_H_

#include <QtCore/QList>
#include <QtCore/QRegExp>
#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "../interfaces/filesystemifilecontainer.h"


FILE_SYSTEM_NS_BEGIN

class Filters : public IFileContainer::Filter
{
public:
	Filters(int size = 0)
	{
		m_filters.reserve(size);
	}

	virtual ~Filters();
	virtual bool match(const IFileInfo *info) const;

	void add(Filter *filter)
	{
		Holder holder(filter);
		m_filters.push_back(holder.data());
		holder.release();
	}

private:
	typedef QList<Filter *> Container;

private:
	Container m_filters;
};


class FileNameFilter : public IFileContainer::Filter
{
public:
	FileNameFilter(const QString &pattern) :
		m_pattern(pattern)
	{
		m_pattern.setPatternSyntax(QRegExp::Wildcard);
	}

	virtual bool match(const IFileInfo *info) const;

private:
	QRegExp m_pattern;
};


class DateFilterRange : public IFileContainer::Filter
{
public:
	DateFilterRange(const QDateTime &from, const QDateTime &to) :
		m_from(from),
		m_to(to)
	{}

protected:
	bool match(const QDateTime &dateTime) const
	{
		return dateTime >= m_from && dateTime <= m_to;
	}

private:
	QDateTime m_from;
	QDateTime m_to;
};


class ModificationTimeFilterRange : public DateFilterRange
{
public:
	ModificationTimeFilterRange(const QDateTime &from, const QDateTime &to) :
		DateFilterRange(from, to)
	{}

	virtual bool match(const IFileInfo *info) const;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMFILTERS_H_ */
