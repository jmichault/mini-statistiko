#ifndef EVAL_H
#define EVAL_H

typedef double  TF(int x);
typedef int  TF2(QString x);

double evalTok(TF getvari);
void evalExpr(QString Sexpr, TF2 getvars);


#endif // EVAL_H
