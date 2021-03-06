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
#include "guimodelnode.h"
#include <iostream>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVariant>
#include <QInputDialog>
#include "moduledescription.h"
#include <QGroupBox>
#include <QStringList>
#include "linknode.h"
#include <QScrollArea>
#include <QComboBox>
#include <QMap>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QWebView>
#include <QUrl>
#include <dmmodule.h>
#include <dmgroup.h>
#include <groupnode.h>
#include <dmporttuple.h>
#include <QTextEdit>


void GUIModelNode::openFileDialog() {
    QString s = QFileDialog::getOpenFileName(this,
                                             tr("Open file"), "", tr("Files (*.*)")) ;
    if (!s.isEmpty())
        emit selectFiles (s);
}


GUIModelNode::GUIModelNode(DM::Module * m, ModelNode *mn, QWidget* parent) :QWidget(parent)
{
    this->module = m;
    this->modelnode = mn;

    QGridLayout *layout = new QGridLayout;

    layout1 = new QGridLayout;
    QGroupBox *gbox = new QGroupBox;
    gbox->setTitle("Parameter");
    std::map<std::string, int> parameter = m->getParameterList();
    std::vector<std::string> NameList = m->getParameterListAsVector();
    foreach(std::string name, NameList){
        int ID = parameter[name];//it->second;
        if (ID == DM::DOUBLE || ID == DM::LONG || ID == DM::INT) {
            QLabel * l = new QLabel;
            QLineEdit * le = new QLineEdit;
            QCheckBox * cb = new QCheckBox("from Outside");
            double val = -1;
            QString s  = QString().fromStdString(name);
            std::string s_tmp = name;

            if (ID == DM::DOUBLE)
                val = m->getParameter<double>(s_tmp);
            if (ID == DM::LONG)
                val = m->getParameter<long>(s_tmp);
            if (ID == DM::INT)
                val = m->getParameter<int>(s_tmp);

            QString numberAsText = QString::number(val, 'g', 15);
            l->setText(s);
            le->setText(numberAsText);
            elements.insert(s, le);
            QString s1;
            s1= "InputDouble|DoubleIn_"+ s;
            cb->setObjectName(s1);
            layout1->addWidget(l, layout1->rowCount(),0);
            layout1->addWidget(le,layout1->rowCount()-1,1);
            layout1->addWidget(cb,layout1->rowCount()-1,2);


        }
        if ( ID == DM::BOOL) {
            std::string s_tmp = name;
            QString s  = QString::fromStdString(name);
            bool val  =  m->getParameter<bool>(s_tmp);
            QLabel * l = new QLabel;
            QCheckBox * le = new QCheckBox;
            le->setChecked(val);
            l->setText(s);
            elements.insert(s, le);

            layout1->addWidget(l, layout1->rowCount(),0);
            layout1->addWidget(le,layout1->rowCount()-1,1);
        }
        if ( ID == DM::STRING) {
            std::string s_tmp = name;
            QString s  = QString::fromStdString(name);
            QString val  = QString::fromStdString( m->getParameter<std::string>(s_tmp));
            QLabel * l = new QLabel;
            QLineEdit * le = new QLineEdit;
            le->setText(val);
            l->setText(s);
            elements.insert(s, le);

            layout1->addWidget(l, layout1->rowCount(),0);
            layout1->addWidget(le,layout1->rowCount()-1,1);
        }
        if ( ID == DM::FILENAME) {
            std::string s_tmp = name;
            QString s  = QString().fromStdString(name);
            QLabel * l = new QLabel;
            QLineEdit * le = new QLineEdit;
            QPushButton * pb = new QPushButton;
            le->setText(QString::fromStdString(m->getParameter<std::string>(s_tmp)));
            l->setText(s);
            pb->setText("...");
            elements.insert(s, le);

            layout1->addWidget(l, layout1->rowCount(),0);
            layout1->addWidget(le,layout1->rowCount()-1,1);
            layout1->addWidget(pb,layout1->rowCount()-1,2);

            connect(pb, SIGNAL(clicked()), this, SLOT(openFileDialog()));
            connect(this, SIGNAL(selectFiles(QString)), le, SLOT(setText(QString)));
        }
        if (ID == DM::STRING_MAP ) {
            QString s  = QString().fromStdString(name);
            QGroupBox * box= new QGroupBox;
            QGridLayout * layout_grid = new QGridLayout;
            box->setLayout( layout_grid);

            box->setTitle(s);
            QPushButton * pb = new QPushButton;
            pb->setText("+");
            QString s1;
            s1 = "UserDefindedTupleItem|" + s;
            pb->setObjectName(s1);
            connect(pb, SIGNAL(clicked()), this, SLOT(addUserDefinedTuple()));
            layout_grid->addWidget(pb, 0,0,1,3);
            layout1->addWidget(box, layout1->rowCount(),0,1,3);
            std::string stds = s.toStdString();
            std::map<std::string, std::string> entries;
            entries = this->module->getParameter<std::map<std::string, std::string> >(stds);

            for (std::map<std::string, std::string>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
                QLabel * l1 = new QLabel;
                QLineEdit * l = new QLineEdit;
                l->setText(QString::fromStdString((std::string) it->second));
                l1->setText(QString::fromStdString((std::string) it->first));
                layout_grid->addWidget(l1, layout_grid->rowCount(),0);
                layout_grid->addWidget(l, layout_grid->rowCount()-1,1);
                QString n = s + "|" + QString::fromStdString((std::string) it->first);
                this->elements.insert(n, l);
            }
            this->UserDefinedContainer.insert(s, layout_grid);




        }


    }


    gbox->setLayout(layout1);
    layout->addWidget(gbox);



    QDialogButtonBox * bbox  = new QDialogButtonBox(QDialogButtonBox::Ok
                                                    | QDialogButtonBox::Cancel | QDialogButtonBox::Help);

    layout->isWidgetType();
    QScrollArea *bar = new QScrollArea;
    bar->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    bar->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    bar->setWidgetResizable(true);
    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    bar->setWidget(widget);
    QVBoxLayout * l = new QVBoxLayout;
    l->addWidget(bar);

    l->addWidget(bbox);

    setLayout(l);
    //setLayout(layout);
    connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(bbox, SIGNAL(helpRequested ()), this, SLOT(help()));
}
void GUIModelNode::help() {


    QWidget * helpWindow = new QWidget(this->parentWidget());
    QTextEdit * text = new QTextEdit();
    //text->setText(QString::fromStdString(this->module->generateHelp()));
    //QWebView * help = new QWebView;

    //help->setUrl(url);
    //help->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    QVBoxLayout * vbox = new QVBoxLayout;

    helpWindow->setLayout(vbox);
    helpWindow->layout()->addWidget(text);
    helpWindow->show();
}

