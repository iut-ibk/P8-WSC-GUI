#include "stormwaterpage.h"
#include "ui_stormwaterpage.h"
#include "wizardq.h"

StormwaterPage::StormwaterPage(Wizardq *parent) :
    QWizardPage(parent),
    ui(new Ui::StormwaterPage)
{
    this->w = parent;
    ui->setupUi(this);
}

StormwaterPage::~StormwaterPage()
{
    delete ui;
}

bool StormwaterPage::isComplete() const
{
    if(ui->chbHydrology->isChecked() || ui->chbSEI->isChecked() || ui->chbTreatment->isChecked()){
        return true;
    }else{
        return false;
    }
}

int StormwaterPage::nextId() const
{
    this->w->unsetRain();
    this->w->unsetStreamErosion();
    this->w->unsetStreamHydrology();
    this->w->unsetTreatment();
    if(ui->chbRain->isChecked()){
        this->w->setRain();
    }
    if(ui->chbHydrology->isChecked()){
        this->w->setStreamHydrology();
    }
    if(ui->chbTreatment->isChecked()){
        this->w->setTreatment();
    }
    if(ui->chbSEI->isChecked()){
        this->w->setStreamErosion();
    }
    return 3;
}

void StormwaterPage::on_chbSEI_released()
{
    this->completeChanged();
}

void StormwaterPage::on_chbTreatment_released()
{
    this->completeChanged();
}

void StormwaterPage::on_chbHydrology_released()
{
    this->completeChanged();
}
