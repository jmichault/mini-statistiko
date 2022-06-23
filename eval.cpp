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

static struct TTok lesToken[100];
static int NbToken;

// fonction qui calcule la valeur numérique de l'expression qui a été précédemment analysée par EvalExpr
// la fonction getvari renvoie la valeur numérique de la variable
double evalTok(TF getvari)
{
  QVarLengthArray<double, 20> laPile(0);
  for(int posTok=0 ; posTok<NbToken ; posTok++)
  {
    double x1;
    switch (lesToken[posTok].typeTok)
    {
     case tok_nop:// rien à faire
      break;
     case tok_var:
      laPile.append(getvari(lesToken[posTok].indice));
      break;
     case tok_val:
      laPile.append(lesToken[posTok].valeur);
      break;
     case op_plus:
      x1=laPile.last();
      laPile.resize(laPile.count()-1);
      laPile.last() += x1;
      break;
     case op_moins:
      x1=laPile.last();
      laPile.resize(laPile.count()-1);
      laPile.last() -= x1;
      break;
     case op_div:
      x1=laPile.last();
      laPile.resize(laPile.count()-1);
      laPile.last() /= x1;
      break;
     case op_mult:
      x1=laPile.last();
      laPile.resize(laPile.count()-1);
      laPile.last() *= x1;
      break;
     case fct:
      if (lesToken[posTok].indice==1)
        laPile.last()=log(laPile.last());
      else
        laPile.last()=exp(laPile.last());
      break;
     default:
      break;
    }
  }
 return laPile.last();
}

void printResEval()
{
  QString formule="";
  for (int posTok =0 ; posTok<NbToken ; posTok ++)
  {
    switch (lesToken[posTok].typeTok)
    {
     case tok_nop:// rien à faire
        formule.append(QString("nop "));
      break;
     case tok_var:
      formule.append(QString(":%1 ").arg(lesToken[posTok].indice));
      break;
     case tok_val:
        formule.append(QString("%1 ").arg(lesToken[posTok].valeur));
      break;
     case op_plus:
      formule.append(QString("+ "));
      break;
     case op_moins:
        formule.append(QString("- "));
      break;
     case op_div:
        formule.append(QString("/ "));
      break;
     case op_mult:
        formule.append(QString("* "));
      break;
     case fct:
      if (lesToken[posTok].indice==1)
          formule.append(QString("log "));
      else
        formule.append(QString("exp "));
      break;
     default:
        formule.append(QString("? "));
      break;
    }
  }
  // pour tests :
  QMessageBox msgBox;
  msgBox.setText("formule :  "+formule);
  msgBox.exec();
}



// fonction récursive qui transforme l'expression Sexpr en une suite de tokens de type «notation polonaise»
// la fonction getvars renvoie le numéro de la variable
// attention, cette fonction ne marche que pour des expressions simples.
// elle est notamment boguée dans la gestion des parenthèses et dans la gestion de la priorité des opérateurs…
void EvalExpr2(QString sExpr,TF2 getvars)
{
 double tmpLf;
 int posErr,posPf,nbPar;
 QString sRes,sExpr2;
 QChar car1,carTest;
 int posVal;
 TTok tokMem[4];
 int nbMem;
 int i;
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
      sExpr2=sExpr.mid(1,posPf-1);// copie de l'expression entre parenthèses
      sExpr=sExpr.mid(posPf+1,sExpr.length());// suppression dans la chaîne d'origine
      posErr=0;
      // mémoriser éléments après celui en cours
      nbMem=NbToken-posVal;
      for (i=0 ; i<nbMem ; i++)
        tokMem[i]=lesToken[posVal+i];
      NbToken=posVal;
      // traiter l'expression entre parenthèses
      EvalExpr2(sExpr2,getvars);
      lesToken[NbToken].typeTok=tok_nop;// protéger l'expression
      NbToken++;
      // rétablir les éléments mémorisés
      for (i=0 ; i< nbMem ; i++)
        lesToken[NbToken+i]=tokMem[i];
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
        if (carTest=='_' || carTest.isLetterOrNumber() ) posErr++;
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
        if ( lesToken[posVal].indice==-1)
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
        if ( (lesToken[NbToken-1].typeTok==op_plus) || (lesToken[NbToken-1].typeTok==op_moins) ) //
        {// gestion de la priorité de * sur +
          // FIXME
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
        if ( (lesToken[NbToken-1].typeTok==op_plus) || (lesToken[NbToken-1].typeTok==op_moins) ) //
        {
          // FIXME
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
  for(int i=0 ; i<100 ; i++)
  {
      lesToken[i].indice=0;
      lesToken[i].typeTok=op_plus;
      lesToken[i].valeur=0.0;
  }
  NbToken=0;
  EvalExpr2(sExpr,getvars);
  //printResEval();
}

