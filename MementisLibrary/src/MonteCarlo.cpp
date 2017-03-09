#include <stdio.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include "MonteCarlo.hpp"
#include "Produit.hpp"
#include "ConstantRateModel.hpp"

using namespace std;

MonteCarlo::MonteCarlo() {
    this->mod_= new BlackScholesModel();
    this->fdStep_ = 0.0;
    this->nbSamples_ = 0;
}


MonteCarlo::~MonteCarlo() {
}


MonteCarlo::MonteCarlo(BlackScholesModel *bsModel, Produit* produit, PnlRng* pnlRng, double steps, int nbSamples) {
    this->mod_= bsModel;
    this->pdt_ = produit;
    this->rng_ = pnlRng;
    this->fdStep_ = steps;
    this->nbSamples_ = nbSamples;
}

void MonteCarlo::price(double &prix, double &ic) {
    //RateModel* interet = mod_->parameters->r_;
    double maturite = pdt_->T_;
    double termeExp = exp(-mod_->parameters->r_->integrateRate(0,maturite));
    double sommePayOff = 0;
    double sommePayOffCarre = 0;
    PnlMat *path = pnl_mat_create(pdt_->nbTimeSteps_ + 1, mod_->parameters->size_);
    // Application de Montecarlo
    for (int i = 0; i< nbSamples_; i++) {
        mod_->asset(path, rng_);
        double res =  pdt_->payoff(path);
        // on somme les payoffs pour faire la moyenne
        sommePayOff += res;
        // On somme le carré des payoffs pour calculer la variance
        sommePayOffCarre += res*res;
    }

    double M = (double) nbSamples_;
    double moyenne = sommePayOff / M;
    double variance = (sommePayOffCarre/M - moyenne*moyenne);
    //Calcule de l'intervalle de confiance pour le prix
    ic = sqrt(variance/M) * 2.0 * 1.96 * termeExp;
    prix = termeExp * moyenne;
    pnl_mat_free(&path);
}

// Sans ic
void MonteCarlo::price(const PnlMat* past, double t, double& prix) {
    double maturite = pdt_->T_;
    double termeExp = exp(-mod_->parameters->r_->integrateRate(t,maturite));
    double sommePayOff = 0;

    PnlMat *path = pnl_mat_create(pdt_->nbTimeSteps_ + 1,mod_->parameters->size_);
    for (int i = 0; i < nbSamples_; i++) {
        //On simule le path à partir de t
        mod_->asset(path, t, past, rng_);
        sommePayOff += pdt_->payoff(path);
    }
    prix = termeExp * sommePayOff / (double)nbSamples_;
    // pnl_mat_free(&path);
    //delete interet;
}

// Avec ic
void MonteCarlo::price(const PnlMat* past, double t, double& prix, double& ic) {

    if (t < 0) {
		cerr << "La date de pricing est négative!" << endl;
    } else if (t > pdt_->T_) {
        cerr << "La date de pricing est supérieure à la maturité du produit!" << endl;
    } else {
        double maturite = pdt_->T_;
        double termeExp = exp(-mod_->parameters->r_->integrateRate(t,maturite));
        double sommePayOff = 0;
        double sommePayOffCarre = 0;
        // on crée le path
        PnlMat *path = pnl_mat_create(pdt_->nbTimeSteps_ + 1,mod_->parameters->size_);
        for (int i = 0; i < nbSamples_; i++) {
            //On simule le path à partir de t
            mod_->asset(path, t, past, rng_);
            double res =  pdt_->payoff(path);
            sommePayOff += res;
            sommePayOffCarre += res*res;
        }
		double M = (double)nbSamples_;
    double moyenne = sommePayOff / M;
		double variance = termeExp * (sommePayOffCarre/M - moyenne*moyenne);
		ic = sqrt(variance / M) * 2.0 * 1.96 * termeExp;
    prix = termeExp * moyenne;
       // pnl_mat_free(&path);
        //delete interet;
    }
}

void MonteCarlo::affichagePrice(double currentDate, double &prix, double &icPrix) {
  cout << "---------------- Prix et delta a t = " << currentDate << " -----------------" << endl;
  cout << "Prix a t : " << prix << " - Intervalle de confiance : " << icPrix << endl;
}


