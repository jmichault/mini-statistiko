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

#ifndef QDSVTABLEMODEL_H
#define QDSVTABLEMODEL_H

#include <QAbstractTableModel>
static int sortCol;

//a template-based matrix to store 2D data
template<class T>
class QDsvMatrix {
    QList<QList<T>> m_data;
    int m_rowCount;
    int m_columnCount;

public:
    QDsvMatrix(int row = 0, int column = 0):
        m_rowCount(row), m_columnCount(column)
    {
        if (rowCount() > 0 && columnCount() > 0) {
            for (int i = 0; i < rowCount(); ++i) {
                QList<T> rowData;
                for (int j = 0; j < columnCount(); ++j) {
                    rowData.insert(j, T());
                }
                m_data.insert(i, rowData);
            }
        }
    }

    T at(int row, int column) const {
        if (row >= rowCount() || column >= columnCount())
            return T();
        QList<T> rowData = m_data.at(row);
        return rowData.value(column);
    }

    QList<T> rowAt(int row) const{
        if (row >= rowCount())
            return QList<T>();
        QList<T> rowData = m_data.at(row);
        return rowData;
    }
    static bool compareN(const QList<T>  &x1, const QList<T> &x2)
    {
        bool ok1,ok2;
        double d1,d2;
        d1=x1.value(sortCol).toDouble(&ok1);
        d2=x2.value(sortCol).toDouble(&ok2);
        if (ok1 && ok2) return (d1 < d2);
        else return (x1.value(sortCol) < x2.value(sortCol));
    }
    static bool compareNdesc(const QList<T>  &x1, const QList<T> &x2)
    {
        bool ok1,ok2;
        double d1,d2;
        d1=x1.value(sortCol).toDouble(&ok1);
        d2=x2.value(sortCol).toDouble(&ok2);
        if (ok1 && ok2) return (d1 > d2);
        else return (x1.value(sortCol) > x2.value(sortCol));
    }
    bool sortN(int col, int firstrow=0, int count=0, Qt::SortOrder order = Qt::AscendingOrder){
        if (col >= m_columnCount ) return false;
        if (firstrow >= rowCount() ) return false;
        sortCol=col;
        if (count==0) count = rowCount() - firstrow;
        if (order==Qt::AscendingOrder) std::sort(m_data.begin()+firstrow,m_data.begin()+firstrow+count,&compareN);
        else std::sort(m_data.begin()+firstrow,m_data.begin()+firstrow+count,&compareNdesc);
        return true;
    };

    bool setValue(int row, int column, T value) {
        if (column >= columnCount()) {
            m_columnCount = column + 1;
            for (int i = 0; i < rowCount(); ++i) {
                QList<T> rowData = m_data.at(i);
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                m_data[i] = rowData;
            }
        }
        if (row >= rowCount()) {
            m_rowCount = row + 1;
            while (m_data.size() < rowCount()) {
                QList<T> rowData;
                while (rowData.size() < columnCount()) {
                    rowData.append(T());
                }
                m_data.append(rowData);
            }
        }

        m_data[row][column] = value;
        return true;
    }

    int rowCount() const {
        return m_rowCount;
    }

    int columnCount() const {
        return m_columnCount;
    }

    void clear() {
        m_rowCount = 0;
        m_columnCount = 0;
        m_data.clear();
    }

    void append(const QList<T> &value) {
        if (value.size() > columnCount())
            m_columnCount = value.size();
        m_data.append(value);
        ++m_rowCount;
    }
    void insert(qsizetype i, const QList<T> &value) {
        if (value.size() > columnCount())
            m_columnCount = value.size();
        m_data.insert(i,value);
        ++m_rowCount;
    }
    void remove(qsizetype i, qsizetype n = 1) {
        if(m_rowCount<n) return;
        m_data.remove(i,n);
        m_rowCount -= n;
    }
    double getDouble(int row, int col){
        QList<T> rowData1 = m_data[row];
        return rowData1.value(col).toDouble();
    }
    void removeKol(int Kol)
    {
      if (Kol < columnCount()) {
        for (int i = 0; i < rowCount(); ++i)
        {
          QList<T> rowData = m_data.at(i);
          rowData.remove(Kol);
          m_data[i] = rowData;
        }
        m_columnCount --;
      }
    }
};


//QDsvTableModel class
class QDsvTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit QDsvTableModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent) const;
    int rowCount() {return dsvMatrix.rowCount();};
    int columnCount(const QModelIndex &parent) const;
    int columnCount() {return dsvMatrix.columnCount();};
    void clearData() {dsvMatrix.clear();};
    QVariant data(const QModelIndex &index, int role) const;
    QVariant data(int ligne,int colonne) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool setData(int ligne,int colonne, const QVariant &value);
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool loadFromFile(const QString &fileName, const QChar &delim = '\000');
    bool loadFromData(const QByteArray &data, const QString &format);

    enum DsvWriteFlag {             //use when writing data to file
        UseDoubleQuotesIfNeeded,    //embed data on double quotes ONLY when needed
        AlwaysUseDoubleQuotes       //strictly embed data within double quotes
    };

    bool save(const QString &fileName, const QChar &delim = (short)0,
              DsvWriteFlag flag = UseDoubleQuotesIfNeeded);
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());
    QStringList rowAt(int row) const{
        return dsvMatrix.rowAt(row);
    }
    QStringList header1,header2;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    void setNbHeader(int nb);
    int getNbHeader() const {return nbHeader;};
    bool sortN(int col, int firstrow=0, int count=0, Qt::SortOrder order = Qt::AscendingOrder) {
        if(col==sorted && firstrow==0 && count==0 && order==Qt::AscendingOrder) return true;
        bool res= dsvMatrix.sortN(col,firstrow,count,order);
        if(firstrow==0 && count==0 && order==Qt::AscendingOrder) sorted=col;
        else sorted=-1;

        return res;
    }
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder)
    {
        if(column<0 || column>=columnCount()) return;
        layoutAboutToBeChanged();
        sortN(column,0,0,order);
        layoutChanged();

    }
    void removeKol(int Kol)
    {
      if(Kol <0 ||Kol >=columnCount()) return;
      dsvMatrix.removeKol(Kol);
      if(nbHeader>=1) header1.remove(Kol);
      if(nbHeader>=2) header2.remove(Kol);
    }
signals:
    
public slots:

private:
    QDsvMatrix<QString> dsvMatrix;
    QChar delimiter;
    int nbHeader=0;
    void checkString(QString &temp, QList<QString> &list, const QChar &character);
    int sorted=-1; //colonne triée
};

#endif // QDSVTABLEMODEL_H
