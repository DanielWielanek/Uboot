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
	Int_t fNEvents;
	Bool_t fRemove;
	void GenerateInput();
	void GenerateRunQMD();
	void RunQMD();
public:
	UrQMDCall(Int_t nevents=1, Bool_t remove = kTRUE);
	virtual ~UrQMDCall();
	ClassDef(UrQMDCall,1)
};

#endif /* SRC_URQMDCALL_H_ */
