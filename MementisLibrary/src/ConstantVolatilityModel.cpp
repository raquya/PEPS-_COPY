#include "ConstantVolatilityModel.hpp"


ConstantVolatilityModel::ConstantVolatilityModel(){

}


ConstantVolatilityModel::ConstantVolatilityModel(double v){
	constantVolatility = v;
}


ConstantVolatilityModel::~ConstantVolatilityModel(){

}

double ConstantVolatilityModel::GetVolatility(double volatility){
	return constantVolatility;
}
