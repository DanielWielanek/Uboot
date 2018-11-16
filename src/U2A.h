/*
 * U2A.h
 *
 *  Created on: 21 paź 2018
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_U2A_H_
#define SRC_U2A_H_

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
#include <TH1D.h>

#include "UItyp.h"
#include "UPdgConvert.h"
#include "UEvent.h"
#include "UParticle.h"
#include "UFile.h"

class U2A : public TObject{
private:
	enum kFlag{
		kMeson,
		kBaryon,
		kOther,
		kUnknown,
		kBad
	};
	std::ofstream fUrQMDFile;
	Int_t *fFlags;
	Int_t fFlagsSize;
	UFile *fTrashFile;
	UFile *fInputFile;
	UPdgConvert *fPDG;
	UEvent *fEvent;
	UEvent *fUrQMDEvent;
	UEvent *fEventTrash;
	TString fFilename;
	Int_t fStatus;
	Bool_t fUseStatus;
	void ReadUnigen();
	void WriteUrQMD();
	TString Format(Double_t val);
public:
	U2A(TString name);
	/**
	 * if true then status of particles is used, in such case all particles with different status than given here
	 * are not passed to UrQMD
	 * @param stat
	 */
	void SetStatus(Int_t stat){fUseStatus = kTRUE;fStatus = stat;};
	/**
	 * convert files
	 */
	void Convert(Int_t event);
	Int_t GetTotalEvents(){return fInputFile->GetEntries();};
	virtual ~U2A();
	ClassDef(U2A,1)
};

#endif /* SRC_U2A_H_ */
