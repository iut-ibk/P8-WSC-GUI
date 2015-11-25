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
    this->sim->GUIaddModule(m,pos);
    return m;
}


void wizard::on_pushButton_released()
{   DM::Module *micro, *import;
    micro = createModule("Average Summer Heat", QPointF(0,-100));
    import = createModule("Import Land Cover Map", QPointF(-400,0));
    GUILink * tmp_link = new GUILink();
    ModelNode * mmicro = new ModelNode(micro, this->sim);
    ModelNode * mimport = new ModelNode(import, this->sim);

    tmp_link->setInPort(mimport->getGUIPort(new DM::Port(import,import->getOutPort("City")->getPortType(),import->getOutPort("City")->getLinkedDataName())));//new GUIPort(mimport,new DM::Port(import,import->getOutPort("City")->getPortType(),import->getOutPort("City")->getLinkedDataName())));
    tmp_link->setOutPort(mmicro->getGUIPort(new DM::Port(micro,micro->getInPort("City")->getPortType(),micro->getInPort("City")->getLinkedDataName())));//new GUIPort(mmicro,new DM::Port(micro,micro->getInPort("City")->getPortType(),micro->getInPort("City")->getLinkedDataName())));
    tmp_link->setVIBeLink(this->sim->addLink(import->getOutPort("City"),micro->getInPort("City")));
    tmp_link->setSimulation(this->sim);
    tmp_link = 0;
    //tmp_link->setVIBeLink(l);//this->sim->addLink(import->getOutPort("City"),micro->getInPort("City")));
    //tmp_link->setSimulation(this->sim);
    //tmp_link = 0;
    //this->sim->updateSimulation();

}

void wizard::setSimulation(GUISimulation *sim)
{
    this->sim = sim;
}
