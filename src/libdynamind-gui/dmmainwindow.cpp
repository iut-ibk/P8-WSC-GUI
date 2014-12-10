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
 * The class contains the main windows for the DAnCE4Water GUI
 */

#include "dmmainwindow.h"
//#include <boost/python.hpp>


#include "QDir"
#include "QThread"
#include "QTreeWidgetItem"
#include "QFileDialog"
#include "QSettings"
#include "modulereader.h"
#include <sstream>
#include "simulationio.h"
#include "guilink.h"
#include <QTreeWidget>
#include "simulationmanagment.h"
#include <projectviewer.h>
#include <modelnode.h>
#include <dmmodule.h>
#include <dmgroup.h>
#include <dmsimulation.h>
#include <groupnode.h>
#include <QWidget>
#include <QThreadPool>
#include <QDesktopServices>
#include <dmlog.h>
#include <guilogsink.h>
#include <dmpythonenv.h>
#include <guisimulationobserver.h>
#include <guisimulation.h>
#include <rootgroupnode.h>
#include "preferences.h"
#include "projectviewer.h"
#include "guihelpviewer.h"
#include "startupdialog.h"
#include <iostream>
#include "checkboxlist/exportfiles.h"
#include <QProgressDialog>
#include "dmlogsink2.h"


void outcallback( const char* ptr, std::streamsize count, void* pTextBox )
{
    (void) count;
    QPlainTextEdit * p = static_cast< QPlainTextEdit* >( pTextBox );

    if (*(ptr) > 37 ) {
        int c = *(ptr);
        QString s = QString::fromAscii(ptr);
        p->appendPlainText( ptr );
    }
}

void DMMainWindow::ReloadSimulation() {
    this->simulation->reloadModules();
    createModuleListView();
}

void DMMainWindow::startEditor() {
    DM::PythonEnv::getInstance()->startEditra();
}
void DMMainWindow::removeGroupWindows(QString uuid) {

    DM::Logger(DM::Debug) << "Remove ProjectWindow " << uuid.toStdString();
    //Check if already exists
    QMap<int, ProjectViewer * > groupscenes_tmp;
    int inew = 0;
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if (pv != 0) {

            if ((pv->getUUID()).compare(uuid) == 0) {

                this->tabWidget_4->removeTab(this->tabWidget_4->indexOf(tabmap[pv]));

                delete pv;

            } else {
                groupscenes_tmp[inew++] = pv;
            }
        }
    }
    this->groupscenes = groupscenes_tmp;
}
void DMMainWindow::addNewGroupWindows(GroupNode * g) {

    //Check if already exists
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        DM::Module * rg = pv->getRootNode()->getDMModel();
        if (!rg)
            return;
        if ((pv->getRootNode()->getDMModel()->getUuid()).compare(g->getDMModel()->getUuid()) == 0) {
            return;
        }
    }

    ProjectViewer * newgroup = new ProjectViewer(g );


    if (g->getDMModel()->getUuid().compare(simulation->getRootGroup()->getUuid()) == 0)
        this->simulation->addSimulationObserver(new GUISimulationObserver(newgroup));

    this->simulation->addSimulationObserver(new GUIStatusBarObserver(this));

    connect(simulation, SIGNAL(addedGroup(GroupNode*)), newgroup, SLOT(addGroup(GroupNode*)));
    connect(simulation, SIGNAL(GroupNameChanged(GroupNode*)), this, SLOT(renameGroupWindow(GroupNode*)));
    connect(simulation, SIGNAL(addedModule(ModelNode*)), newgroup, SLOT(addModule(ModelNode*)));
    connect(newgroup, SIGNAL(NewModule(QString,QPointF, DM::Module *)), simulation, SLOT(GUIaddModule(QString,QPointF, DM::Module  *)));
    connect(simulation, SIGNAL(showHelpForModule(std::string)), this, SLOT(showHelp(std::string)));
    connect(this,SIGNAL(statusChanged(double)), this,SLOT(updateStatus(double)));
    newgroup->setResultViewer(this);



    QGraphicsView * gv = new QGraphicsView(newgroup);
    gv->setRenderHints(QPainter::Antialiasing);
    gv->setAcceptDrops(true);

    QString name = QString::fromStdString(g->getDMModel()->getName());
    if (name.isEmpty()) {
        name = g->getName();
    }
    if (firstrun)
    {
        this->groupscenes[this->tabWidget_4->addTab(gv,"new project")] = newgroup;
        firstrun=false;
    }
    tabmap[newgroup] = gv;

}

