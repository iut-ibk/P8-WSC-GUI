#include "exportfiles.h"
#include "ui_exportfiles.h"
#include "QSettings"
#include <iostream>

ExportFiles::ExportFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportFiles)
{
    ui->setupUi(this);
    QSettings settings;
    ui->le_folder->setText(settings.value("dataPath").toString());
    workfolder=settings.value("workPath").toString();
    checkboxlist.load(settings.value("installPath").toString()+ "/files.txt",workfolder+"/states.txt",workfolder);
    ui->lv_list->setModel(&checkboxlist);
}

ExportFiles::~ExportFiles()
{
    delete ui;
}

void ExportFiles::on_pb_cancel_clicked()
{
    this->close();
}

void ExportFiles::on_pb_folder_clicked()
{
    QString ExportFolder=QFileDialog::getExistingDirectory(this,"Select export folder",ui->le_folder->text());
    ui->le_folder->setText(ExportFolder);
    QSettings settings;
    settings.setValue("dataPath",ExportFolder);
}

void ExportFiles::on_pb_export_clicked()
{
    QSettings settings;
    workfolder = settings.value("workPath").toString();
    QStringList files=checkboxlist.getFiles();
    QString exportfolder=ui->le_folder->text();
    for (int i = 0; i<files.length(); i++)
    {
        if(QFile::exists(exportfolder + "/" + files[i]))
        {
            if(ui->cb_overwrite->isChecked())
            {
                QFile::remove(exportfolder + "/" + files[i]);
                QFile::copy(workfolder + "/" + files[i],exportfolder + "/" + files[i]);
            }
            else
            {
                continue;
            }
        }
        else
        {
            QFile::copy(workfolder + "/" + files[i],exportfolder + "/" + files[i]);
        }
    }
    this->close();
}