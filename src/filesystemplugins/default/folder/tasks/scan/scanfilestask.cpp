#include "scanfilestask.h"
#include "../../items/filesystemfolderbaseitem.h"


FILE_SYSTEM_NS_BEGIN

ScanFilesTask::ScanFilesTask(TasksNode *receiver, const IFileContainer *container, const TasksNode::TasksItemList &files) :
	ScanFilesBaseTask(receiver),
	m_snapshot(createSnapshot(container, files.size())),
	m_files(files)
{}

Snapshot ScanFilesTask::scan(const volatile Flags &aborted)
{
	for (TasksNode::TasksItemList::size_type i = 0, size = m_files.size(); i < size && !aborted; ++i)
		ScanFilesBaseTask::scan(m_snapshot, m_files.at(i), static_cast<FolderBaseItem *>(m_files.at(i))->info().fileName(), aborted);

	return m_snapshot;
}


ScanFilesExtendedTask::ScanFilesExtendedTask(TasksNode *receiver, ICopyControl::Holder &destination, const TasksNode::TasksItemList &files) :
	ScanFilesExtendedBaseTask(receiver, destination),
	m_snapshot(createSnapshot(destination.data(), files.size())),
	m_files(files)
{}

Snapshot ScanFilesExtendedTask::scan(const volatile Flags &aborted)
{
	for (TasksNode::TasksItemList::size_type i = 0, size = m_files.size(); i < size && !aborted; ++i)
		ScanFilesExtendedBaseTask::scan(m_snapshot, m_files.at(i), static_cast<FolderBaseItem *>(m_files.at(i))->info().fileName(), aborted);

	return m_snapshot;
}

FILE_SYSTEM_NS_END