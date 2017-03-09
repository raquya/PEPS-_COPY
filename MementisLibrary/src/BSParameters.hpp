#pragma once

#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "RateModel.hpp"
#include <map>


class BSParameters
{
public:

	int size_; /// nombre d'actifs du mod�le
	RateModel * r_; /// taux d'int�r�t
	PnlMat* matCorr; /// param�tre de corr�lation
	PnlVect *sigma_; /// vecteur de volatilit�s
	PnlVect *spot_; /// valeurs initiales du sous-jacent
	PnlMat *CorrelationMat;
	int cholesky;
	PnlVect *mu_;
	PnlVect *Gn;
	int hedgingDateNb_;
	std::map<int,int> mapDevises;
	
	BSParameters(int size, RateModel* r, PnlMat* matCorr, PnlVect * sigma, PnlVect* spot, PnlVect* mu, int hedgingDateNumber, std::map<int,int> mapDevises);

	~BSParameters();
};
