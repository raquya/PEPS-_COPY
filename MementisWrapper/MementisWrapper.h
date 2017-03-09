using namespace System;
using namespace MementisLibrary;

namespace MementisWrapper {
	
	public ref class WrapperPriceAndHedge
	{
	private:
		dataPriceAndHedgeOut *ref;

	public:
		// Accesseurs
		int getNbPdtCouv();
		array<double>^ getDelta();
		array<double>^ getIcDelta();
		double getPrix();
		double getIcPrix();

		void priceAndHedgeAtCurrentDate(double currentDate, array<double>^ past, int nbSamples, bool isHistorical);
		//void priceAndHedgeAtCurrentDate(double currentDate, array<double, 2>^ pastWrapped, int nbSamples, array<double>^ muWrapped, array<double>^ sigmaWrapped, double correlationWrapped);
	};

	public ref class WrapperPnL
	{
	private:
		dataPnLOut *ref;

	public:
		// Accesseurs
		int getNbHedgingDate();
		array<double>^ getValPtfCouv();
		array<double>^ getPrixPdt();
		double getPnL();

		void PnL(int nbHedgingDate, double historicalDate, double currentDate, array<double>^ past, int nbSamples, bool isHistorical);
		//void PnL(double currentDate, array<double, 2>^ pastWrapped, int nbSamples, array<double>^ muWrapped, array<double>^ sigmaWrapped, double correlationWrapped);
	};

	public ref class WrapperTest
	{
	public:
		double testWrapper(double m);
	};
}
