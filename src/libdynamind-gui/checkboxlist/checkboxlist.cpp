#include "checkboxlist.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <iostream>

using namespace std;

CheckboxList::CheckboxList()
{
    size=0;
}

void CheckboxList::load(QString listfilename, QString statefilename, QString workfolder)
{
    QDir workdir = QDir(workfolder);
    workdir.setFilter(QDir::Files);
    QStringList files = workdir.entryList(); // get files in workdir
    statefilestring = statefilename;
    QFile listfile(listfilename);
    if (!listfile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return;
    }
    QTextStream liststream(&listfile);
    QStringList titlesstrings=liststream.readAll().split(QRegExp("[\r\n]"),QString::SkipEmptyParts);    // get "excluding" file names
    listfile.close();    

    QFile statefile(statefilename);
    int statesize=-1;
    QStringList statestrings;
    if (statefile.open(QIODevice::ReadOnly|QIODevice::Text))    // get state list
    {
        QTextStream statestream(&statefile);
        statestrings=statestream.readAll().split(QRegExp("[\r\n]"),QString::SkipEmptyParts);
        statefile.close();
        statesize=statestrings.size();
    }
    for(int i = 0; i<files.length(); i++)
    {
        if(files[i] == "states.txt")
            continue;
        if(titlesstrings.contains(files[i]))
            continue;
        if(statestrings.contains(files[i]+"*0") || statestrings.contains(files[i]+"*2"))
        {
            continue;
        }
        else
        {
            statestrings.append(files[i] + "*0");
        }
    }
    size = statestrings.size();

    if(statefile.open(QIODevice::WriteOnly))    // if statefile doesnt exist write one with only zeros
    {                                           // happense when new project is made in new folder
        QTextStream outstream (&statefile);
        for(int i = 0; i<statestrings.length();i++)
        {
            if(i == statestrings.length()-1)
            {
                outstream << statestrings.at(i);
            }
            else
            {
                outstream << statestrings.at(i) << endl;
            }
        }
    }
    titels.clear();
    emit beginResetModel();
    checkedStatus.resize(size);
    enabledStatus.resize(size);
    for(int i = 0; i < size; ++i)
    {
        if (statestrings.at(i).split("*").at(1).toInt())
            checkedStatus[i] = Qt::Checked;
        else
            checkedStatus[i] = Qt::Unchecked;

        enabledStatus[i]=true;//presentstrings.contains(titlesstrings[i]);
        titels.append(statestrings.at(i).split("*").at(0));
    }

    setStringList(titels);
    emit endResetModel();
}

QStringList CheckboxList::getFiles()
{
    QStringList retVal;
    QFile statefile(statefilestring);
    if(statefile.open(QIODevice::WriteOnly))
    {
        QTextStream outstream (&statefile);
        for (int i=0;i<size;i++)
        {
            if(i == size-1)
            {
                outstream << titels[i] << "*" << checkedStatus[i];
            }
            else
            {
                outstream << titels[i] << "*" << checkedStatus[i] << endl;
            }
        }
    }
    statefile.close();
    for (int i=0;i<size;i++)
    {

        if (enabledStatus[i]&&checkedStatus[i])
            retVal<<titels[i];
    }
    return retVal;
}

void CheckboxList::setChecked()
{
    emit beginResetModel();
    for(int i = 0;i<this->checkedStatus.size();i++)
        this->checkedStatus[i] = Qt::Checked;
    emit endResetModel();

}
void CheckboxList::setUnchecked()
{
    emit beginResetModel();
    for(int i = 0;i<this->checkedStatus.size();i++)
        this->checkedStatus[i] = Qt::Unchecked;
    emit endResetModel();
}

void CheckboxList::set(const QStringList &strings)
{
    emit beginResetModel();
    checkedStatus.resize(strings.size());
    enabledStatus.resize(strings.size());
    titels.clear();
    for(int i = 0; i < checkedStatus.size(); ++i)
    {
        checkedStatus[i] = Qt::Unchecked;
        enabledStatus[i]= 1;
        titels.append(strings[i]);
    }

    setStringList(titels);
    emit endResetModel();
}


Qt::ItemFlags CheckboxList::flags(const QModelIndex& index) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return Qt::NoItemFlags;

    if (enabledStatus[index.row()])
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    else
        return Qt::ItemIsUserCheckable;
}

QVariant CheckboxList::data (const QModelIndex  &index , int role ) const
{
    if (index.row() < 0 || index.row() >= rowCount())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
        return stringList().at(index.row());

    if(role == Qt::CheckStateRole)
        return checkedStatus[index.row()];

    return QVariant();
}

bool CheckboxList::setData (const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() < 0 || index.row() >= rowCount())
        return false;

    if(role == Qt::EditRole || role == Qt::DisplayRole)
        stringList().replace(index.row(), value.toString());

    if(role == Qt::CheckStateRole)
        checkedStatus[index.row()] = static_cast<Qt::CheckState>(value.toUInt());

    emit dataChanged(index, index);
    return true;
}


