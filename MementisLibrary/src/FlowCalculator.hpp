#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include <map>
#include "RateModel.hpp"
#include <math.h>


class FlowCalculator
{
public:
	double VLO;

	FlowCalculator();
	FlowCalculator(double VLO);
	~FlowCalculator();

	void performanceInTheYear(int i, const PnlMat * path, int size, PnlVect* PerformanceVect, std::map<int,int> mapDevise, RateModel* interest);

	double performanceAtEntryPoint(const PnlMat * path, int size, PnlVect* performanceVect);

	void performanceComparedToEntryPoint(int i, const PnlMat * path, int size, PnlVect* performanceComparedAtEntryPoint, double entryPerformance);

	void dividende(int i, const PnlMat * path, int size, PnlVect * vectDividende, PnlVect* PerformanceVect);
	
};