void DMMainWindow::renameGroupWindow(GroupNode * g) {
    foreach(int i, groupscenes.keys()) {
        ProjectViewer * pv = groupscenes[i];
        if ((pv->getRootNode()->getDMModel()->getUuid()).compare(g->getDMModel()->getUuid()) == 0) {
            this->tabWidget_4->setTabText(i, QString::fromStdString(g->getDMModel()->getName()));
        }
    }
}

DMMainWindow::DMMainWindow(QWidget * parent)
{
    /*
    if(QDateTime::currentDateTime().secsTo(QDateTime(QDate(2014,10,31))) < 0)
    {
        QMessageBox::warning(this,"Time Expired","Free Time for this Version is Expired");
        exit(0);
    }*/
    setupUi(this);
    QPalette pal;
    pal.setColor(QPalette::Window, QColor(255,255,255));
    //    pal.setColor(QPalette::WindowText, QColor(0,0,0));
    pal.setColor(QPalette::Text, QColor(0,0,0));
    log_widget->setPalette(pal);
    //    l_Scenario->hide();
    //    l_simulationno->setText("Simulation -");
    popup1 = new SimulationPopup(l_simulationno, this, Qt::Popup | Qt::Window );
    firstrun=true;
    log_updater = new GuiLogSink();
    DM::Log::init(log_updater,DM::Debug);
    running =  false;
    this->setParent(parent);

    workPath=QDir::tempPath()+"/P8Tool";
    if (!QFile::exists(workPath))
    {
        QDir dir;
        dir.mkpath(workPath);
    }
    QSettings settings;
    settings.setValue("workPath",workPath);
    // std::cout<<workPath.toStdString()+"\n"<<settings.applicationName().toStdString()<<settings.organizationName().toStdString();

    // add log export to file
    QString logfilepath = QDir::temp().tempPath() + "/WSC-Modelling-Toolkit" + QDateTime::currentDateTime().toString("_yyMMdd_hhmmss_zzz")+".log";
    if(QFile::exists(logfilepath))
        QFile::remove(logfilepath);

    outputFile = new ofstream(logfilepath.toStdString().c_str());
    DM::Log::addLogSink(new DM::OStreamLogSink(*outputFile));

    DM::OStreamLogSink2 *logsink2 = new DM::OStreamLogSink2(*outputFile);
    DM::Log::addLogSink(logsink2);


    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath(QApplication::applicationDirPath().toStdString());
    env->addOverWriteStdCout();

    this->simulation = new GUISimulation();

    connect(this->simulation, SIGNAL(addedGroup(GroupNode*)), this, SLOT(addNewGroupWindows(GroupNode*)));
    this->simulation->registerRootNode();
    this->simulation->addModulesFromSettings();
    //this->simulation->registerStatusBar(this);
    this->helpviewer = new GUIHelpViewer();
    //this->verticalLayout->addWidget(helpviewer);


    log_widget->connect(log_updater, SIGNAL(newLogLine(QString)), SLOT(appendPlainText(QString)), Qt::QueuedConnection);
    connect( actionRun, SIGNAL( activated() ), this, SLOT( runSimulation() ), Qt::DirectConnection );
    connect( actionPreferences, SIGNAL ( activated() ), this, SLOT(preferences() ), Qt::DirectConnection );
    connect(actionSave, SIGNAL(activated()), this , SLOT(saveSimulation()), Qt::DirectConnection);
    connect(actionSaveAs, SIGNAL(activated()), this , SLOT(saveAsSimulation()), Qt::DirectConnection);
    connect(actionOpen, SIGNAL(activated()), this , SLOT(loadSimulation()), Qt::DirectConnection);
    connect(actionNew, SIGNAL(activated()), this , SLOT(clearSimulation()), Qt::DirectConnection);
    connect(actionImport, SIGNAL(activated()), this , SLOT(importSimulation()), Qt::DirectConnection);
    connect(actionExport_Files, SIGNAL(activated()),this,SLOT(exportFiles()),Qt::DirectConnection);
    connect(actionEditor, SIGNAL(activated()), this , SLOT(startEditor()), Qt::DirectConnection);
    connect(actionReload_Modules, SIGNAL(activated()), this , SLOT(ReloadSimulation()), Qt::DirectConnection);
    connect(actionUpdate, SIGNAL(activated()), this , SLOT(updateSimulation()), Qt::DirectConnection);
    connect(actionShow_debug_window, SIGNAL(activated()),this,SLOT(showDebug()),Qt::DirectConnection);
    connect(actionGetting_started_Tutorial, SIGNAL(activated()),this,SLOT(showTutorial()),Qt::DirectConnection);
    connect(actionShow_Temp_Folder, SIGNAL(activated()),this,SLOT(showTempFolder()),Qt::DirectConnection);
    connect(actionSupport, SIGNAL(activated()),this,SLOT(showSupport()),Qt::DirectConnection);
    connect(actionAbout, SIGNAL(activated()),this,SLOT(showAbout()),Qt::DirectConnection);
    connect(actionShow_Temporary_File_Folder,SIGNAL(activated()),this,SLOT(showP8ToolFolder()),Qt::DirectConnection);
    connect(logsink2,SIGNAL(callError()),SLOT(showError()),Qt::QueuedConnection);
    connect(actionResetModel,SIGNAL(activated()),SLOT(resetModel()),Qt::DirectConnection);

    currentDocument = "";

    //QSettings settings;
    if(settings.value("pythonModules").toString().isEmpty()) {
        counter++;
        this->preferences();
    }

    this->simmanagment = new SimulationManagment();

    createModuleListView();
    this->rootItemModelTree = new QTreeWidgetItem();
    this->rootItemModelTree->setText(0, "Groups");
    this->rootItemModelTree->setText(1, "");
    this->rootItemModelTree->setExpanded(true);

    hwin=this;


    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // toolBar is a pointer to an existing toolbar
    toolBar_3->addWidget(spacer);

    QLabel *label = new QLabel(this);
    //label->setPixmap(QPixmap(":/Icons/ressources/list-remove.png"));
    //label->setPixmap(QPixmap(":/Icons/ressources/list-add.png"));
    label->setPixmap(QPixmap(":/Icons/ressources/crc.png"));
    this->toolBar_3->addWidget(label);
    this->setWindowTitle("WSC Modelling Toolkit");
    this->show();
    StartupDialog startupdialog;
    int startmode=startupdialog.exec();
    if (startmode==1)
        loadSimulation();
    if (startmode==2)
        clearSimulation();
    this->pgDia = new QProgressDialog(this);
    pgDia->setWindowTitle("Simulation running");
    pgDia->setMinimum(0);
    pgDia->setMaximum(0);
    pgDia->setLabelText("Please wait...");
    pgDia->setCancelButton(0);
    pgDia->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
}

