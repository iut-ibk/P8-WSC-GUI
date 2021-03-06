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

#ifndef GUISIMULATION_H
#define GUISIMULATION_H

#include <dmsimulation.h>
#include <QVector>
#include <QString>
#include <QPointF>

class ModelNode;
class GroupNode;
class GUILink;
class GUISimulation :  public DM::Simulation
{
    Q_OBJECT
public:
    GUISimulation();
    void registerRootNode();
    GroupNode * getGroupNode(DM::Group * g);
    void changeGroupName(GroupNode*);
    void clearSimulation();

private:
    QVector<ModelNode*> modelNodes;
    QVector<GroupNode*> groupNodes;

signals:
    void addedModule(ModelNode*);
    void addedGroup(GroupNode*);
    void GroupNameChanged(GroupNode*);
    void showHelpForModule(std::string);


public slots:
    void GUIaddModule( QString  name, QPointF pos, DM::Module *group);
    void GUIaddModule(DM::Module *, QPointF pos);
    void updateSimulation();
    void showHelp(std::string classname);



};

#endif // GUISIMULATION_H

