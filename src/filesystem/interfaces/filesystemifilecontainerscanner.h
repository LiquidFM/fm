#ifndef FILESYSTEMIFILECONTAINERSCANNER_H_
#define FILESYSTEMIFILECONTAINERSCANNER_H_

#include "../tasks/filesystembasetask.h"
#include "../model/items/filesystemnodeitem.h"
#include "../containers/filesystemsnapshot.h"
#include "../../tools/pointers/pscopedpointer.h"


FILE_SYSTEM_NS_BEGIN

class IFileContainerScanner
{
public:
	class IEnumerator
	{
	public:
		typedef PScopedPointer<IEnumerator> Holder;

	public:
		virtual ~Enumerator();

		virtual bool next() = 0;
		virtual QString fileName() const = 0;
		virtual NodeItem *create() const = 0;
		virtual bool isObsolete(const NodeItem *item) const = 0;
	};

public:
	virtual ~IFileContainerScanner();

	virtual void enumerate(IEnumerator::Holder &enumerator) const = 0;
	virtual void scan(Snapshot &snapshot, const volatile BaseTask::Flags &aborted) const = 0;
	virtual void refresh(Snapshot &snapshot, const volatile BaseTask::Flags &aborted) const = 0;
};

FILE_SYSTEM_NS_END

#endif /* FILESYSTEMIFILECONTAINERSCANNER_H_ */
