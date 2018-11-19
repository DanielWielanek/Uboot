/*
 * UPdgConvert.h
 *
 *  Created on: 17 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UPDGCONVERT_H_
#define SRC_UPDGCONVERT_H_
#include <TObject.h>
#include <TDatabasePDG.h>
#include <TParticlePDG.h>
#include <TSystem.h>
#include <fstream>
#include <iostream>
#include "UParticle.h"
#include "UItyp.h"
#include "UDecayParticle.h"

class UPdgConvert : public TObject{
	static UPdgConvert *fgInstance;
	TDatabasePDG *fPDG;
	std::map<Int_t,UItyp> fPDG2Ityp;  //! table with pdg
	std::map<UItyp,Int_t> fItyp2PDG;
	std::map<Int_t,UDecayParticle> fPDG2Decay;
	Int_t fIsoSpin[150];
	Char_t GetQuarkCode(Char_t q, Bool_t anti);
	TString GetQuarks(Int_t pdt);
	Int_t GetQuarkCharge(Char_t)const;
	Int_t CalcI3(Int_t pdg);
	Int_t CalcS(Int_t pdg);
	UItyp Pdg2Ityp(Int_t pid);
	void AddDummyParticles();
public:
	UPdgConvert();
	static  UPdgConvert* Instance();
	Bool_t Stable(Int_t pdg_code);
	Int_t Ityp2Pdg(UItyp pid);
	Int_t GetCharge(Int_t pdg_code);
	Int_t Status(Int_t pdg_code);
	Int_t DecayParticle(UParticle *mother,TClonesArray *array,Int_t shift);
	void Pdg2U(Int_t pdg_code, Int_t &ityp, Int_t &ichg, Int_t &i3);
	virtual ~UPdgConvert();
	ClassDef(UPdgConvert,1)
};

#endif /* SRC_UPDGCONVERT_H_ */
