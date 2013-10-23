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
#include <modelnode.h>

#include <iostream>
#include <sstream>

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>

#include "ColorPalette.h"
#include <guimodelnode.h>
#include <guiviewdataformodules.h>
#include <guiport.h>
#include <rootgroupnode.h>
#include <guisimulation.h>
#include <dmsystem.h>

#include "dmviewerwindow.h"

#include <sstream>
#include <guihelpviewer.h>
#include <dmmainwindow.h>


std::string ModelNode::getParameterAsString(std::string name) {

    std::ostringstream val;
    int id = this->getDMModel()->getParameterList()[name];
    if (id == DM::DOUBLE || id == DM::LONG || id == DM::STRING) {
        if (id == DM::DOUBLE)
            val << this->getDMModel()->getParameter<double>(name);
        if (id == DM::LONG)
            val << this->getDMModel()->getParameter<long>(name);
        if (id == DM::STRING)
            val << this->getDMModel()->getParameter<std::string>(name);
        return val.str();
    }
}

std::string ModelNode::getGroupUUID() {
    return this->getDMModel()->getGroup()->getUuid();
}

/*
void ModelNode::setMySelected ( bool selected )
{
    exit(0);

    QGraphicsItem::setSelected ( selected );
    this->ResultWidget->createModuleListView();
}
*/

void ModelNode::updatePorts () {

    //Add Ports
    //If Port exists is checked by addPort
    foreach (DM::Port * p, this->getDMModel()->getInPorts()){
        this->addPort(p);
    }
    foreach (DM::Port * p, this->getDMModel()->getOutPorts()) {
        this->addPort(p);
    }

    for (int i = this->ports.size()-1; i > -1; i--) {
        GUIPort * gp = this->ports[i];
        if (!gp->getVIBePort()) {
            if (gp->getPortType() > DM::OUTPORTS) {
                ExistingInPorts.removeAt(ExistingInPorts.indexOf(gp->getPortName()));
                this->inputCounter--;
            } else {
                ExistingOutPorts.removeAt(ExistingOutPorts.indexOf(gp->getPortName()));
                this->outputCounter--;
            }

            this->ports.remove(i);
            delete gp;
            continue;
        }
        if (gp->getVIBePort()->isPortTuple())
            continue;
        if (gp->getPortType()  > DM::OUTPORTS ) {
            bool  portExists = false;

            foreach (DM::Port * p, this->getDMModel()->getInPorts()){
                std::string portname1 = p->getLinkedDataName();
                std::string portname2 = gp->getPortName().toStdString();
                if (portname1.compare(portname2) == 0) {
                    portExists = true;
                }
            }
            if (!portExists) {
                ExistingInPorts.removeAt(ExistingInPorts.indexOf(gp->getPortName()));
                this->ports.remove(i);
                delete gp;
            }
        }
        if (gp->getPortType()  < DM::OUTPORTS ) {
            bool  portExists = false;

            foreach (DM::Port * p, this->getDMModel()->getOutPorts()){
                if (p->getLinkedDataName().compare(gp->getPortName().toStdString()) == 0) {
                    portExists = true;
                }
            }
            if (!portExists) {
                ExistingOutPorts.removeAt(ExistingOutPorts.indexOf(gp->getPortName()));
                this->ports.remove(i);
                delete gp;
            }
        }

    }
    this->update();
}
void ModelNode::resetModel() {
    DM::Module *oldmodule = this->getDMModel();
    this->VIBeModuleUUID = this->simulation->resetModule(this->getDMModel()->getUuid())->getUuid();

    if(this->getDMModel()==oldmodule)
    {
        DM::Logger(DM::Error) << "The code-base has changed for module \"" << this->getDMModel()->getName() << "\". There is an error in the new code-base";
    }

    foreach(GUIPort * p, this->ports) {
        DM::Port * po = 0;
        if ((this->getDMModel()->getInPort( p->getPortName().toStdString()) == 0)) {
            po = this->getDMModel()->getOutPort( p->getPortName().toStdString());
        } else {
            po = this->getDMModel()->getInPort( p->getPortName().toStdString());
        }
        p->updatePort( po );
    }

}

