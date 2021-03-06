/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include <iostream>
#include "application.h"
#include "dmmainwindow.h"
#include <sstream>
#include <cstdio>
#include <QApplication>
#include <QSplashScreen>
#include <QThreadPool>
#include <dynamindmainwindow.h>
#include <QGLFormat>
#include <QTime>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    
    QCoreApplication::setOrganizationName("CRC");
    QCoreApplication::setApplicationName("P8-WSC");
    
    QGLFormat glf = QGLFormat::defaultFormat();
    glf.setSampleBuffers(true);
    glf.setSamples(4);
    QGLFormat::setDefaultFormat(glf);
    //Q_INIT_RESOURCE(icons);
    Q_INIT_RESOURCE(splash);
    QTime time;
    qsrand(time.currentTime().second());
    int random = (qrand() % 4) +1;
    QString path = QString(":/Splash/ressources/WSC-GUI-Loading-screen0"+QString::number(random)+".png");
    QPixmap pixmap(path);
    QSplashScreen *splash1 = new QSplashScreen(pixmap);

    splash1->show();
    //splash1->showMessage("Loading Modules");
    app.processEvents();

    QThreadPool::globalInstance()->setMaxThreadCount(0);

    DMMainWindow * mw = new DMMainWindow();

    mw->show();

    splash1->finish(mw);


    return app.exec();


}
