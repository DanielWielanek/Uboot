/*
 * U2U.h
 *
 *  Created on: 11 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef U2U_H_
#define U2U_H_
#define UQMD_VER 3.2
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
/**
 * class for conversion Unigen to UrQMD files
 */
class U2U : public TObject{
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
	Int_t fCTO[60];
	UFile *fTrashFile;
	UFile *fInputFile;
	UPdgConvert *fPDG;
	UEvent *fEvent;
	UEvent *fUrQMDEvent;
	UEvent *fEventTrash;
	TString fFilename;
	Int_t fTimeFlag;
	Int_t fMaxEvents;
	Int_t fStatus;
	Bool_t fUseStatus;
	Double_t fFreezoutTime;
	TH1D *fFreezoutHisto;
	Double_t *fTau;
	Int_t fTauSize;
	void SetCTOs();
	void ReadUnigen();
	void WriteUrQMD();
	Double_t EstimateTime();
	void Interpolate(Double_t t_min);
	TString Format(Double_t val);
public:
	/**
	 * @param name of Unigen file, files are converted to u2boot_temp/test_U2boot"
	 */
	U2U(TString name="");
	/**
	 *
	 * @param n number of events to conversion
	 */
	void SetNEvents(Int_t n){fMaxEvents = n;};
	/**
	 * set flag that define time of "transition into UrQMD"
	 * @param flag  0 - time of creation of the first particle
	 * 1 - use freezout time form @SetFreezoutTime
	 * 2 - use avarage value
	 * 3 - use last particle's freezout time
	 */
	void SetTimeFlag(Int_t flag){fTimeFlag = flag;};
	/**
	 * if true then status of particles is used, in such case all particles with different status than given here
	 * are not passed to UrQMD
	 * @param stat
	 */
	void SetStatus(Int_t stat){fUseStatus = kTRUE;fStatus = stat;};
	/**
	 * set freezout time (time of transition to UrQMD)
	 * @param freez_t
	 */
	void SetFreezoutTime(Double_t freez_t){fFreezoutTime = freez_t;};
	/**
	 * convert files
	 */
	void Convert();
	/**
	 * get number of events to conversion
	 * @return
	 */
	Int_t GetEvents()const{return fMaxEvents;};
	virtual ~U2U();
	ClassDef(U2U,1)
};

#endif /* U2U_H_ */
