#include "FlowCalculator.hpp"
#include <iostream>

using namespace std;


FlowCalculator::FlowCalculator()
{
}

FlowCalculator::FlowCalculator(double VLO)
{
	this->VLO = VLO;
}

FlowCalculator::~FlowCalculator()
{
}


void FlowCalculator::performanceInTheYear(int i, const PnlMat * path, int size, PnlVect* PerformanceVect, std::map<int,double> mapDevise, RateModel* interest)
{
	double sum = 0;
	int indiceDevise ;
	//double valRef = 0;

	PnlVect *V = pnl_vect_create(path->m);
	PnlVect *W = pnl_vect_create(path->m);

	if (i == 0) {
		pnl_vect_set(PerformanceVect, 0 , 1);
	}else{
		pnl_mat_get_row(V, path, i);
		pnl_mat_get_row(W, path, 0);
		pnl_vect_div_vect_term(V, W);
		for (int j =0 ; j< size ; j++) {
			indiceDevise = mapDevise[j];
			if (indiceDevise != 3){

				sum += GET(V,j)/GET(V, size + indiceDevise)*exp(interest->integrateRate(0, i, indiceDevise));
			}else{
				sum += GET(V,j);
			}
		}
		//sum = pnl_vect_sum(&V);
		//std::cout << sum/size << std::endl;
		pnl_vect_set(PerformanceVect, i , sum/size);
	}

	pnl_vect_free(&V);
	pnl_vect_free(&W);
}


double FlowCalculator::performanceAtEntryPoint(const PnlMat * path, int size, PnlVect* performanceVect)
{
	double perf_min = 1;
	double perf_i;
	for (int i = 1; i <= 4; i++) {
		perf_i = GET(performanceVect, i);
		if (perf_min >= perf_i){
			perf_min = perf_i;
		}
	}
	return perf_min;
}


void FlowCalculator::performanceComparedToEntryPoint(int i, const PnlMat * path, int size, PnlVect* performanceComparedAtEntryPoint, double entryPerformance)
{
	double result = 0;
	double perf = 0;
	//double valRef = 0;

	PnlVect *V = pnl_vect_create(path->m);
	PnlVect *W = pnl_vect_create(path->m);

	pnl_mat_get_row(V, path, i);
	pnl_mat_get_row(W, path, 0);
	pnl_vect_mult_scalar(W,entryPerformance);
	pnl_vect_minus_vect (V, W);
	pnl_vect_div_vect_term(V, W);

	//Limiter les coefficients de V à 0.1

	//sum = pnl_vect_sum(&V);


	for (int k = 0; k < size; k++) {
		//valRef = MGET(path, 0, k);
		//perf = (MGET(path, i, k) - valRef*entryPerformance) / valRef;
		perf = pnl_vect_get(V, k);
		if (perf >= 0.1){
			perf = 0.1;
		}
		result += perf;
	}
	pnl_vect_set(performanceComparedAtEntryPoint, i, result / size);
	pnl_vect_free(&V);
	pnl_vect_free(&W);
}


void FlowCalculator::dividende(int i, const PnlMat * path, int size, PnlVect * vectDividende, PnlVect* PerformanceVect){
	double div = 0;
	if (i == 0){
		div = 0;
		LET(vectDividende, i) = div;
	}else if (i <= 4 && i >= 1) {
		div = 0.056;
		LET(vectDividende, i) = div;
	}else{
		div = 0.6*GET(vectDividende, i-1);
		double perfAnnuel = GET(PerformanceVect, i);
		double tmp = perfAnnuel- div;
		if (tmp >= 0) {
			div = perfAnnuel;
		}
		LET(vectDividende, i) = div;
	}
}
