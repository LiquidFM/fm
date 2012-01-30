#include "filesystemfolderpasteintofolderaction.h"


FILE_SYSTEM_NS_BEGIN

FolderPasteIntoFolderAction::FolderPasteIntoFolderAction() :
	m_action(tr("Paste into folder"), 0)
{}

const QAction *FolderPasteIntoFolderAction::action() const
{
	return &m_action;
}

void FolderPasteIntoFolderAction::process(const FilesList &files) const
{

}

FILE_SYSTEM_NS_END
