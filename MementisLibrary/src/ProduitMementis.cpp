#include "math.h"
#include "ProduitMementis.hpp"
#include <iostream>

using namespace std;

ProduitMementis::ProduitMementis()
{
}

ProduitMementis::~ProduitMementis()
{
}

ProduitMementis::ProduitMementis(FlowCalculator* flow, double maturity, int nbTimeSteps_, int nbAsset_, int nbConstationDates, RateModel* interest, std::map<int,int> mapDevises) {
	this->flowCalculator = flow;
	this->T_ = maturity;
	this->nbTimeSteps_= nbTimeSteps_;
	this->nbAssets_ = nbAsset_;
	this->nbConstationDates = nbConstationDates;
	interestRate_ = interest;
	this->mapDevises = mapDevises;
	PayOffReel = 0;
}


// On actualise les différents flux de dividende à la date de maturité
double ProduitMementis::payoff(const PnlMat *path){
	//
	double payOff = 0;
	double temp;
	double vlo = flowCalculator->VLO;
	PnlVect * vectDividende  = pnl_vect_create_from_zero(this->nbConstationDates + 1);
	PnlVect * PerformanceVect = pnl_vect_create_from_zero(this->nbConstationDates + 1);
	PnlVect * PerfomanceComparedtoEntryPoint = pnl_vect_create_from_zero(this->nbConstationDates + 1);


	//double moyenne;
	for (int i = 0 ; i<= this->nbConstationDates; i++){
		flowCalculator->performanceInTheYear(i, path, nbAssets_, PerformanceVect, this->mapDevises, interestRate_);
	}

	double entryPerformance = flowCalculator->performanceAtEntryPoint(path, nbAssets_, PerformanceVect);

	for (int i = 0; i <= this->nbConstationDates; i++){
		flowCalculator->performanceComparedToEntryPoint(i, path, nbAssets_, PerfomanceComparedtoEntryPoint, entryPerformance);
		flowCalculator->dividende(i, path, nbAssets_, vectDividende, PerfomanceComparedtoEntryPoint);

		temp = GET(vectDividende, i)*exp(interestRate_->integrateRate(i,T_));
		payOff += temp;
	}


	// = moyenne/12.0;
	double plusValue = 0;

	double sum_div = 0;
	double sum_perf = 0;

	for (int i = 1; i <= T_; i++) {
		sum_div += GET(vectDividende, i);
		sum_perf += GET(PerformanceVect,i) ;
	}

	plusValue = ((sum_perf/T_) - 1) - sum_div;

	if (plusValue <= 0) {
		plusValue = 0;
	}
	payOff += 1 + plusValue;
	PayOffReel = (pnl_vect_sum(vectDividende) + plusValue + 1)*vlo;
	pnl_vect_free(&vectDividende);
	pnl_vect_free(&PerformanceVect);
	pnl_vect_free(&PerfomanceComparedtoEntryPoint);
	return ((payOff)*vlo) ;
}
