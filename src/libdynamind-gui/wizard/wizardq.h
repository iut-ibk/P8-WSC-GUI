#ifndef WIZARDQ_H
#define WIZARDQ_H

#include <QWizard>

namespace Ui {
class Wizardq;
}

class Wizardq : public QWizard
{
    Q_OBJECT
    
public:
    explicit Wizardq(QWidget *parent = 0);
    ~Wizardq();
    
private slots:
    void on_Wizardq_accepted();

private:
    Ui::Wizardq *ui;
};

#endif // WIZARDQ_H
