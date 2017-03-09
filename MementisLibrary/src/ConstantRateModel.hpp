#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "RateModel.hpp"

/// \brief Modèle de Black Scholes
class ConstantRateModel: public RateModel
{
public:

	double constantRate;
	/*!
	* \brief Constructeur par défaut
	*/
	ConstantRateModel();

	/*!
	* \brief Constructeur
	*/
	ConstantRateModel(double r);

	/*!
	* \brief Destructeur
	*/
	virtual ~ConstantRateModel();


	double getRate(double t);

	double integrateRate(double t1, double t2);

};