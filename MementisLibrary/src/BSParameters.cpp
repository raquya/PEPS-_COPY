#include "BSParameters.hpp"

#include <iostream>


BSParameters::BSParameters(int size, RateModel* r, PnlMat* matCorr, PnlVect * sigma, PnlVect* spot,
 PnlVect* mu, int hedgingDateNumber, std::map<int,int> mapDevises) {
	this->sigma_ = sigma;
	this->spot_ = spot;
	this->size_ = size;
	this->r_ = r;
	this->hedgingDateNb_ = hedgingDateNumber;
	this->mu_ = mu;
	this->Gn = pnl_vect_create(size_);
	this->CorrelationMat = matCorr;

	// Factorisation de cholesky de la matrice de correlation
	this->cholesky = pnl_mat_chol(CorrelationMat);
}

BSParameters::~BSParameters() {
}
