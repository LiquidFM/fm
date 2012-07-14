#include "m3uplugin.h"
#include "m3unode.h"

#include <QtCore/QTextCodec>


M3U_PLUGIN_NS_BEGIN

Plugin::Plugin(::Tools::Settings::Option *parentOption) :
	m_settings(parentOption)
{}

void Plugin::registered()
{

}

::Tools::Settings::Page *Plugin::settings()
{
	return &m_settings;
}

Plugin::FileTypeIdList Plugin::fileTypes() const
{
	FileTypeId type;
	type.mime = QString::fromLatin1("audio/x-mpegurl");

	return FileTypeIdList() << type;
}

Node *Plugin::open(const IFileContainer *container, const IFileInfo *file, Node *parent) const
{
	return NULL;
}

//Node *M3uPlugin::node(const IFileInfo *info, IFile *file, Node *parent) const
//{
//	if (QTextCodec *codec = QTextCodec::codecForName("UTF-8"))
//	{
//		uchar data[64] = {};
//
//		for (IFile::size_type i = 0, size = file->read(data, 64); size > m_identity.size(); ++i)
//			if (data[i] == '\n' || data[i] == '\r')
//			{
//				data[i] = 0;
//
//				if (m_identity == codec->toUnicode((const char*)data, i))
//					return new M3uNode(info->absoluteFilePath(), parent);
//				else
//					break;
//			}
//	}
//
//	return 0;
//}

M3U_PLUGIN_NS_END