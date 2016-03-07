#include "wizardq.h"
#include "ui_wizardq.h"
#include "intropage.h"
#include "microclimatepage.h"
#include "stormwaterpage.h"
#include "endpage.h"
#include "QDebug"
#include "guisimulation.h"
#include "rootgroupnode.h"
#include <guilink.h>

Wizardq::Wizardq(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::Wizardq)
{
    setWizardStyle(QWizard::ClassicStyle);
    setPage(0,new IntroPage(this));
    setPage(1,new MicroclimatePage(this));
    setPage(2,new StormwaterPage(this));
    setPage(3,new endpage(this));

    importLandcover = false;
    importMsf = false;
    mcExtreme = false;
    mcAverage = false;
    streamErosion = false;
    streamHydrology = false;
    Treatment = false;
    Rain = false;
    Eco = false;
    connect( this, SIGNAL( accepted() ), this, SLOT( buildSimulation() ), Qt::DirectConnection );

    ui->setupUi(this);
}

Wizardq::~Wizardq()
{
    delete ui;
}

void Wizardq::setImportLandcover()
{
    this->importLandcover = true;
}

void Wizardq::setImportMsf()
{
    this->importMsf = true;
}

void Wizardq::setMcExtreme()
{
    this->mcExtreme = true;
}

void Wizardq::setMcAverage()
{
    this->mcAverage = true;
}

void Wizardq::setStreamErosion()
{
    this->streamErosion = true;
}

void Wizardq::setStreamHydrology()
{
    this->streamHydrology = true;
}

void Wizardq::setTreatment()
{
    this->Treatment = true;
}
void Wizardq::setRain()
{
    this->Rain = true;
}

void Wizardq::setEco()
{
    this->Eco = true;
}

void Wizardq::unsetMcExtreme()
{
    this->mcExtreme = false;
}

void Wizardq::unsetMcAverage()
{
    this->mcAverage = false;
}

void Wizardq::unsetStreamErosion()
{
    this->streamErosion = false;
}

void Wizardq::unsetStreamHydrology()
{
    this->streamHydrology = false;
}

void Wizardq::unsetTreatment()
{
    this->Treatment = false;
}

void Wizardq::unsetRain()
{
    this->Rain = false;
}

void Wizardq::setAll()
{
    setImportLandcover();
    setImportMsf();
    setMcExtreme();
    setMcAverage();
    setStreamErosion();
    setStreamHydrology();
    setTreatment();
    setRain();
    setEco();
}

void Wizardq::unsetAll()
{
    this->importLandcover = false;
    this->importMsf = false;
    this->mcExtreme = false;
    this->mcAverage = false;
    this->streamErosion = false;
    this->streamHydrology = false;
    this->Treatment = false;
    this->Rain = false;
    this->Eco = false;
}

