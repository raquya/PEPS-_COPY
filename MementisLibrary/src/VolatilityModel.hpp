#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"

class VolatilityModel
{
public:
	
	virtual double GetVolatility(double t) = 0;
};