void ModelNode::addPort(DM::Port * p) {
    if (p->getPortType() < DM::OUTPORTS ) {
        foreach (QString pname, ExistingInPorts) {
            if (pname.compare(QString::fromStdString(p->getLinkedDataName())) == 0) {
                return;
            }
        }
        ExistingInPorts << QString::fromStdString(p->getLinkedDataName());
    } else {
        foreach (QString pname, ExistingOutPorts) {
            if (pname.compare(QString::fromStdString(p->getLinkedDataName())) == 0) {
                return;
            }
        }
        ExistingOutPorts << QString::fromStdString(p->getLinkedDataName());
    }

    GUIPort * gui_p = new  GUIPort(this, p);
    ports.append(gui_p);
    if  (p->getPortType() < DM::OUTPORTS) { //out
        if(this->getName()=="Current_Realisation")
        {
            gui_p->setPos(250,40);//335,110); coords für ecken
        }
        if(this->getName()=="Current_RealisationModule")
        {
            gui_p->setPos(250,40);
        }
        else if(this->getName()=="RainModule")
        {
            gui_p->setPos(185,40);
        }
        else if(this->getName()=="TreatmentPerformanceResults")
        {
            gui_p->setPos(155,120);
        }
        else if(this->getName()=="TreatmentPerformanceResultsModule")
        {
            gui_p->setPos(285,150);
        }
        else if(this->getName()=="EnviromentalBenefitsResults")
        {
            gui_p->setPos(200,100);
        }
        else if(this->getName()=="EnviromentalBenefitsResults2")
        {
            gui_p->setPos(285,160);
        }
        else if(this->getName()=="StreamHydrologyandWaterquality")
        {
            gui_p->setPos(298,175);
        }
        else if(this->getName()=="Economic")
        {
            gui_p->setPos(230,110);
        }
        else if(this->getName()=="Analyser")
        {
            gui_p->setPos(280,137);
        }
        else if(this->getName()=="AnalyserModule")
        {
            gui_p->setPos(280,137);
        }
        else if(this->getName()=="RealisationsSettings")
        {
            gui_p->setPos(285,137);
        }
        else if(this->getName()=="Microclimate")
        {
            gui_p->setPos(285,152);
        }
        else if(this->getName()=="ImportMSF")
        {
            gui_p->setPos(200,100);
        }
        else if(this->getName()=="StreamErosionIndex")
        {
            gui_p->setPos(300,170);
        }
        else if(this->getName()=="ImportRasterData")
        {
            gui_p->setPos(275,155);
        }
        else
        {
            gui_p->setPos(this->boundingRect().width(),70+gui_p->boundingRect().height()*this->outputCounter++);
            if (outputCounter > inputCounter)
                this->h = h+20;
        }

    }else {//in
        if(this->getName()=="Current_Realisation")
        {
            gui_p->setPos(75,160);//10,110); coords für ecken
        }
        else if(this->getName()=="RainModule")
        {
            gui_p->setPos(10,55);
        }
        else if(this->getName()=="RealisationsSettings")
        {
            gui_p->setPos(15,137);
        }
        else if(this->getName()=="Analyser")
        {
            gui_p->setPos(15,137);
        }
        else if(this->getName()=="AnalyserModule")
        {
            gui_p->setPos(15,137);
        }
        else if(this->getName()=="TreatmentPerformanceResults")
        {
            gui_p->setPos(35,120);
        }
        else if(this->getName()=="TreatmentPerformanceResultsModule")
        {
            gui_p->setPos(15,145);
        }
        else if(this->getName()=="EnviromentalBenefitsResults")
        {
            gui_p->setPos(0,100);
        }
        else if(this->getName()=="StreamHydrologyandWaterquality")
        {
            gui_p->setPos(20,175);
        }
        else if(this->getName()=="Economic")
        {
            gui_p->setPos(13,110);
        }
        else if(this->getName()=="Microclimate")
        {
            gui_p->setPos(20,152);
        }
        else if(this->getName()=="StreamErosionIndex")
        {
            gui_p->setPos(20,170);
        }
        else
        {
            gui_p->setPos(0,70+gui_p->boundingRect().height()*this->inputCounter++);
            if (outputCounter < inputCounter)
               this->h = h+20;
        }
    }
}


/*void ModelNode::removePort(int Type, QString s) {
    int counter = 0;
    int itemID;

    this->update();
}*/


