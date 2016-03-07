#include "microclimatepage.h"
#include "ui_microclimatepage.h"
#include "wizardq.h"
#include "QDebug"


MicroclimatePage::MicroclimatePage(Wizardq *parent) :
    QWizardPage(parent),
    ui(new Ui::MicroclimatePage)
{
    this->w = parent;
    ui->setupUi(this);
}

MicroclimatePage::~MicroclimatePage()
{
    delete ui;
}

int MicroclimatePage::nextId() const
{
    this->w->unsetMcAverage();
    this->w->unsetMcExtreme();
    if(ui->chbAverage->isChecked()){
        this->w->setMcAverage();
    }
    if(ui->chbExtreme->isChecked()){
        this->w->setMcExtreme();
    }
    return 3;
}

bool MicroclimatePage::isComplete() const
{
    if(ui->chbAverage->isChecked() || ui->chbExtreme->isChecked())
        return true;
    else
        return false;
}


void MicroclimatePage::on_chbAverage_released()
{
    this->completeChanged();
}

void MicroclimatePage::on_chbExtreme_released()
{
    this->completeChanged();
}
