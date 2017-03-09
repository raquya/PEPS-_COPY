#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "RateModel.hpp"

class Produit
{
public:
		double T_; /// maturité
		int nbTimeSteps_; /// nombre de pas de temps de discrétisation
		RateModel* interestRate_;

		virtual double payoff(const PnlMat *path) = 0;
};
