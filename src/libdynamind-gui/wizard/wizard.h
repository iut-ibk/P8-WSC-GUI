#ifndef WIZARD_H
#define WIZARD_H

#include <QDialog>
#include <guisimulation.h>
#include <projectviewer.h>
namespace Ui {
class wizard;
}

class wizard : public QDialog
{
    Q_OBJECT
    
public:
    explicit wizard(QWidget *parent = 0);
    ~wizard();
    
    void setSimulation(GUISimulation *sim);
    void setScene(ProjectViewer *pv);

private slots:
    void on_pushButton_released();

private:
    Ui::wizard *ui;
    GUISimulation *sim;
    ProjectViewer *pv;
    DM::Module *createModule(QString name, QPointF pos);

};

#endif // WIZARD_H
