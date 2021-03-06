/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2015 Dmitriy Vilkov, <dav.daemon@gmail.com>
 *
 * QFM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * QFM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with QFM. If not, see <http://www.gnu.org/licenses/>.
 */

#include "application.h"

#include <lvfs-core/INode>

#if defined(Q_WS_WIN)
#	include <QtCore/qt_windows.h>
#endif
#include <QtCore/QThread>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <QtGui/QKeyEvent>
#include <QtGui/QMessageBox>



Application::Application(const char *settings, int &argc, char **argv) :
	QApplication(argc, argv, true),
    m_appSettings("QFM"),
	m_settings(settings),
	m_module(m_settings),
    m_mainWindow(&m_appSettings)
{
	QApplication::setApplicationName(QString::fromLatin1("qfm"));
	QApplication::setOrganizationName(QString::fromLatin1("qde"));
	QApplication::setApplicationVersion(version());
	connect(this, SIGNAL(lastWindowClosed()), this, SLOT(cleanup()));

    m_settings.manage(&m_appSettings);
    m_settings.load();

    m_mainWindow.open();
}

Application::~Application()
{
    LVFS::Core::INode::lastCheck();
}

QString Application::version() const
{
//	if (QFM_VERSION_RELEASE % 2 == 0)
//		return QString::fromLatin1(QFM_VERSION_STRING);
//	else
//		return QString::fromLatin1(QFM_VERSION_STRING).append(QString::fromLatin1(" (unstable)"));

    return QString();
}

bool Application::notify(QObject *receiver, QEvent *event)
{
    if (event->type() == QEvent::KeyPress &&
        (static_cast<QKeyEvent*>(event)->key() + static_cast<QKeyEvent*>(event)->modifiers()) == Qt::NoModifier + Qt::Key_Tab)
    {
        m_mainWindow.switchToOtherPanel();
        event->accept();
        return true;
    }

    return QApplication::notify(receiver, event);
}

qint32 Application::exec()
{
//    QTranslator qtTranslator;
//    qtTranslator.load(QString::fromLatin1("qt_").append(QLocale::system().name()), QApplication::applicationDirPath());
//    installTranslator(&qtTranslator);
//
//    QTranslator appTranslator;
//    qtTranslator.load(QString::fromLatin1("app_").append(QLocale::system().name()), QApplication::applicationDirPath());
//    installTranslator(&appTranslator);

    m_mainWindow.show();
    return QApplication::exec();
}

void Application::cleanup()
{
    m_mainWindow.close();
    LVFS::Core::INode::cleanup();
    m_settings.save();
}
