#ifndef WIZARDQ_H
#define WIZARDQ_H

#include <QWizard>
#include "dmmodule.h"
#include "guisimulation.h"

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
    void setAll();
    void setSimulation(GUISimulation *sim);


private slots:
    void buildSimulation();
    DM::Module* createModule(QString name, QPointF pos);


private:
    Ui::Wizardq *ui;
    bool importLandcover, importMsf, mcExtreme, mcAverage, streamErosion, streamHydrology, Treatment, Rain, Eco;
    GUISimulation *sim;

};

#endif // WIZARDQ_H
