#include "wizard.h"
#include "ui_wizard.h"
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmmodulelink.h>
#include <guilink.h>
#include "dmport.h"
#include "guiport.h"
#include "modelnode.h"
#include "rootgroupnode.h"

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

bool wizard::createLink( DM::Port *OutPort, DM::Port *InPort)
{
    DM::ModuleLink *l = this->sim->addLink( OutPort,InPort);
    DM::Module *predecessor = OutPort->getModule();
    DM::Module *successor = InPort->getModule();
    ModelNode *guimpredecessor = 0;
    ModelNode *guimsuccessor = 0;

    QVector<ModelNode * > mnvector = this->pv->getRootNode()->getChildNodes();

    foreach (ModelNode * mn, mnvector)
    {
        if (mn->getDMModel() == predecessor)
            guimpredecessor = mn;

        if (mn->getDMModel() == successor)
            guimsuccessor = mn;
    }

    if (!guimpredecessor || !guimsuccessor)
        return false;

    GUILink * gui_link = new GUILink();
    gui_link->setOutPort(guimpredecessor->getGUIPort(OutPort));
    gui_link->setInPort(guimsuccessor->getGUIPort(InPort));

    gui_link->setVIBeLink(l);
    gui_link->setSimulation(this->sim);

    this->pv->addItem(gui_link);
    this->pv->update();
    this->sim->updateSimulation();

    return true;
}


void wizard::on_pushButton_released()
{
    DM::Module *micro, *import;
    micro = createModule("Average Summer Heat", QPointF(0,-100));
    import = createModule("Import Land Cover Map", QPointF(-400,0));

    if(!createLink(import->getOutPort("Data"),micro->getInPort("City")))
        std::cout << "Place error message here" << std::endl;
}

void wizard::setSimulation(GUISimulation *sim)
{
    this->sim = sim;
}

void wizard::setScene(ProjectViewer *pv)
{
    this->pv = pv;
}