//ModelNode

DM::Module * ModelNode::getDMModel() {
    return this->simulation->getModuleWithUUID(this->VIBeModuleUUID);
}

ModelNode::ModelNode( DM::Module *VIBeModule,GUISimulation * simulation)
{
    this->guiPortObserver.setModelNode(this);
    this->guiResultObserver.setResultWidget(this->ResultWidget);
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->parentGroup = 0;
    this->VIBeModuleUUID = VIBeModule->getUuid();
    this->id = VIBeModule->getID();
    this->simulation = simulation;
    this->nodes = 0;

    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem ("Module: " + QString::fromStdString(VIBeModule->getClassName()));
    double w = this->simpleTextItem->boundingRect().width()+40;

    QGraphicsSimpleTextItem tn ("Name: " + QString::fromStdString(VIBeModule->getName()));
    w = w < tn.boundingRect().width() ? tn.boundingRect().width() : w;


    w = w < 140 ? 140 : w;
    l = w+4;
    h =  80;
    VIBeModule->addPortObserver( & this->guiPortObserver);
    this->updatePorts();

    Color = COLOR_MODULE;
}

ModelNode::ModelNode(QGraphicsItem * parent, QGraphicsScene * scene) :QGraphicsItem(parent, scene) {

}


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    painter->setFont( QFont("Helvetica", 20));
    QImage img;
    if(this->isSelected() == true) {
        Color = COLOR_MODULESELECTED;
        //painter->setPen(QColor(255,255,255));
        //painter->setBrush(QBrush(QColor(220,220,220)));
        painter->setPen(QColor(0,0,0));
        //painter->setBrush(QBrush(QColor(255,255,255)));
        painter->drawRect(0, 0, l, h);
    }
    else if (this->getDMModel()->isExecuted()) {
        Color = COLOR_EXECUTED;
    }
    else {
        Color = COLOR_MODULE;
    }
    if(this->visible){
        QPen pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        //painter->setBrush(QBrush(QColor(220,220,180)));
        //painter->setPen(QPen(QColor(220,220,220),2));
        if(this->getName()=="RainModule")
        {
            img = QImage(":/Icons/ressources/Rainfall.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-20,h-10,QString("Rain"));
        }
        else if(this->getName()=="TreatmentPerformanceResultsModule")
        {
            img = QImage(":/Icons/ressources/TreatmentPerformance.png");//MircorClimateHeatIslandEffect.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-140,h-10,QString("Treatment Performance"));
        }
        else if(this->getName()=="Current_Realisation")
        {

            img = QImage(":/Icons/ressources/Realisation.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            QString zwei = QString::fromStdString(this->getDMModel()->getParameterAsString("RealisationNr"));
            painter->drawText(l/2-120,h-10,QString("Current Realisation ") + zwei);

        }
        else if(this->getName()=="Current_RealisationModule")
        {

            img = QImage(":/Icons/ressources/Realisation.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            QString zwei = QString::fromStdString(this->getDMModel()->getParameterAsString("RealisationNr"));
            painter->drawText(l/2-120,h-10,QString("Current Realisation ") + zwei);

        }
        else if(this->getName()=="Economic")
        {
            img = QImage(":/Icons/ressources/EconomicValuation.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-30,h-10,QString("Economic"));

        }
        else if(this->getName()=="Microclimate")
        {
            img = QImage(":/Icons/ressources/MicroClimateCool.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-120,h-10,QString("Microclimate Benefits"));

        }
        else if(this->getName()=="Analyser")
        {
            img = QImage(":/Icons/ressources/Analyser.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-40,h-10,QString("Analyser"));

        }
        else if(this->getName()=="AnalyserModule")
        {
            img = QImage(":/Icons/ressources/Analyser.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-40,h-10,QString("Analyser"));

        }
        else if(this->getName()=="RealisationsSettings")
        {
            img = QImage(":/Icons/ressources/RealisationsSettings.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-130,h-10,QString("Number of realisations"));

        }
        else if(this->getName()=="EnviromentalBenefitsResults2")
        {
            img = QImage(":/Icons/ressources/EnviromentalBenefits.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-130,h-10,QString("Enviromental Benefits"));

        }
        else if(this->getName()=="StreamHydrologyandWaterquality")
        {
            img = QImage(":/Icons/ressources/EnvironmentalBenefits.png");
            h = img.height();
            l = img.width();
            painter->setFont( QFont("Helvetica", 14));
            painter->drawImage(0,0,img);
            painter->drawText(l/2-150,h-10,QString("Stream Hydrology and Water quality"));
            painter->setFont( QFont("Helvetica", 20));

        }
        else if(this->getName()=="ImportMSF")
        {
            img = QImage(":/Icons/ressources/FileUpload.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-60,h-10,QString("ImportMSF"));

        }
        else if(this->getName()=="StreamErosionIndex")
        {
            img = QImage(":/Icons/ressources/StreamErosion.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-130,h-10,QString("Stream Erosion Index"));

        }
        else if(this->getName()=="ImportRasterData")
        {
            img = QImage(":/Icons/ressources/7-MircorClimate---HeatIslandEffect.png");
            h = img.height();
            l = img.width();
            painter->drawImage(0,0,img);
            painter->drawText(l/2-130,h-10,QString("Import Imperviousness"));

        }
        else
        {
            QLinearGradient linearGrad(QPointF(0, h), QPointF(0, 0));
            QColor c1 = Color;
            QColor c2 = Color;
            linearGrad.setColorAt(0, c1);
            linearGrad.setColorAt(.4, c2);
            QBrush brush(linearGrad);

            painter->setBrush(Qt::white);
            painter->setPen(pen);
            QPainterPath path;
            path.addRect(0, 0,l,h);
            painter->fillPath(path, brush);
            painter->strokePath(path, pen);

            if (!this->getDMModel()->getName().empty())
                painter->drawText(QPoint(22,35), "Name: " + QString::fromStdString(this->getDMModel()->getName()));

            painter->drawText(QPoint(22,15), "Module: " + QString::fromStdString(this->getDMModel()->getClassName()));
        }
            /*
        if (this->parentGroup)
            painter->drawText(QPoint(22,55), "GRP Group: "+ QString::fromStdString(this->parentGroup->getDMModel()->getName()));*/
    }
}


QRectF ModelNode::boundingRect() const {
    QRect r (0, 0,
             l, h);
    return r;

}

QVariant ModelNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

ModelNode::~ModelNode() {

    DM::Module * m = this->getDMModel();


    if (m!=0) {
        foreach (GUIPort * p, ports) {
            delete p;
            p = 0;
        }
    }
    if (this->getDMModel()!= 0)
        this->simulation->removeModule(this->VIBeModuleUUID);

    if (this->parentGroup != 0) {
        this->parentGroup->removeModelNode(this);
    }


    ports.clear();
    delete this->simpleTextItem;

}



void ModelNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {

    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
        this->parentGroup->update();
    }

    QGraphicsItem::mouseMoveEvent(event);



}

void ModelNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) {

    if(this->visible){
        this->simulation->updateSimulation();
        if (this->getDMModel()->createInputDialog() == false )
        {

            QWidget * gui  = new GUIModelNode(this->getDMModel(), this);
            gui->show();
        }
    }
}


void ModelNode::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event )
{
    //cout << this->getDMModel()->getSimulation()->getResultWidget << endl;
    hwin->createModuleListView();
    QGraphicsItem::mouseReleaseEvent (event);
}


GUIPort * ModelNode::getGUIPort(DM::Port * p) {

    foreach (GUIPort * gui_p, this->ports){
        if (gui_p->getVIBePort() == p)
            return gui_p;
    }


    return 0;
}

void ModelNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction  * a_edit = menu.addAction("edit");
    QAction * a_rename = menu.addAction("rename");
    QAction * a_delete = menu.addAction("delete");
    QAction * a_showData = menu.addAction("showData");
    QAction * a_viewData = menu.addAction("viewData");
    QAction * a_showHelp = menu.addAction("showHelp");
    QMenu * GroupMenu =     menu.addMenu("Groups");

//    a_viewData->setEnabled(this->getDMModel()->getOutPorts().size() > 0);
    a_viewData->setEnabled(true);

    GroupMenu->setTitle("Group");
    QVector<QAction *> actions;
    std::vector<DM::Group*> gs = this->simulation->getGroups();
    foreach (DM::Group * g, gs) {
        if (this->getDMModel()->getUuid().compare(g->getUuid())) {
            QAction *a = GroupMenu->addAction(QString::fromStdString(g->getName()));
            a->setObjectName(QString::fromStdString(g->getUuid()));
            actions.push_back(a);
        }

    }
    DM::Module * rgroup = this->simulation->getRootGroup();
    DM::Module * group = this->getDMModel()->getGroup();
    if (group != 0 && rgroup != 0) {
        if (this->getDMModel()->getGroup()->getUuid().compare(this->simulation->getRootGroup()->getUuid()) != 0) {
            QAction *a = GroupMenu->addAction("RootGroup");
            a->setObjectName(QString::fromStdString(this->simulation->getRootGroup()->getUuid()));
            actions.push_back(a);
        }
    }

    foreach (QAction *a,  actions) {
        connect( a, SIGNAL( activated() ), this, SLOT( addGroup() ), Qt::DirectConnection );
    }

    connect( a_delete, SIGNAL( activated() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_edit, SIGNAL( activated() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_rename, SIGNAL(activated() ), this, SLOT( renameModelNode() ), Qt::DirectConnection);
    connect( a_showData, SIGNAL(activated() ), this, SLOT( printData() ), Qt::DirectConnection);
    connect( a_viewData, SIGNAL(activated() ), this, SLOT( viewData() ), Qt::DirectConnection);
    connect( a_showHelp, SIGNAL(activated() ), this, SLOT( showHelp() ), Qt::DirectConnection);
    menu.exec(event->screenPos());

}
void ModelNode::editModelNode() {
    GUIModelNode * gui  = new GUIModelNode(this->getDMModel() ,this);
    gui->show();
}

void ModelNode::renameModelNode() {
    QString text =QInputDialog::getText(0, "Name", tr("Input:"), QLineEdit::Normal);
    if (!text.isEmpty())
        this->getDMModel()->setName(text.toStdString());

    if (this->isGroup())
    {
        this->simulation->changeGroupName((GroupNode*) this);
    }
}

void ModelNode::deleteModelNode() {

    delete this;
}
void ModelNode::removeGroup() {
    QString name = QObject::sender()->objectName();
    this->update(this->boundingRect());
}

void ModelNode::setMinimized(bool b) {
    if (!this->isGroup() ){
        this->minimized = b;

        bool visible = true;
        if  (b) {
            visible = false;

        }
        this->setVisible(visible);
    }
}


void ModelNode::addGroup() {
    //Find GroupNode
    QString name = QObject::sender()->objectName();
    DM::Group * g;
    if (name.compare(QString::fromStdString(this->simulation->getRootGroup()->getUuid())) == 0) {
        this->getDMModel()->setGroup((DM::Group * ) this->simulation->getRootGroup());
        this->getSimulation()->GUIaddModule(this->getDMModel(), this->pos());
        this->VIBeModuleUUID = "";
        delete this;
        return;
    }

    foreach (DM::Module * m, this->simulation->getModules()) {
        if (m->getUuid().compare(name.toStdString()) == 0) {
            g = (DM::Group *) m;
        }
    }
    if (g == 0)
        return;

    if (this->getDMModel()->getGroup()->getUuid().compare(g->getUuid()) == 0) {
        return;
    }

    this->getDMModel()->setGroup(g);
    this->getSimulation()->GUIaddModule(this->getDMModel(), this->pos());
    //Set VIBeModule empty otherwise destructor deletes DynaMind module
    this->VIBeModuleUUID = "";
    delete this;




}

void ModelNode::printData() {

    GUIViewDataForModules * gv = new GUIViewDataForModules(this->getDMModel());
    gv->show();

    
}

void ModelNode::viewData() {
    //TODO hook(er) me up
    DM::Port *p = this->getDMModel()->getOutPorts()[0];
    cout << "port: "<< p->getLinkedDataName() << endl;
    DM::System *system = this->getDMModel()->getData(p->getLinkedDataName());
    
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();
}

void ModelNode::showHelp() {

    this->simulation->showHelp(this->getDMModel()->getClassName());


}


void ModelNode::setSuccessors(QStringList list)
{
    successors=list;
}

QStringList ModelNode::getSuccessors()
{
    return successors;
}

bool ModelNode::isSuccessor(QString successor)
{

    return successors.contains(successor);
}
