/*
 * HedgingCallTest.cpp
 */

#include <iostream>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include "pnl/pnl_finance.h"
#include <time.h>
#include <string.h>


#include "../src/BlackScholesModel.hpp"
#include "../src/MonteCarlo.hpp"
#include "../src/parser.hpp"
#include "../src/FlowCalculator.hpp"
#include "../src/ProduitMementis.hpp"
#include "../src/BasketOption.hpp"

#include "../src/ConstantRateModel.hpp"

using namespace std;

using namespace std;

int nbActions = 1;
int nbTimeSteps = 12;

double maturity = 12;
double corr = 0.3;
double interest = 0.054;
double strike = 100.0;
PnlVect* payoffCoeff = pnl_vect_create_from_scalar(nbActions,1);
int hedgingDateNumber = 100;


void prixEtDeltaFormuleFerme(double temps, PnlVect* sigma, double strike, double maturity, double interest , double st, double & prix, double& delta){
    double sigm = GET(sigma,0);

    double interieurd1 = st/(strike);//*exp(-interest*(maturity-temps)));
    double d1 = 1/(sigm*sqrt(maturity-temps))*(log(interieurd1) + (interest +0.5*sigm*sigm)*(maturity-temps));
    double d2 = d1 - sigm*sqrt(maturity-temps);
    double Nd1 = 0;
    double unMoinsNd1 = 0;
    double unMoinsNd2 = 0;
    int status = 0;
    double bound =0 ;
    int which = 1;
    double mean = 0;
    double standard = 1;
    pnl_cdf_nor(&which, &Nd1, &unMoinsNd1, &d1, &mean, &standard, &status, &bound);
    double Nd2 = 0;
    pnl_cdf_nor(&which, &Nd2, &unMoinsNd2, &d2, &mean, &standard, &status, &bound);

    prix = st*Nd1 - strike*exp(-interest*(maturity-temps))*Nd2;
    delta = Nd1;
}


void profitAndLoss(PnlVect *V, double &PnL, PnlVect *prixPdt, PnlVect* sigma, double strike, double maturity, double interest, BlackScholesModel* mod_, PnlRng *rng) {

    PnlVect* valPortefeuilleCouv = pnl_vect_create_from_zero(mod_->parameters->hedgingDateNb_ + 1);
    PnlVect* valProduit = pnl_vect_create_from_zero(mod_->parameters->hedgingDateNb_ + 1);



    PnlMat *marketPath = pnl_mat_create(mod_->parameters->hedgingDateNb_ + 1, mod_->parameters->size_);
    mod_->asset(marketPath, rng, true);
    double valeur;
    double prix;
    double ic;
    PnlVect *delta1 = pnl_vect_create_from_zero(mod_->parameters->size_);
    PnlVect *delta2 = pnl_vect_create_from_zero(mod_->parameters->size_);
    PnlVect *Stauxi = pnl_vect_create_from_zero(mod_->parameters->size_);
    PnlMat *past = pnl_mat_new();
    PnlMat *pastcopy = pnl_mat_new();

    double pas =maturity / (double) mod_->parameters->hedgingDateNb_;
    double pasConstatation = maturity / (double) mod_->nbTimeSteps_;
    PnlVect *sub = pnl_vect_create(mod_->parameters->size_);
    int cpt = 0;

    // Initialisation du portefeuille pour t=0
    pnl_mat_extract_subblock(past, marketPath, 0, 1, 0, marketPath->n);
    pnl_mat_get_row(Stauxi,marketPath,0);

    prixEtDeltaFormuleFerme(0, sigma, strike, maturity, interest , MGET(marketPath,0,0), prix, GET(delta1,0));

    valeur = prix - pnl_vect_scalar_prod(delta1,Stauxi);
    double valPortefeuilleCouverture = prix;

    pnl_vect_set(V,0,valeur);
    pnl_vect_set(prixPdt,0, prix);
    LET(valPortefeuilleCouv,0) = valPortefeuilleCouverture;
    LET(valProduit,0) = prix;
    //cout << "     " << valPortefeuilleCouverture << "              |           " << prix << "      " << endl;

    // Calcul de la valeur du portefeuille à chaque instant
    for (int j = 1; j < mod_->parameters->hedgingDateNb_ + 1; j++) {
        // Mise a jour de la matrice past
        if ((pas*j > pasConstatation*cpt) == 1) {
            pastcopy = pnl_mat_copy(past);
            pnl_mat_resize(past, (past->m + 1), mod_->parameters->size_);
            pnl_mat_set_subblock(past,pastcopy,0,0);
            cpt ++;
        }
        pnl_mat_get_row(sub,marketPath,j);
        pnl_mat_set_row(past,sub,(past->m - 1));

        //Calcul du prix du produit et des deltas
        double st = MGET(marketPath,j,0);
        prixEtDeltaFormuleFerme(pas*j, sigma, strike, maturity, interest , st, prix, GET(delta2,0));

        //pnl_vect_minus_vect(delta2, delta1);
        pnl_mat_get_row(Stauxi,past,(past->m - 1));

        // Calcul nouvelle valeur du portefeuille
        valPortefeuilleCouverture = GET(V,j-1) * exp((mod_->parameters->r_->integrateRate(pas*j,pas*(j+1)))) + pnl_vect_scalar_prod(delta1,Stauxi);

        //pnl_vect_minus_vect(delta2, delta1);
        valeur = valPortefeuilleCouverture - pnl_vect_scalar_prod(delta2,Stauxi);
        pnl_vect_set(V,j,valeur);
        // Mise a jour des vecteurs deltas pour la suite des calculs
        //pnl_vect_plus_vect(delta2, delta1);
        delta1 = pnl_vect_copy(delta2);
        pnl_vect_set(prixPdt,j, prix);
        LET(valPortefeuilleCouv,j) = valPortefeuilleCouverture;
        LET(valProduit,j) = prix;
        //cout << "     " << valPortefeuilleCouverture << "      |        " << prix << "      " << endl;
        //cout << "     " << exp((mod_->parameters->r_->integrateRate(pas*j,pas*(j+1)))) << endl;
    }

    // Calcul du PnL
    PnL = pnl_vect_get(V,mod_->parameters->hedgingDateNb_) + pnl_vect_scalar_prod(delta2,Stauxi) - prix;
    std::cout<< PnL << std::endl;
    // On free les objets alloués
    pnl_vect_free(&valProduit);
    pnl_vect_free(&valPortefeuilleCouv);
    pnl_vect_free(&delta1);
    pnl_vect_free(&delta2);
    pnl_vect_free(&Stauxi);
    pnl_vect_free(&sub);
    pnl_mat_free(&past);
    pnl_mat_free(&pastcopy);

}



