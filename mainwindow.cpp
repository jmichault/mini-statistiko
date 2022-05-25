#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include "qdsvtablemodel.h"

#define MAXVAR 100
#define MinP 0.020
#define MaxP 0.980

bool
  TVar[MAXVAR] // la variable est-elle numérique ?
 ,Donnees=false;

int
  NbVar
 ;
long
  LCount// nombre de lignes dans les données
 ,LPoints// nombre de points utilisés
 ;

double
  TlfSommes[MAXVAR]// somme des x
 ,TlfCarres[MAXVAR]// somme des x*x
 ,TlfEcart[MAXVAR] // ecart-type de x
 ,TlfVar[MAXVAR]
 ,TlfPdt[MAXVAR][MAXVAR] // somme des x*y
;

QString
  AsNomsVar[MAXVAR]
 ,AsDescVar[MAXVAR]
 ;
/******************* *
 TlfMat:array[0..MAXPOLY,0..MAXPOLY+1]of extended;
  lfPoly:array[0..MAXPOLY]of extended;
 lfX0,lfX1,lfY0,lfY1:extended;
 Precisionx:integer;//nb de points sur la courbe moyenne
 MinEchX:integer;// nb min de valeurs pour unpoint pour l'afficher
 Ficin:Textfile;
 Derpoly:integer;
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
 *********** */

// lit les noms des variables dans la première ligne de l'onglet données.
void MainWindow::litNomsVar()
{
 int i;
  LCount=0;
  if (Donnees)
  {
    NbVar=ui->TV_Datoj->model()->columnCount()-1;
    for (i=0 ; i<NbVar ; i++)
    {
        QModelIndex idx = (ui->TV_Datoj->model()->index(0, i));
      AsNomsVar[i]=ui->TV_Datoj->model()->data(idx).toString();
    }
  }
/*
  else
  {
    nbvar:=MAXVAR-1;
    assignfile(ficin,EFichier.text);
    reset(ficin);
    case RGEntete.itemindex of
     1:
      begin
       LitEnteteTxt(ficin,asNomsVar,nbvar);
       for i:=1 to nbvar do
        asDescVar[i]:=asnomsvar[i];
      end;
     2:
      begin
       LitEnteteTxt(ficin,asDescVar,nbvar);
       LitEnteteTxt(ficin,asNomsVar,nbvar);
      end;
     else
       for i:=1 to MAXVAR do
        begin
         asnomsVar[i]:=intTostr(i);
         asDescVar[i]:=asnomsvar[i];
        end;
     end;
  }
***********************/
}

void MainWindow::litVar()
{
 long i;
  TlfVar[0]=1;
  if (Donnees)
  {
    for (i=0 ; i<NbVar ; i++)
    {
      if (TVar[i])
      {
        QModelIndex idx = (ui->TV_Datoj->model()->index(LCount+1,i));
        bool doubleOk;
        TlfVar[i]=ui->TV_Datoj->model()->data(idx).toDouble(&doubleOk);
        if (!doubleOk)
          TVar[i]=false;
      }
    }
  }
  //else litLigneTxt(ficin,TVar,TlfVar,nbVar);
}



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
        ui->TV_Datoj->setModel(model);
        Donnees = true;
    }
}

