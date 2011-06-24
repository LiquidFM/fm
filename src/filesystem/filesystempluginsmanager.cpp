#include "filesystempluginsmanager.h"
#include <QtCore/QStringList>
#include <QtCore/QFile>


FILE_SYSTEM_NS_BEGIN

PluginsManager::PluginsManager()
{

}

PluginsManager::~PluginsManager()
{
	qDeleteAll(m_dynamicFilePlugins);
	qDeleteAll(m_dynamicFoldersPlugins);
}

Node *PluginsManager::node(const IFileControl *control, Node *parent) const
{
	Node *res = 0;

	if (control->isFile())
	{
		QString error;
		QScopedPointer<IFile> file(control->open(IFile::ReadOnly, error));

		if (file)
		{
			for (PluginsList::size_type i = 0, size = m_staticFilePlugins.size(); i < size; ++i)
				if (res = m_staticFilePlugins.at(i)->node(control, file.data(), parent))
					break;

			if (res == 0)
				for (PluginsList::size_type i = 0, size = m_dynamicFilePlugins.size(); i < size; ++i)
					if (res = m_dynamicFilePlugins.at(i)->node(control, file.data(), parent))
						break;
		}
	}
	else
		if (control->isDir())
			if (FolderPlugin *plugin = m_staticFoldersPlugins.value(control->absoluteFilePath(), 0))
				res = plugin->node(control, parent);

	return res;
}

FILE_SYSTEM_NS_END
