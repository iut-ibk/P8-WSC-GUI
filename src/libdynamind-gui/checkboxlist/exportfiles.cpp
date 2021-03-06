#include "exportfiles.h"
#include "ui_exportfiles.h"
#include <iostream>

using namespace std;

ExportFiles::ExportFiles(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportFiles)
{
    ui->setupUi(this);
    cout << "hh"<<endl;
    QSettings settings;
    ui->le_folder->setText(settings.value("dataPath").toString());
    workfolder= QDir::tempPath()+"/P8Tool/"+QString::number(QApplication::applicationPid());

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
    workfolder= QDir::tempPath()+"/P8Tool/"+QString::number(QApplication::applicationPid());
    QStringList files=checkboxlist.getFiles();

    QString exportfolder=ui->le_folder->text()+"/"+ui->le_szenario->text();
    QDir dir;
    dir.mkdir(exportfolder);
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

void ExportFiles::on_lineEdit_editingFinished()
{
}

void ExportFiles::on_le_szenario_textChanged(const QString &arg1)
{
    if (ui->le_szenario->text().isEmpty())
        ui->pb_export->setEnabled(false);
    else
        ui->pb_export->setEnabled(true);
}

void ExportFiles::on_chkbox_all_toggled(bool checked)
{
    //set all items in checkboxlist to toggled value
    if(checked)
        this->checkboxlist.setChecked();
    else
        this->checkboxlist.setUnchecked();
}
