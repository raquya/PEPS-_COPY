#include <iostream>
#include <time.h>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include <assert.h>     /* assert */
#include "../src/ProduitMementis.hpp"
#include "../src/ConstantRateModel.hpp"

using namespace std;

int nbActions = 25;
int nbTimeSteps = 12;

double maturity = 12;
double interest = 0.054;
double nbConstationDates = 12;

void remplirPath(PnlMat * path) {
	for (int i = 0 ; i < 13; i++) {
		for (int j = 0; j < 25; j++) {
			MLET(path, i , j) = 100 - j - i;
		}
	}

}


int main(int argc, char **argv) {

	//Cas ou on connait deja des donnees historiques
	PnlMat * path = pnl_mat_create(13,25);
	remplirPath(path);

   	double vlo  = 100;
   	FlowCalculator * flow = new FlowCalculator(vlo);
	RateModel* interest_ = new ConstantRateModel(interest);

	ProduitMementis *produit = new ProduitMementis(flow, maturity, nbTimeSteps, nbActions, nbConstationDates , interest_);

	produit->payoff(path);
	//130.659 correspond à la valeur la plus faible qu'on peut toucher pour une part du produit
	//


	//double pirePayOffEndDur = 130.658912256 - 130.65;
	//double payOffCalcule = produit->PayOffReel - 130.65;
	double pirePayOffEndDur = 130.658912256;
	double payOffCalcule = produit->PayOffReel;
	double difference = produit->PayOffReel- pirePayOffEndDur;


	if (difference< 0.00001 && (difference > -0.00001) ){
		std::cout<< "Le payOff calculé est: "<< produit->PayOffReel<< " ; Le payOff attendu est :"<< pirePayOffEndDur << std::endl;
	}else{
		std::cout<< "Le payOff est mal calculé, il doit valoir " << pirePayOffEndDur << ", mais il vaut " << payOffCalcule<<   std::endl;
	}


	delete produit;
	delete interest_;

	return 0;
}
