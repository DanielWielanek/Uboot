/*
 * UDecayChannel.h
 *
 *  Created on: 25 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UDECAYCHANNEL_H_
#define SRC_UDECAYCHANNEL_H_
#define max_Decays 3
#include <TObject.h>
#include <TDatabasePDG.h>
/**
 * class that represents decay channel
 */
class UDecayChannel: public TObject {
	Int_t *fPdgCodes;//[fDaughters]
	Double_t *fMass;//[fDaughters]
	Int_t fDaughters;
    Double_t fBranchRatio;
public:
	UDecayChannel();
	UDecayChannel(const UDecayChannel &other);
	UDecayChannel operator=(const UDecayChannel &other);
	/**
	 * add daughter to decay channel
	 * @param pdg daugheter pid code
	 */
	void AddDaughter(Int_t pdg);
	/**
	 * return daughter pdg
	 * @param id daugher index
	 * @return
	 */
	Int_t  GetDaughterPDG(Int_t id) const{return fPdgCodes[id];}
	/**
	 *
	 * @return number of daughters
	 */
	Int_t GetDaughterNo()const{return fDaughters;};
	/**
	 *
	 * @param id
	 * @return mass of given daughter
	 */
	Double_t GetDaughterMass(Int_t id)const{return fMass[id];};
	/**
	 *
	 * @return branch ratio of this decay
	 */
	Double_t GetBranchingRatio() const{return fBranchRatio;};
	/**
	 * set daughter params
	 * @param pdg
	 * @param id
	 */
	void   SetParticle(Int_t pdg,Int_t id){fPdgCodes[id]=pdg;};
	/**
	 * set branch ratio
	 * @param brRatio
	 */
	void   SetBranchingRatio(Double_t  brRatio){fBranchRatio = brRatio;};
	virtual ~UDecayChannel();
	ClassDef(UDecayChannel,1)
};

#endif /* SRC_UDECAYCHANNEL_H_ */
