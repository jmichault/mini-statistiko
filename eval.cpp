#include <QString>
#include <QMessageBox>
#include "eval.h"


enum tok : int{op_plus,op_moins,op_div,op_mult,par_ouv,par_ferm
          ,tok_var,fct,tok_val,tok_nop} ;
struct TTok {
 tok typeTok;
 int indice;  // si tok == tok_var
 double valeur; // si tok == op_plus,...
};

struct TTok *pttok;
//typedef double  TF(int x);
//typedef int  TF2(QString x);

//double evalTok(TF getvari);
//void evalExpr(QString Sexpr, TF2 getvars);

double laPile[100];
struct TTok lesToken[100];
int NbToken;


// fonction qui calcule la valeur numérique de l'expression qui a été précédemment analysée par EvalExpr
// la fonction getvari renvoie la valeur numérique de la variable
double evalTok(TF getvari)
{
 int posTok,posPile;
  posTok=0;
  posPile=-1;
  while (posTok<NbToken)
  {
    switch (lesToken[posTok].typeTok)
    {
     case tok_nop:// rien à faire
      break;
     case tok_var:
      posPile++;
      laPile[posPile]=getvari(lesToken[posTok].indice);
      break;
     case tok_val:
      posPile++;
      laPile[posPile]=lesToken[posTok].valeur;
      break;
     case op_plus:
      laPile[posPile-1]=laPile[posPile-1]+laPile[posPile];
      posPile--;
      break;
     case op_moins:
      laPile[posPile-1]=laPile[posPile-1]-laPile[posPile];
      posPile--;
      break;
     case op_div:
      laPile[posPile-1]=laPile[posPile-1]/laPile[posPile];
      posPile--;
      break;
     case op_mult:
      laPile[posPile-1]=laPile[posPile-1]*laPile[posPile];
      posPile--;
      break;
     case fct:
      if (lesToken[posTok].indice==1)
        laPile[posPile]=log(laPile[posPile]);
      else
        laPile[posPile]=exp(laPile[posPile]);
      break;
     default:
      break;
    }
    posTok++;
  }
 return laPile[0];
}

