#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "eval.h"

#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QGraphicsTextItem>

#define MAXVAR 100
#define MinP 0.020
#define MaxP 0.980

// degré maximum du polynôme
#define MAXPOLY 10

bool
  TVarIsNum[MAXVAR] // la variable est-elle numérique ?
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
//int NbHeader=0; // nombre de lignes d'en-têtes

double
  TlfMat[MAXPOLY][MAXPOLY]
;

int DerPoly;

/******************* *
 TlfMat:array[0..MAXPOLY,0..MAXPOLY+1]of extended;
  lfPoly:array[0..MAXPOLY]of extended;
 lfX0,lfX1,lfY0,lfY1:extended;
 Precisionx:integer;//nb de points sur la courbe moyenne
 MinEchX:integer;// nb min de valeurs pour unpoint pour l'afficher
 Ficin:Textfile;
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
 *********** */

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(0,true);
    ui->tabWidget->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(2,false);
    ui->tabWidget->setTabEnabled(3,false);

}


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

// lit une ligne de données dans TV_Datoj
void MainWindow::litVar(int ligne)
{
 long i;
  TlfVar[0]=1;
  if (Donnees)
  {
    for (i=0 ; i<ModelDatoj.columnCount() ; i++)
    {
      if (TVarIsNum[i])
      {
        QModelIndex idx = (ui->TV_Datoj->model()->index(ligne,i));
        bool doubleOk;
        TlfVar[i]=ui->TV_Datoj->model()->data(idx).toDouble(&doubleOk);
        if (!doubleOk)
          TVarIsNum[i]=false;
      }
    }
  }
  //else litLigneTxt(ficin,TVarIsNum,TlfVar,nbVar);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_Butono_sxargi_clicked()
{
  ModelDatoj.setNbHeader(0);
  for (int i=0 ; i<MAXVAR ; i++)
    TVarIsNum[i] = true;// au départ on suppose que toutes les variables sont numériques

  QString fileName = QFileDialog::getOpenFileName(this, ("Open File"), ".", ("Fichier Texte(*.csv *.txt)"));
  if (!fileName.isEmpty()) {
        if (ui->CB_Apartigilo->currentText() == "")
        {
          QString extension = QFileInfo(QFile(fileName)).completeSuffix();
          if (extension.toLower() == "csv" || extension.toLower() == "tsv") //known file extensions
            ModelDatoj.loadFromFile(fileName,';');
          else {
            while (true) {
                bool ok;
                QString s = QInputDialog::getText(this, tr("Unknown File Format"),
                                                  tr("Enter Delimiter:"), QLineEdit::Normal,
                                                  "", &ok);
                if (ok && s.size() == 1) {
                    QChar delim = s.constData()[0];
                    ModelDatoj.loadFromFile(fileName, delim);
                    break;
                }
            }
          }
        }
        else if (ui->CB_Apartigilo->currentText() == "\\t")
          ModelDatoj.loadFromFile(fileName,'\t');
        else  ModelDatoj.loadFromFile(fileName,ui->CB_Apartigilo->currentText()[0]);
        ui->TV_Datoj->setModel(&ModelDatoj);
        Donnees = true;
  }
  ui->Butono_Kor->setEnabled(true);
  ui->tabWidget->setTabEnabled(1,true);
  ui->tabWidget->setTabEnabled(2,false);
  ui->tabWidget->setTabEnabled(3,false);
  ModelDatoj.setNbHeader(ui->CB_kapo->currentIndex());
}

void MainWindow::on_Butono_Kor_clicked()
{
 int i,j;
 double lfTmp;
  for (i=0 ; i<MAXVAR ; i++)
  {
    TVarIsNum[i] = true;// au départ on suppose que toutes les variables sont numériques
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
      litVar(LCount);
      LCount++;
      LPoints++;
    }
    for (i=0 ; i< NbVar ; i++)
    {
      TlfSommes[i]=TlfSommes[i]+TlfVar[i];
      TlfCarres[i]=TlfCarres[i]+TlfVar[i]*TlfVar[i];
      for ( j=0 ; j< NbVar ; j++)
       if(TVarIsNum[i] && TVarIsNum[j])
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
    if(TVarIsNum[i])
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
    if(! TVarIsNum[i]) continue;
    for ( j=0 ; j< NbVar ; j++)
    {
      if(TVarIsNum[j])
      {
         lfTmp=TlfPdt[i][j]/LPoints-TlfSommes[i]*TlfSommes[j]/LPoints/LPoints;
         if ( (TlfEcart[i] != 0) && (TlfEcart[j]!=0))
            lfTmp= lfTmp/TlfEcart[i]/TlfEcart[j];
         else lfTmp=0;
         QTableWidgetItem *item=new QTableWidgetItem(QString("%1").arg(lfTmp,0,'g',3));
         //int brillance=trunc(abs(lfTmp*127));
         //item->setBackground(QColor(100+brillance/2,100+brillance,100+brillance/2));
         //item->setForeground(QColor(128+brillance,128+brillance,128+brillance));

         if(lfTmp>0.9 || lfTmp<-0.9)
           item->setForeground(Qt::red);
         else if(lfTmp>0.5 || lfTmp<-0.5)
           item->setForeground(Qt::magenta);
         else if(lfTmp>0.3 || lfTmp<-0.3)
           item->setForeground(Qt::darkMagenta);
         else
           item->setForeground(Qt::gray);

         ui->TW_Cor->setItem(j,i,item);
         //ui->TW_Cor->setItem(j,i,new QTableWidgetItem(QString("%1").arg(lfTmp,0,'g',3)));
      }
    }
  }
  ui->tabWidget->setTabEnabled(0,true);
  ui->tabWidget->setTabEnabled(1,true);
  ui->tabWidget->setTabEnabled(2,true);
  ui->tabWidget->setTabEnabled(3,true);
}


/**********************
 * résout le système de (n+1) équations à (n+1) inconnues contenu dans TlfMat.
 *  equation i :  somme(0<=j<=n) des ( Xj * TlfMat[i][j] ) + TlfMat[i][n+1] = 0
 * TlfMat est donc un tableau de (n+1) lignes * (n+2) colonnes.
*************************/
void resoutMatrice(int n)
{
 int i,i2,j;
 double lfTmp;
  for (i=0 ; i<=n ; i++)
  {
    //ramener le coefficient de la ieme inconnue à 1 :
    lfTmp=TlfMat[i][i];
    //TODO gérer le cas TlfMat[i][i] == 0.0 ...
    //
    for ( j=0 ; j<=n+1 ; j++)
       if (lfTmp !=0.0)
         TlfMat[i][j]=TlfMat[i][j] / lfTmp;
    // on a donc Xi = - (somme des (Xj * TlfMat[i][j] ) -TlfMat[i][n] .
    // on remplace Xi par sa valeur dans les équations suivantes :
    // dans les équations suivantes :
    for ( i2=i+1 ; i2<=n ; i2++)
    {
      for ( j=i+1 ; j<=n+1 ; j++)
          TlfMat[i2][j]=TlfMat[i2][j] - TlfMat[i][j]*TlfMat[i2][i];
      TlfMat[i2][i]=0;
    }
  }
  // on obtient ainsi la valeur de Xn dans -TlfMat[n][n+1], on répercute sa valeur
  // dans les équations précédentes, ce qui nous donne Xn-1, ... :
  for (i=n ; i>=1 ; i--)
  {
    // remplacer la ieme inconnue par sa valeur
    // dans les équations précédentes :
    for ( i2=i-1 ; i2>=0 ; i2--)
    {
        TlfMat[i2][n+1] =TlfMat[i2][n+1]- TlfMat[i][n+1]*TlfMat[i2][i];
        TlfMat[i2][i]=0;
    }
  }
   // normalement, on obtient une matrice carrée diagonale unitaire + les solutions dans la dernière colonne
}

int getvars(QString x)
{
  for ( int i=0 ; i<NbVar ; i++ )
  {
    QString var=AsNomsVar[i];
    var.replace(' ','_');
    if (x==var)
        return i+1;
  }
  return -1;
}

double getvari(int x)
{
  return TlfVar[x-1];
}

//Régression linéaire multiple :
// on utilise la méthode des moindres carrés «ordinaire» (i.e. sur l'ordonnée= en vertical)
// avec n variables (V0 à Vn-1) :
// droite cherchée : f(x) = A0 * V0 + A1 * V1 ....+ An-1 * Vn-1 + An
// somme des carrés = somme( (Y-f(x)) * (Y-f(x)) )
//                  = somme ( (Y - A0*V0 -A1*V1... -An ) * (Y - A0*V0 -A1*V1... -An ) )
//                  = somme ( Y*Y) + Ai*Ai*somme(Vi*Vi) + An*An*somme(1)
//                      + 2*Ai*Aj*somme[(Vi*Vj)]...(pour j de 0 à n-1 avec j!=i)
//                      - 2*Ai*somme(Y*Vi)
//                      + 2*An*Ai*somme(Vi)
//                      - 2*An*somme(Y)
//
// le moindre carré est obtenu quand toutes les dérivées s'annulent :
//      pour tout i : d S/ d Ai = 0
//      pour i<n : d S / d Ai = d AiCarré*somme(ViCarré) + 2*Ai*Aj*somme (Vi*Vj)pour(j!=i) -2*Ai*somme(Y*Vi)
//                                +  2*An*Ai*somme(Vi)/ d Ai
//              = 2*Ai*somme(Vi*Vi) +2*Aj*somme(Vj*Vi) -2*somme(Y*Vi) +2*An*somme(Vi)
//              -->       Ai*somme(Vi*Vi)
//                        + Aj*somme(Vj*Vi)...(pour tout j de 0 à n-1 avec j!=i)
//                        + An*somme(Vi)
//                        - somme(Y*Vi)
//                                = 0
//      pour i=n : d S / d An = 2*An*somme(1)
//                                + 2*Ai*somme(Vi)...(pour i de 0 à n-1)
//                                - 2*somme(Y)
//              -->     Ai*somme(Vi) + An*somme(1) - somme(Y) = 0
//
//
// tVars = tableau des variables
// nbvars = nb de variables
// ligne = numéro de ligne dans la QTableWidget
void MainWindow::RegLin(int *tVars, int nbvars, int ligne)
{
 int varY;// numéro de la variable à expliquer
 QString formule,tmpstr;
  varY=ui->SB_Reg_V0->value()-1;
    // remplir la matrice des équations à résoudre
    for (int i=0 ; i<nbvars ; i++)
    {
      for (int j=0 ; j<nbvars ; j++)
        TlfMat[i][j]=TlfPdt[tVars[i]][tVars[j]]/LPoints;
      TlfMat[i][nbvars]= TlfSommes[tVars[i]]/LPoints;
      TlfMat[i][nbvars+1]= -TlfPdt[tVars[i]][varY]/LPoints;
    }
    for (int j=0 ; j<nbvars ; j++)
      TlfMat[nbvars][j]=TlfSommes[tVars[j]];
    TlfMat[nbvars][nbvars] = LPoints;
    TlfMat[nbvars][nbvars+1] = -TlfSommes[varY];
    // résoudre la matrice
    resoutMatrice(nbvars);
    //afficher la liste des variables
    formule="";
    for (int i=0 ; i<nbvars ; i++)
    {
      formule.append(QString("-%1").arg(tVars[i]+1));
      ModelRegLin.setData(ligne,4+i,QString("%1").arg(tVars[i]+1));
      //ui->TV_RegLin->setItem(ligne,4+i,new QTableWidgetItem(QString("%1").arg(tVars[i]+1)));
    }
    ModelRegLin.setData(ligne,0,formule);
    //
    // afficher le polynôme
    formule="";
    if (ui->CBUzuMoy->isChecked())
    {
      double constante = -TlfMat[nbvars][nbvars+1];
      if (ui->CBNomoj->isChecked())
      {
        for (int j=0 ; j<nbvars ; j++)
        {
          QString Nomo=ModelDatoj.header1[tVars[j]];
          Nomo.replace(' ','_');
          double moy=TlfSommes[tVars[j]]/LPoints;
          constante += moy*(-TlfMat[j][nbvars+1]);
          if (moy>=0.0)
            formule.append(QString("(%1-%2)*%3+").arg(Nomo).arg(moy,0,'G',4).arg(-TlfMat[j][nbvars+1],0,'G',4));
          else
            formule.append(QString("(%1+%2)*%3+").arg(Nomo).arg(-moy,0,'G',4).arg(-TlfMat[j][nbvars+1],0,'G',4));
        }
        if(constante <0)
            formule.truncate(formule.length()-1);
        formule.append(QString("%1").arg(constante,0,'G',4));
      }
      else
      {
        for (int j=0 ; j<nbvars ; j++)
        {
          double moy=TlfSommes[tVars[j]]/LPoints;
          constante += moy*(-TlfMat[j][nbvars+1]);
          if (moy>=0.0)
            formule.append(QString("(:%1-%2)*%3+").arg(tVars[j]+1).arg(moy,0,'G',4).arg(-TlfMat[j][nbvars+1],0,'G',4));
          else
            formule.append(QString("(:%1+%2)*%3+").arg(tVars[j]+1).arg(-moy,0,'G',4).arg(-TlfMat[j][nbvars+1],0,'G',4));
        }
        formule.append(QString("%1").arg(constante,0,'G',4));
      }

    }
    else
    {
      formule=QString("%1").arg(-TlfMat[nbvars][nbvars+1]);
      if (ui->CBNomoj->isChecked())
      {
        for (int j=0 ; j<nbvars ; j++)
        {
          QString Nomo=ModelDatoj.header1[tVars[j]];
          Nomo.replace(' ','_');
          formule.append(QString("+%1*%2").arg(Nomo).arg(-TlfMat[j][nbvars+1],0,'G',4));
        }
      }
      else
      {
        for (int j=0 ; j<nbvars ; j++)
          formule.append(QString("+:%1*%2").arg(tVars[j]+1).arg(-TlfMat[j][nbvars+1],0,'G',4));
      }
    }
    ModelRegLin.setData(ligne,1,formule);
    //calcul moyenne et écart type
    double lfS,lfSC,lfS2,lfSC2,lfP,lfZ,lfTmp,lfEcart,lfY;
    lfS=0.0;
    lfSC=0.0;
    lfS2=0.0;
    lfSC2=0.0;
    lfP=0.0;
    formule = ModelRegLin.data(ligne,1).toString();
    QList<struct TTok> evalTokens;
    evalExpr(formule,getvars,&evalTokens);
    for(int ligneEnCours=0 ; ligneEnCours < ui->TV_Datoj->model()->rowCount() ; ligneEnCours++ )
    {
      litVar(ligneEnCours);
      /*
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
     */
      {
        lfZ=evalTok(getvari,&evalTokens);
        lfY=TlfVar[varY];
      }
      lfS=lfS+lfZ;
      lfSC=lfSC+lfZ*lfZ;
      lfS2=lfS2+lfZ-lfY;
      lfSC2=lfSC2+(lfZ-lfY)*(lfZ-lfY);
      lfP=lfP+lfZ*lfY;
    } // fin while
    // calcul ecart type de (Z-variable)
    lfTmp=(lfSC2/LPoints-lfS2*lfS2/LPoints/LPoints);
    if (lfTmp >0) lfTmp=sqrt(lfTmp);
    else lfTmp=0;
    ModelRegLin.setData(ligne,2,QString("%1").arg(lfTmp));
    // calcul correlation Z et variable
    // d'abord calcul ecart type de Z :
    lfTmp=lfSC/LPoints-lfS*lfS/LPoints/LPoints;
    if (lfTmp >0) lfEcart=sqrt(lfTmp);
    else lfEcart=0;
    lfTmp=lfP/LPoints-lfS*TlfSommes[varY]/LPoints/LPoints;
    if (( TlfEcart[varY]!= 0) && (lfEcart>0))
       lfTmp= lfTmp/TlfEcart[varY]/lfEcart;
    else lfTmp=0;
    ModelRegLin.setData(ligne,3,QString("%1").arg(abs(lfTmp)));
}

// fait les régressions linéaires en ajoutant une variable à celles déjà sélectionnées
// tVars = tableau des variables imposées
// nbvars = nb de variables total (on a donc nbvars-1 variables déjà dans le tableau)
void MainWindow::RegLins(int *tVars, int nbvars)
{
 int varY;// numéro de la variable à expliquer
 QString formule,tmpstr;
 int i,x;
 int ligne;//N° de ligne dans la QTableWidget;
  ligne=nbvars-1;
  varY=ui->SB_Reg_V0->value()-1;
  // on calcule la droite la plus proche pour chaque variable non traitée
  for (x=0 ; x<NbVar ; x++)
  {
    if ((! TVarIsNum[x]) || (x==varY)) continue; // sauter les variables non numériques et varY
    // sauter les variables déjà intégrées
    for (i=0 ; i<nbvars-1 ; i++)
       if ((x==tVars[i]) || (varY==x)) break;
    // si on est sorti de la boucle précédente par le break, on saute cette variable :
    if ( (i<nbvars-1) && ((x==tVars[i]) || (varY==x))) continue;
    tVars[nbvars-1]= x;
    RegLin(tVars,nbvars,ligne);
    //ui->TV_RegLin->setItem(ligne,3,new QTableWidgetItem(lfTmp));
    ligne++;
  }//for x
  ModelRegLin.sortN(3,nbvars-1,0,Qt::DescendingOrder); // trier
}

// calcul des régressions linéaires
void MainWindow::on_PBRegLin_clicked()
{
 int nbVars;
 int tVars[MAXVAR];
  ModelRegLin.setNbHeader(1);
  ModelRegLin.clearData();
  ModelRegLin.header1 << "Variables"<<"formule"<<"EC(f-variable)"<<"abs(corr)";
  for (int i=1 ; i<=ui->SBNbImp->value() ; i++)
    ModelRegLin.header1 << QString("V%1").arg(i);
  if(ui->SBNbImp->value() >0)
  {
    tVars[0]=ui->SBVar1->value()-1;
    RegLin(tVars,1,0);
  }
  if(ui->SBNbImp->value() >1)
  {
    tVars[1]=ui->SBVar2->value()-1;
    RegLin(tVars,2,1);
  }
  if(ui->SBNbImp->value() >2)
  {
    tVars[2]=ui->SBVar3->value()-1;
    RegLin(tVars,3,2);
  }
  if(ui->SBNbImp->value() >3)
  {
    tVars[3]=ui->SBVar4->value()-1;
    RegLin(tVars,4,3);
  }
  if(ui->SBNbImp->value() >4)
  {
    tVars[4]=ui->SBVar5->value()-1;
    RegLin(tVars,5,4);
  }
  for (nbVars=ui->SBNbImp->value()+1 ; nbVars <= ui->SBNbVars->value() ; nbVars++)
  {
    RegLins(tVars,nbVars);
    tVars[nbVars-1] = ModelRegLin.data(nbVars-1,nbVars+3).toInt()-1;
  }
  ui->TV_RegLin->setModel(&ModelRegLin);
}

void MainWindow::on_CB_kapo_currentIndexChanged(int index)
{
  ModelDatoj.setNbHeader(index);
}

void MainWindow::on_SBNbImp_valueChanged(int arg1)
{
    ui->SBVar1->setEnabled((arg1>=1)?true:false);
    ui->SBVar2->setEnabled((arg1>=2)?true:false);
    ui->SBVar3->setEnabled((arg1>=3)?true:false);
    ui->SBVar4->setEnabled((arg1>=4)?true:false);
    ui->SBVar5->setEnabled((arg1>=5)?true:false);
}

void MainWindow::on_PBForigu_clicked()
{
 int Kol;
  //Kol = ui->TV_Datoj->selectionModel()->selectedColumns()[0].column();
  Kol = ui->TV_Datoj->selectionModel()->currentIndex().column();
  ModelDatoj.removeKol(Kol);
  ModelDatoj.headerDataChanged(Qt::Horizontal, 0, ModelDatoj.header1.count());
}

void MainWindow::on_PBAldonu_clicked()
{
 int Kol;
  Kol = ModelDatoj.columnCount();
  litNomsVar();

  QList<struct TTok> evalTokens;
  evalExpr(ui->LEFormule->text(),getvars,&evalTokens );
  for (int i=0 ; i<ModelDatoj.rowCount() ; i++ )
  {
    litVar(i);
    ModelDatoj.setData(i, Kol, evalTok(getvari,&evalTokens));
  }
  if(ModelDatoj.getNbHeader()>=1) ModelDatoj.header1 << ui->LEFormule->text().arg(Kol+1);
  ModelDatoj.headerDataChanged(Qt::Horizontal, 0, ModelDatoj.header1.count());
}

static void remplitmatrice(int n,double tlfSXk[], double tlfSXkY[],int nbPoints)
{
  for ( int i=0 ; i<=n ; i++)
  {
    for (int j=0 ; j<=n ; j++)
    {
      TlfMat[i][j]=tlfSXk[j+i]/nbPoints;
      TlfMat[i][n+1]= -tlfSXkY[i]/nbPoints;
    }
  }
}

void MainWindow::on_PBGraf_clicked()
{
  if(!Donnees) return;
 QGraphicsScene *scene = new QGraphicsScene(this);
 QGraphicsTextItem *text;
 QGraphicsLineItem *line;
 ui->GVGraf->setScene(scene);
 ui->GVGraf->show();

 QBrush redBrush(Qt::red);
 QBrush greenBrush(Qt::green);
 QBrush blueBrush(Qt::blue);
 QPen outlinePen(Qt::black);
 outlinePen.setWidth(2);
 QPen pointPen(Qt::red);
 pointPen.setWidth(1);
 QPen moyPen(Qt::blue);
 moyPen.setWidth(2);
 QPen polyPen(Qt::green);
 polyPen.setWidth(1);

  DerPoly=ui->SBDerPoly->value();
  // calcul des polynômes : préparation
  double TlfSXk[MAXPOLY*2+2];
  double TlfSXkY[MAXPOLY*2+2];
  for (int i=0 ; i<=2*DerPoly ; i++)
  {
    TlfSXk[i]=0;
    TlfSXkY[i]=0;
  }

  // dessin du nuage de points :
 qreal W(ui->GVGraf->width());
 qreal H(ui->GVGraf->height());
 int varX=ui->SBGraVarX->value()-1;
 int varY=ui->SBGraVarY->value()-1;
  ModelDatoj.sortN(varX,0,0,Qt::AscendingOrder); // trier

  double moyX=TlfSommes[varX]/LCount;
  double minX=moyX-2*TlfEcart[varX];
  double maxX=moyX+2*TlfEcart[varX];
  double moyY=TlfSommes[varY]/LCount;
  double minY=moyY-2*TlfEcart[varY];
  double maxY=moyY+2*TlfEcart[varY];
  qreal grafX=(moyX-minX)*W/(maxX-minX);
  qreal grafY=(maxY-moyY)*H/(maxY-minY);

  scene->addLine(2,grafY,W-2,grafY,outlinePen);
  scene->addLine(grafX,H-2,grafX,2,outlinePen);
  text = scene->addText(QString("X=%1").arg(minX), QFont("Arial", 8) );
  text->setPos(1,grafY);
  text = scene->addText(QString("X=%1").arg(maxX), QFont("Arial", 8) );
  text->setPos(W - text->boundingRect().width()-1,grafY);
  text = scene->addText(QString("Y=%1").arg(maxY), QFont("Arial", 8) );
  text->setPos(grafX,1);
  text = scene->addText(QString("Y=%1").arg(minY), QFont("Arial", 8) );
  text->setPos(grafX,H - text->boundingRect().height()-1);
  int tailleEch=ModelDatoj.rowCount()/W*10;
  int nbEch=0;
  double totX=0.0, totY=0.0;

  for (int i=0 ; i<ModelDatoj.rowCount() ; i++ )
  {
    litVar(i);
    double X=TlfVar[varX];
    double Y=TlfVar[varY];
    totX += X;
    totY += Y;
    if (++nbEch >= tailleEch)
    {
      double moyX=  totX/nbEch;
      double moyY = totY/nbEch;
//      if ((minX<moyX && moyX<maxX) && (minY<moyY && moyY<maxY))
      {
        grafX=(moyX-minX)*W/(maxX-minX);
        grafY=(maxY-moyY)*H/(maxY-minY);
        scene->addLine(grafX-2,grafY,grafX+2,grafY,moyPen);
        line = scene->addLine(grafX,grafY-2,grafX,grafY+2,moyPen);
        line->setToolTip(QString("X=%1,Y=%2").arg(moyX).arg(moyY));
      }
      totY=0.0;
      totX=0.0;
      nbEch=0;
    }
    grafX=(X-minX)*W/(maxX-minX);
    grafY=(maxY-Y)*H/(maxY-minY);
//    if ((minX<X && X<maxX) && (minY<Y && Y<maxY))
    {
        //scene->addLine(grafX-1,grafY,grafX+1,grafY,outlinePen);
        line = scene->addLine(grafX,grafY,grafX,grafY,pointPen);
        line->setToolTip(QString("X=%1,Y=%2").arg(X).arg(Y));
    }
    // préparation polynômes :
    double lfTmp=1.0;
    for (int j=0 ; j<=2*DerPoly ; j++)
    {
      TlfSXk[j]=TlfSXk[j]+lfTmp;
      lfTmp=lfTmp*TlfVar[varX];
    }
    lfTmp=TlfVar[varY];
    for (int j=0 ; j<=DerPoly ; j++)
    {
      TlfSXkY[j]=TlfSXkY[j]+lfTmp;
      lfTmp=lfTmp*TlfVar[varX];
    }

  }
  // calcul des polynômes :
  ModelPoly.setNbHeader(2);
  ModelPoly.clearData();

  for (int i=0 ; i<=DerPoly ; i++)
  {
    remplitmatrice(i,TlfSXk,TlfSXkY,LPoints);
    resoutMatrice(i);
    // affichage des polynômes :
    ModelPoly.header1 << QString("P%1").arg(i);     // horizontal header
    ModelPoly.header2 << QString("Polynome de degré %1").arg(i);     // horizontal header
    //SGPoly.cells[i+1,0]:='P'+IntTostr(i);
    // affichage des coefficients :
    QString formule="";
    QString degre="1";
    for (int j=0 ; j<= i ; j++)
    {
      ModelPoly.setData(j,i,QString("%1").arg(-TlfMat[j][i+1]));
      formule.append( QString("%1*%2").arg(degre).arg(-TlfMat[j][i+1]));
      if(j==0) degre=QString("+:%1").arg(varX+1);
      else degre.append(QString("*:%1").arg(varX+1));
    }
    ModelPoly.setData(DerPoly+1,i,formule);
    //  SGPoly.cells[i+1,j+1]:=floatToStr(-TlfMat[j,i+1]);
  }
  ui->TV_Poly->setModel(&ModelPoly);
  // dessin des Polynômes :
  // degré 1 :
  double realY = ModelPoly.data(0,1).toDouble()+minX*ModelPoly.data(1,1).toDouble();
  double realY2 = ModelPoly.data(0,1).toDouble()+maxX*ModelPoly.data(1,1).toDouble();
  grafY=(maxY-realY)*H/(maxY-minY);
  double grafY2=(maxY-realY2)*H/(maxY-minY);
  line = scene->addLine(0,grafY,W,grafY2,polyPen);
  line->setToolTip(QString("poly 1"));
  for (int grafX=1 ; grafX<W-1 ; grafX++)
  {
      //    grafX=(X-minX)*W/(maxX-minX);
    double realX=minX+(grafX*1.0)/W*(maxX-minX);
    double realY;
    for (int degre=2 ; degre<= DerPoly ; degre++)
    {
        realY=ModelPoly.data(0,degre).toDouble();
        double Xn=realX;
        for (int i=1 ; i<= degre ; i++)
        {
          realY += ModelPoly.data(i,degre).toDouble()*Xn;
          Xn *= realX;
        }
        grafY=(maxY-realY)*H/(maxY-minY);
        line = scene->addLine(grafX,grafY,grafX,grafY,polyPen);
        line->setToolTip(QString("poly %1").arg(degre));
    }

  }

}
