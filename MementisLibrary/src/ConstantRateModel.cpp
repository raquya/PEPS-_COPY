#include "ConstantRateModel.hpp"

ConstantRateModel::ConstantRateModel(){
	std::map<int,int> mapRateDevises;
	/*constantRateEuro = 0.042;
	constantRateUSD = 0.025;
	constantRateGBP = 0.041;
	contantRateJPY = 0.02;*/
	mapRateDevises[3] = 0.042;
	mapRateDevises[0] = 0.025;
	mapRateDevises[1] = 0.041;
	mapRateDevises[2] = 0.020;
}


/*ConstantRateModel::ConstantRateModel(double r){
}*/


ConstantRateModel::~ConstantRateModel(){

}

double ConstantRateModel::getRate(double t){
	return mapRateDevises[t];
}


double ConstantRateModel::integrateRate(double t1, double t2, double indice){
	return (mapRateDevises[indice]*(t2-t1));
}