void DMMainWindow::createModuleListView()
{
    this->treeWidget->clear();

    ModelNode *selectedModule=NULL;
    int currentIndex = this->tabWidget_4->currentIndex();
    ProjectViewer * pviewer = groupscenes[currentIndex];
    QList<QGraphicsItem *> sItems;
    if (pviewer) {
        sItems = pviewer->selectedItems();
        if (sItems.size() > 0) {
            selectedModule = (ModelNode*) sItems.first();
        }
    }


    QStringList successors;
    if (selectedModule!=NULL)
    {
        QString name=QString::fromStdString(selectedModule->getDMModel()->getClassName());
        cout << "Selected: " << name.toStdString() << endl;
        /*if (name=="URBAN_FORM") {
            //successors<<"RealisationsSettings";
        }
        else if (name == "RealisationsSettings")
        {
            //successors<<"SCENARIO";
        }
        else if (name=="ImportShapeFile") {
            //successors<<"ExportToShapeFile";
        }
        else if (name == "SCENARIO")
        {
            //successors<<"Current_Realisation";
        }
        else
        {*/
        //for normal version

        successors<<"Analyser"<<"Current Realisation"<<"Stream Hydrology and Water Quality"<< "Microclimate Extreme Heat" <<
                    "Treatment and Harvesting Performance (MUSIC)"<<"Rainfall"<<"Import MUSIC File (.msf)"<<"Stream Erosion and Minor Flooding"<<
                    "ImportRasterData"<<"Microclimate Average Summer Heat"<<"ExportToGISShapeFile"<<"Import Map"<<
                    "GetPreviousBlocks"<<"GetSystems"<<"WriteResults2MUSIC"<<"Urban Form"<<"Scenario Setup";

        //successors <<"Microclimate Average Summer Heat" << "Microclimate Extreme Heat" <<"Import Map";
        //}

    }
    else
    {
        if (actionShow_all_modules->isChecked())
        {
            successors.empty();
            //successors << "Microclimate Extreme Heat" <<"Import Map";
            //exit(1);
        }
        else
        {
            successors<<"Analyser"<<"Current Realisation"<<"Stream Hydrology and Water Quality"<<"Microclimate Extreme Heat" <<
                        "Treatment and Harvesting Performance (MUSIC)"<<"Rainfall"<<"Import MUSIC File (.msf)"<<"Stream Erosion and Minor Flooding"<<
                        "ImportRasterData"<<"Microclimate Average Summer Heat"<<"ExportToGISShapeFile"<<"Import Map" <<
                        "GetPreviousBlocks"<<"GetSystems"<<"WriteResults2MUSIC"<<"Urban Form"<<"Scenario Setup";
            //successors <<"Microclimate Average Summer Heat" << "Microclimate Extreme Heat" <<"Import Map";

        }
    }
    std::list<std::string> mlist = (this->simulation->getModuleRegistry()->getRegisteredModules());
    std::map<std::string, std::vector<std::string> > mMap (this->simulation->getModuleRegistry()->getModuleMap());
    this->treeWidget->setColumnCount(1);
    for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
        QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
        std::string name = it->first;
        items->setText(0, QString::fromStdString(name));
        std::vector<std::string> names = it->second;
        std::sort(names.begin(), names.end());
        bool containsSuccessors=false;
        foreach(std::string name, names) {
            QTreeWidgetItem * item;
            //          cout << "it-name: ->" << name << "<-" << endl;
            if (successors.contains(QString::fromStdString(name))||successors.isEmpty())
            {
                cout << "OK"<<endl;
                item = new QTreeWidgetItem(items);
                item->setText(0,QString::fromStdString(name));
                item->setText(1,"Module");
                containsSuccessors=true;
            }
        }
        if (containsSuccessors||successors.isEmpty())
        {
            items->setDisabled(false);
            items->setExpanded(true);
        }
        else
            items->setDisabled(true);
    }

    //Add VIBe Modules
    QStringList filters;
    filters << "*.dyn";
    QSettings settings;
    QStringList moduleshome = settings.value("VIBeModules",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < moduleshome.size(); index++) {
        QDir d = QDir(moduleshome[index]);
        QStringList list = d.entryList(filters);
        QString module_name = d.absolutePath().split("/").last();
        mMap.clear();
        foreach(QString name, list) {
            std::vector<std::string> vec = mMap[module_name.toStdString()];
            vec.push_back(name.toStdString());
            mMap[module_name.toStdString()]  = vec;
        }

        for (std::map<std::string, std::vector<std::string> >::iterator it = mMap.begin(); it != mMap.end(); ++it) {
            if (successors.contains(QString::fromStdString(it->first))||successors.isEmpty())
            {
                QTreeWidgetItem * items = new QTreeWidgetItem(this->treeWidget);
                std::string name = it->first;
                items->setText(0, QString::fromStdString(name));
                std::vector<std::string> names = it->second;
                std::sort(names.begin(), names.end());

                foreach(std::string name, names) {
                    QTreeWidgetItem * item;
                    item = new QTreeWidgetItem(items);
                    item->setText(0,QString::fromStdString(name));
                    item->setText(1,"VIBe");
                    stringstream filename;
                    filename << moduleshome[0].toStdString() << "/" << name;
                    item->setText(2,QString::fromStdString(filename.str()));
                }
            }
        }
    }
}

