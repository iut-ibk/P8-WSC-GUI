#ifndef PAGE1_H
#define PAGE1_H

#include <QWizardPage>

namespace Ui {
class page1;
}

class page1 : public QWizardPage
{
    Q_OBJECT
    
public:
    explicit page1(QWidget *parent = 0);
    ~page1();
    
private:
    Ui::page1 *ui;
};

#endif // PAGE1_H
