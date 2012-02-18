#include "arcunpackhereaction.h"
#include "../../../application.h"


ARC_PLUGIN_NS_BEGIN

UnPackHereAction::UnPackHereAction() :
	Action(tr("Unpacking..."), Application::desktopService()->unpackActionIcon(16), tr("Unpack here"))
{}

bool UnPackHereAction::prepare(const FilesList &files)
{
	return true;
}

void UnPackHereAction::process(PerformActionTask *task, const FilesList &files, const volatile Flags &aborted) const
{

}

ARC_PLUGIN_NS_END
