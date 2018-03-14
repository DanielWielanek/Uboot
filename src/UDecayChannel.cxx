/*
 * UDecayChannel.cxx
 *
 *  Created on: 25 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UDecayChannel.h"
#include <iostream>
UDecayChannel::UDecayChannel() :fDaughters(0),fBranchRatio(0),fPdgCodes(NULL),fMass(NULL){
}

void UDecayChannel::AddDaughter(Int_t pdg) {
	Int_t *temp = fPdgCodes;
	Double_t *temp2 = fMass;
	fPdgCodes = new Int_t[fDaughters+1];
	fMass = new Double_t[fDaughters+1];
	for(int i=0;i<fDaughters;i++){
		fPdgCodes[i] = temp[i];
		fMass[i] = temp2[i];
	}
	if(temp)delete []temp;
	if(temp2)delete []temp2;
	fPdgCodes[fDaughters] = pdg;
	fMass[fDaughters] = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();
	++fDaughters;
}

UDecayChannel UDecayChannel::operator =(const UDecayChannel& other) {
	if(fPdgCodes) delete []fPdgCodes;
	fDaughters = other.fDaughters;
	fBranchRatio = other.fBranchRatio;
	if(fDaughters){
		fPdgCodes = new Int_t[fDaughters];
		fMass = new Double_t[fDaughters];
		for(int i=0;i<fDaughters;i++){
			fPdgCodes[i] = other.fPdgCodes[i];
			fMass[i] = other.fMass[i];
		}
	}
	return *this;
}

UDecayChannel::~UDecayChannel() {
	if(fPdgCodes){
		delete []fPdgCodes;
	}
}

UDecayChannel* UDecayChannel::MakeCopy() const{
	UDecayChannel *copy = new UDecayChannel();
	copy->fDaughters = this->fDaughters;
	copy->fBranchRatio = this->fBranchRatio;
	copy->fPdgCodes = new Int_t[fDaughters];
	copy->fMass = new Double_t[fDaughters];
	for(int i=0;i<fDaughters;i++){
		copy->fMass[i] = fMass[i];
		copy->fPdgCodes[i] = fPdgCodes[i];
	}
	return copy;
}