void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta) {
      double M = (double)nbSamples_;
      //RateModel * interet = mod_->parameters->r_ ;
      double maturite = pdt_->T_;
      double facteurExp;

      double expo = exp(-mod_->parameters->r_->integrateRate(t,maturite));

      // On crée les vecteurs qui vont contenir Le payoff+ et le payoff-
      PnlMat *shift_path_up = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);
      PnlMat *shift_path_down = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);
      PnlMat *path = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);

      double sommeDiffPayOff = 0;

      // On fait le calcul M fois (MonteCarlo)
      for(int i=0; i < M; i++) {
          // On simule le path
          mod_->asset(path, t, past, rng_);
          for (int j = mod_->parameters->mapDevises.size() ; j < mod_->parameters->size_; j++){
            
          }
          // Pour chaque asset on fait le calcul correspondant
          for (int idAsset=0; idAsset < mod_->parameters->size_; idAsset++) {
            // On fait le shift avec fdStep
            mod_->shiftAsset(shift_path_up, path, idAsset, fdStep_, t , (maturite / (double)pdt_->nbTimeSteps_));
            // On fait le shift avec -fdStep
            mod_->shiftAsset(shift_path_down, path, idAsset, -fdStep_, t , (maturite / (double)pdt_->nbTimeSteps_));
            // On calcule la difference des payOff shiftés
            sommeDiffPayOff = pdt_->payoff(shift_path_up) - pdt_->payoff(shift_path_down);
            // On met le resultat dans le vecteur delta
            LET(delta,idAsset) = GET(delta,idAsset) + sommeDiffPayOff;
          }
      }

      // On reparcourt le vecteur delta pour faire la moyenne et multiplier par le facteur correspondant
      for (int idAsset=0; idAsset < mod_->parameters->size_; idAsset++) {
          double st = pnl_mat_get(past, (past->m - 1), idAsset);
          facteurExp = expo/(M*2*st*fdStep_);
          LET(delta,idAsset) = GET(delta,idAsset)*facteurExp;
      }

      pnl_mat_free(&shift_path_up);
      pnl_mat_free(&shift_path_down);
      pnl_mat_free(&path);
      //delete interet;
}




void MonteCarlo::delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *ic) {

    if (t < 0) {
        cerr << "La date de pricing est négative!" << endl;
    } else if (t > pdt_->T_) {
        cerr << "La date de pricing est supérieure à la maturité du produit!" << endl;
    } else {
        double M = (double)nbSamples_;
        //RateModel* interet = mod_->parameters->r_ ;
        double maturite = pdt_->T_;
        double tempdelta = 0;
        double factor;
        double expo = exp(-mod_->parameters->r_->integrateRate(t,maturite));
        // On crée les vecteurs qui vont contenir Le payoff+ et le payoff-
        PnlMat *shift_path_up = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);
        PnlMat *shift_path_down = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);

        PnlMat *path = pnl_mat_create(pdt_->nbTimeSteps_ +1, mod_->parameters->size_);
        double sommeDiffPayOff = 0;
        PnlVect *sommeDiffPayOffSquared = pnl_vect_create_from_zero(mod_->parameters->size_);
        // On fait le calcul M fois (MonteCarlo)
        for(int i=0; i < M; i++) {
            // On simule le path
			mod_->asset(path, t, past, rng_);
            for (int idAsset=0; idAsset < mod_->parameters->size_; idAsset++) {
                // Pour chaque asset on fait le calcul correspondant
                // On fait le shift avec fdStep et -fdStep
                mod_->shiftAsset(shift_path_up, path, idAsset, fdStep_, t , (maturite / (double)pdt_->nbTimeSteps_));
                mod_->shiftAsset(shift_path_down, path, idAsset, -fdStep_, t , (maturite / (double)pdt_->nbTimeSteps_));

                sommeDiffPayOff = pdt_->payoff(shift_path_up) - pdt_->payoff(shift_path_down);
                // On met le resultat dans le vecteur delta
                LET(delta,idAsset) = GET(delta,idAsset) + sommeDiffPayOff;

                // On met le calcul du resultat au carree dans le vecteur ic (pour les ICs)
                LET(sommeDiffPayOffSquared,idAsset) = GET(sommeDiffPayOffSquared,idAsset) + sommeDiffPayOff*sommeDiffPayOff;
            }
        }

        // On reparcourt le vecteur delta et le vecteur ic pour compléter les calculs (moyenne et facteurs)
        for (int idAsset=0; idAsset < mod_->parameters->size_; idAsset++) {
            factor = 1/(2*pnl_mat_get(past, (past->m - 1), idAsset)*fdStep_);
            double carreUn = GET(sommeDiffPayOffSquared,idAsset);
            double carreDeux = GET(delta,idAsset);
            LET(delta,idAsset) = GET(delta,idAsset)*expo*factor/M;
            LET(ic,idAsset) = expo*factor*sqrt((carreUn/M - (carreDeux*carreDeux)/(M*M))/M)*1.96;

        }
        pnl_mat_free(&shift_path_up);
        pnl_mat_free(&shift_path_down);
        //pnl_mat_free(&path);
        pnl_vect_free(&sommeDiffPayOffSquared);
        //delete interet;
    }
}

