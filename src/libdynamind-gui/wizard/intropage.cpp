#include "intropage.h"
#include "ui_intropage.h"
#include "wizardq.h"
#include "QDebug"

IntroPage::IntroPage(Wizardq *parent) :
    QWizardPage(parent),
    ui(new Ui::IntroPage)
{
    this->w = parent;
    ui->setupUi(this);
}

IntroPage::~IntroPage()
{
    delete ui;
}

int IntroPage::nextId() const
{
    if(ui->rbMicro->isChecked()){
        this->w->setImportLandcover();
        return 1;
    }
    if(ui->rbStorm->isChecked()){
        this->w->setImportMsf();
        return 2;
    }
    if(ui->rbComp->isChecked()){
        this->w->setAll();
        return 3;
    }
    if(ui->rbBlank->isChecked()){
        return 3;
    }
}

bool IntroPage::isComplete() const
{
    if(ui->rbMicro->isChecked()){
        return true;
    }else if(ui->rbStorm->isChecked()){
        return true;
    }else if(ui->rbComp->isChecked()){
        return true;
    }else if(ui->rbBlank->isChecked()){
        return true;
    }else{
        return false;
    }
}

void IntroPage::on_rbMicro_released()
{
    this->completeChanged();
}

void IntroPage::on_rbStorm_released()
{
    this->completeChanged();
}

void IntroPage::on_rbComp_released()
{
    this->completeChanged();
}

void IntroPage::on_rbBlank_released()
{
    this->completeChanged();
}