void MainWindow::on_Butono_Cor_clicked()
{
 int i,j;
 double lfTmp,lfX,lfY[MAXVAR];
  for (i=0 ; i<MAXVAR ; i++)
  {
    TVar[i] = true;// au départ on suppose que toutes les variables sont numériques
    TlfSommes[i] = 0;
    TlfCarres[i] = 0;// somme des x*x
    TlfEcart[i] = 0;// ecart-type de x
    for (j=0 ; j<MAXVAR ; j++)
      TlfPdt[i][j] =0.0;
  }
  litNomsVar();
  // boucle de lecture de la tableview :
  LCount = 0;
  LPoints = 0;
  while (LCount <= ui->TV_Datoj->model()->rowCount()-2)
  {

    if (ui->CBGroupe->isChecked() )
    { /*
      for (j=0 ; j<nbVar ; j++)
      {
        Lfx[j]=0;
        Lfy[j]=0;
      }
      for (i=1 ; i< ui->SEGroupe->value() )
      {
        if not finvar then
         begin
          litVar;
          for j:=0 to nbVar do
           begin
            lfx[j]:=lfx[j]+TlfVar[j];
            lfy[j]:=lfy[j]+TlfVar[j];
           end;
          inc(lcount);
         end
        else
         break;
       end;
      if i <SEGroupe.value+1 then
       break;
      inc(lPoints);
      for j:=0 to nbVar do
       begin
        TlfVar[j]:=lfx[j]/segroupe.value;
        TlfVar[j]:=lfy[j]/segroupe.value;
       end;
    */
    }
    else
    {
      litVar();
      LCount++;
      LPoints++;
    }
    for (i=0 ; i< NbVar ; i++)
    {
      TlfSommes[i]=TlfSommes[i]+TlfVar[i];
      TlfCarres[i]=TlfCarres[i]+TlfVar[i]*TlfVar[i];
      for ( j=0 ; j< NbVar ; j++)
       if(TVar[i] && TVar[j])
         TlfPdt[i][j]=TlfPdt[i][j] +TlfVar[i]*TlfVar[j];
     }
  }
  // if not donnees then closefile(FicIn);
  ui->TW_Cor->setRowCount(NbVar+3);
  ui->TW_Cor->setColumnCount(NbVar+1);
  QStringList *VLabels=new QStringList();
  QStringList *HLabels=new QStringList();
  for (i=0 ; i< NbVar ; i++)
  {
    if(TVar[i])
    {
       lfTmp=(TlfCarres[i]/LPoints-TlfSommes[i]*TlfSommes[i]/LPoints/LPoints);
       if (lfTmp >0) TlfEcart[i]=sqrt(lfTmp);
       else TlfEcart[i]=0;
//       if (ui->CB_kapo->currentIndex() ==1 )
         VLabels->append(QString("%1 : %2").arg(i+1).arg(AsNomsVar[i]));
//       if (ui->CB_kapo->currentIndex() ==2 )
//         VLabels->append(QString("%1 : %2 : %3").arg(i).arg(AsNomsVar[i]).arg(AsDescVar[i])));
//       else
//         VLabels->append(QString("%1").arg(i));
       if (i==0)
         HLabels->append(QString("N°:      %1\nmoyenne :  %2\nécart type : %3").arg(i+1).arg(TlfSommes[i]/LPoints,0,'g',3).arg(TlfEcart[i],0,'g',3));
       else
         HLabels->append(QString("%1\n%2\n%3").arg(i+1).arg(TlfSommes[i]/LPoints,0,'g',3).arg(TlfEcart[i],0,'g',3));
       ui->TW_Cor->setColumnWidth(i+1,30);
    }
    else
    {
      ui->TW_Cor->setColumnWidth(i,3);
      ui->TW_Cor->setRowHeight(i+2,3);
    }
  }
  ui->TW_Cor->setVerticalHeaderLabels(*VLabels);
  ui->TW_Cor->setHorizontalHeaderLabels(*HLabels);
/*
    for i:=1 to nbVar+1 do
    begin
     if(not TVar[i])then continue;
     for j:=1 to nbVar+1 do
      begin
       if(TVar[j]) then
        begin
         lfTmp:=TlfPdt[i][j]/LPoints-TlfSommes[i]*TlfSommes[j]/LPoints/LPoints;
         if( TlfEcart[i]<> 0)and (TlfEcart[j]<>0) then
            lfTmp:= lfTmp/TlfEcart[i]/TlfEcart[j]
         else lfTmp:=0;
         Form1.SGCorrel.cells[i,j+2]
//            :=FloatToStr(lfTmp);
            :=FloatToStrF(lfTmp,ffFixed,4,3);
        end;
     end;
   end;
*/
}
