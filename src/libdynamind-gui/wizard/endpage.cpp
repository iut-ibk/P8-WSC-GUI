#include "endpage.h"
#include "ui_endpage.h"
#include "wizardq.h"

endpage::endpage(Wizardq *parent) :
    QWizardPage(parent),
    ui(new Ui::endpage)
{
    this->w = parent;
    ui->setupUi(this);
}

endpage::~endpage()
{
    delete ui;
}

int endpage::nextId() const
{
    this->ui->chmAverage->show();
    this->ui->chmExtreme->show();
    this->ui->chmSEI->show();
    this->ui->chmRain->show();
    this->ui->chmHydro->show();
    this->ui->chmTreatment->show();

    if(!this->w->isAverage()){
        this->ui->chmAverage->hide();
    }
    if(!this->w->isExtreme()){
        this->ui->chmExtreme->hide();
    }
    if(!this->w->isSEI()){
        this->ui->chmSEI->hide();
    }
    if(!this->w->isHydro()){
        this->ui->chmHydro->hide();
    }
    if(!this->w->isTreatment()){
        this->ui->chmTreatment->hide();
    }
    if(!this->w->isRain()){
        this->ui->chmRain->hide();
    }
    return -1;
}
