#include "wizardq.h"
#include "ui_wizardq.h"
#include "intropage.h"
#include "microclimatepage.h"
#include "stormwaterpage.h"
#include "endpage.h"
#include "QDebug"
#include "guisimulation.h"

Wizardq::Wizardq(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::Wizardq)
{
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
    //connect( this, SIGNAL( accepted() ), this, SLOT( buildSimulation() ), Qt::DirectConnection );


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

void Wizardq::buildSimulation()
{
    if(importLandcover){
        createModule("Import Land Cover Map",QPointF(0,0));
    }
    if(importMsf){
        createModule("Import WSUD Layout(.msf)",QPointF(0,0));
    }
    if(mcExtreme){
        createModule("Extreme Heat",QPointF(0,0));
    }
    if(mcAverage){
        createModule("Average Summer Heat",QPointF(0,0));
    }
    if(streamErosion){
        createModule("Stream Erosion and Minor Flooding",QPointF(0,0));
    }
    if(streamHydrology){
        createModule("Stream Hydrology and Water Quality",QPointF(0,0));
    }
    if(Treatment){
        createModule("Treatment and Harvesting Performance (MUSIC)",QPointF(0,0));
    }
    if(Rain){
        createModule("Future Rainfall (Test Version)",QPointF(0,0));
    }
    if(Eco){
        createModule("Benefits Assesment",QPointF(0,0));
    }
    qDebug() << "at end";
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

void Wizardq::setSimulation(GUISimulation *sim)
{
    this->sim = sim;
}
