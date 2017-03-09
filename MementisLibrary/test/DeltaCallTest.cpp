#include <iostream>
#include <time.h>
#include "pnl/pnl_random.h"
#include "pnl/pnl_finance.h"
#include "pnl/pnl_vector.h"

#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"

#include "../src/ProduitMementis.hpp"
#include "../src/BasketOption.hpp"
#include "../src/ConstantRateModel.hpp"
#include "../src/Utils.hpp"

using namespace std;

int nbActions = 1;
int nbTimeSteps = 12;

double maturity = 12;
double corr = 0.3;
double interest = 0.054;
double strike = 100.0;
PnlVect* payoffCoeff = pnl_vect_create_from_scalar(nbActions,1);
int hedgingDateNumber = 100;

int main(int argc, char **argv) {
    std::map<int,int> mapDevises;
    Utils::mapsDevises(mapDevises);

	bool historical = true;
	int nbSamples = 50000;
	double currentDate = 0;
	double h = 0.1;

	PnlVect *spot = pnl_vect_create_from_scalar(nbActions, 100);
	PnlVect *mu = pnl_vect_create_from_scalar(nbActions, 0.1); // To Change
	PnlVect *sigma = pnl_vect_create_from_scalar(nbActions, 0.2); // To Change
	//PnlVect *divid = pnl_vect_create_from_zero(nbActions);
	//PnlVect *payoffCoeff;

	//Cas ou on connait deja des donnees historiques
	PnlMat *past = pnl_mat_create(1,nbActions);
	for(int i = 0; i < nbActions; i++) {
		pnl_mat_set(past,0,i,100);
	}

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, time(NULL) + (unsigned long)argv[1]);
	mu = pnl_vect_create_from_scalar(nbActions,0.1);
	PnlMat* matCorr = pnl_mat_create(nbActions,nbActions);
	double tirage;
	for(int i = 0; i < nbActions; i++) {
        	for(int j = i; j < nbActions; j++) {
            		if (i == j) {
                		MLET(matCorr, i, j) = 1;
            		} else {
                		tirage = pnl_rng_uni_ab(0.0,0.4,rng);
                		MLET(matCorr, i, j) = tirage;
                		MLET(matCorr, j, i) = tirage;
            		}
        	}
    	}


	RateModel* interest_ = new ConstantRateModel(interest);

	Produit *produit = new BasketOption(maturity, nbTimeSteps, nbActions, strike , payoffCoeff, interest_);

	mu = pnl_vect_create_from_scalar(nbActions,0.1);
	BSParameters* bsparameters = new BSParameters(nbActions, interest_, matCorr, sigma, spot,mu, hedgingDateNumber, mapDevises);
  BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, nbTimeSteps);
	MonteCarlo *montecarlo = new MonteCarlo(model,produit,rng,h,nbSamples);


	double prix;
	double icPrix;

	PnlVect *delta = pnl_vect_create(nbActions);
	PnlVect *icDelta = pnl_vect_create(nbActions);
	montecarlo->delta(past, currentDate, delta, icDelta);
	montecarlo->affichageDelta(delta, icDelta);


	pnl_vect_free(&delta);
	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&mu);
	pnl_rng_free(&rng);
	pnl_mat_free(&past);
	delete bsparameters;
	delete model;
	delete produit;
	delete montecarlo;
	delete interest_;

	return 0;
}
