#ifndef EXPORTFILES_H
#define EXPORTFILES_H

#include <QDialog>

#include "checkboxlist.h"

namespace Ui {
class ExportFiles;
}

class ExportFiles : public QDialog
{
    Q_OBJECT

public:
    explicit ExportFiles(QWidget *parent = 0);
    ~ExportFiles();

private slots:
    void on_pb_cancel_clicked();

    void on_pb_folder_clicked();

    void on_pb_export_clicked();

private:
    Ui::ExportFiles *ui;
    QString workfolder;
    CheckboxList checkboxlist;
};

#endif // EXPORTFILES_H
