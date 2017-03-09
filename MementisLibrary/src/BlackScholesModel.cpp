#include "BlackScholesModel.hpp"
#include <iostream>

using namespace std;

BlackScholesModel::BlackScholesModel()
{

}

BlackScholesModel::BlackScholesModel(BSParameters* parameters, double maturity, int nbTimeSteps)
{
	this->parameters = parameters;
	this->maturity_ = maturity;
	this->nbTimeSteps_ = nbTimeSteps;
}

BlackScholesModel::~BlackScholesModel()
{
};

// Asset à t = 0
void BlackScholesModel::asset(PnlMat *path, PnlRng* rng_) {

	double LG;
	PnlVect Ldt;
	double exprExp;
	double volatilite;
	double pasTemps = maturity_ / nbTimeSteps_;
	double r = parameters->r_->getRate(0);

	pnl_mat_set_row(path, parameters->spot_, 0);

	for (int n = 0; n < nbTimeSteps_; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = GET(parameters->sigma_, d);
			exprExp = (r - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp);
		}
	}
}

// Asset avec un past à partir de t (t doit etre entre 0 et T)
void BlackScholesModel::asset(PnlMat *path, double t, const PnlMat *past, PnlRng* rng_) {

	int debutSimulation = past->m - 1;
	int estDateConstatation = t - floor(t);
	if (estDateConstatation != 0) {
		debutSimulation--;
	}

	pnl_mat_set_subblock(path, past, 0, 0);
	double u;
	double sTilde;
	double sSimul;
	double st;
	double r = parameters->r_->getRate(0);

	double LG;
	PnlVect Ldt;

	bool prem = true;
	double volatilite;
	double t1,t2;

	for (int n = debutSimulation; n < nbTimeSteps_; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		t1 = (prem)?t:(maturity_ /(double)nbTimeSteps_)*n;
		t2 = (maturity_ /(double)nbTimeSteps_)*(n+1);
		u = t2 - t1;
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			st = MGET(past, (past->m - 1), d);
			volatilite = GET(parameters->sigma_, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			sTilde = exp((r - (volatilite * volatilite) / 2.0)*u + volatilite * sqrt(u) * LG);
			if (prem) {
				sSimul = st * sTilde;
				prem = false;
			} else {
				sSimul = MGET(path, n, d) * sTilde;
			}
			MLET(path, n+1, d) = sSimul;
		}
	}

}

// Simulation d'un marché à t = 0
// pnl == true -> nbHedgingDate
// pnl == false -> nbTimeStep
void BlackScholesModel::simulMarket(PnlMat *path, PnlRng* rng_, bool pnl) {

	double LG;
	PnlVect Ldt;
	double exprExp;
	double volatilite;

	double ndDates;
	if (pnl) {
		ndDates = parameters->hedgingDateNb_;
	} else {
		ndDates = nbTimeSteps_;
	}
	double pasTemps = maturity_ / ndDates;

	pnl_mat_set_row(path, parameters->spot_, 0);

	for (int n = 0; n < ndDates; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = GET(parameters->sigma_, d);
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			MLET(path, n + 1, d) = MGET(path, n, d) * exp(exprExp);
		}
	}
}

// Simulation d'un marché avec un past à partir de t entre 0 et T
// pnl == true -> nbHedgingDate
// pnl == false -> nbTimeStep
void BlackScholesModel::simulMarket(PnlMat *path, double t, const PnlMat *past, PnlRng* rng_, bool pnl) {

	double ndDates;
	if (pnl) {
		ndDates = parameters->hedgingDateNb_;
	} else {
		ndDates = nbTimeSteps_;
	}

	double pasTemps = maturity_ / ndDates;
	int debutSimulation = past->m - 1;
	int estDateConstatation = t - pasTemps*(debutSimulation);
	if (estDateConstatation != 0) {
		debutSimulation--;
	}

	pnl_mat_set_subblock(path, past, 0, 0);
	double u;
	double sTilde;
	double sSimul;
	double st;

	double LG;
	PnlVect Ldt;

	bool prem = true;
	double volatilite;
	double t1, t2;

	for (int n = debutSimulation; n < ndDates; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		t1 = (prem) ? t : (maturity_ / (double)nbTimeSteps_)*n;
		t2 = (maturity_ / (double)nbTimeSteps_)*(n + 1);
		u = t2 - t1;
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			st = MGET(past, (past->m - 1), d);
			volatilite = GET(parameters->sigma_, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			
			sTilde = exp((GET(parameters->mu_, d) - volatilite * volatilite / 2.0)*u + volatilite * sqrt(u) * LG);
			if (prem) {
				sSimul = st * sTilde;
				prem = false;
			}
			else {
				sSimul = MGET(path, n, d) * sTilde;
			}
			MLET(path, n + 1, d) = sSimul;
		}
	}

}

// Simulation d'un marché jusqu'à t entre 0 et T
void BlackScholesModel::simulMarket(PnlMat *path, double t, PnlRng* rng_) {

	double LG;
	PnlVect Ldt;
	double exprExp;
	double volatilite;

	pnl_mat_set_row(path, parameters->spot_, 0);

	int cutDate = floor(t) + 1;
	double reste = 1 - (cutDate - t);
	double pasTemps = maturity_ / (double)nbTimeSteps_;

	for (int n = 0; n < cutDate; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = GET(parameters->sigma_, d);
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			MLET(path, n + 1, d) = MGET(path, n, d) * exp(exprExp);
		}
	}

	if (reste != 0) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = GET(parameters->sigma_, d);
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * reste*pasTemps + volatilite * sqrt(reste*pasTemps) * LG;
			MLET(path, cutDate, d) = MGET(path, cutDate - 1, d) * exp(exprExp);
		}
	}

}


void BlackScholesModel::shiftAsset(PnlMat *shift_path, const PnlMat *path, int d, double h, double t, double timestep) {
    int compteur = 0;
    double pas = 0;

    //Initialisation de la matrice shift_path avec les valeurs de path
    pnl_mat_set_subblock(shift_path,path,0,0);
    //Recuperation de la ligne de la matrice path qui correspond a la date t
    while (pas <= t) {
        compteur++;
        pas += timestep;
    }

    double valeurShift = 0;

    //On shift sur le sous-jacent d
    for (int i = compteur; i < path->m ; i++) {
        valeurShift = pnl_mat_get(path,i,d) * (1 + h);
        pnl_mat_set(shift_path,i,d,valeurShift);
    }
}
