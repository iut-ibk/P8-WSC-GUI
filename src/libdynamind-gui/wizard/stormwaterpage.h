#ifndef STORMWATERPAGE_H
#define STORMWATERPAGE_H

#include <QWizardPage>
#include "wizardq.h"

namespace Ui {
class StormwaterPage;
}

class StormwaterPage : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit StormwaterPage(Wizardq *parent = 0);
    ~StormwaterPage();
    bool isComplete() const;
    int nextId() const;
    
private slots:
    void on_chbSEI_released();

    void on_chbTreatment_released();

    void on_chbHydrology_released();

private:
    Wizardq *w;
    Ui::StormwaterPage *ui;
};

#endif // STORMWATERPAGE_H
