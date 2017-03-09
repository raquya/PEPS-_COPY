#pragma once

#include "pnl/pnl_random.h"
#include <map>

namespace Utils {
  void matriceCorrelation(PnlMat* matCorr, PnlRng* rng, double nbActions);
  void mu(PnlVect *mu);
  void sigma(PnlVect *sigma);
  void mapsDevises(std::map<int,int>& devises);
};
