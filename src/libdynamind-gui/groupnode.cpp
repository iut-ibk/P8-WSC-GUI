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
#include "groupnode.h"
#include <linknode.h>
#include <moduledescription.h>
#include <QGraphicsDropShadowEffect>
#include <modelnodebutton.h>
#include <ColorPalette.h>

#include <dmgroup.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <guiport.h>
#include <dmporttuple.h>
#include <dmgroup.h>
#include <iostream>
#include <cmath>
#include "dmmainwindow.h"
#include "dmviewerwindow.h"


GroupNode::GroupNode()
{

}
GroupNode::~GroupNode() {


    this->OutputTuplePorts.clear();
    this->InPortTuplePorts.clear();


    DM::Logger(DM::Debug) << "Remove GroupNode " << this->UUID;

    emit removeGroupNode(QString::fromStdString(this->UUID));

}


void GroupNode::viewData()
{
    DM::Group* g=(DM::Group*)this->getDMModel();
    DM::Port* p=g->getOutPortTuples().at(0)->getInPort();
    cout << "port: "<< p->getLinkedDataName() << endl;
    cout << "g: "<< g << endl;
    DM::System *system = g->getData(p->getLinkedDataName());
    cout << "system: "<< system << endl;
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();


    /*
    DM::Port *p = this->getDMModel()->getOutPorts()[0];
    DM::System *system = this->getDMModel()->getData(p->getLinkedDataName());
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();
    */
}


void GroupNode::removeModelNode(ModelNode *m) {
    int index = childnodes.indexOf(m);
    this->childnodes.remove(index);
}

void GroupNode::changeGroupID(QString Name) {
    this->getName() = Name;
}
void GroupNode::minimize() {
    this->minimized = true;
    foreach(ModelNode * m, this->childnodes) {
        if (m->isGroup()) {
            GroupNode * g = (GroupNode*) m;
            g->setMinimized( true );
            g->minimize();
        } else {
            m->setMinimized( true );
            m->update();
        }
    }
    recalculateLandH();
    if(this->scene() != 0)
        this->scene()->update();

}


void GroupNode::maximize() {
    this->minimized = false;
    foreach(ModelNode * m, this->childnodes) {
        if (m->isGroup()) {
            GroupNode * g = (GroupNode*) m;
            //g->maximize();
            g->setMinimized (false);
        } else {
            m->setMinimized (false);
            m->update();
        }
    }
    recalculateLandH();
    if(this->scene() != 0)
        this->scene()->update();


}

void GroupNode::updatePorts () {
    DM::Group * g = (DM::Group*)this->getDMModel();

    foreach (DM::PortTuple * p,g->getInPortTuples()){
        this->addTuplePort(p);

    }
    foreach (DM::PortTuple * p,g->getOutPortTuples()){
        this->addTuplePort(p);

    }
    ModelNode::updatePorts();
}

void GroupNode::addTuplePort(DM::PortTuple * p) {

    //Inport
    if  (p->getPortType() > DM::OUTPORTS) {
        foreach (QString pname, ExistingInPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }
        ExistingInPorts << QString::fromStdString(p->getName());
        GUIPort * gui_p = new  GUIPort(this, p->getInPort());
        this->ports.append(gui_p);
        gui_p->setPos(0,gui_p->boundingRect().height()*this->inputCounter++);

    } else {//Outport
        foreach (QString pname, ExistingOutPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }
        ExistingOutPorts << QString::fromStdString(p->getName());

        GUIPort * gui_p = new  GUIPort(this,p->getOutPort());
        this->ports.append(gui_p);
        gui_p->setPos( this->boundingRect().width(),gui_p->boundingRect().height()*this->outputCounter++);

    }


}
GUIPort *  GroupNode::getGUIPort(DM::Port * p) {
    foreach(GUIPortTuple * gui_pt,this->OutputTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }
    foreach(GUIPortTuple * gui_pt,this->InPortTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
    }



    return ModelNode::getGUIPort( p);

    return 0;
}

void GroupNode::removeTuplePort(int Type, QString s) {

}

GroupNode::GroupNode(  DM::Module *module, GUISimulation * s): ModelNode( module, s)
{

    this->childnodes = QVector<ModelNode*>();
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
    this->UUID = module->getUuid();


    this->outputCounter = 1;
    this->inputCounter = 1;
    this->setZValue(-1);
    this->setGraphicsEffect(new  QGraphicsDropShadowEffect(this));
    //this->setVisible(false);

    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem (QString::number(id));
    double w = this->simpleTextItem->boundingRect().width()+40;
    w = w < 80 ? 80 : w;
    l = w+4;
    h =  this->simpleTextItem->boundingRect().height()+65;



    Color = COLOR_MODULE;

    this->updatePorts();

}

void GroupNode::RePosTuplePorts() {
    foreach(GUIPort * gui_p, this->ports) {
        if (gui_p->getPortType()  < DM::OUTPORTS) {
            gui_p->setPos(this->boundingRect().width(), gui_p->pos().y());
        }
    }

}
void GroupNode::setMySelected(  bool selected ) {
    foreach(ModelNode * m, this->childnodes) {
        m->setMySelected(true);
        if (m->isGroup()) {
            GroupNode * g = (GroupNode *) m;
            g->setMySelected(selected);
        }
    }

    ModelNode::setMySelected(selected);
}

void GroupNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    if (this->visible ) {
        recalculateLandH();
        if(this->isSelected()== true) {
            painter->setBrush(Qt::gray);

        } else {
            painter->setBrush(Qt::white);
        }
        if (h< 65)
            h = 65;
        //QString name=QString("Name:")+ QString::fromStdString(this->getDMModel()->getName())+" " +QString::number(this->zValue());
//        QString name="Baseline";
         QString name=this->getName();
        this->simpleTextItem->setText(name);
        float lold = l;
        if (simpleTextItem->boundingRect().width()+140 > l)
            l = simpleTextItem->boundingRect().width()+140;
 //        painter->drawRect(0, 0, l,h);
        painter->drawEllipse(0, 0, l, h);


        // New connections
        /*
        for (int i=0;i<360;i=i+45)
        {
            int dx=l/2+int(double(l)/2*cos(3.1416*i/180))-5;
            int dy=h/2+int(double(h)/2*sin(3.1416*i/180))-5;
            painter->drawEllipse(dx,dy,10,10);
        }
*/
        if (this->childnodes.size() > 0)
            this->setPos(x1-40, y1-20);

        painter->drawText(QPoint(55,20), name);

        if (lold != l)
            RePosFlag = true;
        if(RePosFlag) {
            RePosTuplePorts();
            RePosFlag = false;
        }

    }

}
QRectF GroupNode::boundingRect() const {
    return QRect(0, 0, l, h);

}







