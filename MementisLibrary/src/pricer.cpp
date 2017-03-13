#include <iostream>
#include <string>
#include <ctime>
#include "pnl/pnl_random.h"
#include "pnl/pnl_vector.h"
#include <time.h>
#include <string.h>
#include <map>

#include "BlackScholesModel.hpp"
#include "MonteCarlo.hpp"
#include "parser.hpp"
#include "FlowCalculator.hpp"
#include "ProduitMementis.hpp"
#include "BasketOption.hpp"
#include "AsianOption.hpp"
#include "ConstantRateModel.hpp"
#include "Utils.hpp"

using namespace std;

int main(int argc, char **argv)
{
    double steps, maturity, interest, strike, corr;
    PnlVect *spot, *mu, *sigma, *divid, *payoffCoeff;
    string type;
    int size, timeStepsNb, hedgingDateNumber;
    size_t sample;
    double vlo = 100;

    map<int,double> mapDevises;
    Utils::mapsDevises(mapDevises);

    
    int argFileNb;

    if(argc == 2) {
        argFileNb = 1;
    } else if(argc == 3) {
        // if -c //
        char *parametre = argv[1];
        if(strcmp(parametre,"-c") != 0){
            std::cerr << "Le paramètre passé n'est pas le bon"<< parametre << std::endl;
            return 1;
        }
        argFileNb = 2;

    } else {
        std::cerr << "Le nombre de paramètres passés au programme n'est pas le bon" << std::endl;
        return 1;
    }

    char *infile = argv[argFileNb];


    Param *P = new Parser(infile);
    int nbDevises = 3;
    // Extraction des parametres avec le parser
    P->extract("option size", size);
    P->extract("spot", spot, size);
    P->extract("maturity", maturity);
    P->extract("volatility", sigma, size);
    P->extract("interest rate", interest);
    P->extract("correlation", corr);
    P->extract("trend", mu, size);
    P->extract("strike", strike);
    P->extract("option type", type);
    P->extract("payoff coefficients", payoffCoeff, size);
    P->extract("timestep number", timeStepsNb);
    P->extract("hedging dates number", hedgingDateNumber);
    P->extract("fd step", steps);
    P->extract("sample number", sample);
    if (P->extract("dividend rate", divid, size) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    // Gestion des cas interdits
    if (maturity < 0 ) {
        std::cerr << "La date de maturité est négative" << std::endl;
        return 1;
    } else if (size <= 0) {
        std::cerr << "La taille de l'option est négative ou nulle" << std::endl;
        return 1;
    } else if (strike < 0) {
        std::cerr << "Le strike de l'option a une valeur négative" << std::endl;
        return 1;
    } else if (sample <= 0) {
        std::cerr << "Le nombre d'échantillon pour la méthode de montecarlo est négatif ou nul" << std::endl;
        return 1;
    } else if (timeStepsNb <= 0) {
        std::cerr << "Le nombre de dates à laquelle on évalue le prix du sous-jacent est négatif ou nul" << std::endl;
        return 1;
    } else if (hedgingDateNumber <= 0) {
        std::cerr << "Le nombre de dates à laquelle on évalue le prix du portefeuille de couverture est négatif ou nul" << std::endl;
        return 1;
    }
    PnlRng *rng = pnl_rng_create(PNL_RNG_MERSENNE);
    pnl_rng_sseed(rng, time(NULL));


    PnlMat *past = pnl_mat_create(1,size + nbDevises);
    Produit *produit;
    FlowCalculator * flow = NULL;
    RateModel* interest_ = new ConstantRateModel();
    PnlMat* matCorr = pnl_mat_create(size + nbDevises, size + nbDevises);

    if(type == "mementis") {
        flow = new FlowCalculator(vlo);
        produit = new ProduitMementis(flow, maturity, timeStepsNb, size, timeStepsNb, interest_, mapDevises);
        Utils::matriceCorrelation(matCorr, rng, size + nbDevises);

        mu = pnl_vect_create(size + nbDevises);
        Utils::mu(mu);
        sigma = pnl_vect_create(size + nbDevises);
        Utils::sigma(sigma);
        pnl_vect_resize(spot,(size + nbDevises));
        for (int i =0; i< (size + nbDevises); i++) {
            if (i <size){
                pnl_vect_set(spot, i, pnl_rng_uni_ab(80,120,rng));
            }else if (i == size){
                pnl_vect_set(spot, i, 0.943);
            }else if (i == size +1){
                pnl_vect_set(spot, i, 1.1460);
            }else{
                pnl_vect_set(spot, i, 0.0082);
            }
        }
    } else if (type == "basket") {
        PnlMat* matCorr = pnl_mat_create(size,size);
        produit = new BasketOption(maturity, timeStepsNb, size, strike , payoffCoeff, interest_);
        for(int i = 0; i < size; i++) {
            for(int j = i; j < size; j++) {
                if (i == j) {
                    MLET(matCorr, i, j) = 1;
                } else {
                    MLET(matCorr, i, j) = corr;
                    MLET(matCorr, j, i) = corr;
                }
            }
        }
        mu = pnl_vect_create_from_scalar(size, 0.05 );
        sigma = pnl_vect_create_from_scalar(size,0.2);
    } else if(type == "asian") {
        PnlMat* matCorr = pnl_mat_create(size,size);
        produit = new AsianOption(maturity, timeStepsNb, size, strike, payoffCoeff, interest_);
        for(int i = 0; i < size; i++) {
            for(int j = i; j < size; j++) {
                if (i == j) {
                    MLET(matCorr, i, j) = 1;
                } else {
                    MLET(matCorr, i, j) = corr;
                    MLET(matCorr, j, i) = corr;
                }
            }
        }
        mu = pnl_vect_create_from_scalar(size, 0.1 );
        sigma = pnl_vect_create_from_scalar(size,0.2);
    } else {
        exit(0);
    }

    pnl_mat_set_row(past,spot, 0);

    BSParameters* bsparameters = new BSParameters(size + nbDevises, interest_, matCorr, sigma, spot, mu, hedgingDateNumber, mapDevises);
    BlackScholesModel *model = new BlackScholesModel(bsparameters, maturity, timeStepsNb);
    MonteCarlo *montecarlo = new MonteCarlo(model, produit, rng, steps, sample);

    double prix;
    double icPrix;
    double PnL;

    if(argc == 2) {
        montecarlo->price(prix, icPrix);
        montecarlo->affichagePrice(0.0, prix, icPrix);

        PnlVect *delta = pnl_vect_create_from_zero(size + nbDevises);
        PnlVect *icDelta = pnl_vect_create_from_zero(size + nbDevises);
        montecarlo->delta(past, 0.0, delta, icDelta);
        montecarlo->affichageDelta(delta, icDelta);

        pnl_vect_free(&spot);
        pnl_vect_free(&sigma);
        pnl_vect_free(&mu);
        pnl_vect_free(&divid);
        pnl_rng_free(&rng);
        pnl_mat_free(&past);
        pnl_mat_free(&matCorr);
        delete bsparameters;
        delete P;
        delete model;
        delete produit;
        delete montecarlo;
        if (flow) {
            delete flow;
        }
        delete interest_;
        return 1;
    }


    PnlVect *V = pnl_vect_create(hedgingDateNumber + 1);
    PnlVect * listPrix = pnl_vect_create(hedgingDateNumber + 1 );
    montecarlo->profitAndLoss(V, PnL, listPrix);
    montecarlo->affichagePnL(V, PnL, listPrix);

    pnl_vect_free(&mu);
    pnl_vect_free(&V);
    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    pnl_mat_free(&past);
    pnl_mat_free(&matCorr);
    pnl_rng_free(&rng);
    delete bsparameters;
    delete P;
    delete model;
    delete produit;
    delete montecarlo;
    if (flow) {
        delete flow;
    }
    delete interest_;


    exit(0);
}
