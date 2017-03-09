#pragma once

#define DLLEXP __declspec( dllexport )

class DLLEXP dataPriceAndHedgeOut
{
public:
	int nbPdtCouv;
	double *delta; // Il y aura nbPdtCouv valeurs dans ce vecteur
	double *icDelta; // Il y aura nbPdtCouv valeurs dans ce vecteurs
	double prix;
	double icPrix;
};
