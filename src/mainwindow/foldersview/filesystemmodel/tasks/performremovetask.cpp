#include "performremovetask.h"
#include "../../../../application.h"
#include <QtCore/QDir>


PerformRemoveTask::PerformRemoveTask(Params *params) :
	PerformTask(params, params->receiver),
	m_shoulRemoveEntry(true),
	m_skipAllIfNotRemove(false),
	m_skipAllIfNotExists(false),
	m_canceled(false)
{
	Q_ASSERT(params->entry);
	Q_ASSERT(params->subtree);
}

void PerformRemoveTask::run(const volatile bool &stopedFlag)
{
	QScopedPointer<FileSystemTree> subtree(parameters()->subtree);
	remove(subtree.data(), stopedFlag);

	QScopedPointer<Event> event(new Event(m_canceled ? Event::RemoveFilesCanceled : Event::RemoveFilesComplete));
	event->params().fileSystemTree = parameters()->fileSystemTree;
	event->params().entry = parameters()->entry;
	event->params().shoulRemoveEntry = m_shoulRemoveEntry;
	Application::postEvent(parameters()->receiver, event.take());
}

void PerformRemoveTask::remove(FileSystemTree *tree, const volatile bool &stopedFlag)
{
	bool tryAgain;

	for (FileSystemTree::size_type i = 1, size = tree->size(); i < size && !stopedFlag && !m_canceled; ++i)
		if (static_cast<FileSystemEntry*>(tree->child(i))->fileInfo().isDir())
		{
			remove(static_cast<FileSystemTree*>(tree->subtree(tree->child(i))), stopedFlag);

			tryAgain = false;
			removeEntry(static_cast<FileSystemEntry*>(tree->child(i)), tryAgain, stopedFlag);

			if (tryAgain)
				--i;
		}
		else
		{
			tryAgain = false;
			removeEntry(static_cast<FileSystemEntry*>(tree->child(i)), tryAgain, stopedFlag);

			if (tryAgain)
				--i;
		}
}

void PerformRemoveTask::removeEntry(FileSystemEntry *entry, bool &tryAgain, const volatile bool &stopedFlag)
{
	if (entry->fileInfo().exists())
	{
		bool res;
		QDir dir = entry->fileInfo().absoluteDir();

		if (entry->fileInfo().isDir())
			res = dir.rmdir(entry->fileInfo().fileName());
		else
			res = dir.remove(entry->fileInfo().fileName());

		if (!res && !m_skipAllIfNotRemove)
		{
			QuestionAnswerParams::Result result;
			QScopedPointer<QuestionAnswerEvent> event(new QuestionAnswerEvent(QuestionAnswerEvent::QuestionAnswer));
			event->params().buttons = QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Retry | QMessageBox::Cancel;
			event->params().title = entry->lockReason();
			event->params().result = &result;

			if (entry->fileInfo().isDir())
				event->params().question = tr("Failed to remove directory \"%1\". Skip it?").arg(entry->fileInfo().absoluteFilePath());
			else
				event->params().question = tr("Failed to remove file \"%1\". Skip it?").arg(entry->fileInfo().absoluteFilePath());

			Application::postEvent(parameters()->receiver, event.take());

			if (result.waitFor(stopedFlag))
				if (result.answer() == QMessageBox::YesToAll)
				{
					m_shoulRemoveEntry = false;
					m_skipAllIfNotRemove = true;
				}
				else
					if (result.answer() == QMessageBox::Retry)
						tryAgain = true;
					else
						if (result.answer() == QMessageBox::Cancel)
							m_canceled = true;
						else
							m_shoulRemoveEntry = false;
		}
	}
	else
		if (!m_skipAllIfNotExists)
		{
			QuestionAnswerParams::Result result;
			QScopedPointer<QuestionAnswerEvent> event(new QuestionAnswerEvent(QuestionAnswerEvent::QuestionAnswer));
			event->params().buttons = QMessageBox::Yes | QMessageBox::YesToAll | QMessageBox::Cancel;
			event->params().title = entry->lockReason();
			event->params().result = &result;

			if (entry->fileInfo().isDir())
				event->params().question = tr("Directory \"%1\" does not exists. Skip it?").arg(entry->fileInfo().absoluteFilePath());
			else
				event->params().question = tr("File \"%1\" does not exists. Skip it?").arg(entry->fileInfo().absoluteFilePath());

			Application::postEvent(parameters()->receiver, event.take());

			if (result.waitFor(stopedFlag))
				if (result.answer() == QMessageBox::YesToAll)
					m_skipAllIfNotExists = true;
				else
					if (result.answer() == QMessageBox::Cancel)
						m_canceled = true;
		}
}