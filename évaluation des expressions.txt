objet : transformer une expression arithmétique en notation infixée vers la notation polonaise inverse.

exemples de résultats désirés :

a+b   --> ab+
a+b+c --> ab+c+
a+b-c --> ab+c-
a-b+c --> ab-c+
a+b*c --> abc*+
a*b+c --> ab*c+
a+(b+c)*d --> abc+d*+  
(a+b)*(c+d) --> ab+cd+* 
(a+b)*c --> ab+c*
(a-b)*c+d --> ab-c*d+
(a-b)*c+(d-e)*f --> ab-c*de-f*+
 ( ; a ( ; a -( ; ab- ( ; ab-( ; ab-( * ; ab-(c*
 ab-(c* + ; ab-(c* (+ ; ab-(c*de-(f *+
 
 
(:3-5)*2+(:5-4)*3 


algorithme à valider :
on va utiliser une pile LIFO d'opérations en attente.
un nombre ou une variable --> en fin de liste, puis on dépile les opérations jusqu'à la première parenthèse.
une opération non prioritaire --> dans la pile
une opération prioritaire --> si dernière opération dépilée non prioritaire : on la rempile
                            --> puis : dans la pile
une parenthése ouvrante : --> dans la pile
une parenthèse fermante : --> on dépile la parenthèse (c'est un nop prioritaire qui protège d'un rempilage).
                            --> puis on dépile les opérations jusqu'à la première parenthèse.
une fonction :                             
                        
