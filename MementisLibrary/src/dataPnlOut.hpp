#pragma once

#define DLLEXP __declspec( dllexport )

class DLLEXP dataPnLOut
{
public:
	int nbHedgingDate;
	double* valPtfCouv;
	double* prixPdt;
	double pnl;
};
