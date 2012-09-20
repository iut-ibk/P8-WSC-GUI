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
#include "rootgroupnode.h"
#include <linknode.h>
#include <moduledescription.h>
#include <QGraphicsDropShadowEffect>

#include <ColorPalette.h>

#include <dmgroup.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <guiport.h>
#include <dmporttuple.h>
#include <dmgroup.h>
#include <iostream>
#include <groupnode.h>


RootGroupNode::RootGroupNode()
{

}
RootGroupNode::~RootGroupNode() {

    DM::Logger(DM::Debug) << "Remove RootGroupNode ";

    if (this->getDMModel() == 0)
        return;
    while (this->childnodes.size() > 0)
        delete this->childnodes[0];

    this->OutputTuplePorts.clear();
    this->InPortTuplePorts.clear();




}


void RootGroupNode::removeModelNode(ModelNode *m) {
    int index = childnodes.indexOf(m);
    if (index > -1)
        this->childnodes.remove(index);
}

void RootGroupNode::changeGroupID(QString Name) {
    this->getName() = Name;
}



void RootGroupNode::updatePorts () {
    DM::Group * g = (DM::Group*)this->getDMModel();

    foreach (DM::PortTuple * p,g->getInPortTuples()){
        this->addTuplePort(p);

    }
    foreach (DM::PortTuple * p,g->getOutPortTuples()){
        this->addTuplePort(p);

    }
    ModelNode::updatePorts();
}

void RootGroupNode::addTuplePort(DM::PortTuple * p) {

    //Inport
    if  (p->getPortType() > DM::OUTPORTS) {
        foreach (QString pname, ExistingInPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }

        ExistingInPorts << QString::fromStdString(p->getName());
        GUIPort * gui_p = new  GUIPort(this, p->getOutPort());
        this->ports.append(gui_p);
        gui_p->setPos(0,gui_p->boundingRect().height()*this->inputCounter++);

        //Outport
    } else {
        foreach (QString pname, ExistingOutPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }
        ExistingOutPorts << QString::fromStdString(p->getName());
        GUIPort *gui_p = new  GUIPort(this, p->getInPort());
        this->ports.append(gui_p);
        gui_p->setPos(  l,gui_p->boundingRect().height()*this->outputCounter++);
    }


}
GUIPort *  RootGroupNode::getGUIPort(DM::Port * p) {
    foreach(GUIPortTuple * gui_pt,this->OutputTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }
    foreach(GUIPortTuple * gui_pt,this->InPortTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }



    return ModelNode::getGUIPort( p);

    return 0;
}

void RootGroupNode::removeTuplePort(int Type, QString s) {

}

RootGroupNode::RootGroupNode(  DM::Module *module, GUISimulation * s): ModelNode( module, s)
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


    this->outputCounter = 1;
    this->inputCounter = 1;
    this->setZValue(-1);
    this->setGraphicsEffect(new  QGraphicsDropShadowEffect(this));
    this->setVisible(true);

    this->setFlag(QGraphicsItem::ItemIsSelectable, false);
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem (QString::number(id));
    double w = this->simpleTextItem->boundingRect().width()+40;
    w = w < 140 ? 140 : w;
    l = w+4;
    h =  this->simpleTextItem->boundingRect().height()+65;
    Color = COLOR_MODULE;
    updatePorts();
}

