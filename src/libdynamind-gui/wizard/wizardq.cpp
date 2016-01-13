#include "wizardq.h"
#include "ui_wizardq.h"
#include "page1.h"

Wizardq::Wizardq(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::Wizardq)
{
    setPage(0,new page1);

    ui->setupUi(this);
}

Wizardq::~Wizardq()
{
    delete ui;
}

void Wizardq::on_Wizardq_accepted()
{

}
