#pragma once

class RateModel
{
public:

	virtual double getRate(int t) = 0;
	virtual double integrateRate(double t1, double t2, int indice = 3) =0;

};
