#include "Produit.hpp"
#include "FlowCalculator.hpp"

class ProduitMementis : public Produit
{
public:
	ProduitMementis();

	ProduitMementis(FlowCalculator* flow, double maturity, int nbTimeSteps_, int nbAssets_ , int nbConstationDates, RateModel* interest);

	virtual ~ProduitMementis();

	FlowCalculator* flowCalculator;
	int nbAssets_;
	int nbConstationDates;
	double PayOffReel;

	double payoff(const PnlMat *path);
};