void RootGroupNode::RePosTuplePorts() {
    foreach(GUIPort * p, this->ports) {
        if (p->getPortType() > DM::OUTPORTS) {
            p->setPos(l, p->pos().y());
        }
    }
}
/*
void RootGroupNode::setMySelected(  bool selected ) {
    foreach(ModelNode * m, this->childnodes) {
        m->setMySelected(true);
        if (m->isGroup()) {
            GroupNode * g = (GroupNode *) m;
            g->setMySelected(selected);
        }
    }
    ModelNode::setMySelected(selected);

//    QGraphicsItem::setSelected ( selected );


}
*/
void RootGroupNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
/*
    QString name;
    //DM::Group;
    this->setGroupZValue();

    recalculateLandH();

    if(this->isSelected()== true) {
        painter->setBrush(Qt::gray);

    } else {
        painter->setBrush(Qt::white);
    }
    this->simpleTextItem->setText("Name:"+ QString::fromStdString(this->getDMModel()->getName()));
    if (simpleTextItem->boundingRect().width()+40 > l)
        l = simpleTextItem->boundingRect().width()+40;
    //    painter->drawRect(0, 0, l,h);

    this->setPos(x1-40, y1-20);

    //    painter->drawText(QPoint(5,15), "Name:"+ QString::fromStdString(this->getDMModel()->getName()));


    if((RePosFlag) != 0) {
        RePosTuplePorts();
        RePosFlag = false;
    }
*/


}
QRectF RootGroupNode::boundingRect() const {
    return QRect(-100, -100, l+200, h+200);

}

void RootGroupNode::addModelNode(ModelNode *m) {
    this->childnodes.push_back(m);
    //DM::Module * m1 = m->getDMModel();
    m->getDMModel()->setGroup((DM::Group *)this->getDMModel());
    m->setParentGroup(this);
    //    m->paint(0,0);
    //this->recalculateLandH();
    //this->update();
}

void RootGroupNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
    //this->setMySelected(true);
    /*
    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
        this->parentGroup->update();
    }

    ModelNode::QGraphicsItem::mouseMoveEvent(event);*/
}


void  RootGroupNode::setGroupZValue() {
    /*
    DM::Logger(DM::Debug) << "Set Z Level";
    if (this->parentGroup != 0) {
        DM::Logger(DM::Debug) << "ParentGroup Z Level" << this->parentGroup->zValue();
        if (this->parentGroup->zValue()+1 != this->zValue())
            this->setZValue(this->parentGroup->zValue()+1);
    }
    foreach (ModelNode * m, this->childnodes ){
        DM::Module * m1 = m->getDMModel();
        if(m->getDMModel()->isGroup()) {
            GroupNode * g = (GroupNode * ) m;
            //g->setGroupZValue();
        } else {
            if (m->zValue() != this->zValue()+1)
                m->setZValue(this->zValue()+1);
        }
    }*/
}

void RootGroupNode::recalculateLandH() {
    /*
    float lold = l;


    this->setFlag(QGraphicsItem::ItemIsMovable, true);


    if (this->childnodes.size() > 0) {
        x1 = childnodes[0]->sceneBoundingRect().x();
        y1 = childnodes[0]->sceneBoundingRect().y();
        x2 = childnodes[0]->sceneBoundingRect().x() + childnodes[0]->sceneBoundingRect().width();
        y2 = childnodes[0]->sceneBoundingRect().y() + childnodes[0]->sceneBoundingRect().height();
        for(int i = 0; i < childnodes.size(); i++) {
            ModelNode * m = childnodes.at(i);
            if (m->sceneBoundingRect().x() < x1)
                x1 = m->sceneBoundingRect().x();
            if (m->sceneBoundingRect().y() < y1)
                y1 = m->sceneBoundingRect().y();
            if (m->sceneBoundingRect().x()+m->sceneBoundingRect().width() > x2)
                x2 = m->sceneBoundingRect().x()+m->sceneBoundingRect().width();
            if (m->sceneBoundingRect().y()+m->sceneBoundingRect().height() > y2)
                y2 = m->sceneBoundingRect().y()+m->sceneBoundingRect().height();
        }
    }

    if(!this->minimized) {
        l = x2-x1+80;
        h =  y2-y1+40;
    } else {
        l = 100;
        h = 85;
    }
    if((lold - l) != 0) {
        RePosFlag = true;
        this->prepareGeometryChange();
        this->update(this->boundingRect());
    }
    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
    }
*/
}
