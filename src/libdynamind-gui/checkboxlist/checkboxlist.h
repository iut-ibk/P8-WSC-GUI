#ifndef CHECKBOXLIST_H
#define CHECKBOXLIST_H

#include <QStringListModel>
#include <QtGui>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <QModelIndex>

class CheckboxList: public QStringListModel
{
public:
    CheckboxList();
    void set(const QStringList &strings);
    void load(QString listfilename, QString statefilename, QString workfolder);
    QStringList getFiles();
    void setCheckedStatus(int pos, bool status);
    int getSize();

private:
    QStringList titels;
    QVector<Qt::CheckState> checkedStatus;
    QVector<bool> enabledStatus;
    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data (const QModelIndex  &index , int role ) const;
    bool setData (const QModelIndex &index, const QVariant &value, int role);
    int size;
    QString statefilestring;
};

#endif // CHECKBOXLIST_H
