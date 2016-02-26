#ifndef MICROCLIMATEPAGE_H
#define MICROCLIMATEPAGE_H

#include <QWizardPage>
#include "wizardq.h"

namespace Ui {
class MicroclimatePage;
}

class MicroclimatePage : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit MicroclimatePage(Wizardq *parent = 0);
    ~MicroclimatePage();
    int nextId() const;
    bool isComplete() const;
    
private slots:
    void on_chbAverage_released();

    void on_chbExtreme_released();

private:
    Wizardq *w;
    Ui::MicroclimatePage *ui;
};

#endif // MICROCLIMATEPAGE_H
