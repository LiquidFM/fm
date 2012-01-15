#include "filesystemtasknodeitemlist.h"
#include "../../../application.h"


FILE_SYSTEM_NS_BEGIN

TaskNodeListItem::TaskNodeListItem(Base *parent) :
	FileSystemListItem(parent),
	m_total(0),
	m_timeElapsed(0)
{}

void TaskNodeListItem::lock(const QString &reason)
{
	MODEL_ITEM_LOCKER_LOCK(reason, Application::desktopEnvironment()->processingIcon(16));
}

FILE_SYSTEM_NS_END
