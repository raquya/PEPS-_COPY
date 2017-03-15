#pragma once

#include "pnl/pnl_random.h"
#include <map>
#include <string>

namespace Utils {
  void matriceCorrelation(PnlMat* matCorr, PnlRng* rng, double nbActions);
  void mu(PnlVect *mu);
  void sigma(PnlVect *sigma);
  void mapsDevises(std::map<int,double>& devises, std::string type, int size);
};