int main(int argc, char **argv) {

    bool historical = true;
    int nbSamples = 50000;
    double currentDate = 0;
    double h = 0.1;

    PnlVect *spot = pnl_vect_create_from_scalar(nbActions, 100);
    PnlVect *mu = pnl_vect_create_from_scalar(nbActions, 0.1);
    PnlVect *sigma = pnl_vect_create_from_scalar(nbActions, 0.2);

    //Cas ou on connait deja des donnees historiques
    PnlMat *past = pnl_mat_create(1,nbActions);
    for(int i = 0; i < nbActions; i++) {
        pnl_mat_set(past,0,i,100);
    }
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL) + (unsigned long)argv[1]);

    mu = pnl_vect_create_from_scalar(nbActions,0.1);
    PnlMat* matCorr = pnl_mat_create(nbActions,nbActions);

    double tirage;
    for(int i = 0; i < nbActions; i++) {
        for(int j = i; j < nbActions; j++) {
            if (i == j) {
                MLET(matCorr, i, j) = 1;
            } else {
                tirage = pnl_rng_uni_ab(0.0,0.4,rng);
                MLET(matCorr, i, j) = tirage;
                MLET(matCorr, j, i) = tirage;
            }
        }
    }
    RateModel* interest_ = new ConstantRateModel(interest);

    double prix;
    double PnL;
    double ic;
   	// Test Prix et delta Call avec prix call Formule fermée
    double temps = 0;
    double st =  MGET(past,0,0)*exp((interest - GET(sigma,0)*GET(sigma,0)*0.5)*temps + GET(sigma,0)*sqrt(temps)*pnl_rng_normal(rng));

    BSParameters* bsparameters = new BSParameters(nbActions, interest_, matCorr, sigma, spot,mu, hedgingDateNumber);
    BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, nbTimeSteps);
    prix = 0;
    double deltaferme = 0;
    prixEtDeltaFormuleFerme(temps, sigma, strike, maturity, interest , st, prix, deltaferme);

    //std::cout<<  "prix " <<prix << " ;; Delta " << deltaferme <<  std::endl;


    // formule fermé

    PnlVect *V = pnl_vect_create(hedgingDateNumber + 1);
    PnlVect * listPrix = pnl_vect_create(hedgingDateNumber + 1 );
    profitAndLoss(V, PnL, listPrix, sigma, strike, maturity, interest, model, rng);

    cout << "-----------------------------------------------------" << endl;
    cout << "   Valeur portefeuille\t|\tValeur produit\t" << endl;
    cout << "-----------------------------------------------------" << endl;
    for (int i = 0; i < V->size; i++) {
      cout << "\t" << GET(V, i) << "\t|\t" << GET(listPrix, i) << "\t" << endl;
    }
    cout << endl << " PnL : " << PnL << endl;

    pnl_vect_free(&V);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_mat_free(&past);
    pnl_rng_free(&rng);
    delete interest_;


    exit(0);
	return 0;
}