void DMMainWindow::setMusicFile(int no)
{
    foreach(DM::Module *module,simulation->getModules())
    {
        //       cout << "-> " << module->getClassName()<<endl;
        if (QString(module->getClassName())=="TreatmentPerformance")
        {
            module->setParameterValue("MusicFileNo",QString("%1").arg(no).toStdString());
            //                 cout << "juhuuuu"<<endl;
        }
    }
}

int DMMainWindow::getMusicRuns()
{
    int retVal=0;
    if (simulation!=NULL)
        foreach(DM::Module *module,simulation->getModules())
        {
            if (QString(module->getClassName())=="TreatmentPerformance")
            {
                cout << "runs: " << 5/*module->getParameterAsString("Runs")*/ <<endl;
                retVal=5;//QString::fromStdString(module->getParameterAsString("Runs")).toInt();
            }
        }
    return retVal;
}

void DMMainWindow::runSimulation() {
    simulation->start();
    pgDia->show();
    return;

}
void DMMainWindow::updateSimulation() {
    this->simulation->updateSimulation();
}

void DMMainWindow::showDebug()
{
    this->dockWidget_2->setVisible(!this->dockWidget_2->isVisible());
}

void DMMainWindow::SimulationFinished() {

}

void DMMainWindow::preferences() {
    Preferences *p =  new Preferences(this);
    p->exec();
}

