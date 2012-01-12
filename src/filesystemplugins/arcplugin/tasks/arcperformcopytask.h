#ifndef ARCPERFORMCOPYTASK_H_
#define ARCPERFORMCOPYTASK_H_

#include "arctaskevent.h"
#include "../nodes/items/arcnodeitem.h"
#include "../../../tools/pointers/pscopedpointer.h"
#include "../../../filesystem/tasks/interfaces/filesystemicopycontrol.h"


ARC_PLUGIN_NS_BEGIN

class PerformCopyTask : public BaseTask
{
public:
	class Event : public TaskEvent
	{
	public:
		Event(BaseTask *task, bool move) :
			TaskEvent(CopyComplete),
			task(task),
			control(control.take()),
			canceled(canceled),
			move(move)
		{}

		BaseTask *task;
		const ArcNodeItem::Base *item;
		PScopedPointer<ICopyControl> control;
		QString error;
		bool canceled;
		bool move;
	};

public:
	PerformCopyTask(const QString &fileName, const ArcNodeItem::Base *item, PScopedPointer<ICopyControl> &control, bool move, TasksNode *receiver);

protected:
	virtual void run(const volatile Flags &aborted);

private:
	QString m_fileName;
	const ArcNodeItem::Base *m_item;
	PScopedPointer<ICopyControl> m_control;
	bool m_move;
};

ARC_PLUGIN_NS_END

#endif /* ARCPERFORMCOPYTASK_H_ */