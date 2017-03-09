#pragma once

class RateModel
{
public:

	virtual double getRate(double t) = 0;
	virtual double integrateRate(double t1, double t2) =0;

};
