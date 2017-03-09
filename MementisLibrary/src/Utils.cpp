#include "Utils.hpp"

void Utils::matriceCorrelation(PnlMat* matCorr, PnlRng* rng, double nbActions) {
    double tirage;
    for(int i = 0; i < nbActions; i++) {
        for(int j = i; j < nbActions; j++) {
            if (i == j) {
                MLET(matCorr, i, j) = 1;
            } else {
                //tirage = pnl_rng_uni_ab(0.0,0.4,rng);
                tirage = 0.3;
                MLET(matCorr, i, j) = tirage;
                MLET(matCorr, j, i) = tirage;
            }
        }
    }
}

//Initialisation de mu avec des valeurs historiques (mu de l'année 2016)
void Utils::mu(PnlVect *mu) {
    //TOKYO GAS
    LET(mu,0) =0.0229;
    //KIRIN BREWERY
    LET(mu,1) = 0.0203;
    //TOYOTA
    LET(mu,2) = 0.0298;
    //NINTENDO
    LET(mu,3) = 0.0052;
    //TOKYO ELECTRIC
    LET(mu,4) = 0.0211;
    //APPLE
    LET(mu,5) = 0.0190;
    //ALTADIS
    LET(mu,6) = 0.0216;
    //ASTRA
    LET(mu,7) = 0.0332;
    //CA
    LET(mu,8) = 0.0509;
    //CASINO
    LET(mu,9) = 0.0640;
    //DEUTSCHE TELEKOM
    LET(mu,10) = 0.0353;
    //EUROPEAN
    LET(mu,11) = 0.0132;
    //ENI SPA
    LET(mu,12) = 0.0539;
    //FRANCE TELECOM
    LET(mu,13) = 0.0256;
    //GAS NATURAL
    LET(mu,14) = 0.0396;
    //INTENSA
    LET(mu,15) = 0.0591;
    //MEDIASET SPA
    LET(mu,16) = 0.0190;
    //PFIZER
    LET(mu,17) = 0.0403;
    //RENTOKIL
    LET(mu,18) = 0.0158;
    //RWE
    LET(mu,19) = 0.0187;
    //VINCI
    LET(mu,20) = 0.0304;
    //STORA
    LET(mu,21) = 0.0328;
    //TUI
    LET(mu,22) = 0.0469;
    //VALEO SA
    LET(mu,23) = 0.0187;
    //VERIZON INC
    LET(mu,24) = 0.0438;
}

void Utils::sigma(PnlVect *sigma) {
    //TOKYO GAS
    LET(sigma,0) = 0.10;
    //KIRIN BREWERY
    LET(sigma,1) = 0.095;
    //TOYOTA
    LET(sigma,2) = 0.065;
    //NINTENDO
    LET(sigma,3) = 0.105;
    //TOKYO ELECTRIC
    LET(sigma,4) = 0.091;
    //APPLE
    LET(sigma,5) = 0.110;
    //ALTADIS
    LET(sigma,6) = 0.068;
    //ASTRA
    LET(sigma,7) = 0.087;
    //CA
    LET(sigma,8) = 0.102;
    //CASINO
    LET(sigma,9) = 0.104;
    //DEUTSCHE TELEKOM
    LET(sigma,10) = 0.074;
    //EUROPEAN
    LET(sigma,11) = 0.054;
    //ENI SPA
    LET(sigma,12) = 0.063;
    //FRANCE TELECOM
    LET(sigma,13) = 0.12;
    //GAS NATURAL
    LET(sigma,14) = 0.078;
    //INTENSA
    LET(sigma,15) = 0.092;
    //MEDIASET SPA
    LET(sigma,16) = 0.085;
    //PFIZER
    LET(sigma,17) = 0.108;
    //RENTOKIL
    LET(sigma,18) = 0.099;
    //RWE
    LET(sigma,19) = 0.111;
    //VINCI
    LET(sigma,20) = 0.088;
    //STORA
    LET(sigma,21) = 0.096;
    //TUI
    LET(sigma,22) = 0.086;
    //VALEO SA
    LET(sigma,23) = 0.084;
    //VERIZON INC
    LET(sigma,24) = 0.077;
}


void Utils::mapsDevises(std::map<int,int>& devises) {
    double YJP = 1;
    double USD = 1;
    double EUR = 1;
    devises[0] =  YJP;
    devises[1] =  YJP;
    devises[2] =  YJP;
    devises[3] =  YJP;
    devises[4] =  YJP;
    devises[5] =  YJP;
    devises[6] =  YJP;
    devises[7] =  YJP;
    devises[8] =  YJP;
    devises[9] =  YJP;
    devises[10] =  YJP;
    devises[11] =  YJP;
    devises[12] =  YJP;
    devises[13] =  YJP;
    devises[14] =  YJP;
    devises[15] =  YJP;
    devises[16] =  YJP;
    devises[17] =  YJP;
    devises[18] =  YJP;
    devises[19] =  YJP;
    devises[20] =  YJP;
    devises[21] =  YJP;
    devises[22] =  YJP;
    devises[23] =  YJP;
    devises[24] =  YJP;
}
