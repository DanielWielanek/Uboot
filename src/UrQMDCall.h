/*
 * UrQMDCall.h
 *
 *  Created on: 12 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_URQMDCALL_H_
#define SRC_URQMDCALL_H_
#include <TSystem.h>
#include <TObject.h>
#include <fstream>
#include <iostream>
#include <TROOT.h>
#include "U2U.h"
#include "UConfigurationParams.h"
/**
 * class for calling UrQMD
 */
class UrQMDCall : public TObject{
	Bool_t fRemove;
	Int_t fCalls;
	Int_t fCalculationTime;
	Int_t fOutputTime;
	Int_t fNEvents;
	Double_t fDeltaTime;
	U2U *fUrQMDWriter;
	void GenerateInput();
	void GenerateRunQMD(Int_t event);
	void RunQMD(Int_t event);
	void Print(TString text)const;
public:
	UrQMDCall(UConfigurationParams *params=NULL);
	void LoadConfiguration(UConfigurationParams *params);
	void SetOutputTime(Int_t time){fOutputTime =time;};
	void SetCalculationTime(Int_t time){fCalculationTime = time;};
	void SetDeltaTime(Double_t time){fDeltaTime = time;};
	void Convert();
	virtual ~UrQMDCall();
	ClassDef(UrQMDCall,1)
};

#endif /* SRC_URQMDCALL_H_ */
