#include "MementisLibrary.hpp"
#include "dataPriceAndHedgeOut.hpp"
#include <iostream>
#include <time.h>
#include "pnl/pnl_random.h"

#include "BlackScholesModel.hpp"
#include "MonteCarlo.hpp"
#include "FlowCalculator.hpp"
#include "ProduitMementis.hpp"
#include "ConstantRateModel.hpp"
#include "Utils.hpp"

using namespace std;

int nbActions = 25;
int nbTimeSteps = 12;
double vlo = 100;
double maturity = 12;
double interest = 0.042;

int main(int argc, char **argv) {

	dataPriceAndHedgeOut *refPriceAndHedge;
	dataPnLOut *refPnl;
	double* tmp1;
	bool isHistorical = true;
	int nbSamples = 1000;
	int nbHedgingDate = 24;
	double currentDate = 2.6;

	double** pastPricing;
	int nbDatePricingPast = ceil(currentDate) + 1;

	double** pastPnl;
	int nbDatePnlPast = 1;
	double freqHedging = maturity / nbHedgingDate;
	double cpt = currentDate;
	while (cpt>0) {
		cpt -= freqHedging;
		nbDatePnlPast++;
	}

	if (isHistorical) {
		pastPricing = new double*[nbDatePricingPast];
		for (int i = 0; i < nbDatePricingPast; i++) {
			pastPricing[i] = new double[nbActions];
			for (int j = 0; j < nbActions; j++) {
				if (i == 0) {
					pastPricing[i][j] = vlo;
				}
				else {
					pastPricing[i][j] = 104 - i * 3;  // FAKE
				}
			}
		}
		pastPnl = new double*[nbDatePnlPast];
		for (int i = 0; i < nbDatePnlPast; i++) {
			pastPnl[i] = new double[nbActions];
			for (int j = 0; j < nbActions; j++) {
				if (i == 0) {
					pastPnl[i][j] = vlo;
				}
				else {
					pastPnl[i][j] = 104 - i * 3;  // FAKE
				}
			}
		}
	}

	cout << "*** Price and Hedge" << endl;
	refPriceAndHedge = MementisLibrary::priceAndHedgeAtCurrentDate(currentDate, pastPricing, isHistorical, nbSamples, tmp1, tmp1);
//	cout << "*** ComputePnL à t = 0" << endl;
//	refPnl = MementisLibrary::computePnL(nbHedgingDate, currentDate, 0, pastPnl, isHistorical, nbSamples, tmp1, tmp1);
	cout << "*** ComputePnL à t = " << currentDate << endl;
	refPnl = MementisLibrary::computePnL(nbHedgingDate, currentDate, currentDate, pastPnl, isHistorical, nbSamples, tmp1, tmp1);

	if (isHistorical) {
		for (int i = 0; i < nbDatePricingPast; i++) {
			delete[] pastPricing[i];
		}
		delete[] pastPricing;
		for (int i = 0; i < nbDatePnlPast; i++) {
			delete[] pastPnl[i];
		}
		delete[] pastPnl;
	}

/*cout << "Press a key to stop !" << endl;
	getchar();*/
	return 1;
}



dataPriceAndHedgeOut* MementisLibrary::priceAndHedgeAtCurrentDate(double currentDate, double** pastWrapped, bool isHistorical, int nbSamples, double* muWrapped, double* sigmaWrapped) {

	double h = 0.1;
	PnlVect *spot;
	PnlVect *mu = pnl_vect_create(nbActions);
	Utils::mu(mu);
	PnlVect *sigma = pnl_vect_create(nbActions);
	Utils::sigma(sigma);
	//PnlVect *divid = pnl_vect_create_from_zero(nbActions);
	//PnlVect *payoffCoeff;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, (unsigned long)time(NULL));

	int nbDatePast = ceil(currentDate) + 1;
	PnlMat *past = pnl_mat_create(nbDatePast, nbActions);
	if (isHistorical) {
		spot = pnl_vect_create_from_ptr(nbActions, pastWrapped[0]);
		for (int i = 0; i < nbDatePast; i++) {
			for (int j = 0; j < nbActions; j++) {
				MLET(past, i, j) = pastWrapped[i][j];
			}
		}
	} else {
		spot = pnl_vect_create(nbActions);
		for (int i = 0; i < nbActions; i++) {
			LET(spot, i) = pnl_rng_uni_ab(80, 120, rng);
		}
	}

	PnlMat* matCorr = pnl_mat_create(nbActions, nbActions);
	Utils::matriceCorrelation(matCorr, rng, nbActions);

	FlowCalculator *flow = new FlowCalculator(vlo);
	RateModel* interest_ = new ConstantRateModel(interest);
	Produit *produit = new ProduitMementis(flow, maturity, nbTimeSteps, nbActions, nbTimeSteps, interest_);
	BSParameters* bsparameters = new BSParameters(nbActions, interest_, matCorr, sigma, spot, mu, 0);
	BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, nbTimeSteps);
	MonteCarlo *montecarlo = new MonteCarlo(model, produit, rng, h, nbSamples);

	if (!isHistorical) {
		model->simulMarket(past, rng, false);
	}

	/* *** Price *** */
	double prix;
	double icPrix;
	montecarlo->price(past, currentDate, prix, icPrix);
	montecarlo->affichagePrice(currentDate, prix, icPrix);

	/* *** Hedge *** */
	PnlVect *delta = pnl_vect_create(nbActions);
	PnlVect *icDelta = pnl_vect_create(nbActions);
	montecarlo->delta(past, currentDate, delta, icDelta);
	montecarlo->affichageDelta(delta, icDelta);

	dataPriceAndHedgeOut *resultats = new dataPriceAndHedgeOut();
	resultats->nbPdtCouv = nbActions;
	resultats->delta = delta->array;
	resultats->icDelta = icDelta->array;
	resultats->prix = prix;
	resultats->icPrix = icPrix;

	delta->array = NULL;
	icDelta->array = NULL;

	pnl_vect_free(&delta);
	pnl_vect_free(&icDelta);
	//pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&mu);
	pnl_rng_free(&rng);
	//pnl_mat_free(&past);
	pnl_mat_free(&matCorr);
	delete bsparameters;
	delete model;
	delete produit;
	delete montecarlo;
	delete flow;
	delete interest_;

	return resultats;
}



