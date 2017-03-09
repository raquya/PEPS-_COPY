/*
 * HedgingCallTest.cpp
 */

#include <iostream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_finance.h"
#include <time.h>
#include <string.h>


#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/parser.hpp"
#include "../src/FlowCalculator.hpp"
#include "../src/ProduitMementis.hpp"

#include "../src/ConstantRateModel.hpp"
#include "../src/Utils.hpp"

using namespace std;

int nbActions = 25;
int nbTimeSteps = 12;

double maturity = 12;
double corr = 0.3;
double interest = 0.0420;
double strike = 100.0;
PnlVect* payoffCoeff = pnl_vect_create_from_scalar(nbActions,1);
int hedgingDateNumber = 144;



int main(int argc, char **argv) {
	map<int,int> mapDevises;
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
	pnl_rng_sseed(rng, time(NULL));
	mu = pnl_vect_create_from_scalar(nbActions,0.1);
	PnlMat* matCorr = pnl_mat_create(nbActions,nbActions);
	double tirage = 0.3;
	for(int i = 0; i < nbActions; i++) {
        	for(int j = i; j < nbActions; j++) {
            		if (i == j) {
                		MLET(matCorr, i, j) = 1;
            		} else {
                		MLET(matCorr, i, j) = tirage;
                		MLET(matCorr, j, i) = tirage;
            		}
        	}
    }
  	Utils::mu(mu);
  	Utils::sigma(sigma);
	double vlo = 100;
	FlowCalculator * flow = new FlowCalculator(vlo);
	RateModel* interest_ = new ConstantRateModel(interest);

	Produit *produit = new ProduitMementis(flow, maturity, nbTimeSteps, nbActions, 12, interest_);

	mu = pnl_vect_create_from_scalar(nbActions,0.1);
	BSParameters* bsparameters = new BSParameters(nbActions, interest_, matCorr, sigma, spot,mu, hedgingDateNumber,mapDevises);
  BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, nbTimeSteps);
	MonteCarlo *montecarlo = new MonteCarlo(model,produit,rng,h,nbSamples);


	double prix;
	double icPrix;
	double PnL;
	int nbDePnl = 12;
	PnlMat* market = pnl_mat_create(hedgingDateNumber+1,nbActions);
	model->asset(market,rng,true);
	PnlVect *Pnl = pnl_vect_create_from_zero(nbDePnl);
	PnlVect *V = pnl_vect_create_from_zero(hedgingDateNumber+1);
	PnlVect *prixPdt = pnl_vect_create_from_zero(hedgingDateNumber+1);


	for (int i=1; i < nbDePnl+1; i++) {
		montecarlo->mod_->parameters->hedgingDateNb_ = 12 * i;
		montecarlo->profitAndLoss(0, NULL, V, PnL, prixPdt, market);
		LET(Pnl,i-1) = PnL;
		std::cout << PnL<< std::endl;
	}

	pnl_vect_print(Pnl);

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
