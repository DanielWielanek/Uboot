/*
 * UParameters.h
 *
 *  Created on: 21 lis 2018
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UCONFIGURATIONPARAMS_H_
#define SRC_UCONFIGURATIONPARAMS_H_
#include "TObject.h"
#include "TString.h"

class UConfigurationParams : public TObject{
	Bool_t fRemoveTemp;
	Bool_t fNoDecay;
	Bool_t fAfterburner;
	Bool_t fDecayOnly;
	Bool_t fFeedDown;
	Bool_t fUseUrQMD;
	Bool_t fUseStatus;
	Int_t fTimeFlag;
	Int_t fNevents;
	Int_t fStatus;
	Double_t fTau;
	Int_t fUrQMDTime_out;
	Int_t fUrQMDTime_calc;
	Double_t fUrQMDTime_dt;
	TString fInputFile;
	TString fOutputFile;
	void ParCheck(TString par, TString flag,double &val)const;
public:
	UConfigurationParams(int argc=0, char *argv[]=NULL);
	void SetRemoveTemp(Bool_t removeTemp) {fRemoveTemp = removeTemp;}
	void PrintConfiguration()const;
	void SetNevents(Int_t ev){fNevents = ev;}
	Bool_t DecayOnly() const {return fDecayOnly;}
	Bool_t DontDecay() const {return fNoDecay;}
	Bool_t Decay()const{return !fNoDecay;}
	Bool_t RemoveTemp() const {return fRemoveTemp;}
	Bool_t FeedDown()const{return fFeedDown;};
	Bool_t UseUrQMD()const{return fUseUrQMD;}
	Bool_t UseStatus()const{return fUseStatus;};
	Int_t GetStatus() const {return fStatus;}
	Int_t GetTimeFlag() const {return fTimeFlag;}
	Int_t GetUrQMDTimeCalc() const {return fUrQMDTime_calc;}
	Int_t GetUrQMDTimeOut()const{return fUrQMDTime_out;}
	Int_t GetNevents() const {return fNevents;}
	Double_t GetUrQmdTimeDt() const {return fUrQMDTime_dt;}
	Double_t GetTau() const {return fTau;}
	TString GetInputFile() const {return fInputFile;}
	TString GetOutputFile() const {return fOutputFile;}
	virtual ~UConfigurationParams();
	ClassDef(UConfigurationParams,1)
};

#endif /* SRC_UCONFIGURATIONPARAMS_H_ */
