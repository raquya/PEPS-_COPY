#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "BSParameters.hpp"

/// \brief Modele de Black Scholes
class BlackScholesModel
{
public:
	BSParameters *parameters;
	double maturity_;
	int nbTimeSteps_;

	/*!
	* \brief Constructeur par defaut
	*/
	BlackScholesModel();

	/*!
	* \brief Constructeur
	*/
	BlackScholesModel(BSParameters* parameters, double maturity, int nbTimeSteps);


	/*!
	* \brief Destructeur
	*/
	virtual ~BlackScholesModel();

	// Simulation d'un marché à t = 0
	// pnl == true -> nbHedgingDate
	// pnl == false -> nbTimeStep
	void simulMarket(PnlMat *path, PnlRng* rng_, bool pnl);

	// Simulation d'un marché avec un past à partir de t entre 0 et T
	// pnl == true -> nbHedgingDate
	// pnl == false -> nbTimeStep
	void simulMarket(PnlMat *path, double t, const PnlMat *past, PnlRng* rng_, bool pnl);

	// Simulation d'un marché jusqu'à t entre 0 et T
	void simulMarket(PnlMat *path, double t, PnlRng* rng_);

	// Asset à t = 0
	void asset(PnlMat *path, PnlRng* rng_);

	// Asset avec un past à partir de t entre 0 et T
	void asset(PnlMat *path, double t, const PnlMat *past, PnlRng* rng_);

	void shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep);
};
