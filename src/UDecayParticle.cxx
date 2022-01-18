/*
 * UDecayParticle.cxx
 *
 *  Created on: 25 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UDecayParticle.h"
#include "TRandom.h"
#include "UItyp.h"
#include <iostream>
//#define DEBUG
UDecayParticle::UDecayParticle(Int_t motherPDG) : fMass(0), fGamma(0), fMotherPDG(motherPDG), fDecayChannelN(0) {
  fMass = TDatabasePDG::Instance()->GetParticle(motherPDG)->Mass();
}

void UDecayParticle::ScaleDecays() {
  fMass          = TDatabasePDG::Instance()->GetParticle(fMotherPDG)->Mass();
  Double_t total = 0;
  for (int i = 0; i < fDecayChannelN; i++) {
    total += fDecays[i].GetBranchingRatio();
  }
  Double_t tot_sum = 0;
  for (int i = 0; i < fDecayChannelN; i++) {
    tot_sum += fDecays[i].GetBranchingRatio() / total;
    fBranchRatio[i] = tot_sum;
  }
  fBranchRatio[fDecayChannelN - 1] = 1.1;  // just to be sure :)
}

Double_t UDecayParticle::BreitWigner(Double_t minMass) const {
#ifdef USE_BREIT_WIGNER
  Double_t x = 0, y;
  do {
    y = gRandom->Rndm();
    x = fMass + fGamma / 2 * TMath::Tan(TMath::Pi() * (y - 0.5));
  } while (x < minMass);
  return x;
#else
  return fMass;
#endif
}

void UDecayParticle::AddDecayChannel(const UDecayChannel dec) {
  fDecays.push_back(dec);
  fBranchRatio.push_back(0);
  fDecayChannelN++;
}

Double_t UDecayParticle::GetRandomDecayTime(UParticle* mother) const {
  if (GetDecayChannels() == 0) return 1e+34;  // stable particle
  return CalcDecayTime(mother);
}

void UDecayParticle::Print() const {
  std::cout << fMotherPDG << std::endl;
  for (int i = 0; i < fDecayChannelN; i++) {
    std::cout << " => " << fBranchRatio[i] << std::endl;
    for (int j = 0; j < fDecays[i].GetDaughterNo(); j++) {
      std::cout << "    " << fDecays[i].GetDaughterPDG(j) << std::endl;
    }
  }
}

Double_t UDecayParticle::CalcDecayTime(UParticle* mother) const {
  TLorentzVector mP = mother->GetMomentum();
  if (fGamma == 0) { return 1E+10; }
  double tTau0 = mP.E() / (fMass * fGamma);
  // When it decays
  return -tTau0 * TMath::Log(gRandom->Rndm());
}

UDecayParticle::~UDecayParticle() {}

bool UDecayParticle::operator<(const UDecayParticle& other) const { return fMotherPDG < other.fMotherPDG; }

Int_t UDecayParticle::Decay(UParticle* mother, TClonesArray* daughter, Int_t shift) {
  const UDecayChannel& channel = GetRandomChannel();
  Int_t daughters              = channel.GetDaughterNo();
#ifdef DEBUG
  Int_t size = daughter->GetEntriesFast();
#endif
  switch (daughters) {
    case 2: {
      Decay2Body(mother, daughter, channel, shift);
#ifdef DEBUG
      Double_t M         = mother->GetMomentum().M();
      UParticle* dau1    = (UParticle*) daughter->UncheckedAt(size);
      UParticle* dau2    = (UParticle*) daughter->UncheckedAt(size + 1);
      TLorentzVector sum = dau1->GetMomentum() + dau2->GetMomentum();
      if (TMath::Abs(sum.M() - M) > 1E-5) {
        std::cout << "!! 2body decay " << size << " " << size + 1 << " " << mother->GetPdg() << " - >\t" << M << " " << sum.M()
                  << std::endl;
      }
      std::cout << mother->GetFirstChild() << " " << mother->GetLastChild() << std::endl;
#endif
      return 2;
    } break;
    case 3: {
      Decay3Body(mother, daughter, channel, shift);
#ifdef DEBUG
      Double_t M         = mother->GetMomentum().M();
      UParticle* dau1    = (UParticle*) daughter->UncheckedAt(size);
      UParticle* dau2    = (UParticle*) daughter->UncheckedAt(size + 1);
      UParticle* dau3    = (UParticle*) daughter->UncheckedAt(size + 2);
      TLorentzVector sum = dau1->GetMomentum() + dau2->GetMomentum() + dau3->GetMomentum();
      if (TMath::Abs(sum.M() - M) > 1E-5) {
        std::cout << "!! 3body decay " << mother->GetPdg() << " - >\t" << M << " " << sum.M() << std::endl;
      }
#endif
      return 3;
    } break;
    default: {
      return 0;
    } break;
  }
}

void UDecayParticle::Decay2Body(UParticle* mother, TClonesArray* daughters, const UDecayChannel& channel, Int_t shift) {
  Double_t tTime;
  TLorentzVector mP = mother->GetMomentum();
  TLorentzVector mX = mother->GetPosition();
  Int_t pdg1        = channel.GetDaughterPDG(0);
  Int_t pdg2        = channel.GetDaughterPDG(1);
  Double_t m1       = channel.GetDaughterMass(0);
  Double_t m2       = channel.GetDaughterMass(1);
  Double_t M        = BreitWigner(m1 + m2);
  tTime             = CalcDecayTime(mother);
  // Decay posistion
  TVector3 boost = mP.BoostVector();
  Double_t X     = mX.X() + boost.X() * tTime;
  Double_t Y     = mX.Y() + boost.Y() * tTime;
  Double_t Z     = mX.Z() + boost.Z() * tTime;
  Double_t T     = mX.T() + tTime;

  Double_t tMC1 = (M * M - (m1 + m2) * (m1 + m2));
  Double_t tMC2 = (M * M - (m1 - m2) * (m1 - m2));
  Double_t tMom = TMath::Sqrt(tMC1 * tMC2) / (2 * M);

  Double_t px1, py1, pz1;
  Double_t px2, py2, pz2;
  gRandom->Sphere(px1, py1, pz1, tMom);
  px2 = -px1;
  py2 = -py1;
  pz2 = -pz1;
  TLorentzVector dau1, dau2;
  dau1.SetPxPyPzE(px1, py1, pz1, TMath::Sqrt(tMom * tMom + m1 * m1));
  dau2.SetPxPyPzE(px2, py2, pz2, TMath::Sqrt(tMom * tMom + m2 * m2));
  dau1.Boost(boost);
  dau2.Boost(boost);
  Int_t size           = daughters->GetEntriesFast();
  UParticle* daughter1 = (UParticle*) daughters->ConstructedAt(size);
  UParticle* daughter2 = (UParticle*) daughters->ConstructedAt(size + 1);
  daughter1->SetParent(mother->GetIndex());
  daughter2->SetParent(mother->GetIndex());
  daughter1->SetPdg(pdg1);
  daughter2->SetPdg(pdg2);
  daughter1->SetIndex(shift + size);
  daughter2->SetIndex(shift + size + 1);
  daughter1->SetMomentum(dau1);
  daughter2->SetMomentum(dau2);
  daughter1->SetPosition(X, Y, Z, T);
  daughter2->SetPosition(X, Y, Z, T);
  daughter1->SetStatus(STATUS_DECAY);
  daughter2->SetStatus(STATUS_DECAY);
  mother->SetDecay(1);
  mother->SetFirstChild(daughter1->GetIndex());
  mother->SetLastChild(daughter2->GetIndex());
}

void UDecayParticle::Decay3Body(UParticle* mother, TClonesArray* daughters, const UDecayChannel& channel, Int_t shift) {

  Double_t tE  = mother->E();
  Double_t tM1 = channel.GetDaughterMass(0);
  Double_t tM2 = channel.GetDaughterMass(1);
  Double_t tM3 = channel.GetDaughterMass(2);

  // Father mass via BreitWigner
  Double_t tM = BreitWigner(tM1 + tM2 + tM3);
  Double_t tES1, tES2, tP1, tP2, tCos12, tZ;
  do {
    // Generate E1 and E2 with the Monte-Carlo method
    do {
      tES1 = gRandom->Rndm() * (tM - tM2 - tM3 - tM1) + tM1;
      tES2 = gRandom->Rndm() * (tM - tM1 - tM3 - tM2) + tM2;
    } while (tES1 + tES2 > tM);  // The sum of both energies must be smaller than the resonance mass
    tP1 = TMath::Sqrt(tES1 * tES1 - tM1 * tM1);
    tP2 = TMath::Sqrt(tES2 * tES2 - tM2 * tM2);
    tZ  = tM - tES1 - tES2;
    tZ *= tZ;
    tCos12 = (tZ - tP1 * tP1 - tP2 * tP2 - tM3 * tM3) / (2 * tP1 * tP2);
  } while ((tCos12 < -1.0) || (tCos12 > 1.0));  // Cos Theta must exist (be within -1.0 to 1.0 )

  Double_t tTime      = CalcDecayTime(mother);
  TLorentzVector momP = mother->GetMomentum();
  TLorentzVector momX = mother->GetPosition();
  // Decay coordinates
  Double_t X = momX.X() + (momP.Px() / tE) * tTime;
  Double_t Y = momX.Y() + (momP.Py() / tE) * tTime;
  Double_t Z = momX.Z() + (momP.Pz() / tE) * tTime;
  Double_t T = momX.T() + tTime;

  Double_t tPxr2 = tP2 * TMath::Sqrt(1 - tCos12 * tCos12);
  Double_t tPzr2 = tP2 * tCos12;
  Double_t tPxr3 = -tPxr2;
  Double_t tPzr3 = -(tP1 + tPzr2);
  Double_t tP3   = TMath::Hypot(tPxr3, tPzr3);
  Double_t tES3  = TMath::Hypot(tM3, tP3);

  // Generating Euler angles
  Double_t tPhi   = gRandom->Rndm() * 2 * TMath::Pi();
  Double_t tKsi   = gRandom->Rndm() * 2 * TMath::Pi();
  Double_t tCosTh = gRandom->Rndm() * 2.0 - 1.0;

  Double_t sp = TMath::Sin(tPhi);
  Double_t cp = TMath::Cos(tPhi);
  Double_t sk = TMath::Sin(tKsi);
  Double_t ck = TMath::Cos(tKsi);
  Double_t st = TMath::Sqrt(1.0 - tCosTh * tCosTh);
  Double_t ct = tCosTh;

  // Rotating the whole system
  Double_t tPxp1 = -st * ck * tP1;
  Double_t tPyp1 = st * sk * tP1;
  Double_t tPzp1 = ct * tP1;

  Double_t tPxp2 = (cp * ct * ck - sp * sk) * tPxr2 + (-st * ck) * tPzr2;
  Double_t tPyp2 = (-cp * ct * sk - sp * ck) * tPxr2 + (st * sk) * tPzr2;
  Double_t tPzp2 = cp * st * tPxr2 + ct * tPzr2;

  Double_t tPxp3 = (cp * ct * ck - sp * sk) * tPxr3 + (-st * ck) * tPzr3;
  Double_t tPyp3 = (-cp * ct * sk - sp * ck) * tPxr3 + (st * sk) * tPzr3;
  Double_t tPzp3 = cp * st * tPxr3 + ct * tPzr3;
  TVector3 boost = momP.BoostVector();

  tES1 = TMath::Sqrt(tM1 * tM1 + tPxp1 * tPxp1 + tPyp1 * tPyp1 + tPzp1 * tPzp1);
  tES2 = TMath::Sqrt(tM2 * tM2 + tPxp2 * tPxp2 + tPyp2 * tPyp2 + tPzp2 * tPzp2);
  tES3 = TMath::Sqrt(tM3 * tM3 + tPxp3 * tPxp3 + tPyp3 * tPyp3 + tPzp3 * tPzp3);

  TLorentzVector p1, p2, p3;
  p1.SetPxPyPzE(tPxp1, tPyp1, tPzp1, tES1);
  p2.SetPxPyPzE(tPxp2, tPyp2, tPzp2, tES2);
  p3.SetPxPyPzE(tPxp3, tPyp3, tPzp3, tES3);
  p1.Boost(boost);
  p2.Boost(boost);
  p3.Boost(boost);
  Int_t size           = daughters->GetEntriesFast();
  UParticle* daughter1 = (UParticle*) daughters->ConstructedAt(size);
  UParticle* daughter2 = (UParticle*) daughters->ConstructedAt(size + 1);
  UParticle* daughter3 = (UParticle*) daughters->ConstructedAt(size + 2);
  daughter1->SetParent(mother->GetIndex());
  daughter2->SetParent(mother->GetIndex());
  daughter3->SetParent(mother->GetIndex());
  daughter1->SetPdg(channel.GetDaughterPDG(0));
  daughter2->SetPdg(channel.GetDaughterPDG(1));
  daughter3->SetPdg(channel.GetDaughterPDG(2));
  daughter1->SetMomentum(p1);
  daughter2->SetMomentum(p2);
  daughter3->SetMomentum(p3);
  daughter1->SetPosition(X, Y, Z, T);
  daughter2->SetPosition(X, Y, Z, T);
  daughter3->SetPosition(X, Y, Z, T);
  daughter1->SetIndex(shift + size);
  daughter2->SetIndex(shift + size + 1);
  daughter3->SetIndex(shift + size + 2);
  daughter1->SetStatus(STATUS_DECAY);
  daughter2->SetStatus(STATUS_DECAY);
  daughter3->SetStatus(STATUS_DECAY);
  mother->SetFirstChild(daughter1->GetIndex());
  mother->SetLastChild(daughter3->GetIndex());
}

const UDecayChannel& UDecayParticle::GetRandomChannel() const {
  Double_t randval = gRandom->Rndm();
  Int_t channel    = 0;
  while (randval > fBranchRatio[channel]) {
    channel++;
  }
  return fDecays[channel];
}
