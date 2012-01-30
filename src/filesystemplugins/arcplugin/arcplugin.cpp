#include "arcplugin.h"
#include "nodes/arcnode.h"
#include "../../application.h"


ARC_PLUGIN_NS_BEGIN
static Plugin *instance = 0;


Plugin::Plugin()
{
	m_archivers[0] = &m_libArchive;
	m_archivers[1] = 0;
	instance = this;
}

void Plugin::registered()
{

}

Node *Plugin::node(const IFileInfo *info, Node *parent) const
{
	return new ArcNode(Info(info->absoluteFilePath(), true), parent);
}

Plugin::FileTypeIdList Plugin::fileTypes() const
{
	using namespace ::DesktopEnvironment;
	return FileTypeIdList() <<
			fileTypeId(FileTypes::Application::GZipFile) <<
			fileTypeId(FileTypes::Application::TarFile) <<
			fileTypeId(FileTypes::Application::CompressedTarFile) <<
			fileTypeId(FileTypes::Application::BZipCompressedTarFile) <<
			fileTypeId(FileTypes::Application::ZipFile) <<
			fileTypeId(FileTypes::Application::BZipFile) <<
			fileTypeId(FileTypes::Application::RarFile) <<
			fileTypeId(FileTypes::Application::TarzFile) <<
			fileTypeId(FileTypes::Application::BZip2File);
}

const Archive **Plugin::archivers()
{
	return instance->m_archivers;
}

ARC_PLUGIN_NS_END
