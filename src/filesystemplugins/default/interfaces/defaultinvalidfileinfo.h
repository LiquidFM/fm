#ifndef DEFAULTINVALIDINFO_H_
#define DEFAULTINVALIDINFO_H_

#include "../default_ns.h"
#include "../../../filesystem/interfaces/filesystemifileinfo.h"


DEFAULT_PLUGIN_NS_BEGIN

class InvalidInfo : public IFileInfo, public IFileType
{
public:
	InvalidInfo();

	/* IFileType */
	virtual FileTypeId id() const;
	virtual QIcon icon() const;
	virtual QString name() const;
	virtual QString description() const;

	/* IFileInfo */
	virtual bool isDir() const;
	virtual bool isFile() const;
	virtual bool isLink() const;
	virtual size_type fileSize() const;
	virtual QString fileName() const;
	virtual QDateTime lastModified() const;
	virtual int permissions() const;
};

DEFAULT_PLUGIN_NS_END

#endif /* DEFAULTINVALIDINFO_H_ */