// fonction récursive qui transforme l'expression Sexpr en une pile de tokens
// la fonction getvars renvoie le numéro de la variable
// attention, cette fonction ne marche que pour des expressions simples.
void EvalExpr2(QString sExpr,TF2 getvars, bool init=true)
{
 double tmpLf;
 int posErr,posPf,nbPar;
 QString sRes,sExpr2;
 QChar car1,carTest;
 int posVal;
 TTok tokMem[4];
 int nbMem;
 int i;
  if (init)
    NbToken=0;
  //DecimalSeparator='.';// positionner le séparateur pour fonction val
  posVal=NbToken;
  // saut des blancs
  while (sExpr.length()>0)
  {
    posErr=0;
    while (true)
    {
      if (posErr >sExpr.length()) break;
      if (sExpr[posErr]==' ') posErr++;
      else break;
    }
    if(posErr) {sExpr=sExpr.mid(posErr);posErr=0;}
    car1=sExpr[posErr];
    switch (car1.toLatin1())
    {
     case '(':  // gestion des parenthèses :
      posPf=2;
      nbPar=1;
      while (true) // recherche parenthèse fermante
      {
        if (posPf >sExpr.length()) break;//erreur à gérer todo
        carTest=sExpr[posPf];
        if (carTest=='(') nbPar++;
        else if (carTest==')') nbPar--;
        if (nbPar <1) break;
        posPf++;
      }
      sExpr2=sExpr.mid(1,posPf-2);// copie de l'expression entre parenthèses
      sExpr=sExpr.mid(posPf+1,sExpr.length());// suppression dans la chaîne d'origine
      posErr=1;
      // mémoriser éléments après celui en cours
      nbMem=NbToken-posVal;
      for (i=1 ; i<=nbMem ; i++)
        tokMem[i]=lesToken[posVal+i];
      NbToken=posVal;
      // traiter l'expression entre parenthèses
      EvalExpr2(sExpr2,getvars,false);
      lesToken[NbToken].typeTok=tok_nop;// protéger l'expression
      NbToken++;
      // rétablir les éléments mémorisés
      for (i=1 ; i<= nbMem ; i++)
        lesToken[NbToken+i-1]=tokMem[i];
      NbToken += nbMem;
      break;
     case ':':  // c'est une variable donnée par son indice
      posErr++;
      while (true)
      {
        if (posErr >=sExpr.length()) break;
        carTest=sExpr[posErr];
        if (carTest.isDigit() || carTest=='.' || carTest ==',')
          posErr++;
        else break;
      }
      lesToken[posVal].typeTok=tok_var;
      sRes=sExpr.mid(1,posErr-1);
      lesToken[posVal].indice=sRes.toInt();
      NbToken++;
      break;
     case '0' ... '9': case '.': case ',': case '+': case '-':// c'est un nombre
      // récupération du premier chiffre
      // on garde le + ou le - du signe
      if ((sExpr[posErr]=='-') || (sExpr[posErr]=='+')) posErr++;
      while (true)
      {
        if (posErr >=sExpr.length()) break;
        carTest=sExpr[posErr];
        if(carTest.isDigit() || carTest=='.' || carTest==',') posErr++;
        else if( carTest=='E') posErr += 2;
        else break;
      }
      sRes=sExpr.mid(0,posErr);
      tmpLf=sRes.toDouble();
      lesToken[posVal].typeTok=tok_val;
      lesToken[posVal].valeur=tmpLf;
      NbToken++;
      break;
     default: // c'est un nom de variable (ou de fonction)
      while (true)
      {
        if (posErr >= sExpr.length()) break;
        carTest=sExpr[posErr];
        if (carTest.isLetterOrNumber() ) posErr++;
        else break;
      }
      sRes=sExpr.mid(0,posErr);
      if ( (sRes=="exp") || (sRes=="Ln"))
      { //fonction exponentielle de x ou Log népérien
        for ( i=NbToken+1 ; i>=posVal+2 ; i--)
        {
          lesToken[i]=lesToken[i-1];
        }
        lesToken[posVal+1].typeTok=fct;
        if ( sRes=="exp" )
          lesToken[posVal+1].indice=0;
        else
          lesToken[posVal+1].indice=1;
        NbToken++;
        sExpr=sExpr.mid(posErr,sExpr.length());
        continue;
      }
      else
      {
        // retrouver cette variable
        lesToken[posVal].indice=getvars(sRes);
        // gérer l'erreur si on ne trouve pas la variable;
        if ( lesToken[posVal].valeur==-1)
        {
          QMessageBox msgBox;
          msgBox.setText("variable "+sRes+" inconnue");
          msgBox.exec();
          exit(1);
        }
        lesToken[posVal].typeTok=tok_var;
      }
      NbToken++;
      break;
    } // fin switch
    // sauter les espaces
    while (posErr < sExpr.length())
    {
      if (sExpr[posErr] == ' ') posErr++;
      else break;
    }
    if (posErr < sExpr.length())
    {
      switch( sExpr[posErr].toLatin1())
      {
       case '+':
        lesToken[NbToken+1].typeTok=op_plus;
        posVal=NbToken;
        NbToken++;
        break;
       case '-':
        lesToken[NbToken+1].typeTok=op_moins;
        posVal=NbToken;
        NbToken++;
        break;
       case '*':
        if ( (lesToken[NbToken-1].typeTok==op_plus) || (lesToken[NbToken-1].typeTok==op_moins) )
        {
          lesToken[NbToken+1].typeTok=lesToken[NbToken-1].typeTok;
          lesToken[NbToken].typeTok=op_mult;
          posVal=NbToken-1;
        }
        else
        {
          lesToken[NbToken+1].typeTok=op_mult;
          posVal=NbToken;
        }
        NbToken++;
        break;
       case '/':
        if ( (lesToken[NbToken-1].typeTok=op_plus) || (lesToken[NbToken-1].typeTok=op_moins) )
        {
          lesToken[NbToken+1].typeTok=lesToken[NbToken-1].typeTok;
          lesToken[NbToken].typeTok=op_div;
          posVal=NbToken-1;
        }
        else
        {
          lesToken[NbToken+1].typeTok=op_div;
          posVal=NbToken;
        }
        NbToken++;
        break;
      }
    }
    sExpr=sExpr.mid(posErr+1,sExpr.length());
  }
}

void evalExpr(QString sExpr, TF2 getvars)
{
  EvalExpr2(sExpr,getvars);
}

