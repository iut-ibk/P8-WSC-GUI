#include "wizard.h"
#include "ui_wizard.h"
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmmodulelink.h>
#include <guilink.h>
#include "dmport.h"
#include "guiport.h"
#include "modelnode.h"

wizard::wizard(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wizard)
{
    ui->setupUi(this);
}

wizard::~wizard()
{
    delete ui;
}

DM::Module *wizard::createModule(QString name, QPointF pos)
{
    DM::Module *m;
    m = this->sim->addModule(name.toStdString());
    m->init();
    m->setName(name.toStdString());
    this->sim->GUIaddModule(m,pos);
    return m;
}


void wizard::on_pushButton_released()
{
    DM::Module *micro, *import;
    micro = createModule("Average Summer Heat", QPointF(0,-100));
    import = createModule("Import Land Cover Map", QPointF(-400,0));
    //this->sim->addLink( import->getOutPort("City"),micro->getInPort("City"));

    GUILink * tmp_link = new GUILink();

    ModelNode * mmicro = new ModelNode(micro, this->sim);
    ModelNode * mimport = new ModelNode(import, this->sim);
    DM::ModuleLink *l = this->sim->addLink( import->getOutPort("Data"),micro->getInPort("City"));


    tmp_link->setOutPort(mimport->getGUIPort(import->getOutPort("Data")));
    tmp_link->setInPort(mmicro->getGUIPort(micro->getInPort("City")));


    //Create Link
    tmp_link->setVIBeLink(l);
    tmp_link->setSimulation(this->sim);

    this->pv->addItem(tmp_link);
    this->pv->update();
    //Run Simulation
    //this->sim->updateSimulation();

//    todo with link copied from dmmainwindow.cpp
//    gui_link->setOutPort(outmodule->getGUIPort(l->getOutPort()));
//    gui_link->setInPort(inmodule->getGUIPort(l->getInPort()));
//    gui_link->setVIBeLink(l);
//    gui_link->setSimulation(this->simulation);

//    currentView->addItem(gui_link);
//    currentView->update();

}

void wizard::setSimulation(GUISimulation *sim)
{
    this->sim = sim;
}

void wizard::setScene(ProjectViewer *pv)
{
    this->pv = pv;
}
