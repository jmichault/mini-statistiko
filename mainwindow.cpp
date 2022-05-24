#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include "qdsvtablemodel.h"

#define MAXVAR 100
#define MinP 0.020
#define MaxP 0.980

/*
 *
 TlfMat:array[0..MAXPOLY,0..MAXPOLY+1]of extended;
  lfPoly:array[0..MAXPOLY]of extended;
 lfX0,lfX1,lfY0,lfY1:extended;
 TVar:array[0..MAXVAR]of boolean;
 TlfVar:array[0..MAXVAR] of extended;
 asDescVar:array[1..MAXVAR] of string;
 asNomsVar:array[1..MAXVAR] of string;
 Precisionx:integer;//nb de points sur la courbe moyenne
 MinEchX:integer;// nb min de valeurs pour unpoint pour l'afficher
 Ficin:Textfile;
 nbVar:integer;// nombre de variable dans les données
 Derpoly:integer;
LCount:longInt;// nombre de lignes dans les données
LPoints:longInt;// nombre de points utilisés
TlfSommes// somme des x
  ,TlfCarres// somme des x*x
  ,TlfEcart// ecart-type de x
 :array[0..MAXVAR] of extended;
TlfPdt// somme des x*y
 :array[0..MAXVAR,0..MAXVAR] of extended;
TlfSomme,TlfCompte,TlfCarre:array[0..PRECISIONXMax] of extended;

TlfA     // coefficients A des plans (Z = a.X + b.Y + c)
  ,TlfB  // coefficients B des plans
  ,TlfC  // coefficients C des plans
  ,TlfS  // somme des Z
  ,TlfSC // sommes des Z*Z
  ,TlfP  // somme des Z*variable
  ,TlfS2 // somme des (Z-variable)
  ,TlfSC2// somme des (Z-variable)*(Z-variable)
 :array[0..MAXVAR*MAXVAR] of extended;
 Tix     // indice variable x
 ,Tiy    // indice variable y
 :array[0..MAXVAR*MAXVAR] of integer;

ligneEnCours:integer;

 * */


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Butono_sxargi_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, ("Open File"), ".", ("Fichier Texte(*.csv *.txt)"));
    if (!fileName.isEmpty()) {
        QDsvTableModel *model = new QDsvTableModel(this);
        QString extension = QFileInfo(QFile(fileName)).completeSuffix();
        if (extension.toLower() == "csv" || extension.toLower() == "tsv") //known file extensions
            model->loadFromFile(fileName,';');
        else {
            while (true) {
                bool ok;
                QString s = QInputDialog::getText(this, tr("Unknown File Format"),
                                                  tr("Enter Delimiter:"), QLineEdit::Normal,
                                                  "", &ok);
                if (ok && s.size() == 1) {
                    QChar delim = s.constData()[0];
                    model->loadFromFile(fileName, delim);
                    break;
                }
            }
        }
        ui->tableView->setModel(model);
    }
}
