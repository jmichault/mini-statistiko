#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
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
int NbHeader=0; // nombre de lignes d'en-têtes

QDsvTableModel *Model;
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

// lit les noms des variables dans l'en-tête de l'onglet données.
void MainWindow::litNomsVar()
{
 int i;
  LCount=0;
  if (Donnees)
  {
    NbVar=ui->TV_Datoj->model()->columnCount();
    for (i=0 ; i<NbVar ; i++)
    {
      AsNomsVar[i]=ui->TV_Datoj->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString();
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
        QModelIndex idx = (ui->TV_Datoj->model()->index(LCount,i));
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
        Model = new QDsvTableModel;
        QString extension = QFileInfo(QFile(fileName)).completeSuffix();
        if (extension.toLower() == "csv" || extension.toLower() == "tsv") //known file extensions
            Model->loadFromFile(fileName,';');
        else {
            while (true) {
                bool ok;
                QString s = QInputDialog::getText(this, tr("Unknown File Format"),
                                                  tr("Enter Delimiter:"), QLineEdit::Normal,
                                                  "", &ok);
                if (ok && s.size() == 1) {
                    QChar delim = s.constData()[0];
                    Model->loadFromFile(fileName, delim);
                    break;
                }
            }
        }
        ui->TV_Datoj->setModel(Model);
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
  while (LCount < ui->TV_Datoj->model()->rowCount())
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
  ui->TW_Cor->setRowCount(NbVar);
  ui->TW_Cor->setColumnCount(NbVar);
  QStringList *VLabels=new QStringList();
  QStringList *HLabels=new QStringList();
  for (i=0 ; i< NbVar ; i++)
  {
    if(TVar[i])
    {
       lfTmp=(TlfCarres[i]/LPoints-TlfSommes[i]*TlfSommes[i]/LPoints/LPoints);
       if (lfTmp >0) TlfEcart[i]=sqrt(lfTmp);
       else TlfEcart[i]=0;
       ui->TW_Cor->setColumnWidth(i+1,70);
    }
    else
    {
      ui->TW_Cor->setColumnWidth(i,2);
      ui->TW_Cor->setRowHeight(i,2);
    }
    if (ui->CB_kapo->currentIndex() ==1 )
      VLabels->append(QString("%1 : %2").arg(i+1).arg(AsNomsVar[i]));
    else if (ui->CB_kapo->currentIndex() ==2 )
      VLabels->append(QString("%1 : %2 : %3").arg(i+1).arg(AsNomsVar[i]).arg(AsDescVar[i]));
    else
      VLabels->append(QString("%1").arg(i+1));
    if (i==0)
      HLabels->append(QString("N°:      %1\nmoyenne :  %2\nécart type : %3").arg(i+1).arg(TlfSommes[i]/LPoints,0,'g',3).arg(TlfEcart[i],0,'g',3));
    else
      HLabels->append(QString("%1\n%2\n%3").arg(i+1).arg(TlfSommes[i]/LPoints,0,'g',3).arg(TlfEcart[i],0,'g',3));
  }
  ui->TW_Cor->setVerticalHeaderLabels(*VLabels);
  ui->TW_Cor->setHorizontalHeaderLabels(*HLabels);

  for (i=0 ; i<NbVar ; i++)
  {
    if(! TVar[i]) continue;
    for ( j=0 ; j< NbVar ; j++)
    {
      if(TVar[j])
      {
         lfTmp=TlfPdt[i][j]/LPoints-TlfSommes[i]*TlfSommes[j]/LPoints/LPoints;
         if ( (TlfEcart[i] != 0) && (TlfEcart[j]!=0))
            lfTmp= lfTmp/TlfEcart[i]/TlfEcart[j];
         else lfTmp=0;
         ui->TW_Cor->setItem(j,i,new QTableWidgetItem(QString("%1").arg(lfTmp,0,'g',3)));
      }
    }
  }
}

/*
procedure faittest(tvars:array of integer;nbvars:integer);
var
varY:integer;// numéro de la variable à expliquer
formule,tmpstr:string;
tmpc:char;
virgule:boolean;
i,j,k,l,n,x:integer;
lfS,lfSC,lfS2,lfSC2,lfP,lfZ,lfVar,lfTmp,lfEcart,lfY,cor1,cor2
  :extended;
ligne:integer;//N° de ligne dans la stringgrid;
begin
ligne:=nbvars;
varY:=form1.SEVarY.value;;
form1.sgdata2.colcount:=nbvars+4;
for x:= 1 to nbVar do
 begin
  if (not tvar[x]) or (x=varY) then continue;
  tvars[nbvars]:= x;
  for i:=1 to nbvars-1 do
   if (x=tvars[i]) or (vary=x) then break;
  if (i<nbvars) and ((x=tvars[i]) or (vary=x)) then continue;
  // remplir la matrice
  for i:=0 to nbVars do
   begin
     for j:=0 to nbVars do
      TlfMat[i,j]:=TlfPdt[tvars[i]][tvars[j]]/LPoints;
     TlfMat[i,nbVars+1]:= -TlfPdt[tvars[i]][varY]/LPoints;
   end;
  // résoudre la matrice
  resoutMatrice(nbVars);
  //afficher le polynome
  formule:='';
  for i:=1 to nbvars do
   begin
    formule:=formule+'-'+inttostr(tvars[i]);
    form1.SGData2.Cells[3+i,ligne]:=IntTostr(tvars[i]);
   end;
  form1.sgdata2.cells[0,ligne]:=formule;
  formule:='';
  for j:=0 to nbVars do
   begin
    tmpstr:=floatToStr(-TlfMat[j,nbVars+1]);
    l:=0;virgule:=false;
    for k:=1 to length(tmpstr) do
     begin
      tmpc:=tmpstr[k];
      if (tmpc=',') or (tmpc='.') then virgule:=true;
      if (tmpc>='1')and(tmpc<='9') then inc(l);
      if (l>=4) and virgule then break;
     end;
    if j=0 then
      formule:=formule+' '+copy(tmpstr,1,k)
    else
      if form1.rgEntete.itemindex=2 then
       formule:=formule+'+'+asNomsVar[tvars[j]]+'*'+copy(tmpstr,1,k)
      else
       formule:=formule+'+'+':'+intTostr(tvars[j])+'*'+copy(tmpstr,1,k);
    if pos('E',tmpstr) >0 then
      formule:=formule+copy(tmpstr,pos('E',tmpstr),10);
   end;
  form1.sgdata2.cells[1,ligne]:=formule;
  //calcul moyenne et écart type
  lfS:=0;
  lfSC:=0;
  lfS2:=0;
  lfSC2:=0;
  lfP:=0;
  // boucle de la stringGrid :
  evalexpr(formule,getvars);
  ligneEnCours:=1;
  while ligneEnCours < form1.SGDATA.rowcount do
   begin
    if form1.CBgroupe.checked then
     begin
      lfZ:=0;
      lfy:=0;
      for i:=1 to form1.SEGroupe.value do
       begin
        if ligneEncours < form1.SGDATA.rowcount then
         begin
          lfZ:=lfZ+EvalTok(getvari);
          lfY:=lfY+strToFloat(form1.SGData.cells[varY,ligneEnCours]);
          inc(ligneEnCours);
         end
        else
         break;
       end;
      if i <form1.SEGroupe.value+1 then
       break;
      lfZ:=lfZ/form1.segroupe.value;
      lfY:=lfy/form1.segroupe.value;
     end
    else
     begin
      lfZ:=EvalTok(getvari);
      lfY:=strToFloat(form1.SGData.cells[varY,ligneEnCours]);
      inc(ligneEncours);
     end;
    lfS:=lfS+lfZ;
    lfSC:=lfSC+lfZ*lfZ;
    lfS2:=lfS2+lfZ-lfY;
    lfSC2:=lfSC2+(lfZ-lfY)*(lfZ-lfY);
    lfP:=lfP+lfZ*lfY;
   end;
    // calcul ecart type de (Z-variable)
    lfTmp:=(lfSC2/LPoints-lfS2*lfS2/LPoints/LPoints);
    if lfTmp >0 then lfTmp:=sqrt(lfTmp)
    else lfTmp:=0;
    form1.SGData2.Cells[2,ligne]:=FloatTostrF(lfTmp,ffFixed,4,3);
    // calcul correlation Z et variable
    // d'abord calcul ecart type de Z :
    lfTmp:=lfSC/LPoints-lfS*lfS/LPoints/LPoints;
    if lfTmp >0 then lfEcart:=sqrt(lfTmp)
    else lfEcart:=0;
    lfTmp:=lfP/LPoints-lfS*TlfSommes[varY]/LPoints/LPoints;
    if( TlfEcart[varY]<> 0)and (lfEcart>0) then
       lfTmp:= lfTmp/TlfEcart[varY]/lfEcart
    else lfTmp:=0;
    form1.SGData2.Cells[3,ligne]:=FloatTostrF(lfTmp,ffFixed,5,4);
    inc(ligne);
    form1.sgdata2.rowcount:=ligne+1;
 end;//for x
 // trier les lignes :
 for i:= nbvars to ligne-2 do
  begin
   for j:=i+1 to ligne-1 do
    begin
     cor1:=StrToFloat(form1.SGData2.cells[3,i]);
     cor2:=StrToFloat(form1.SGData2.cells[3,j]);
     if cor2 >cor1 then
      begin
       form1.SGData2.rows[0]:=form1.SGData2.rows[i];
       form1.SGData2.rows[i]:=form1.SGData2.rows[j];
       form1.SGData2.rows[j]:=form1.SGData2.rows[0];
      end;
    end;
  end;
 form1.SGData2.cells[0,0]:='';
 form1.SGData2.cells[1,0]:='formule';
 form1.SGData2.cells[2,0]:='EC(f-variable)';
 form1.SGData2.cells[3,0]:='correlation';

end;
*/

// calcul des régressions linéaires
void MainWindow::on_PBRegLin_clicked()
{

}

void MainWindow::on_CB_kapo_currentIndexChanged(int index)
{
  Model->setNbHeader(index);
}