GUIModelNode::~GUIModelNode() {

}

void GUIModelNode::addTuplePort() {
    QString text = QInputDialog::getText(this, "Input", "");
    QStringList ls;
    if (!text.isEmpty()){
        QString s = QObject::sender()->objectName();
        QStringList ls1 = s.split("|");
        QString TypeInfo = ls1[0];
        if (TypeInfo.compare("InputTupleRaster") == 0 || TypeInfo.compare("InputTupleVector") == 0 || TypeInfo.compare("InputTupleDouble") == 0) {
            DM::Group * g = (DM::Group *) this->module;
            QString s =text;
            QLabel * l = new QLabel;
            QPushButton * delp = new QPushButton ;

            delp->setObjectName(ls1[0] + "|"  + s);
            delp->setText("-");
            l->setText(s);
            connect(delp, SIGNAL(clicked()), this, SLOT(removeUserDefinedItem()));
            this->UserDefinedContainer[ ls1[1]]->addWidget(l, this->UserDefinedContainer[ ls1[1]]->rowCount(),1);
            this->UserDefinedContainer[ ls1[1]]->addWidget(delp, this->UserDefinedContainer[ ls1[1]]->rowCount()-1,0);
            GroupNode * gn = (GroupNode * )this->module;

            DM::PortTuple * pt;

            gn->addTuplePort(pt);
        }
    }
}
void GUIModelNode::addUserDefinedItem() {
    QString text = QInputDialog::getText(this, "Input", "");
    QStringList ls;

    if (!text.isEmpty()){

        QString s = QObject::sender()->objectName();
        QStringList ls1 = s.split("|");
        std::string stds = ls1[1].toStdString();

        if (ls.indexOf(text) < 0) {

            ls.append(text);
            // if (!this->modelNode->isGroup()) {
            DM::Logger(DM::Debug) << "addUserDefinedItem" << text.toStdString();
            //this->module->appendToUserDefinedParameter(stds,text.toStdString());

            if (this->modelnode != 0)
                this->modelnode->updatePorts();
            // }


            QLabel * l = new QLabel;
            l->setText(text);
            QPushButton * delp = new QPushButton;
            delp->setObjectName(ls1[1] + "|" + text);
            delp->setText("-");
            connect(delp, SIGNAL(clicked()), this, SLOT(removeUserDefinedItem()));
            QGridLayout * g = this->UserDefinedContainer.value(ls1[1]);
            g->addWidget(l,g->rowCount() ,1);
            g->addWidget(delp,g->rowCount()-1 ,0);

        } else{
            QMessageBox msgBox;
            msgBox.setText("Already Exists");
            msgBox.exec();
        }
    }
}
void GUIModelNode::addUserDefinedDoubleItem() {
    int Type = 0;
    QCheckBox * b = (QCheckBox *)QObject::sender();
    QString s =b->objectName();
    QStringList ls1 = s.split("|");
    if (ls1[0].compare("InputDouble") == 0)
        Type = INDOUBLE;
    if (b->checkState ()) {
        if (Type != 0) {
            /*this->module->appendToUserDefinedParameter(ls1[0].toStdString(),ls1[1].toStdString());
            if (this->modelnode != 0)
                this->modelnode->updatePorts();*/
        }
    } else {
        //this->modelNode->removePort(Type, ls1[1]);
    }



}

