/* 
 * File:   BasketOption.cpp
 * Author: samotd
 * 
 * Created on September 16, 2016, 9:44 AM
 */

#include "BasketOption.hpp"
#include <iostream>

BasketOption::BasketOption() {
}

BasketOption::BasketOption(const BasketOption& orig) {
}
 

BasketOption::BasketOption(double monT, int monNbTimeStep, int maSize, double monStrike, PnlVect *payoffcoeff, RateModel* interestRate){
   T_ = monT;
   nbTimeSteps_ = monNbTimeStep;
   size_ = maSize;
   strike = monStrike;
   payoffCoeff_ = payoffcoeff;
   interestRate_ = interestRate;
}

BasketOption::~BasketOption() {
}

double BasketOption::payoff(const PnlMat* path) {
    double somme = 0;

    for (int i = 0; i < path->n; i++) {
        somme = somme + pnl_vect_get(payoffCoeff_,i) * pnl_mat_get(path,nbTimeSteps_,i);
    }
    //pnl_mat_free(&poids);
    somme -= strike;

    if (somme > 0) {
        return somme;
    } else {
        return 0.0;
    }


} 

