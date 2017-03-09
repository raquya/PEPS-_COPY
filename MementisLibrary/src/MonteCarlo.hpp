#pragma once

#include <iostream>
#include <fstream>
#include "Produit.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"


class MonteCarlo
{
public:
    BlackScholesModel *mod_; /*! pointeur vers le modèle */
    Produit *pdt_; /*! pointeur sur le produit */
    PnlRng *rng_; /*! pointeur sur le générateur */
    double fdStep_; /*! pas de différence finie */
    int nbSamples_; /*! nombre de tirages Monte Carlo */

    /*!
     * \brief Constructeur par defaut
     */
    MonteCarlo();

    /*!
     * \brief Constructeur
     */
    MonteCarlo(BlackScholesModel *bsModel, Produit* produit, PnlRng* pnlRng, double steps, int nbSamples);



    /*
    * Destructeur
    */
    virtual ~MonteCarlo();


    /**
     * Calcule le prix du produit à la date 0
     *
     * @param[out] prix valeur de l'estimateur Monte Carlo
     * @param[out] ic largeur de l'intervalle de confiance
     */
    void price(double &prix, double &ic);

    /**
     * Calcule le prix du produit à la date t avec ic
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     * @param[out] ic contient la largeur de l'intervalle
     * de confiance sur le calcul du prix
     */
    void price(const PnlMat *past, double t, double &prix, double &ic);

    /**
     * Calcule le prix du produit à la date t sans ic
     *
     * @param[in]  past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] prix contient le prix
     */
    void price(const PnlMat* past, double t, double& prix);

    /**
     * Calcule le delta du produit à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * de confiance sur le calcul du delta
     */
    void delta(const PnlMat *past, double t, PnlVect *delta);


    /**
     * Calcule le delta du produit à la date t
     *
     * @param[in] past contient la trajectoire du sous-jacent
     * jusqu'à l'instant t
     * @param[in] t date à laquelle le calcul est fait
     * @param[out] delta contient le vecteur de delta
     * de confiance sur le calcul du delta
     * @param[out] ic vecteur qui contient la moitié de la largeur
     *de l'intervalle de confiance sur le calcul des deltas
     */
    void delta(const PnlMat *past, double t, PnlVect *delta, PnlVect *ic);

    void affichagePrice(double currentDate, double &prix, double &icPrix);
    void affichageDelta(PnlVect *delta, PnlVect *icDelta);

    // Calcul le pnl (à maturité), les valeurs du ptf de couverture,les prix du produit
    // Toutes les datas sont générées, et on commence à partir de 0
    void profitAndLoss(PnlVect *V, double &PnL, PnlVect *prixPdt);

    // Calcul le pnl (à maturité), les valeurs du ptf de couverture,les prix du produit
    // On commence à partir de currentDate
    void profitAndLoss(double currentDate, PnlVect *currentDateSpot, PnlVect *V, double &PnL, PnlVect *prixPdt, PnlMat* Market);

    void affichagePnL(PnlVect *V, double &PnL, PnlVect *prixPdt);

    void GenerateFile(PnlVect* data, std::ofstream ofs);
};
