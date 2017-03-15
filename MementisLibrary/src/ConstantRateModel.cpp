#include "ConstantRateModel.hpp"

ConstantRateModel::ConstantRateModel(){
	this->mapRateDevises[3] = 0.042;
	this->mapRateDevises[0] = 0.025;
	this->mapRateDevises[1] = 0.041;
	this->mapRateDevises[2] = 0.020;
}




ConstantRateModel::ConstantRateModel(double r){
	this->mapRateDevises[3] = r;
	this->mapRateDevises[0] = 0.025;
	this->mapRateDevises[1] = 0.041;
	this->mapRateDevises[2] = 0.020;
}


ConstantRateModel::~ConstantRateModel(){

}

double ConstantRateModel::getRate(int t){
	return mapRateDevises[t];
}


double ConstantRateModel::integrateRate(double t1, double t2, int indice){
	return (mapRateDevises[indice]*(t2-t1));
}