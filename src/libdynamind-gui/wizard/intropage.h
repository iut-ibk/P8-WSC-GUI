#ifndef INTROPAGE_H
#define INTROPAGE_H

#include <QWizardPage>
#include "wizardq.h"

namespace Ui {
class IntroPage;
}

class IntroPage : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit IntroPage(Wizardq *parent = 0);
    ~IntroPage();
    int nextId() const;
    bool isComplete() const;

private slots:
    void on_rbMicro_released();

    void on_rbStorm_released();

    void on_rbComp_released();

    void on_rbBlank_released();

private:
    Wizardq *w;
    Ui::IntroPage *ui;
};

#endif // INTROPAGE_H
