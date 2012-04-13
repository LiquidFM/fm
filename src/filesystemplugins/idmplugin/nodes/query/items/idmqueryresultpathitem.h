#ifndef IDMQUERYRESULTPATHITEM_H_
#define IDMQUERYRESULTPATHITEM_H_

#include "idmqueryresultitem.h"
#include "../../../../../filesystem/filesystemnode.h"
#include "../../../../../filesystem/tasks/concrete/containers/filesysteminfoitem.h"


IDM_PLUGIN_NS_BEGIN

class QueryResultPathItem : public QueryResultItem, public IFileInfo
{
public:
	typedef QList<QueryResultItem *> Container;

public:
	QueryResultPathItem(const InfoItem *item, Base *parent);
	QueryResultPathItem(const IFileContainer *container, const QString &fileName, Base *parent);
	virtual ~QueryResultPathItem();

	/* Base */
	virtual Base *at(Base::size_type index) const;
	virtual Base::size_type size() const;
	virtual Base::size_type indexOf(Base *item) const;
	virtual QVariant data(qint32 column, qint32 role) const;

	/* QueryResultItem */
	virtual bool isRoot();
	virtual bool isProperty();
	virtual bool isValue();
	virtual bool isPath();

	/* IFileType */
	virtual FileTypeId id() const;
	virtual QIcon icon() const;
	virtual QString name() const;
	virtual QString description() const;

	/* IFileInfo */
	virtual bool isDir() const;
	virtual bool isFile() const;
	virtual bool isLink() const;
	virtual bool exists() const;
	virtual IFileInfo::size_type fileSize() const;
	virtual QString fileName() const;
	virtual QString absolutePath() const;
	virtual QString absoluteFilePath() const;
	virtual QString absoluteFilePath(const QString &fileName) const;
	virtual QDateTime lastModified() const;
	virtual int permissions() const;
	virtual void refresh();

	const Info &info() const { return m_info; }
	Info &info() { return m_info; }

protected:
	Info m_info;
	Container m_items;
};

IDM_PLUGIN_NS_END

#endif /* IDMQUERYRESULTPATHITEM_H_ */
