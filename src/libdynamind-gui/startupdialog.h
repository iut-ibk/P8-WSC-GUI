#ifndef STARTUPDIALOG_H
#define STARTUPDIALOG_H

#include <QDialog>

namespace Ui {
class StartupDialog;
}

class StartupDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StartupDialog(QWidget *parent = 0);
    ~StartupDialog();
    
private slots:
    void on_pb_open_released();
    void on_pb_new_released();
    void on_pb_bug_released();
    void on_pb_web_released();

public slots:
    int exec();

private:
    Ui::StartupDialog *ui;
    int startmode;
};

#endif // STARTUPDIALOG_H