void MonteCarlo::affichageDelta(PnlVect *delta, PnlVect *icDelta) {
  cout << "-----------------------------------------------------" << endl;
  cout << "\t  Delta\t\t|\tlargeur ic Delta\t" << endl;
  cout << "-----------------------------------------------------" << endl;
  for (int i = 0; i < delta->size; i++) {
    cout << "\t" << GET(delta, i) << "\t|\t" << GET(icDelta, i) << "\t" << endl;
  }
}

// Calcul le pnl (à maturité), les valeurs du ptf de couverture,les prix du produit
// Toutes les datas sont générées, et on commence à partir de 0
void MonteCarlo::profitAndLoss(PnlVect *V, double &PnL, PnlVect *prixPdt) {
    PnlMat *marketPath = pnl_mat_create(mod_->parameters->hedgingDateNb_ + 1, mod_->parameters->size_);
    mod_->simulMarket(marketPath, rng_, true);
    profitAndLoss(0, V, NULL, PnL, prixPdt, marketPath);
}


// Calcul le pnl (à maturité), les valeurs du ptf de couverture,les prix du produit
// On commence à partir de currentDate
void MonteCarlo::profitAndLoss(double currentDate, PnlVect *currentDateSpot, PnlVect *V, double &PnL, PnlVect *prixPdt, PnlMat* marketPath) {
    double valeur;
    double prix;
    PnlVect *deltaPrec = pnl_vect_create(mod_->parameters->size_);
    PnlVect *deltaCourant = pnl_vect_create(mod_->parameters->size_);
    PnlVect *Stauxi = pnl_vect_create(mod_->parameters->size_);
    PnlMat *pastcopy = pnl_mat_new();

    int nbDatePast = 1;
    double freqHedging = mod_->maturity_ / mod_->parameters->hedgingDateNb_;
    double cpt = currentDate;
    while (cpt>0) {
      cpt -= freqHedging;
      nbDatePast++;
    }
    double currentDateCut = (cpt==0)?nbDatePast:nbDatePast-1;

    double pas = pdt_->T_ / (double) mod_->parameters->hedgingDateNb_;
    double pasConstatation = pdt_->T_ / (double) pdt_->nbTimeSteps_;
    PnlVect *sub = pnl_vect_create(mod_->parameters->size_);
    int cptConstatation = floor(currentDate) + 1;

    // Initialisation du portefeuille pour t = currentDate
    PnlMat *past = pnl_mat_create(cptConstatation, mod_->parameters->size_);
    for (int i=0; i<cptConstatation; i++) {
        pnl_mat_get_row(sub,marketPath, i*2);
        pnl_mat_set_row(past,sub, i);
    }
    pnl_mat_get_row(Stauxi, past, (past->m - 1));

    double prixPrec;
    price(past, currentDate, prixPrec);
    delta(past, currentDate, deltaPrec);
    if(currentDateCut == nbDatePast) {
        prix = prixPrec;
    } else {
        PnlMat *pastFirstStep = pnl_mat_create(cptConstatation+1, mod_->parameters->size_);
        pnl_mat_set_subblock(pastFirstStep,past,0,0);
        pnl_mat_set_row(pastFirstStep, currentDateSpot, cptConstatation);
        price(pastFirstStep, currentDate, prix);
        delta(pastFirstStep, currentDate, deltaPrec);
        pnl_mat_free(&pastFirstStep);
    }

    double valeurPrec = prixPrec - pnl_vect_scalar_prod(deltaPrec,Stauxi);
    double valPortefeuilleCouverture = prix;
    LET(V,0) = valPortefeuilleCouverture;
    LET(prixPdt,0) =  prix;
    int cptHedge = 1;

    // Calcul de la valeur du portefeuille à chaque date de rebalancement
    for (int j = currentDateCut; j < mod_->parameters->hedgingDateNb_ + 1; j++) {
        if ((pas*j >= pasConstatation*cptConstatation)) {
            pastcopy = pnl_mat_copy(past);
            pnl_mat_resize(past, (past->m + 1), mod_->parameters->size_);
            pnl_mat_set_subblock(past,pastcopy,0,0);
            cptConstatation++;
        }
        pnl_mat_get_row(sub,marketPath,j);
        pnl_mat_set_row(past,sub,(past->m - 1));

        //Calcul du prix du produit et des deltas
        price(past,pas*j,prix);
        delta(past,pas*j,deltaCourant);
        pnl_mat_get_row(Stauxi,past,(past->m - 1));

        // Calcul nouvelle valeur du portefeuille
        valPortefeuilleCouverture = valeurPrec * exp((mod_->parameters->r_->integrateRate(pas*j,pas*(j+1)))) + pnl_vect_scalar_prod(deltaPrec,Stauxi);

        valeur = valPortefeuilleCouverture - pnl_vect_scalar_prod(deltaCourant,Stauxi);
        valeurPrec = valeur;
        LET(V,cptHedge) = valPortefeuilleCouverture;
        LET(prixPdt, cptHedge) = prix;

        // Mise a jour des vecteurs deltas pour la suite des calculs
        deltaPrec = pnl_vect_copy(deltaCourant);
        cptHedge++;
    }
    // Calcul du PnL
    PnL = GET(V, cptHedge-1) + pnl_vect_scalar_prod(deltaCourant,Stauxi) - prix;//V->Size


    // On free les objets alloués
    pnl_vect_free(&deltaCourant);
    pnl_vect_free(&deltaPrec);
    pnl_vect_free(&Stauxi);
    pnl_vect_free(&sub);
    pnl_mat_free(&past);
    pnl_mat_free(&pastcopy);
}

