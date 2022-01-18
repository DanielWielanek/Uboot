/*
 * UDecayParticle.h
 *
 *  Created on: 25 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UDECAYPARTICLE_H_
#define SRC_UDECAYPARTICLE_H_

#include "UDecayChannel.h"
#include "UParticle.h"
#include <TClonesArray.h>
#include <TDatabasePDG.h>
#include <TObject.h>
#define USE_BREIT_WIGNER
/**
 * class for decaying particles
 */
class UDecayParticle : public TObject {
  Float_t fMass, fGamma;
  Int_t fMotherPDG;
  Int_t fDecayChannelN;
  std::vector<UDecayChannel> fDecays;  //[fDecayChannelN]
  std::vector<Double_t> fBranchRatio;  //[fDecayChannelN]
  Double_t BreitWigner(Double_t minMass) const;
  Double_t CalcDecayTime(UParticle* mother) const;
  const UDecayChannel& GetRandomChannel() const;
  void Decay2Body(UParticle* mother, TClonesArray* daughters, const UDecayChannel& channel, Int_t shift);
  void Decay3Body(UParticle* mother, TClonesArray* daughters, const UDecayChannel& channel, Int_t shift);

public:
  /**
   * basic constructor
   * @param motherPDG mother PDG
   */
  UDecayParticle(Int_t motherPDG = 0);
  /**
   * copy constructor
   * @param other
   */
  UDecayParticle(const UDecayParticle& other) = default;
  /**
   * assignement operator
   * @param other
   * @return
   */
  UDecayParticle& operator=(const UDecayParticle& other) = default;
  virtual bool operator<(const UDecayParticle& other) const;
  /**
   * normalize branching ratio
   */
  void ScaleDecays();
  /**
   *
   * @return mother PDG
   */
  Int_t GetPdg() const { return fMotherPDG; };
  /**
   * decay particle
   * @param mother mother particle to decay
   * @param daughters array with daugher (daughters and granddaughters of this particles are added at end of this array)
   * @param shift number of primary particles in event (to be sure that daughter index number will correspond to it position
   * in array of tracks
   * @return number of daughers
   */
  Int_t Decay(UParticle* mother, TClonesArray* daughters, Int_t shift);
  /**
   *
   * @param mother
   * @return estimated decay time of particle, return 1+E34 if stable
   */
  Double_t GetRandomDecayTime(UParticle* mother) const;
  /**
   * add decay channel
   * @param dec
   */
  void AddDecayChannel(const UDecayChannel dec);
  /**
   *
   * @return numbe of decay channels
   */
  Int_t GetDecayChannels() const { return fDecayChannelN; };
  /**
   *
   * @param gamma mother  gamma
   */
  void SetGamma(Float_t gamma) { fGamma = gamma; }
  /**
   *
   * @return mother gamma
   */
  Float_t GetGamma() const { return fGamma; }
  /**
   * @return mass of mother
   */
  Float_t GetMass() const { return fMass; }
  /**
   *
   * @param mass mothe mass
   */
  void SetMass(Float_t mass) { fMass = mass; }
  /**
   *
   * @param pdg mother PDG
   */
  void SetPdg(Int_t pdg) { fMotherPDG = pdg; }
  /**
   * print decay chanell info
   */
  void Print() const;
  virtual ~UDecayParticle();
  ClassDef(UDecayParticle, 1)
};

#endif /* SRC_UDECAYPARTICLE_H_ */
