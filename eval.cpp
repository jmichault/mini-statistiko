#include <QString>
#include <QMessageBox>
#include "eval.h"

// fonction qui calcule la valeur numérique de l'expression qui a été précédemment analysée par EvalExpr
// la fonction getvari renvoie la valeur numérique de la variable d'indice i
double evalTok(TF getvari,QList<struct TTok> *lesToken)
{
  QVarLengthArray<double, 20> laPile(0);
  for(int posTok=0 ; posTok<lesToken->length() ; posTok++)
  {
    double x1;
    switch ((*lesToken)[posTok].typeTok)
    {
     case tok_nop:// rien à faire
      break;
     case tok_var:
      laPile.append(getvari((*lesToken)[posTok].indice));
      break;
     case tok_val:
      laPile.append((*lesToken)[posTok].valeur);
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
     case tok_exp:
      laPile.last()=exp(laPile.last());
      break;
     case tok_log:
      laPile.last()=log(laPile.last());
     default:
      break;
    }
  }
 return laPile.last();
}

//*
// pour tests :
void printResEval(QList<struct TTok> *lesToken)
{
  QString formule="";
  for(int posTok=0 ; posTok<lesToken->length() ; posTok++)
  {
      switch ((*lesToken)[posTok].typeTok)
    {
     case tok_nop:// rien à faire
        formule.append(QString("nop "));
      break;
     case tok_var:
      formule.append(QString(":%1 ").arg((*lesToken)[posTok].indice));
      break;
     case tok_val:
        formule.append(QString("%1 ").arg((*lesToken)[posTok].valeur));
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
     case tok_log:
        formule.append(QString("log "));
      break;
     case tok_exp:
        formule.append(QString("exp "));
      break;
     default:
        formule.append(QString("? "));
      break;
    }
  }
  QMessageBox msgBox;
  msgBox.setText("formule :  "+formule);
  msgBox.exec();
}
//*/

void afficheMessage(const QString &msg)
{
    QMessageBox msgBox;
    msgBox.setText(msg);
    msgBox.exec();
}

// fonction qui transforme l'expression Sexpr en une suite de tokens de type «notation polonaise inverse»
// Attention, cette fonction a été écrite à la va-vite et ne marche que pour des expressions simples.
// Elle est probablement boguée dans la gestion des parenthèses et dans la gestion de la priorité des opérateurs…
// Les erreurs de syntaxe ne sont pas toutes détectées…
// la fonction getvars renvoie le numéro de la variable
void evalExpr(QString sExpr,TF2 getvars, QList<struct TTok> *lesToken)
{
 double tmpLf;
 int posErr; // position du caractère en cours d'analyse
 QString sRes,sExpr2;
 QChar car1,carTest;
 QList<tok> attente;
 lesToken->clear();
  posErr=0;
  while (sExpr.length()>0)
  {
    // saut des blancs
    while (true)
    {
      if (posErr >=sExpr.length()) break;
      if (sExpr[posErr]==' ') posErr++;
      else break;
    }
    if(posErr) {sExpr=sExpr.mid(posErr);posErr=0;}
    car1=sExpr[posErr];
    struct TTok x;
    // on s'attend à trouver une variable, un nombre ou une parenthèse ouvrante.
    switch (car1.toLatin1())
    {
     case '(':  // gestion des parenthèses :
      attente.append(tok_nop);
      posErr++;
      continue;
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
      x.typeTok=tok_var;
      sRes=sExpr.mid(1,posErr-1);
      x.indice=sRes.toInt();
      lesToken->append(x);
      while(attente.count() && attente.last()!= tok_nop)
      {
        x.typeTok=  attente.last();
        lesToken->append(x);
        attente.resize(attente.count()-1);
      }
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
      x.typeTok=tok_val;
      x.valeur=tmpLf;
      lesToken->append(x);
      while(attente.count() && attente.last()!= tok_nop)
      {
        x.typeTok=  attente.last();
        lesToken->append(x);
        attente.resize(attente.count()-1);
      }
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
      if ( (sRes=="exp"))
      {
          attente.append(tok_exp);
          sExpr=sExpr.mid(posErr,sExpr.length());
          posErr=0;
          continue;
      }
      else if(sRes=="Ln")
      {
          attente.append(tok_log);
          sExpr=sExpr.mid(posErr,sExpr.length());
          posErr=0;
          continue;
      }
      else
      {
        // retrouver cette variable
        x.typeTok=tok_var;
        x.indice=getvars(sRes);
        lesToken->append(x);
        // gérer l'erreur si on ne trouve pas la variable;
        if ( x.indice==-1)  afficheMessage("erreur : variable "+sRes+" inconnue à : "+sRes);
        while(attente.count() && attente.last()!= tok_nop)
        {
          x.typeTok=  attente.last();
          lesToken->append(x);
          attente.resize(attente.count()-1);
        }
      }
      break;
    } // fin switch
    // maintenant on s'attend à trouver un opérateur ou une parenthèse fermante.
    traite_oper:
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
       case ')':  // gestion des parenthèses :
        if(attente.count()<=0 || attente.last()!= tok_nop)
            afficheMessage("erreur parenthèse à :  "+sExpr);
        x.typeTok=  attente.last();
        lesToken->append(x);
        attente.resize(attente.count()-1);
        while(attente.count() && attente.last()!= tok_nop)
        {
          x.typeTok=  attente.last();
          lesToken->append(x);
          attente.resize(attente.count()-1);
        }
        posErr++;
        goto traite_oper;
        break;
       case '+':
        attente.append(op_plus);
        break;
       case '-':
        attente.append(op_moins);
        break;
       case '*':
        if ( (*lesToken).last().typeTok==op_plus || (*lesToken).last().typeTok==op_moins) //
        {// gestion de la priorité de * sur +-
          attente.append((*lesToken).last().typeTok);
          (*lesToken).resize((*lesToken).count()-1);
        }
        attente.append(op_mult);
        break;
       case '/':
          if ( (*lesToken).last().typeTok==op_plus || (*lesToken).last().typeTok==op_moins) //
          {// gestion de la priorité de / sur +-
            attente.append((*lesToken).last().typeTok);
            (*lesToken).resize((*lesToken).count()-1);
          }
          attente.append(op_div);
        break;
        default:
          afficheMessage("erreur à :  "+sExpr);
          break;
      }
    }
    sExpr=sExpr.mid(posErr+1,sExpr.length());
    posErr=0;
  }
  //printResEval(lesToken);
}
