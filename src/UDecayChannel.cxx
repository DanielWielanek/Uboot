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
UDecayChannel::UDecayChannel() : fDaughters(0), fBranchRatio(0) {}

void UDecayChannel::AddDaughter(Int_t pdg) {
  fPdgCodes.push_back(pdg);
  fMass.push_back(TDatabasePDG::Instance()->GetParticle(pdg)->Mass());
  fDaughters = fMass.size();
}

UDecayChannel UDecayChannel::operator=(const UDecayChannel& other) {
  if (&other != this) {
    fPdgCodes  = other.fPdgCodes;
    fMass      = other.fMass;
    fDaughters = other.fDaughters;
  }
  return *this;
}

UDecayChannel::UDecayChannel(const UDecayChannel& other) {
  fDaughters   = other.fDaughters;
  fBranchRatio = other.fBranchRatio;
  fDaughters   = other.fDaughters;
}

UDecayChannel::~UDecayChannel() {}
