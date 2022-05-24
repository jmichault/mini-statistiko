#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include "qdsvtablemodel.h"

#define MAXVAR 100
#define MinP 0.020
#define MaxP 0.980

bool
  TVar[MAXVAR]
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
 ,TlfPdt[MAXVAR][MAXVAR] // somme des x*y
;

QString
  AsNomsVar[MAXVAR]
 ;
/******************* *
 TlfMat:array[0..MAXPOLY,0..MAXPOLY+1]of extended;
  lfPoly:array[0..MAXPOLY]of extended;
 lfX0,lfX1,lfY0,lfY1:extended;
 TlfVar:array[0..MAXVAR] of extended;
 asDescVar:array[1..MAXVAR] of string;
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

void MainWindow::litNomsVar()
{
 int i;
  LCount=0;
  if (Donnees)
  {
    NbVar=ui->TV_Datoj->model()->columnCount()-1;
    for (i=0 ; i<NbVar ; i++)
    {
        QModelIndex idx = (ui->TV_Datoj->model()->index(i, 0));
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
      /*
    if CBgroupe.checked then
     begin
      for j:=0 to nbVar do
       begin
        lfx[j]:=0;
        lfy[j]:=0;
       end;
      for i:=1 to SEGroupe.value do
       begin
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
     end
    else
     begin
      litVar;
      inc(lcount);
      inc(lPoints);
     end;
    for i:=0 to nbVar do
     begin
      TlfSommes[i]:=TlfSommes[i]+TlfVar[i];
      TlfCarres[i]:=TlfCarres[i]+TlfVar[i]*TlfVar[i];
      for j:=0 to nbVar do
       if(TVar[i] and TVar[j]) then
         TlfPdt[i][j]:=TlfPdt[i][j] +TlfVar[i]*TlfVar[j];
     end;
   */
  }
  /*
  if not donnees then closefile(FicIn);
  Form1.SGCorrel.rowcount:=nbVar+3;
  Form1.SGCorrel.colcount:=nbVar+1;
  Form1.SGCorrel.cells[0,1]:='moyenne';
  Form1.SGCorrel.cells[0,2]:='écart type';
  for i:=1 to nbVar do
   begin
     if(TVar[i]) then
     begin
       lfTmp:=(TlfCarres[i]/LPoints-TlfSommes[i]*TlfSommes[i]/LPoints/LPoints);
       if lfTmp >0 then TlfEcart[i]:=sqrt(lfTmp)
       else TlfEcart[i]:=0;
       case form1.rgEntete.itemindex of
        1:
         Form1.SGCorrel.cells[0,i+2]:=IntTostr(i)+' : '+asNomsvar[i];
        2:
         Form1.SGCorrel.cells[0,i+2]:=IntTostr(i)+' : '+asNomsvar[i]+' : '+asDescVar[i];
        else
         Form1.SGCorrel.cells[0,i+2]:=IntTostr(i);
       end;
       Form1.SGCorrel.cells[i,0]:=IntTostr(i);
       Form1.SGCorrel.cells[i,1]:=FloatTostrF(TlfSommes[i]/LPoints,ffGeneral,3,2);
       Form1.SGCorrel.cells[i,2]:=FloatTostrF(TlfEcart[i],ffGeneral,3,2);
       Form1.SGCorrel.colwidths[i]:=30;
      end
     else
      begin
       Form1.SGCorrel.colwidths[i]:=3;
       Form1.SGCorrel.RowHeights[i+2]:=3;
      end;
  end;
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
