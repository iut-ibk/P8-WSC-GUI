#ifndef WIZARDQ_H
#define WIZARDQ_H

#include <QWizard>
#include "dmmodule.h"
#include "guisimulation.h"
#include <projectviewer.h>


namespace Ui {
class Wizardq;
}

class Wizardq : public QWizard
{
    Q_OBJECT
    
public:
    explicit Wizardq(QWidget *parent = 0);
    ~Wizardq();
    void setImportLandcover();
    void setImportMsf();
    void setMcExtreme();
    void setMcAverage();
    void setStreamErosion();
    void setStreamHydrology();
    void setTreatment();
    void setRain();
    void setEco();
    void unsetMcExtreme();
    void unsetMcAverage();
    void unsetStreamErosion();
    void unsetStreamHydrology();
    void unsetTreatment();
    void unsetRain();
    void setAll();
    void unsetAll();
    void setSimulation(GUISimulation *sim);
    void setScene(ProjectViewer *pv);
    bool isExtreme();
    bool isAverage();
    bool isSEI();
    bool isHydro();
    bool isTreatment();
    bool isRain();

private slots:
    void buildSimulation();
    DM::Module* createModule(QString name, QPointF pos);
    bool createLink( DM::Port *OutPort, DM::Port *InPort);



private:
    Ui::Wizardq *ui;
    bool importLandcover, importMsf, mcExtreme, mcAverage, streamErosion, streamHydrology, Treatment, Rain, Eco;
    GUISimulation *sim;
    ProjectViewer *pv;

};

#endif // WIZARDQ_H
