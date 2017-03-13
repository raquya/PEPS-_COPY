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

	pnl_mat_set_row(path, parameters->spot_, 0);
	int sizeStocks = parameters->mapDevises.size();
	int devise;
	double r;
	
	for (int n = 0; n < nbTimeSteps_; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			r = parameters->r_->getRate(3);
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = 0;
			// on regarde si action ou devise
			if (d < sizeStocks) {
				//On regarde la vol étrangère
				devise = parameters->mapDevises[d]; 
				if(devise != 0) {
					volatilite = GET(parameters->sigma_, (sizeStocks - 1 + devise));
				}
				volatilite += GET(parameters->sigma_, d) ;
			}else{
				volatilite = GET(parameters->sigma_, d);
				r -= parameters->r_->getRate(d - sizeStocks); 
			}
			exprExp = (r - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			if (d < sizeStocks) {
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp);
			}else{
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp) * exp(parameters->r_->integrateRate(0, pasTemps*(n+1), devise));
			}
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
	double r;
	int sizeStocks = parameters->mapDevises.size();
	int devise;

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
			r = parameters->r_->getRate(3);
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			st = MGET(past, (past->m - 1), d);
			volatilite = 0;
			// Si je suis un sous-jacent
			if (d< sizeStocks) {
				//Recupère le numero de la devise du sous jacent
				devise = parameters->mapDevises[d];
				if (devise != 0){ // Si je suis en monnaie étrangère
					//Je récupère la vol correspondante à la devise étrangère
					volatilite = GET(parameters->sigma_, (sizeStocks -1 +devise));
				}
				// ma volatilité est égale à la vol de l'actif  + la vol de la devise correspondante
				volatilite += GET(parameters->sigma_, d);
			}else{
				volatilite += GET(parameters->sigma_, d);
				// Si je suis une devise, je récupère le taux rf et je le soustrais au taux domestique
				r-= parameters->r_->getRate(d-sizeStocks);
			}
			sTilde = exp((r - (volatilite * volatilite) / 2.0)*u + volatilite * sqrt(u) * LG);
			if (prem) {
				sSimul = st * sTilde;
				prem = false;
			} else {
				sSimul = MGET(path, n, d) * sTilde;
			}
			if (d < sizeStocks) {
				MLET(path, n+1, d) = sSimul ;
			}else{
				MLET(path, n+1, d) =sSimul * exp(parameters->r_->integrateRate(0, t + u + u*(n-debutSimulation), devise));
			}
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
	int sizeStocks = parameters->mapDevises.size();

	int devise;
	double r;
	
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
			r = parameters->r_->getRate(3);
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = 0;
			// on regarde si action ou devise
			if (d < sizeStocks) {
				//On regarde la vol étrangère
				devise = parameters->mapDevises[d]; 
				if(devise != 0) {
					volatilite = GET(parameters->sigma_, (sizeStocks - 1 + devise));
				}
				volatilite += GET(parameters->sigma_, d) ;
			}else{
				volatilite = GET(parameters->sigma_, d);
				r -= parameters->r_->getRate(d - sizeStocks); 
			}
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			if (d < sizeStocks) {
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp);
			}else{
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp) * exp(parameters->r_->integrateRate(0, pasTemps*(n+1), devise));
			}
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


	double LG;
	PnlVect Ldt;
	double volatilite;
	int sizeStocks = parameters->mapDevises.size();
	int devise;
	double r;
	pnl_mat_set_subblock(path, past, 0, 0);
	double u;
	double sTilde;
	double sSimul;
	double st;
	double pasTemps = maturity_ / ndDates;
	bool prem = true;
	double t1, t2;

	int debutSimulation = past->m - 1;
	int estDateConstatation = t - pasTemps*(debutSimulation);
	if (estDateConstatation != 0) {
		debutSimulation--;
	}

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
			volatilite = 0;
			// on regarde si action ou devise
			if (d < sizeStocks) {
				//On regarde la vol étrangère
				devise = parameters->mapDevises[d]; 
				if(devise != 0) {
					volatilite = GET(parameters->sigma_, (sizeStocks - 1 + devise));
				}
				volatilite += GET(parameters->sigma_, d) ;
			}else{
				volatilite = GET(parameters->sigma_, d);
				r -= parameters->r_->getRate(d - sizeStocks); 
			}
			sTilde = exp((GET(parameters->mu_, d) - volatilite * volatilite / 2.0)*u + volatilite * sqrt(u) * LG);
			if (prem) {
				sSimul = st * sTilde;
				prem = false;
			}
			else {
				sSimul = MGET(path, n, d) * sTilde;
			}
			if (d < sizeStocks) {
				MLET(path, n+1, d) = sSimul;
			}else{
				MLET(path, n+1, d) = sSimul * exp(parameters->r_->integrateRate(0, t + u + u*(n-debutSimulation), devise));
			}
		}
	}

}

// Simulation d'un marché jusqu'à t entre 0 et T
void BlackScholesModel::simulMarket(PnlMat *path, double t, PnlRng* rng_) {

	double LG;
	PnlVect Ldt;
	double exprExp;
	double volatilite;
	int sizeStocks = parameters->mapDevises.size();
	int devise;
	pnl_mat_set_row(path, parameters->spot_, 0);

	int cutDate = floor(t) + 1;
	double reste = 1 - (cutDate - t);
	double pasTemps = maturity_ / (double)nbTimeSteps_;

	for (int n = 0; n < cutDate; n++) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = 0;
			// on regarde si action ou devise
			if (d < sizeStocks) {
				//On regarde la vol étrangère
				devise = parameters->mapDevises[d]; 
				if(devise != 0) {
					volatilite = GET(parameters->sigma_, (sizeStocks - 1 + devise));
				}
				volatilite += GET(parameters->sigma_, d) ;
			}else{
				volatilite = GET(parameters->sigma_, d);
			}
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * pasTemps + volatilite * sqrt(pasTemps) * LG;
			if (d < sizeStocks) {
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp);
			}else{
				MLET(path, n+1, d) = MGET(path, n, d) * exp(exprExp) * exp(parameters->r_->integrateRate(0, pasTemps*(n+1), devise));
			}
		}
	}


	if (reste != 0) {
		pnl_vect_rng_normal(parameters->Gn, parameters->size_, rng_);
		for (int d = 0; d < parameters->size_; d++) {
			Ldt = pnl_vect_wrap_mat_row(parameters->CorrelationMat, d);
			LG = pnl_vect_scalar_prod(&Ldt, parameters->Gn);
			volatilite = 0;
			// on regarde si action ou devise
			if (d < sizeStocks) {
				//On regarde la vol étrangère
				devise = parameters->mapDevises[d]; 
				if(devise != 0) {
					volatilite = GET(parameters->sigma_, (sizeStocks - 1 + devise));
				}
				volatilite += GET(parameters->sigma_, d) ;
			}else{
				volatilite = GET(parameters->sigma_, d);
			}
			exprExp = (GET(parameters->mu_, d) - (volatilite * volatilite / 2.0)) * reste*pasTemps + volatilite * sqrt(reste*pasTemps) * LG;
			if (d < sizeStocks) {
				MLET(path, cutDate, d) = MGET(path, cutDate - 1, d) * exp(exprExp);
			}else{
				MLET(path,cutDate , d) = MGET(path, cutDate -1 , d) * exp(exprExp) * exp(parameters->r_->integrateRate(0, t, devise));
			}
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
