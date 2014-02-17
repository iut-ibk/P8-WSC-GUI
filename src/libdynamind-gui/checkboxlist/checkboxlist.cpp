#include "checkboxlist.h"

#include <QFile>
#include <QTextStream>
#include <QDir>


CheckboxList::CheckboxList()
{
    size=0;
}

void CheckboxList::load(QString listfilename, QString statefilename, QString workfolder)
{
    statefilestring = statefilename;
    QFile listfile(listfilename);
    if (!listfile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return;
    }
    QTextStream liststream(&listfile);
    QStringList titlesstrings=liststream.readAll().split("\n");
    listfile.close();

    QFile statefile(statefilename);
    if (!statefile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        if(statefile.open(QIODevice::WriteOnly))    // if statefile doesnt exist write one with only zeros
        {                                           // happense when new project is made in new folder
            QTextStream outstream (&statefile);
            for(int i = 0; i<titlesstrings.length();i++)
            {
                if(i == titlesstrings.length()-1)
                {
                    outstream << "0";
                }
                else
                {
                    outstream << "0" << endl;
                }
            }
        }
    }
    QTextStream statestream(&statefile);
    QStringList statestrings=statestream.readAll().split("\n");
    statefile.close();

    QStringList presentstrings;
    QDir dir(workfolder);
    foreach (QFileInfo i,dir.entryInfoList(QDir::Files))
        presentstrings<<i.fileName();


    size=titlesstrings.size();

    emit beginResetModel();
    checkedStatus.resize(size);
    enabledStatus.resize(size);
    titels.clear();
    for(int i = 0; i < size; ++i)
    {
        if (statestrings[i].toInt())
            checkedStatus[i] = Qt::Checked;
        else
            checkedStatus[i] = Qt::Unchecked;

        enabledStatus[i]=presentstrings.contains(titlesstrings[i]);
        titels.append(titlesstrings[i]);
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
                outstream << checkedStatus[i];
            }
            else
            {
                outstream << checkedStatus[i] << endl;
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
