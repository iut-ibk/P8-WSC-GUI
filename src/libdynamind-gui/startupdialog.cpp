#include "startupdialog.h"
#include "ui_startupdialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QLabel>


StartupDialog::StartupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartupDialog)
{
    ui->setupUi(this);
    setWindowFlags( Qt::FramelessWindowHint );
    ui->pb_bug->setStyleSheet ("text-align: left");
    ui->pb_web->setStyleSheet ("text-align: left");
    startmode=0;
}

StartupDialog::~StartupDialog()
{
    delete ui;
}


void StartupDialog::on_pb_open_released()
{
    startmode=1;
    accept();
}

void StartupDialog::on_pb_new_released()
{
    startmode=2;
    accept();
}

void StartupDialog::on_pb_bug_released()
{
    QLabel * label_img = new QLabel (this);
    label_img->setWindowFlags(Qt::Window);
    label_img->setPixmap(QPixmap(":/Icons/ressources/Support.png", 0, Qt::AutoColor));
    label_img->show();
}

void StartupDialog::on_pb_web_released()
{
    QDesktopServices::openUrl(QUrl("http://watersensitivecities.org.au/", QUrl::TolerantMode));
}

int StartupDialog::exec()
{
    QDialog::exec();
    return startmode;
}
