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
    //USD
    LET(mu,25) = 0.0039;
    //GBP
    LET(mu,26) = 0.0028;
    //JPY
    LET(mu,27) = 0.0021; 
}

void Utils::sigma(PnlVect *sigma) {
    //TOKYO GAS
    LET(sigma,0) = 0.2410;
    //KIRIN BREWERY
    LET(sigma,1) = 0.24095;
    //TOYOTA
    LET(sigma,2) = 0.24065;
    //NINTENDO
    LET(sigma,3) = 0.24105;
    //TOKYO ELECTRIC
    LET(sigma,4) = 0.237091;
    //APPLE
    LET(sigma,5) = 0.239110;
    //ALTADIS
    LET(sigma,6) = 0.23968;
    //ASTRA
    LET(sigma,7) = 0.24087;
    //CA
    LET(sigma,8) = 0.239102;
    //CASINO
    LET(sigma,9) = 0.24104;
    //DEUTSCHE TELEKOM
    LET(sigma,10) = 0.24074;
    //EUROPEAN
    LET(sigma,11) = 0.24054;
    //ENI SPA
    LET(sigma,12) = 0.24063;
    //FRANCE TELECOM
    LET(sigma,13) = 0.2412;
    //GAS NATURAL
    LET(sigma,14) = 0.24078;
    //INTENSA
    LET(sigma,15) = 0.24092;
    //MEDIASET SPA
    LET(sigma,16) = 0.24085;
    //PFIZER
    LET(sigma,17) = 0.24108;
    //RENTOKIL
    LET(sigma,18) = 0.2399;
    //RWE
    LET(sigma,19) = 0.24111;
    //VINCI
    LET(sigma,20) = 0.2388;
    //STORA
    LET(sigma,21) = 0.2396;
    //TUI
    LET(sigma,22) = 0.2386;
    //VALEO SA
    LET(sigma,23) = 0.2384;
    //VERIZON INC
    LET(sigma,24) = 0.2377;
    //USD
    LET(sigma,25) = 0.00850;
    //GBP
    LET(sigma,26) = 0.00745;
    //JPY
    LET(sigma,27) = 0.008; 
}


void Utils::mapsDevises(std::map<int,double>& devises) {
    double JPY = 3;
    double GBP = 2;
    double USD = 1;
    double EUR = 0;
    devises[0] =  JPY;
    devises[1] =  JPY;
    devises[2] =  JPY;
    devises[3] =  JPY;
    devises[4] =  JPY;
    devises[5] =  USD;
    devises[6] =  GBP;
    devises[7] =  GBP;
    devises[8] =  EUR;
    devises[9] =  EUR;
    devises[10] =  EUR;
    devises[11] =  EUR;
    devises[12] =  EUR;
    devises[13] =  EUR;
    devises[14] =  EUR;
    devises[15] =  EUR;
    devises[16] =  EUR;
    devises[17] =  USD;
    devises[18] =  GBP;
    devises[19] =  EUR;
    devises[20] =  EUR;
    devises[21] =  EUR;
    devises[22] =  EUR;
    devises[23] =  EUR;
    devises[24] =  USD;
}