void GUIModelNode::removeUserDefinedItem() {

    QString s = QObject::sender()->objectName();
    QStringList ls1 = s.split("|");

    //this->modelNode->appendToUserDefinedParameter(stds,text.toStdString());
    //this->modelNode->updatePorts();
    std::string stds = ls1[1].toStdString();

    //DM::Logger(DM::Debug) << "addUserDefinedItem" << text.toStdString();
    //this->module->removeFromUserDefinedParameter(ls1[0].toStdString(),ls1[1].toStdString());
    //this->modelNode->updatePorts();

    QPushButton * p= (QPushButton * ) QObject::sender();
    QGridLayout * g =  (QGridLayout *) p->parentWidget()->layout();


    int currentRow = -1;

    for (int i = 0; i < g->rowCount(); i++) {
        for (int j = 0; j < g->columnCount(); j++) {

            if (g->itemAtPosition(i,j) != 0) {
                if (g->itemAtPosition(i,j)->widget() == p)
                    currentRow = i;
            }
        }
    }
    for (int j = 0; j < g->columnCount(); j++) {

        if (g->itemAtPosition(currentRow,j) != 0) {
            delete g->itemAtPosition(currentRow,j)->widget();
        }
    }

}

void GUIModelNode::removeUserDefinedTuple() {
    QString s = QObject::sender()->objectName();
    QStringList ls1 = s.split("|");
    QVariant va = this->moduleDescription->paramterValues.value(ls1[0]);
    QStringList options = this->moduleDescription->parameterOptions.value(ls1[0]);
    QMap<QString, QVariant>  ms =  va.value<QMap<QString, QVariant> >();
    QString name = ls1[1];
    if (options.indexOf("withRulePrefix") >= 0) {
        name = "Rule_"+ name;
    }

    ms.remove(name);
    this->elements.remove(name);
    QPushButton * p= (QPushButton * ) QObject::sender();
    QGridLayout * g =  (QGridLayout *) p->parentWidget()->layout();


    int currentRow = -1;

    for (int i = 0; i < g->rowCount(); i++) {
        for (int j = 0; j < g->columnCount(); j++) {

            if (g->itemAtPosition(i,j) != 0) {
                if (g->itemAtPosition(i,j)->widget() == p)
                    currentRow = i;
            }
        }
    }
    for (int j = 0; j < g->columnCount(); j++) {
        if (g->itemAtPosition(currentRow,j) != 0) {
            delete g->itemAtPosition(currentRow,j)->widget();


        }
    }


    this->moduleDescription->paramterValues[ls1[0]] = ms;

}

void GUIModelNode::addUserDefinedTuple() {

    QString text = QInputDialog::getText(this, "Input", "");
    if (!text.isEmpty()){
        QString s = QObject::sender()->objectName();
        QString name = s.split("|")[1] + "|" + text;

        QLabel * l = new QLabel;
        l->setText(text);
        QGridLayout * g = this->UserDefinedContainer.value(s.split("|")[1]);
        g->addWidget(l, g->rowCount(),0);
        QLineEdit * le = new QLineEdit;

        g->addWidget(le, g->rowCount()-1,1, 1, 2);


        this->elements.insert(name, le);



    }


}

void GUIModelNode::accept() {

    map<std::string, int> parameter = this->module->getParameterList();


    foreach(QString s,  this->elements.keys()) {
        int ID = parameter[s.split("|")[0].toStdString()];//it->second;
        QAbstractButton * ab;
        QLineEdit * le;
        std::map<std::string, std::string> map;
        switch (ID)
        {
        case (DM::BOOL):
            ab = ( QAbstractButton * ) this->elements.value(s);
            this->module->setParameterNative(s.toStdString(), ab->isChecked());
            break;
        case (DM::STRING_MAP):
            le = ( QLineEdit * ) this->elements.value(s);
            map = this->module->getParameter<std::map<std::string, std::string> > (s.split("|")[0].toStdString());
            map[s.split("|")[1].toStdString()] = le->text().toStdString();
            this->module->setParameterNative(s.split("|")[0].toStdString(), map);
            break;
        default:
            le = ( QLineEdit * ) this->elements.value(s);
            this->module->setParameterValue(s.toStdString(), le->text().toStdString());
            break;
        }




    }

    delete(this);
}
void GUIModelNode::reject() {
    delete(this);
}
