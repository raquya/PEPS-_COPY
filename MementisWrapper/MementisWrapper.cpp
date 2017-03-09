#include "stdafx.h"
#include <math.h>  
#include "MementisWrapper.h"

using namespace MementisLibrary;

namespace MementisWrapper {

	/* ---------------------------- TEST ---------------------------- */
	double WrapperTest::testWrapper(double m) {
		return MementisLibrary::testLibrary(m);
	}


	/* ----------------------- PriceAndHedge ------------------------ */
	void WrapperPriceAndHedge::priceAndHedgeAtCurrentDate(double currentDate, array<double>^ past, int nbSamples, bool isHistorical) {
		int nbActions = 25;
		double* tmp = nullptr; // To change
		int nbDatePast = ceil(currentDate) + 1;
		double** pastWrapped = nullptr;
		if (isHistorical) {
			pastWrapped = new double*[nbDatePast];
			for (int i = 0; i < nbDatePast; i++) {
				pastWrapped[i] = new double[nbActions];
				for (int j = 0; j < nbActions; j++) {
					pastWrapped[i][j] = past[i * nbActions + j];
				}
			}
		}
	
		this->ref = MementisLibrary::priceAndHedgeAtCurrentDate(currentDate, pastWrapped, isHistorical, nbSamples, tmp, tmp);

		if (isHistorical) {
			for (int i = 0; i < nbDatePast; i++) {
				delete[] pastWrapped[i];
			}
			delete[] pastWrapped;
		}
	}

	int WrapperPriceAndHedge::getNbPdtCouv() {
		return this->ref->nbPdtCouv;
	}

	array<double>^ WrapperPriceAndHedge::getDelta() {
		int size = this->ref->nbPdtCouv;
		array<double>^ tabManaged = gcnew array<double>(size*sizeof(double));
		System::Runtime::InteropServices::Marshal::Copy(IntPtr((void *)this->ref->delta), tabManaged, 0, size);
		return tabManaged;
	}

	array<double>^ WrapperPriceAndHedge::getIcDelta() {
		int size = this->ref->nbPdtCouv;
		array<double>^ tabManaged = gcnew array<double>(size*sizeof(double));
		System::Runtime::InteropServices::Marshal::Copy(IntPtr((void *)this->ref->icDelta), tabManaged, 0, size);
		return tabManaged;
	}

	double WrapperPriceAndHedge::getPrix() {
		return this->ref->prix;
	}

	double WrapperPriceAndHedge::getIcPrix() {
		return this->ref->icPrix;
	}


	/* ----------------------------- PnL ----------------------------- */
	void WrapperPnL::PnL(int nbHedgingDate, double historicalDate, double currentDate, array<double>^ past, int nbSamples, bool isHistorical) {
		int nbActions = 25;
		double* tmp1 = nullptr; // To change
		double** tmp2 = nullptr; // To change

		int nbDatePast = ceil(currentDate) + 1;
		double** pastWrapped = nullptr;
		if (isHistorical) {
			pastWrapped = new double*[nbDatePast];
			for (int i = 0; i < nbDatePast; i++) {
				pastWrapped[i] = new double[nbActions];
				for (int j = 0; j < nbActions; j++) {
					pastWrapped[i][j] = past[i * nbActions + j];
				}
			}
		}
		
		this->ref = MementisLibrary::computePnL(nbHedgingDate, historicalDate, currentDate, pastWrapped, isHistorical, nbSamples, tmp1, tmp1);
		
		if (isHistorical) {
			for (int i = 0; i < nbDatePast; i++) {
				delete[] pastWrapped[i];
			}
			delete[] pastWrapped;
		}
	
	}

	int WrapperPnL::getNbHedgingDate() {
		return this->ref->nbHedgingDate;
	}

	array<double>^ WrapperPnL::getValPtfCouv() {
		int size = this->ref->nbHedgingDate;
		array<double>^ tabManaged = gcnew array<double>(size*sizeof(double));
		System::Runtime::InteropServices::Marshal::Copy(IntPtr((void *)this->ref->valPtfCouv), tabManaged, 0, size);
		return tabManaged;
	}

	array<double>^ WrapperPnL::getPrixPdt() {
		int size = this->ref->nbHedgingDate;
		array<double>^ tabManaged = gcnew array<double>(size*sizeof(double));
		System::Runtime::InteropServices::Marshal::Copy(IntPtr((void *)this->ref->prixPdt), tabManaged, 0, size);
		return tabManaged;
	}

	double WrapperPnL::getPnL() {
		return this->ref->pnl;
	}

}





/* Code de conversion :

// ********** CODE 1 **********
array<int>^ to int*, you can do following:
array<int>^ arr;
cli::pin_ptr<int> pArrayElement = &arr[0];

pin_ptr<int> p = &arr[0];   // pin pointer to first element in arr
int* np = p;   // pointer to the first element in arr


// ********** CODE 2  **********
array<int> ^ arrayManaged = gcnew array<int>(5);
int size = Marshal::SizeOf(arrayManaged[0]) * arrayManaged->Length;

IntPtr pnt = Marshal::AllocHGlobal(size);

Marshal::Copy(arrayManaged, 0, pnt, arrayManaged->Length);
int* pintPtr = (int*)pnt.ToPointer();

//FAIRE CE QU'ON VEUT AVEC LE int*
Marshal::FreeHGlobal(pnt);



// ********** CODE 3  **********
PARAM : array<double, 2>^ past
int size = System::Runtime::InteropServices::Marshal::SizeOf(past[0,0]) * past->Length;
IntPtr pnt = System::Runtime::InteropServices::Marshal::AllocHGlobal(size);
System::Runtime::InteropServices::Marshal::Copy(past, 0, pnt, past->Length);
this->ref = MementisLibrary::simulation((double**)pnt.ToPointer());
System::Runtime::InteropServices::Marshal::FreeHGlobal(pnt);

*/