void DMMainWindow::setRunning() {
    this->running = false;
}

void DMMainWindow::sceneChanged() {


}


void DMMainWindow::saveAsSimulation()
{
    QSettings settings;

    QString filename=QFileDialog::getSaveFileName(this,"Save project as",settings.value("savePath").toString()+"/untitled.p8t","*.p8t");
    if (filename.isEmpty())
        return;

    QFileInfo fileinfo(filename);
    settings.setValue("savePath",fileinfo.absolutePath());

    save(filename);
}


void DMMainWindow::saveSimulation()
{
    if (!this->currentDocument.isEmpty())
        save(this->currentDocument);
    else
        this->saveAsSimulation();
}


void DMMainWindow::save(QString projectname)
{
    QSettings settings;
    QString fileName=settings.value("workPath").toString()+"/simulation.dyn";
    this->simulation->writeSimulation(fileName.toStdString());
    this->writeGUIInformation(fileName);
    this->currentDocument = projectname;

    QString foldername=settings.value("workPath").toString();
    QDir dir(foldername);

    dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);
    if (!dir.entryInfoList().isEmpty())
        QMessageBox::warning(NULL,"Warning",QString("Work folder contains subfolders. Subfolders will not be saved in project file."));

    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList();

    QFile projectfile(projectname);
    projectfile.open(QIODevice::WriteOnly);
    QDataStream out(&projectfile);
    out << (quint32)0x50385420;
    out.setVersion(QDataStream::Qt_4_0);
    out << dir.dirName();
    // cout << "dirname: "<<dir.dirName().toStdString() << endl;
    out << (quint64)list.size();
    // cout <<"files: "<<list.size()<<endl;

    for (int i = 0; i < list.size(); ++i)
    {
        QFileInfo fileInfo = list.at(i);
        quint64 size=fileInfo.size();
        out << size;
        // cout << fileInfo.fileName().toStdString()<<" filesize: "<<size<<endl;
        out << fileInfo.fileName();
        QFile savefile(fileInfo.absoluteFilePath());
        savefile.open(QIODevice::ReadOnly);
        QDataStream in(&savefile);

        int blocksize=4096;
        char *buffer=new char[blocksize];
        for (int j=0;j<size/blocksize+1;j++)
        {
            int readsize=in.readRawData(buffer,blocksize);
            out.writeRawData(buffer,readsize);
        }
        savefile.close();
    }
    projectfile.close();
    this->tabWidget_4->setTabText(0,QFileInfo(projectfile).fileName());

}

void DMMainWindow::SimChanged()
{
    emit statusChanged(49.097);
}


void DMMainWindow::writeGUIInformation(QString FileName) {

    //Find upper left corner;
    float minx;
    float miny;

    //Write GUI Informations for every Groupscene

    QFile file(FileName);
    file.open(QIODevice::Append);
    QTextStream out(&file);
    out << "<DynaMindGUI>" << "\n";
    out << "\t"<<"<GUI_Nodes>" << "\n";

    foreach (int scene, this->groupscenes.keys() ) {
        ProjectViewer * viewer = this->groupscenes[scene];


        for (int i = 0; i < viewer->getRootNode()->getChildNodes().size(); i++) {
            ModelNode * m = viewer->getRootNode()->getChildNodes()[i];
            if (i == 0)        {
                minx = m->pos().x();
                miny = m->pos().y();
                continue;
            }
            if (minx > m->pos().x()) {
                minx =  m->pos().x();
            }
            if (miny > m->pos().y()) {
                miny =  m->pos().y();
            }
        }



        foreach(ModelNode * m, viewer->getRootNode()->getChildNodes()) {
            out  << "\t" << "\t"<<"<GUI_Node>" << "\n";
            out << "\t" << "\t"<< "\t" << "<GUI_UUID value=\""
                << QString::fromStdString(m->getDMModel()->getUuid()) << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<GUI_PosX value=\""
                << m->scenePos().x() - minx << "\"/>" << "\n";



            out << "\t" << "\t"<< "\t" << "<GUI_PosY value=\""
                << m->scenePos().y() - miny << "\"/>" << "\n";

            out << "\t" << "\t"<< "\t" << "<GUI_Minimized value=\""
                << m->isMinimized() << "\"/>" << "\n";

            out  << "\t" << "\t"<<"</GUI_Node>" << "\n";

        }
    }
    out << "\t"<<"</GUI_Nodes>" << "\n";

    out << "</DynaMindGUI>" << "\n";
    out << "</DynaMind>"<< "\n";

    file.close();
}

