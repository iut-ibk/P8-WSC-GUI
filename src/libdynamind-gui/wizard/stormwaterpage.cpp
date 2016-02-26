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
    return -1;
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