dataPnLOut* MementisLibrary::computePnL(int nbHedgingDate, double historicalDate, double currentDate, double** pastWrapped, bool isHistorical, int nbSamples, double* muWrapped, double* sigmaWrapped) {

	double h = 0.1;
	PnlVect *spot;
	PnlVect *mu = pnl_vect_create(nbActions);
	Utils::mu(mu);
	PnlVect *sigma = pnl_vect_create(nbActions);
	Utils::sigma(sigma);
	//PnlVect *divid = pnl_vect_create_from_zero(nbActions);
	//PnlVect *payoffCoeff;

	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, (unsigned long)time(NULL));

	int nbDatePast = 1;
	double freqHedging = maturity / nbHedgingDate;
	int nbDateToDo = nbHedgingDate + 1;
	double cpt = historicalDate;
	while (cpt>0) {
		cpt -= freqHedging;
		nbDatePast++;
	}
	cpt = currentDate;
	while (cpt>0) {
		cpt -= freqHedging;
		nbDateToDo--;
	}

	PnlMat *past = pnl_mat_create(nbDatePast, nbActions);
	if (isHistorical) {
		spot = pnl_vect_create_from_ptr(nbActions, pastWrapped[0]);
		for (int i = 0; i < nbDatePast; i++) {
			for (int j = 0; j < nbActions; j++) {
				MLET(past, i, j) = pastWrapped[i][j];
			}
		}
	} else {
		spot = pnl_vect_create(nbActions);
		for (int i = 0; i < nbActions; i++) {
			LET(spot, i) = pnl_rng_uni_ab(80, 120, rng);
		}
	}

	PnlMat* matCorr = pnl_mat_create(nbActions, nbActions);
	Utils::matriceCorrelation(matCorr, rng, nbActions);

	FlowCalculator *flow = new FlowCalculator(vlo);
	RateModel* interest_ = new ConstantRateModel(interest);
	Produit *produit = new ProduitMementis(flow, maturity, nbTimeSteps, nbActions, 12, interest_);
	BSParameters* bsparameters = new BSParameters(nbActions, interest_, matCorr, sigma, spot, mu, nbHedgingDate);
	BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, nbTimeSteps);
	MonteCarlo *montecarlo = new MonteCarlo(model, produit, rng, h, nbSamples);

	PnlMat* market = pnl_mat_create(nbHedgingDate + 1, nbActions);
	PnlVect *currentDateSpot;
	if (isHistorical) {
		if (nbDatePast == nbHedgingDate + 1) {
			market = pnl_mat_copy(past);
		} else {
			model->simulMarket(market, historicalDate, past, rng, true);
			currentDateSpot = pnl_vect_create(nbActions);
			pnl_mat_get_row(currentDateSpot, past, past->m - 1);
		}
	} else {
		model->simulMarket(market, rng, true);

		// Approximation : En généré on se eplace à la dernière date de hedging
		cpt = currentDate;
		while (cpt>0) {
			cpt -= freqHedging;
		}
		if (cpt != 0) {
			currentDate -= freqHedging + cpt;
		}
		currentDateSpot = pnl_vect_create_from_scalar(nbActions, -99);
	}

	double pnl;

	PnlVect *valPtfCouv = pnl_vect_create(nbDateToDo);
	PnlVect *prixPdt = pnl_vect_create(nbDateToDo);
	montecarlo->profitAndLoss(currentDate, currentDateSpot, valPtfCouv, pnl, prixPdt, market);
	montecarlo->affichagePnL(valPtfCouv, pnl, prixPdt);

	dataPnLOut *resultats = new dataPnLOut();

	resultats->nbHedgingDate = nbHedgingDate;
	resultats->valPtfCouv = valPtfCouv->array;
	resultats->prixPdt = prixPdt->array;
	resultats->pnl = pnl;

	valPtfCouv->array = NULL;
	prixPdt->array = NULL;

	pnl_vect_free(&valPtfCouv);
//pnl_vect_free(&prixPdt);
	pnl_vect_free(&spot);
	pnl_vect_free(&sigma);
	pnl_vect_free(&mu);
	pnl_vect_free(&currentDateSpot);
	pnl_rng_free(&rng);
	pnl_mat_free(&past);
	pnl_mat_free(&matCorr);
	delete bsparameters;
	delete model;
	delete produit;
	delete montecarlo;
	delete flow;
	delete interest_;

	return resultats;
}


// Fonction Test
// Tire un nombre aleatoirement selon une loi normale centree reduite via la librairie PNL
double MementisLibrary::testLibrary(double m) {
	PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
	pnl_rng_sseed(rng, (unsigned long)time(NULL));
	m = pnl_rng_normal(rng);
	pnl_rng_free(&rng);
	return m;
}
