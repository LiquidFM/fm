/**
 * This file is part of QFM.
 *
 * Copyright (C) 2011-2014 Dmitriy Vilkov, <dav.daemon@gmail.com>
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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <lvfs/Module>
#include <lvfs/settings/Scope>
#include <QtGui/QApplication>
#include "mainwindow/mainwindow.h"


class Application : public QApplication
{
	Q_OBJECT

public:
	Application(const char *settings, int &argc, char **argv);
	virtual ~Application();

	virtual bool notify(QObject *receiver, QEvent *event);

	qint32 exec();
	QString version() const;

private Q_SLOTS:
	void cleanup();

private:
    LVFS::Settings::Scope m_appSettings;
    LVFS::Settings::Instance m_settings;
    LVFS::Module m_module;

private:
    MainWindow m_mainWindow;
};

#endif /* APPLICATION_H_ */