void DMMainWindow::clearSimulation() {
    this->simulation->clearSimulation();
    this->currentDocument = "";

    QSettings settings;
    workPath=settings.value("workPath").toString();
    QDir dir(workPath);
    foreach (QFileInfo i,dir.entryInfoList(QDir::Files))
    {
        QFile::remove(i.absoluteFilePath());
    }
    this->tabWidget_4->setTabText(0,"new project");
}

void DMMainWindow::importSimulation(QString fileName, QPointF offset) {
    if (fileName.compare("") == 0)
        fileName = QFileDialog::getOpenFileName(this,
                                                tr("Open DynaMind File"), "", tr("DynaMind Files (*.dyn)"));

    if (fileName.isEmpty())
        return;


    std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
    SimulationIO simio;
    simio.loadSimluation(fileName, this->simulation, UUID_Translation);
    this->loadGUIModules((DM::Group *)this->simulation->getRootGroup(), UUID_Translation, simio.getPositionOfLoadedModules());

    this->loadGUILinks(UUID_Translation);


}

void DMMainWindow::exportFiles()
{
    ExportFiles *exportfiles=new ExportFiles;
    exportfiles->show();
}

void DMMainWindow::loadGUIModules(DM::Group * g, std::map<std::string, std::string> UUID_Translation, QVector<LoadModule> posmodules) {

    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }
    ProjectViewer * currentView = 0;
    foreach(int i, this->groupscenes.keys()) {
        ProjectViewer * view = this->groupscenes[i];
        if (g->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
            currentView = view;
    }
    if (currentView == 0)
        return;

    foreach (DM::Module * m , g->getModules()) {

        //Look if exists in Translation list
        if(reveredUUID_Translation.find(m->getUuid()) == reveredUUID_Translation.end())
            continue;


        //GetPos
        QPointF p;
        foreach (LoadModule lm, posmodules) {
            if (lm.tmpUUID.compare(reveredUUID_Translation[m->getUuid()]) == 0) {
                p.setX(lm.PosX);
                p.setY(lm.PosY);
            }
        }


        this->simulation->GUIaddModule(m, p);

        if (m->isGroup())
            this->loadGUIModules((DM::Group * )m,  UUID_Translation,posmodules);

    }

}

void DMMainWindow::loadSimulation(int id)
{
    QSettings settings;
    QString filename=QFileDialog::getOpenFileName(this,"Load project",settings.value("savePath").toString(),"*.p8t");

    if (filename.isEmpty())
        return;

    QFileInfo fileinfo(filename);
    settings.setValue("savePath",fileinfo.absolutePath());

    this->clearSimulation();
    this->currentDocument = filename;

    QString foldername=settings.value("workPath").toString();
    QDir dir(foldername);

    QFile projectfile(filename);
    projectfile.open(QIODevice::ReadOnly);
    QDataStream in(&projectfile);
    quint32 magic;
    in >> magic;
    // cout << "magic:"<<magic<<endl;
    if (magic!=0x50385420) //1345868832
    {
        QMessageBox::warning(NULL,"Error",QString("%1 is not a valid project file.").arg(filename));
        projectfile.close();
        return;
    }
    in.setVersion(QDataStream::Qt_4_0);
    QString dirname;
    in >> dirname;
    // cout << "dirname: "<<dirname.toStdString() << endl;

    quint64 size;
    in >> size;
    // cout << "filecount: " << size << endl;

    for (int i=0;i<size;i++)
    {
        quint64 filesize;
        in >> filesize;
        QString name;
        in >> name;

        QFile file(dir.absolutePath()+"/"+name);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::warning(NULL,"Error",QString("Could not extract %1.").arg(name));
        }
        QDataStream out(&file);

        int blocksize=4096;
        char *buffer=new char[blocksize];
        for (int j=0;j<filesize/blocksize;j++)
        {
            // cout <<".";
            int readsize=in.readRawData(buffer,blocksize);
            out.writeRawData(buffer,readsize);
        }
        int rem=filesize%blocksize;
        int readsize=in.readRawData(buffer,rem);
        out.writeRawData(buffer,readsize);
        // cout << rem << endl;
        // cout << "file: "<<name.toStdString()<<" ("<<filesize<<")"<<endl;
        file.close();
    }
    projectfile.close();

    QString fileName=dir.absolutePath()+"/simulation.dyn";

    std::map<std::string, std::string> UUID_Translation = this->simulation->loadSimulation(fileName.toStdString());
    SimulationIO simio;
    simio.loadSimluation(fileName, this->simulation, UUID_Translation);
    if (this->simulation->getSimulationStatus() == DM::SIM_FAILED_LOAD)  {
        this->simulation->clearSimulation();
        return;
    }
    UUID_Translation[this->simulation->getRootGroup()->getUuid()] = this->simulation->getRootGroup()->getUuid();
    this->loadGUIModules((DM::Group*)this->simulation->getRootGroup(),  UUID_Translation, simio.getPositionOfLoadedModules());
    this->loadGUILinks(UUID_Translation);
    this->tabWidget_4->setTabText(0,fileinfo.fileName());
}


