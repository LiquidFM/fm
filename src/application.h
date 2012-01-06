#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <QtCore/QThread>
#include <QtGui/QApplication>
//#include <QtSingleInstance/QSingleApplication>
#include "tools/exceptionshandler/exceptionshandler.h"
#include "tools/taskspool/taskpool.h"
#include "settings/applicationsettings.h"
#include "mainwindow/mainwindow.h"
#include "de/desktopenvironment.h"


class Application : public QApplication, protected ExceptionHandler
{
	Q_OBJECT

public:
	Application(const QString &name, const QString &label, const QString &description, int &argc, char **argv, bool GUIenabled = true);
	virtual ~Application();

	virtual bool notify(QObject *receiver, QEvent *event);

	DesktopEnvironment &desktopEnvironment() { return m_desktopEnvironment; }
	Tools::TasksPool::TaskPool &taskPool() { return m_taskPool; }
	ApplicationSettings &config() { return m_settings; }
	MainWindow &mainWindow() { return m_mainWindow; }

	qint32 exec();
	QString version() const;

	static Application *instance() { return static_cast<Application*>(QApplication::instance()); }

protected: /* ExceptionHandler */
	virtual void handleException(const char *where);
	virtual void handleException(const QString &message);
	virtual void handleException(const char *where, const char *what);

private:
	DesktopEnvironment m_desktopEnvironment;
	ApplicationSettings m_settings;
	Tools::TasksPool::TaskPool m_taskPool;
	MainWindow m_mainWindow;
};

#endif /* APPLICATION_H_ */
