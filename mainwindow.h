#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qdsvtablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_Butono_sxargi_clicked();

    void on_Butono_Cor_clicked();

    void on_PBRegLin_clicked();

    void on_CB_kapo_currentIndexChanged(int index);

private:
    QDsvTableModel Model;
    void litNomsVar();
    void litVar(int ligne);
    void RegLin(int *tvars, int nbvars);
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
