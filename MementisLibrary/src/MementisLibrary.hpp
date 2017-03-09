#pragma once
#include "dataPriceAndHedgeOut.hpp"
#include "dataPnlOut.hpp"


#define DLLEXP __declspec( dllexport )

namespace MementisLibrary {
	DLLEXP double testLibrary(double a);
	DLLEXP dataPriceAndHedgeOut* priceAndHedgeAtCurrentDate(double currentDate, double** pastWrapped, bool isHistorical, int nbSamples, double* muWrapped, double* sigmaWrapped);
	DLLEXP dataPnLOut* computePnL(int nbHedgingDate, double historicalDate, double currentDate, double** pastWrapped, bool isHistorical, int nbSamples, double* muWrapped, double* sigmaWrapped);
}
