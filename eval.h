#ifndef EVAL_H
#define EVAL_H
#include <QString>

typedef double  TF(int x);
typedef int  TF2(QString x);
enum tok : int{op_plus,op_moins,op_div,op_mult,par_ouv,par_ferm
          ,tok_var,tok_val,tok_nop,tok_log,tok_exp} ;
struct TTok {
 tok typeTok;
 int indice;  // si tok == tok_var ou tok == fct
 double valeur; // si tok == op_plus,...
};

double evalTok(TF getvari,QList<struct TTok> *lesToken);
void evalExpr(QString Sexpr, TF2 getvars,QList<struct TTok> *lesToken);


#endif // EVAL_H
