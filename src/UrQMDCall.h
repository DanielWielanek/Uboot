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

/**
 * class for calling UrQMD
 */
class UrQMDCall : public TObject{
	Bool_t fRemove;
	Int_t fCalls;
	Int_t fCalculationTime;
	Int_t fOutputTime;
	Double_t fDeltaTime;
	void GenerateInput();
	void GenerateRunQMD(Int_t event);
	void RunQMD(Int_t event);
public:
	UrQMDCall(Bool_t remove = kTRUE);
	void SetOutputTime(Int_t time){fOutputTime =time;};
	void SetCalculationTime(Int_t time){fCalculationTime = time;};
	void SetDeltaTime(Double_t time){fDeltaTime = time;};
	void Convert(Int_t nevents);
	virtual ~UrQMDCall();
	ClassDef(UrQMDCall,1)
};

#endif /* SRC_URQMDCALL_H_ */
