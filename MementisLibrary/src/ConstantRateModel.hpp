#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "RateModel.hpp"
#include <map>
#include <iostream>

/// \brief Modèle de Black Scholes
class ConstantRateModel: public RateModel
{
public:

	std::map<int,double> mapRateDevises;
	/*!
	* \brief Constructeur par défaut
	*/
	ConstantRateModel();

	/*!
	* \brief Constructeur

	ConstantRateModel(double r);*/

	/*!
	* \brief Destructeur
	*/
	virtual ~ConstantRateModel();

	double getRate(int t);

	double integrateRate(double t1, double t2, int indice = 3);

};