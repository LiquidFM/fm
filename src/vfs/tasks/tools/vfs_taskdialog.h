#ifndef VFS_TASKDIALOG_H_
#define VFS_TASKDIALOG_H_

#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtGui/QWidget>
#include <QtGui/QMessageBox>
#include "../vfs_basetask.h"


VFS_NS_BEGIN

class QuestionEvent: public BaseTask::Event
{
public:
	class Result
	{
	public:
		Result() :
			m_answer(0),
			m_done(false)
		{}

		void waitFor(const volatile BaseTask::Flags &aborted)
		{
			QMutexLocker lock(&m_mutex);

			while (!m_done && !aborted)
				m_condition.wait(&m_mutex, 1000);
		}

		qint32 answer() const { return m_answer; }

	private:
		friend class QuestionEvent;
		QMutex m_mutex;
		qint32 m_answer;
		volatile bool m_done;
		QWaitCondition m_condition;
	};

public:
	QuestionEvent(const QString &title, const QString &question, qint32 buttons, Result *result) :
		Event(NULL, Question, false),
		m_title(title),
		m_question(question),
		m_buttons(buttons),
		m_result(result)
	{
		Q_ASSERT(m_result);
	}

	/* Must be called from the main thread. */
	void showDialog(QWidget *parent);

private:
	QString m_title;
	QString m_question;
	qint32 m_buttons;
	Result *m_result;
};


class UserInputEvent: public BaseTask::Event
{
public:
	class Result
	{
	public:
		Result() :
			m_answer(0),
			m_done(false)
		{}

		void waitFor(const volatile BaseTask::Flags &aborted)
		{
			QMutexLocker lock(&m_mutex);

			while (!m_done && !aborted)
				m_condition.wait(&m_mutex, 1000);
		}

		qint32 answer() const { return m_answer; }
		const QString &value() const { return m_value; }

	private:
		friend class UserInputEvent;
		QMutex m_mutex;
		qint32 m_answer;
		QString m_value;
		volatile bool m_done;
		QWaitCondition m_condition;
	};

public:
	UserInputEvent(const QString &title, const QString &question, qint32 buttons, Result *result) :
		Event(NULL, UserInput, false),
		m_title(title),
		m_question(question),
		m_buttons(buttons),
		m_result(result)
	{
		Q_ASSERT(m_result);
	}

	/* Must be called from the main thread. */
	void showDialog(QWidget *parent);

private:
	QString m_title;
	QString m_question;
	qint32 m_buttons;
	Result *m_result;
};

VFS_NS_END

#endif /* VFS_TASKDIALOG_H_ */