void DMMainWindow::loadGUILinks(std::map<std::string, std::string> UUID_Translation) {

    std::map<std::string, std::string> reveredUUID_Translation;
    for (std::map<std::string, std::string>::const_iterator it = UUID_Translation.begin();
         it != UUID_Translation.end();
         ++it) {
        reveredUUID_Translation[it->second] = it->first;
    }

    foreach(DM::ModuleLink * l,this->simulation->getLinks()) {

        ModelNode * outmodule = 0;
        ModelNode * inmodule = 0;

        ProjectViewer * currentView = 0;

        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getInPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
                currentView = view;
        }
        ProjectViewer * currentView_out = 0;
        foreach(int i, this->groupscenes.keys()) {
            ProjectViewer * view = this->groupscenes[i];
            if (l->getOutPort()->getModule()->getGroup()->getUuid().compare( view->getRootNode()->getDMModel()->getUuid()) == 0  )
                currentView_out = view;
        }



        if (currentView == 0)
            continue;

        //TODO: Remove dirty hack
        //The Problem is with links that are conncet to the root group.
        //If so the outport module can not be find with current view.
        //In this Loop the view is switch to the view of the
        //Inport and then it works.
        for (int i = 0; i < 2; i++) {
            if (i == 1)
                currentView = currentView_out;
            if (currentView == 0)
                continue;

            if (currentView->getRootNode()->getDMModel() == l->getOutPort()->getModule()) {
                outmodule = currentView->getRootNode() ;

            }

            foreach (ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getDMModel() == l->getOutPort()->getModule()) {
                    outmodule = mn;
                    break;
                }
            }

            if (currentView->getRootNode()->getDMModel() == l->getInPort()->getModule()) {
                inmodule = currentView->getRootNode();

            }

            foreach(ModelNode * mn, currentView->getRootNode()->getChildNodes()) {
                if (mn->getDMModel() == l->getInPort()->getModule()) {
                    inmodule = mn;
                    break;
                }
            }

            if (inmodule != 0 && outmodule != 0) {
                break;
            }
        }

        //Look if exists in Translation list
        if(reveredUUID_Translation.find(inmodule->getDMModel()->getUuid()) == reveredUUID_Translation.end())
            continue;
        if(reveredUUID_Translation.find(inmodule->getDMModel()->getUuid()) == reveredUUID_Translation.end())
            continue;

        if (!outmodule || !inmodule)
            continue;
        GUILink * gui_link  = new GUILink();

        gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
        gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
        gui_link->setVIBeLink(l);
        gui_link->setSimulation(this->simulation);

        currentView->addItem(gui_link);
        currentView->update();

    }

}


DMMainWindow::~DMMainWindow() {
    delete this->simulation;
    delete this->pgDia;
}

void DMMainWindow::on_actionZoomIn_activated(){
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];


    view->scale(1.2, 1.2);

}

void DMMainWindow::on_actionZoomOut_activated()
{
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->scale(0.8, 0.8);
}

