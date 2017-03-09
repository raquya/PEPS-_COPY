#include "ConstantRateModel.hpp"

ConstantRateModel::ConstantRateModel(){
	constantRate = 0.042;
}


ConstantRateModel::ConstantRateModel(double r){
	constantRate = r;
}


ConstantRateModel::~ConstantRateModel(){

}

double ConstantRateModel::getRate(double t){
	return constantRate;
}

double ConstantRateModel::integrateRate(double t1, double t2){
	return (constantRate*(t2-t1));
}