void Wizardq::buildSimulation()
{
    DM::Module *impLand, *impMsf, *mcExtr, *mcAver, *streamEro, *streamHydro, *Treat, *Ra, *Ec,*Analyser;

    if(importLandcover){
        impLand = createModule("Import Land Cover Map",QPointF(-400,-600));
    }
    if(importMsf){
        impMsf = createModule("Import WSUD Layout (.msf)",QPointF(-600,600));
        Analyser = createModule("Analyser",QPointF(800,600));
    }
    if(mcExtreme){
        mcExtr = createModule("Extreme Heat",QPointF(0,-800));
    }
    if(mcAverage){
        mcAver = createModule("Average Summer Heat",QPointF(0,-400));
    }
    if(streamErosion){
        streamEro = createModule("Stream Erosion and Minor Flooding",QPointF(0,300));
    }
    if(streamHydrology){
        streamHydro = createModule("Stream Hydrology and Water Quality",QPointF(0,600));
    }
    if(Treatment){
        Treat = createModule("Treatment and Harvesting Performance",QPointF(0,900));
    }
    if(Rain){
        Ra = createModule("Future Rainfall (Test Version)",QPointF(-250,650));
    }
    if(Eco){
        Ec = createModule("Benefits Assesment",QPointF(800,0));
    }

    if(mcExtreme){
        if(!createLink(impLand->getOutPort("Data"),mcExtr->getInPort("City")))
            std::cout << "couldnt create link between " << impLand->getName() << " " << mcExtr->getName() << std::endl;

    }

    if(mcAverage){
        if(!createLink(impLand->getOutPort("Data"),mcAver->getInPort("City")))
            std::cout << "couldnt create link between " << impLand->getName() << " " << mcAver->getName() << std::endl;

    }

    if(Rain)
    {
        if(!createLink(impMsf->getOutPort("City"),Ra->getInPort("City")))
            std::cout << "couldnt create link between " << impMsf->getName() << " " << Ra->getName() << std::endl;

        if(streamErosion){
            if(!createLink(Ra->getOutPort("City"),streamEro->getInPort("City")))
                std::cout << "couldnt create link between " << Ra->getName() << " " << streamEro->getName() << std::endl;
        }

        if(streamHydrology){
            if(!createLink(Ra->getOutPort("City"),streamHydro->getInPort("City")))
                std::cout << "couldnt create link between " << Ra->getName() << " " << streamHydro->getName() << std::endl;
        }

        if(Treatment){
            if(!createLink(Ra->getOutPort("City"),Treat->getInPort("City")))
                std::cout << "couldnt create link between " << Ra->getName() << " " << Treat->getName() << std::endl;
        }

    }else{

        if(streamErosion){
            if(!createLink(impMsf->getOutPort("City"),streamEro->getInPort("City")))
                std::cout << "couldnt create link between " << impMsf->getName() << " " << streamEro->getName() << std::endl;
            if(!createLink(streamEro->getOutPort("City"),Analyser->getInPort("City")))
                std::cout << "couldnt create link between " << streamEro->getName() << " " << Analyser->getName() << std::endl;
        }

        if(streamHydrology){
            if(!createLink(impMsf->getOutPort("City"),streamHydro->getInPort("City")))
                std::cout << "couldnt create link between " << impMsf->getName() << " " << streamHydro->getName() << std::endl;
            if(!createLink(streamHydro->getOutPort("City"),Analyser->getInPort("City")))
                std::cout << "couldnt create link between " << streamHydro->getName() << " " << Analyser->getName() << std::endl;
        }

        if(Treatment){
            if(!createLink(impMsf->getOutPort("City"),Treat->getInPort("City")))
                std::cout << "couldnt create link between " << impMsf->getName() << " " << Treat->getName() << std::endl;
            if(!createLink(Treat->getOutPort("City"),Analyser->getInPort("City")))
                std::cout << "couldnt create link between " << Treat->getName() << " " << Analyser->getName() << std::endl;
        }
    }
}

DM::Module *Wizardq::createModule(QString name, QPointF pos)
{
    DM::Module *m;
    m = this->sim->addModule(name.toStdString());
    m->init();
    m->setName(name.toStdString());
    this->sim->GUIaddModule(m,pos);
    return m;
}

bool Wizardq::createLink(DM::Port *OutPort, DM::Port *InPort)
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

bool Wizardq::isExtreme()
{
    return this->mcExtreme;
}

bool Wizardq::isAverage()
{
    return this->mcAverage;
}

bool Wizardq::isSEI()
{
    return this->streamErosion;
}

bool Wizardq::isHydro()
{
    return this->streamHydrology;
}

bool Wizardq::isTreatment()
{
    return this->Treatment;
}

bool Wizardq::isRain()
{
    return this->Rain;
}

void Wizardq::setSimulation(GUISimulation *sim)
{
    this->sim = sim;
}

void Wizardq::setScene(ProjectViewer *pv)
{
    this->pv = pv;

}
