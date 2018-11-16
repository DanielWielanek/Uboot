/*
 * UMerger.h
 *
 *  Created on: 18 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UMERGER_H_
#define SRC_UMERGER_H_
#include <TObject.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include <TSystem.h>
#include <TDatabasePDG.h>
#include <iostream>
#include <fstream>
#include <map>
#include <TTree.h>
#include <TFile.h>

#include "UItyp.h"
#include "UPdgConvert.h"
#include "UEvent.h"
#include "UParticle.h"
/**
 * open u2boot_temp/test.f13 and convert to unigen file, then merge this file with
 * u2boot_temp/trash.root (tracks that could't be passed into UrQMD)
 */
class UMerger : public TObject {
	std::ifstream fUrQMDFile;
	UEvent *fEvent;
	UPdgConvert *fPDG;
	TClonesArray *fDecayedArray;//[1000]
	Bool_t fDecay;
	void ReadUrQMD();
	void ReadAfterburner(Int_t event);
	void DecayEvent();
public:
	enum eUMergerMode{
		kUrQMD,
		kAfterburner
	};
	UMerger(TString outFile="data.root", Bool_t decay=kTRUE, eUMergerMode=kUrQMD);
	virtual ~UMerger();
	ClassDef(UMerger,1)
};

#endif /* SRC_UMERGER_H_ */
