#ifndef ENDPAGE_H
#define ENDPAGE_H

#include <QWizardPage>
#include "wizardq.h"

namespace Ui {
class endpage;
}

class endpage : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit endpage(Wizardq *parent = 0);
    ~endpage();
    int nextId() const;
    
private:
    Wizardq *w;
    Ui::endpage *ui;
};

#endif // ENDPAGE_H
