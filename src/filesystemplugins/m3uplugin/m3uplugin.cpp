#include "m3uplugin.h"
#include "m3unode.h"
#include <QtCore/QTextCodec>
#include <QDebug>


M3uPlugin::M3uPlugin() :
	m_identity(QString::fromLatin1("#EXTM3U"))
{}

FileSystem::Node *M3uPlugin::node(const FileSystem::IFileInfo *info, FileSystem::IFile *file, FileSystem::Node *parent) const
{
	if (QTextCodec *codec = QTextCodec::codecForName("UTF-8"))
		if (file->seek(0))
		{
			uchar data[64] = {};

			for (FileSystem::IFile::size_type i = 0, size = file->read(data, 64); size > m_identity.size(); ++i)
				if (data[i] == '\n' || data[i] == '\r')
				{
					data[i] == 0;

					if (m_identity == codec->toUnicode((const char*)data, i))
						return new M3uNode(info->absoluteFilePath(), parent);
					else
						break;
				}
		}

	return 0;
}
