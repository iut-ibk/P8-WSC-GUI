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
    return -1;
}
