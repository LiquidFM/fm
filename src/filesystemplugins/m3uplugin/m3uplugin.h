#ifndef M3UPLUGIN_H_
#define M3UPLUGIN_H_

#include "../../filesystem/filesystempluginsmanager.h"


FILE_SYSTEM_NS_BEGIN

class M3uPlugin : public PluginsManager::Plugin
{
public:
	M3uPlugin();

	virtual Node *node(const IFileInfo *info, Node *parent) const;
	virtual Node *node(const IFileInfo *info, IFile *file, Node *parent) const;

private:
	QString m_identity;
};

FILE_SYSTEM_NS_END

#endif /* M3UPLUGIN_H_ */
