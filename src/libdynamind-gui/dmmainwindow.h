/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * DAnCE4Water (Dynamic Adaptation for eNabling City Evolution for Water) is a
 * strategic planning tool ti identify the transtion from an from a conventional
 * urban water system to an adaptable and sustainable system at a city scale
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
 * @section DESCRIPTION
 *
 * The class contains the main window for the DynaMind Framework
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "dmcompilersettings.h"
#include <QMainWindow>

//#include "plot.h"


//popup
#include <QLineEdit>
#include <QPushButton>
//


#include <QMap>
#include <QVector>
#include <moduledescription.h>
#include <guiresultobserver.h>
#include <ui_dmmainwindow.h>
#include <simulationio.h>
#include <cmath>
class QTreeWidgetItem;


class ProjectViewer;
class ModelNode;
class SimulationManagment;
class GroupNode;
class GuiLogSink;
class GUISimulationObserver;
class GUISimulation;
class GUIHelpViewer;
namespace DM {
class Group;
}
namespace Ui {
class MainWindow;
}

class SimulationPopup;


class DM_HELPER_DLL_EXPORT DMMainWindow : public QMainWindow, public Ui::DMMainWindow
{
    Q_OBJECT
public:
    DMMainWindow(QWidget *parent = 0);
    ~DMMainWindow();
    QTreeWidgetItem * getRootItemModelTree(){return this->rootItemModelTree;}
    GuiLogSink *log_updater;
    GUISimulation * getSimulation() {return this->simulation;}
    void createModuleListView();
    void setMusicFile(int no);
    int getMusicRuns();

private:
    GUISimulation * simulation;
    SimulationManagment * simmanagment;
    GUISimulationObserver * simobserver;
    GUIHelpViewer * helpviewer;

    QMap<QString, ModuleDescription> modules;
    QMap<ProjectViewer *, QWidget *> tabmap;
    QMap<int, ProjectViewer * >  groupscenes;
    QString currentDocument;
    QString workPath;
    int counter;
    bool running;
    bool firstrun;
    QTreeWidgetItem * rootItemModelTree;
    void writeGUIInformation(QString FileName);
    void loadGUIModules(DM::Group * g, std::map<std::string, std::string> UUID_Translation,  QVector<LoadModule> posmodules);
    void loadGUILinks(std::map<std::string, std::string> UUID_Translation);
    SimulationPopup* popup1;
    void save(QString projectname);

public slots:
    void runSimulation();
    void sceneChanged();
    void preferences();
    void saveSimulation();
    void loadSimulation(int id=0);
    void clearSimulation();
    void setRunning();
    void saveAsSimulation();
    void importSimulation(QString fileName = "", QPointF = QPointF(0,0));
    void SimulationFinished();
    void startEditor();
    void ReloadSimulation();
    void addNewGroupWindows(GroupNode *);
    void renameGroupWindow(GroupNode *);
    void removeGroupWindows(QString uuid);
    void updateSimulation();
    void showHelp(std::string classname);


private slots:
    void on_actionZoomReset_activated();
    void on_actionZoomOut_activated();
    void on_actionZoomIn_activated();
    void on_actionExit_triggered();
    void on_actionShow_all_modules_changed();
    void on_l_Scenario_linkHovered(const QString &link);
    void on_l_simulation_linkHovered(const QString &link);
    void on_actionHelp_triggered();
    void on_l_simulation_linkActivated(const QString &link);

signals:
    void updateSplashMessage(QString);

};

extern DMMainWindow *hwin;



class SimulationPopup : public QFrame
{
    Q_OBJECT
public:
    SimulationPopup( QLabel *label, DMMainWindow* parent = 0, Qt::WindowFlags f = 0) : QFrame(NULL,f)
    {
        this->parent=parent;
        this->label=label;
        grid = new QGridLayout(this);
        list= new QList<QPushButton*>;
        count=0;
    }

    ~SimulationPopup()
    {
        disconnect(this,SLOT(hide()));
//        QWidget::~QWidget();		Clemens Heiko
    }

    void leaveEvent ( QEvent * event )
    {
        this->hide();
    }

    void show()
    {
        int newcount=parent->getMusicRuns();
        if (newcount!=count)
        {
            clear();
            count=newcount;
            int rows=1;         //sqrt(double(count));
            int columns=count;  //count/rows+1;
            for (int i=0;i<count;i++)
            {
                QPushButton *button=new QPushButton(this);
                connect(button,SIGNAL(pressed()),this,SLOT(buttonReleased()));
                button->setText(QString("%1").arg(i+1));
                int row=1;
                int column=i;
                grid->addWidget(button,row,column);
                list->append(button);
            }

        }
        QWidget::show();
    }

    void clear()
    {
        int rows=1;         //sqrt(double(count));
        int columns=count;  //count/rows+1;
        foreach(QPushButton *button,*list)
        {
            grid->removeWidget(button);
            disconnect(button,SLOT(buttonReleased()));
            delete button;
        }
        list->clear();
    }

protected slots:
    void buttonReleased()
    {
        foreach(QPushButton *button, *list)
        {
            if (button->isDown())
            {
                int active=button->text().toInt();
                parent->setMusicFile(active);
                cout << "huhu: "<< active << endl;
                label->setText(QString("Simulation %1").arg(active));
            }
        }
    }

protected:
    QGridLayout *grid;
    QList<QPushButton*> *list;
    QLabel *label;
    DMMainWindow *parent;
    int count;
};


#endif // MAINWINDOW_H
