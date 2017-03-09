#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "RateModel.hpp"
#include <map>

/// \brief Modèle de Black Scholes
class ConstantRateModel: public RateModel
{
public:

	std::map<int,int> mapRateDevises;
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

	double getRate(double t);

	double integrateRate(double t1, double t2, double indice = 3);

};