void DMMainWindow::on_actionZoomReset_activated()
{
    int i= this->tabWidget_4->currentIndex();
    QGraphicsView * view = groupscenes[i]->views()[0];
    view->fitInView(view->sceneRect(), Qt::KeepAspectRatio);



}

void DMMainWindow::showHelp(string classname) {
    this->helpviewer->show();
    this->helpviewer->showHelpForModule(classname);
}

void DMMainWindow::showTutorial()
{
    QDesktopServices::openUrl(QUrl("https://www.dropbox.com/sh/lsw36sc1lkxtpnd/AABL4_L7DguoQ-QQb2dfo9DMa?dl=0"));
}
void DMMainWindow::showTempFolder()
{
    QDesktopServices::openUrl(QDir::tempPath());
}

void DMMainWindow::showP8ToolFolder()
{
    QDesktopServices::openUrl(QDir::tempPath() + "/P8Tool");
}

void DMMainWindow::showSupport()
{
    QLabel * label_img = new QLabel (this);
    label_img->setWindowFlags(Qt::Window);
    label_img->setPixmap(QPixmap(":/Icons/ressources/Support.png", 0, Qt::AutoColor));
    label_img->show();
}

void DMMainWindow::showAbout()
{
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(QString("<h4>WSC Modelling Toolkit</h4>\n\n"
               "Version: 12.11.2014\n"
               /*"<a href=\"http://www.such-and-such.com\">http://www.such-and-such.com</a>"*/));
    msgBox.setIconPixmap(QPixmap(":/Icons/ressources/P8-Tool-Logo_small.png"));
    msgBox.exec();
}

void DMMainWindow::updateStatus(double status)
{
    //ui->progressBar->setValue((int)status);
    pgDia->hide();
}

void DMMainWindow::on_actionExit_triggered()
{
    exit(0);
}

DMMainWindow *hwin;

void DMMainWindow::on_actionShow_all_modules_changed()
{
    this->createModuleListView();
}


void DMMainWindow::on_l_Scenario_linkHovered(const QString &link)
{

}



void DMMainWindow::on_l_simulation_linkHovered(const QString &link)
{
    popup1->resize(10,10);
    popup1->move(QWidget::mapToGlobal(l_simulation->pos())+QPoint(20,20));
    popup1->show();
}

void DMMainWindow::on_actionHelp_triggered()
{
    QDesktopServices::openUrl(QUrl("file:///C:/test.html"));
}

void DMMainWindow::on_l_simulation_linkActivated(const QString &link)
{
    on_l_simulation_linkHovered(link)  ;
}

void DMMainWindow::closeEvent (QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "P8-WSC Modeller",
                                                                tr("Would you like to save the project?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes) {
        this->saveSimulation();
        event->accept();    }
    else if(resBtn == QMessageBox::No) {
        event->accept();
    }
    else{
        event->ignore();
    }
}

void DMMainWindow::resetModel()
{
    //Microclimate files
    deleteIfExists(workPath + "/LST after WSUD.mcd");
    deleteIfExists(workPath + "/Reduction in LST.mcd");
    deleteIfExists(workPath + "/LST before WSUD.mcd");
    deleteIfExists(workPath + "/WSUDtech.mcd");

    //EB files
    deleteIfExists(workPath + "/PredevelopRunoffFrequency.TXT");
    deleteIfExists(workPath + "/UntreatedRunoffFrequency.TXT");
    deleteIfExists(workPath + "/TreatedRunoffFrequency.TXT");
    deleteIfExists(workPath + "/ETandRe-useFluxes.TXT");
    deleteIfExists(workPath + "/PredevelopTotalRunoff.TXT");
    deleteIfExists(workPath + "/Exfiltration.TXT");
    deleteIfExists(workPath + "/WQ.TXT");
    deleteIfExists(workPath + "/PredevelopBaseflowFrequency.TXT");
    deleteIfExists(workPath + "/Baseflow.TXT");
    deleteIfExists(workPath + "/Pipe Flow.TXT");

    //SEI files
    deleteIfExists(workPath + "/pretable.csv");
    deleteIfExists(workPath + "/urbtable.csv");
    deleteIfExists(workPath + "/wsudtable.csv");


}

void DMMainWindow::deleteIfExists(QString filename)
{
    if(QFile::exists(filename))
        QFile::remove(filename);
}
void DMMainWindow::showError()
{
    QMessageBox::warning(NULL,"Error",QString("An Error Occured\n\nPlease Check The Log File Here " + workPath));
}
