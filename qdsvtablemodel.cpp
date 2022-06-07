/*************************************************************************
** Copyright (c) 2013 Ronie Martinez <ronmarti18@gmail.com>             **
** All rights reserved.                                                 **
**                                                                      **
** This library is free software; you can redistribute it and/or modify **
** it under the terms of the GNU Lesser General Public License as       **
** published by the Free Software Foundation; either version 2.1 of the **
** License, or (at your option) any later version.                      **
**                                                                      **
** This library is distributed in the hope that it will be useful, but  **
** WITHOUT ANY WARRANTY; without even the implied warranty of           **
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    **
** Lesser General Public License for more details.                      **
**                                                                      **
** You should have received a copy of the GNU Lesser General Public     **
** License along with this library; if not, write to the Free Software  **
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA        **
** 02110-1301  USA                                                      **
*************************************************************************/

#include <QTextStream>
#include <QRegularExpression>
#include <QFileInfo>

#include "qdsvtablemodel.h"

QDsvTableModel::QDsvTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

int QDsvTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dsvMatrix.rowCount();
}

int QDsvTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return dsvMatrix.columnCount();
}

QVariant QDsvTableModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
        if (role == Qt::DisplayRole || role == Qt::EditRole)
            return dsvMatrix.at(index.row(), index.column());
    return QVariant();
}

QVariant QDsvTableModel::data(int ligne,int colonne) const
{
  return dsvMatrix.at(ligne, colonne);
}

bool QDsvTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole)
        return dsvMatrix.setValue(index.row(), index.column(), value.toString());
    return false;
}

bool QDsvTableModel::setData(int ligne,int colonne, const QVariant &value)
{
   return dsvMatrix.setValue(ligne, colonne, value.toString());
}

Qt::ItemFlags QDsvTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QAbstractItemModel::flags(index);
    flags |= Qt::ItemIsEditable;
    return flags;
}

void QDsvTableModel::checkString(QString &temp, QList<QString> &list, const QChar &character )
{
    if(temp.count("\"")%2 == 0) {
        if (temp.startsWith(QChar('\"')) && temp.endsWith( QChar('\"') ) ) {
             temp.remove(QRegularExpression("^\"") );
             temp.remove(QRegularExpression("\"$") );
        }
        temp.replace("\"\"", "\"");
        list.append(temp);
        if (character != delimiter) {
            if(nbHeader>=2 && !header1.isEmpty() && header2.isEmpty()) header2=list;
            else if(nbHeader>=1 && header1.isEmpty()) header1=list;
            else dsvMatrix.append(list);
            list.clear();
        }
        temp.clear();
    } else {
        temp.append(character);
    }
}


bool QDsvTableModel::loadFromFile(const QString &fileName, const QChar &delim)
{
    dsvMatrix.clear();
    //QChar delimiter;
    QFile file(fileName);
    if (delim == 0) {
        QString extension = QFileInfo(file).completeSuffix();
        if (extension.toLower() == "csv")
            delimiter = QChar(',');
        else if (extension.toLower() == "tsv")
            delimiter = QChar('\t');
        else
            return false; //unknown file extension = unknown delimiter
    } else  if (delim == QChar('"'))
            return false; //the ONLY invalid delimiter is double quote (")
    else
        delimiter = delim;
    if (!file.isOpen())
        if (!file.open(QFile::ReadOnly|QFile::Text))
            return false;
    QString temp;
    QChar lastCharacter;
    QTextStream in(&file);
    QList<QString> row;
    while (true) {
        QChar character;
        in >> character;
        if (in.atEnd()) {
            if (lastCharacter == delimiter) //cases where last character is equal to the delimiter
                temp = "";
            checkString(temp, row,   QChar('\n'));
            break;
        } else if (character == delimiter || character == QChar('\n'))
            checkString(temp, row,   character);
        else {
            temp.append(character);
            lastCharacter = character;
        }
    }

    file.close();
    in.reset();
    return true;
}

bool QDsvTableModel::loadFromData(const QByteArray &data, const QString &format)
{
    //not yet implemented ...
    Q_UNUSED(data);
    Q_UNUSED(format);
    return false;
}

bool QDsvTableModel::save(const QString &fileName, const QChar &delim,
                          QDsvTableModel::DsvWriteFlag flag)
{
    QChar delimiter;
    QFile file(fileName);
    if (delim == 0) {
        QString extension = QFileInfo(file).completeSuffix();
        if (extension.toLower() == "csv")
            delimiter = QChar(',');
        else if (extension.toLower() == "tsv")
            delimiter = QChar('\t');
        else
            return false; //unknown file extension = unknown delimiter
    } else  if (delim == QChar('"'))
            return false; //the ONLY invalid delimiter is double quote (")
    else
        delimiter = delim;
    if (!file.open(QIODevice::WriteOnly))
        return false;
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out.setGenerateByteOrderMark(true);
    for (int i = 0; i < dsvMatrix.rowCount(); ++i) {
        for (int j = 0; j < dsvMatrix.columnCount(); ++j) {
            QString s = dsvMatrix.at(i, j);
            if (s.contains("\"")) {
                s.replace("\"", "\"\"");
            }

            if (flag == UseDoubleQuotesIfNeeded) {
                if (s.contains("\"") || s.contains(" ") || s.contains(delim))
                    s = "\"" + s + "\"";
            } else {
                s = "\"" + s + "\"";
            }

            out << s;

            if (j == dsvMatrix.columnCount() - 1)
                out << Qt::endl;
            else
                out << delimiter;
        }
    }
    file.close();
    return true;
}

bool QDsvTableModel::removeRows(int row, int count, const QModelIndex &parent )
{
    if(dsvMatrix.rowCount()<row+count) return false;
    beginRemoveRows(parent,row,row+count);
    dsvMatrix.remove(row,count);
    endRemoveRows();
    return true;
}

void QDsvTableModel::setNbHeader(int nb)
{
    if (nbHeader <=0 && nb >0)
    {
        header1=rowAt(0);
        nbHeader=1;
        headerDataChanged(Qt::Horizontal, 0, header1.count());
        removeRows(0,1);
    }
    if (nbHeader <=1 && nb >1)
    {
        header2=rowAt(0);
        nbHeader=2;
        removeRows(0,1);
    }
    if (nbHeader >=2 && nb <2)
    {
        dsvMatrix.insert(0,header2);
        header2.clear();
        nbHeader=1;
    }
    if (nbHeader >=1 && nb <1)
    {
        dsvMatrix.insert(0,header1);
        header1.clear();
        nbHeader=0;
    }
    headerDataChanged(Qt::Vertical, 0, dsvMatrix.columnCount());
}

QVariant QDsvTableModel::headerData(int section, Qt::Orientation orientation, int role ) const{
  if(role == Qt::DisplayRole)
  {
    if(orientation == Qt::Horizontal)
    {
        if(header1.count()>section)
            return header1[section];
        else
            return QString("%1").arg(section+1);
    };
    return QString("%1").arg(section+1);
  }
  if(role == Qt::ToolTipRole && orientation == Qt::Horizontal)
  {
      if(header2.count()>section)
          return header2[section];
      else if(header1.count()>section)
          return header1[section];
  }
  return QVariant();
}



