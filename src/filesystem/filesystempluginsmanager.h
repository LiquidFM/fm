#ifndef FILESYSTEMPLUGINSMANAGER_H_
#define FILESYSTEMPLUGINSMANAGER_H_

#include "filesystem_ns.h"
#include "filesystemnode.h"
#include "info/filesysteminfo.h"


FILE_SYSTEM_NS_BEGIN

class PluginsManager
{
	Q_DISABLE_COPY(PluginsManager)

public:
	class Plugin
	{
	public:
		virtual ~Plugin() {}
	};
	class FilePlugin : public Plugin
	{
	public:
		typedef const uchar * File;
		typedef qint64        size_type;

	public:
		virtual Node *node(const Info &info, File file, size_type size, Node *parent) const = 0;
	};
	class FolderPlugin : public Plugin
	{
	public:
		virtual Node *node(const Info &info, Node *parent) const = 0;
	};


public:
	PluginsManager();
	virtual ~PluginsManager();

	Node *node(const Info &info, Node *parent) const;

	void registerStatic(FilePlugin *plugin) { m_staticFilePlugins.push_back(plugin); }
	void registerDynamic(FilePlugin *plugin) { m_dynamicFilePlugins.push_back(plugin); }
	void registerStatic(const QString &filePath, FolderPlugin *plugin) { m_staticFoldersPlugins[filePath] = plugin; }
	void registerDynamic(const QString &filePath, FolderPlugin *plugin) { m_dynamicFoldersPlugins[filePath] = plugin; }

private:
	typedef QList<FilePlugin*>           PluginsList;
	typedef QMap<QString, FolderPlugin*> PluginsMap;

private:
	PluginsList m_staticFilePlugins;
	PluginsList m_dynamicFilePlugins;
	PluginsMap m_staticFoldersPlugins;
	PluginsMap m_dynamicFoldersPlugins;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMPLUGINSMANAGER_H_ */
