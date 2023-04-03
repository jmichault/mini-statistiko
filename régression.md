Régression linéaire simple :
 on utilise la méthode des moindres carrés «ordinaire» (i.e. sur l'ordonnée= en vertical)
 droite cherchée : f(x) = A0 * V0 + A1 
 somme des carrés = somme( (Y-f(x))carré )
                  = somme ( (Y - A0*V0 -A1 )*(Y - A0*V0 -A1 )
                  = somme (Y*Y) + A0*A0*somme(V0*V0) +A1*A1*somme(1)
                      - 2*A0*somme(Y*V0)
                      + 2*A1*A0*somme(V0)
                      - 2*A1*somme(Y)
 le moindre carré est obtenu quand toutes les dérivées s'annulent :
	(1)	d S / d A0 = 2*A0*somme(V0*V0) - 2*somme(Y*V0) +2*A1*somme(V0) = 0
	    -->	A0*somme(V0*V0) - somme(Y*V0) + A1*somme(V0) = 0
	    -->	A0*somme(V0*V0) + A1*somme(V0) - somme(Y*V0) = 0
	(2)	d S / d A1 = 2*A1*somme(1) + 2*A0*somme(V0) - 2*somme(Y) = 0
	    -->	A1*somme(1) + A0*somme(V0) - somme(Y) = 0
	    -->	A0*somme(V0) + A1*somme(1) - somme(Y) = 0
	    --> A1 = [somme(Y) - A0*somme(V0)]/somme(1)
	(1) --> A0*somme(V0*V0) -somme(Y*V0) + [somme(Y) - A0*somme(V0)]/somme(1)*somme(V0) = 0
	    --> A0*somme(V0*V0)*somme(1) -somme(Y*V0)*somme(1) +somme(Y)*somme(V0) - A0*somme(V0)*somme(V0)=0
	    --> A0 = [somme(Y*V0)*somme(1)-somme(Y)*somme(V0)]/[somme(V0*V0)*somme(1)-somme(V0)*somme(V0)]
	    --> A0 = covariance(V0,Y)/variance(X)

variance(V0) =  espérance(V0*V0) - espérance(V0)*espérance(V0)
		~ somme(V0*V0)/somme(1) - somme(V0)*somme(V0)/[somme(1)*somme(1)]
covariance(V0,Y) =  espérance [ (V0-espérance(V0)) * (Y-espérance(Y)) ]
		~ somme [ (V0 - somme(V0)/somme(1) ) * ( Y - somme(Y)/somme(1)) ]
		~ somme(V0*Y)/somme(1) + somme(V0)*somme(Y)/[somme(1)*somme(1)] -2*somme(V0)*somme(Y)/[somme(1)*somme(1)] 
		~ somme(V0*Y)/somme(1) - somme(V0)*somme(Y)/[somme(1)*somme(1)] 


Régression linéaire multiple :
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
//		--> 	  Ai*somme(Vi*Vi)
			+ Aj*somme(Vj*Vi)...(pour tout j de 0 à n-1 avec j!=i)
			+ An*somme(Vi)
			- somme(Y*Vi)
				= 0
	    -->	A0*somme(V0*V0) + A1*somme(V0) - somme(Y*V0) = 0
//      pour i=n : d S / d An = 2*An*somme(1)
				+ 2*Ai*somme(Vi)...(pour i de 0 à n-1)
				- 2*somme(Y)
//		-->	Ai*somme(Vi) + An*somme(1) - somme(Y) = 0
//