void MonteCarlo::affichagePnL(PnlVect *V, double &PnL, PnlVect *prixPdt) {
  cout << "-----------------------------------------------------" << endl;
  cout << "   Valeur portefeuille\t|\tValeur produit\t" << endl;
  cout << "-----------------------------------------------------" << endl;
  for (int i = 0; i < V->size; i++) {
    cout << "\t" << GET(V, i) << "\t\t|\t" << GET(prixPdt, i) << "\t" << endl;
  }
  cout << endl << " PnL : " << PnL << endl;
}

void MonteCarlo::GenerateFile(PnlVect* data, std::ofstream ofs) {
    for (int i = 0; i < data->size; i++) {
        ofs << GET(data,i) << " ";
    }
    ofs.close();
}


/* CODE AFFICHAGE */
/*
ofstream fichierCouv("../dataPortefeuilleCouverture.txt", ios::out | ios::trunc);
ofstream fichierProd("../dataProduit.txt", ios::out | ios::trunc);
ofstream fichierDate("../dataDate.txt", ios::out | ios::trunc);

for (int i = 0; i < prixPdt->size + 1 ; i++) {
    fichierCouv << GET(V,i) << "\n";
    fichierProd << GET(prixPdt,i) << "\n";
    fichierDate << i * pdt_->T_ / (double)mod_->parameters->hedgingDateNb_ << "\n";
}
fichierDate.close();
fichierCouv.close();
fichierProd.close();

//GenerateFile(valPortefeuilleCouv,fichierCouv);
//GenerateFile(valProduit,fichierProd);
*/
