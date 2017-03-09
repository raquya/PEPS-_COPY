#pragma once

#include "pnl/pnl_vector.h"
#include "pnl/pnl_matrix.h"
#include "VolatilityModel.hpp"

/// \brief Modèle de Black Scholes
class ConstantVolatilityModel: public VolatilityModel
{
public:

	double constantVolatility;
	/*!
	* \brief Constructeur par défaut
	*/
	ConstantVolatilityModel();

	/*!
	* \brief Constructeur
	*/
	ConstantVolatilityModel(double v);

	/*!
	* \brief Destructeur
	*/
	virtual ~ConstantVolatilityModel();

	double GetVolatility(double volatility);
};