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
#include <iostream>
#define DEBUG
UDecayParticle::UDecayParticle(Int_t motherPDG):fMass(0),fGamma(0),fBranchRatio(NULL),fMotherPDG(motherPDG) {
	fDecayChannelN = 0;
	fDecays = NULL;
	fMass = TDatabasePDG::Instance()->GetParticle(motherPDG)->Mass();
}

void UDecayParticle::ScaleDecays() {
	fMass = TDatabasePDG::Instance()->GetParticle(fMotherPDG)->Mass();
	Double_t total = 0;
	for(int i=0;i<fDecayChannelN;i++){
		UDecayChannel *decay = fDecays[i];
		total +=decay->GetBranchingRatio();
	}
	if(fBranchRatio) delete []fBranchRatio;
	fBranchRatio = new Double_t[fDecayChannelN];
	Double_t tot_sum = 0;
	for(int i=0;i<fDecayChannelN;i++){
		UDecayChannel *decay = fDecays[i];
		tot_sum +=decay->GetBranchingRatio()/total;
		fBranchRatio[i] = tot_sum;
	}
	fBranchRatio[fDecayChannelN-1] = 1.1;//just to be sure :)
}

Double_t UDecayParticle::BreitWigner(Double_t mass) {
	  Double_t x,y;
	  y = gRandom->Rndm();
	  x = fMass + fGamma/2 * TMath::Tan( TMath::Pi() * (y - 0.5) );
	  return x;
}

void UDecayParticle::AddDecayChannel(UDecayChannel* dec) {
	UDecayChannel **temp = fDecays;
	fDecays = new UDecayChannel*[fDecayChannelN+1];
	for(int i=0;i<fDecayChannelN;i++){
		fDecays[i] = temp[i];
	}
	fDecays[fDecayChannelN]=dec;
	fDecayChannelN++;
	delete []temp;
}

UDecayParticle& UDecayParticle::operator =(const UDecayParticle& other) {
	if(&other==this) return *this;
	fMotherPDG= other.fMotherPDG;
	fDecayChannelN = other.fDecayChannelN;
	if(fDecays) delete []fDecays;
	if(fBranchRatio)delete []fBranchRatio;
	fDecays = new UDecayChannel*[fDecayChannelN];
	fBranchRatio = new Double_t[fDecayChannelN];
	fMass = other.fMass;
	fGamma = other.fGamma;
	for(int i=0;i<fDecayChannelN;i++){
		fBranchRatio[i] = other.fBranchRatio[i];
		fDecays[i] = other.fDecays[i]->MakeCopy();
	}
	return *this;
}

UDecayParticle::~UDecayParticle() {
	if(fDecays){
		for(int i=0;i<fDecayChannelN;i++){
			delete fDecays[i];
		}
		delete []fDecays;
	}
	if(fBranchRatio)
	delete []fBranchRatio;
}

UDecayParticle::UDecayParticle(const UDecayParticle& other) {
	fMass = other.fMass;
	fGamma = other.fGamma;
	fDecayChannelN = other.fDecayChannelN;
	fDecays = new UDecayChannel*[fDecayChannelN];
	fBranchRatio = new Double_t[other.fDecayChannelN];
	fMotherPDG = other.fMotherPDG;
	for(int i=0;i<other.fDecayChannelN;i++){
		UDecayChannel *channel =  other.fDecays[i];
		UDecayChannel *copy = channel->MakeCopy();
		fDecays[i] = copy;
		fBranchRatio[i] = other.fBranchRatio[i];
	}
}

bool UDecayParticle::operator <(const UDecayParticle& other) const {
	return fMotherPDG<other.fMotherPDG;
}

Int_t UDecayParticle::Decay(UParticle* mother, TClonesArray* daughter, Int_t shift) {
	UDecayChannel *channel = GetRandomChannel();
	Int_t daughters = channel->GetDaughterNo();
	Int_t size = daughter->GetEntriesFast();
	switch(daughters){
	case 2:{
		Decay2Body(mother,daughter,channel, shift);
#ifdef DEBUG
			Double_t M = mother->GetMomentum().M();
			UParticle *dau1 = (UParticle*)daughter->UncheckedAt(size);
			UParticle *dau2 = (UParticle*)daughter->UncheckedAt(size+1);
			TLorentzVector sum = dau1->GetMomentum()+dau2->GetMomentum();
			if(TMath::Abs(sum.M()-M)>1E-5){
				std::cout<<"!! 2body decay "<<mother->GetPdg()<<" - >\t"<<M<<" "<<sum.M()<<std::endl;
			}
#endif
		return 2;
	}break;
	case 3:{
		Decay3Body(mother,daughter,channel,shift);
#ifdef DEBUG
		Double_t M = mother->GetMomentum().M();
		UParticle *dau1 = (UParticle*)daughter->UncheckedAt(size);
		UParticle *dau2 = (UParticle*)daughter->UncheckedAt(size+1);
		UParticle *dau3 = (UParticle*)daughter->UncheckedAt(size+2);
		TLorentzVector sum = dau1->GetMomentum()+dau2->GetMomentum()+ dau3->GetMomentum();
		if(TMath::Abs(sum.M()-M)>1E-5){
			std::cout<<"!! 3body decay "<<mother->GetPdg()<<" - >\t"<<M<<" "<<sum.M()<<std::endl;
		}
#endif
		return 3;
	}break;
	default:{
		return 0;
	}break;
	}
}

void UDecayParticle::Decay2Body(UParticle* mother, TClonesArray* daughters, UDecayChannel *channel, Int_t shift) {
	Double_t tTime;
	TLorentzVector mP = mother->GetMomentum();
	TLorentzVector mX  = mother->GetPosition();
	Int_t pdg1 = channel->GetDaughterPDG(0);
	Int_t pdg2 = channel->GetDaughterPDG(1);
	Double_t m1 =channel->GetDaughterMass(0);
	Double_t m2 = channel->GetDaughterMass(1);
	Double_t mM = mother->GetMomentum().M();
	Double_t M =mother->GetMomentum().M();//<<tBreitWigner( m1+m2);
	if(fGamma==0){
		tTime = 1E+10;
	}else{
		//M = BreitWigner(m1+m2);
		do{
		M = BreitWigner(mM);
		}while((m1+m2)>M);
	    double tTau0 = mP.E() / (mP.M() * fGamma);
	    // When it decays
	    tTime = -tTau0 * TMath::Log(gRandom->Rndm());
	}
	//Decay posistion
	TVector3 boost = mP.BoostVector();
	Double_t X = mX.X()+boost.X()*tTime;
	Double_t Y = mX.Y()+boost.Y()*tTime;
	Double_t Z = mX.Z()+boost.Z()*tTime;
	Double_t T = mX.T()+tTime;

	Double_t tMC1 = (M * M - (m1 + m2) * (m1 + m2) );
	Double_t tMC2 = (M * M - (m1 - m2) * (m1 - m2) );
	Double_t tMom = TMath::Sqrt(tMC1 * tMC2) / (2 * M);

	Double_t px1,py1,pz1;
	Double_t px2, py2, pz2;
	gRandom->Sphere(px1,py1,pz1,tMom);
	px2 = -px1;
	py2 = -py1;
	pz2 = -pz1;
	TLorentzVector dau1,dau2;
	dau1.SetPxPyPzE(px1,py1,pz1,TMath::Sqrt(tMom*tMom+m1*m1));
	dau2.SetPxPyPzE(px2,py2,pz2,TMath::Sqrt(tMom*tMom+m2*m2));
	dau1.Boost(boost);
	dau2.Boost(boost);
	Int_t size = daughters->GetEntriesFast();
	UParticle *daughter1 = (UParticle*)daughters->ConstructedAt(size);
	UParticle *daughter2 = (UParticle*)daughters->ConstructedAt(size+1);
	daughter1->SetParent(mother->GetIndex());
	daughter2->SetParent(mother->GetIndex());
	daughter1->SetPdg(pdg1);
	daughter2->SetPdg(pdg2);
	daughter1->SetIndex(shift+size);
	daughter2->SetIndex(shift+size+1);
	daughter1->SetMomentum(dau1);
	daughter2->SetMomentum(dau2);
	daughter1->SetPosition(X,Y,Z,T);
	daughter2->SetPosition(X,Y,Z,T);
	daughter1->SetStatus(2);
	daughter2->SetStatus(2);
	mother->SetDecay(1);
	mother->SetFirstChild(daughter1->GetIndex());
	mother->SetLastChild(daughter2->GetIndex());
}

void UDecayParticle::Decay3Body(UParticle* mother, TClonesArray* daughters, UDecayChannel *channel, Int_t shift) {

	  Double_t tE  = mother->E();
	  Double_t tM1 = channel->GetDaughterMass(0);
	  Double_t tM2 = channel->GetDaughterMass(1);
	  Double_t tM3 = channel->GetDaughterMass(2);

	  // Father mass via BreitWigner
	  Double_t tMthr = tM1 + tM2 + tM3;
	  Double_t mM = mother->GetMomentum().M();
	  Double_t tM  =0;//mother->GetMomentum().M();// BreitWigner(tMthr);
	if(fGamma!=0)
		do{
		tM = BreitWigner(mM);
		}while((tMthr)>tM);
		tM = BreitWigner(tM);
	  Double_t tES1, tES2, tP1, tP2, tCos12, tZ;
	  do {
	    // Generate E1 and E2 with the Monte-Carlo method
	    do {
	      tES1 = gRandom->Rndm() * (tM - tM2 - tM3 - tM1) + tM1;
	      tES2 = gRandom->Rndm() * (tM - tM1 - tM3 - tM2) + tM2;
	    } while (tES1+tES2 > tM); // The sum of both energies must be smaller than the resonance mass
	    tP1  = TMath::Sqrt(tES1*tES1 - tM1*tM1);
	    tP2  = TMath::Sqrt(tES2*tES2 - tM2*tM2);
	    tZ = tM - tES1 - tES2;
	    tZ *= tZ;
	    tCos12 = (tZ - tP1*tP1 - tP2*tP2 - tM3*tM3)/(2*tP1*tP2);
	  } while ((tCos12 < -1.0) || (tCos12 > 1.0)); // Cos Theta must exist (be within -1.0 to 1.0 )

	  Double_t tTime;
	  if (fGamma== 0.0)
	    tTime = 1.0e10;
	  else {
		 Double_t tTau0 = tE / (fMass * fGamma);
	    // When it decays
	    tTime = -tTau0 * TMath::Log(gRandom->Rndm());
	  }
	  TLorentzVector momP = mother->GetMomentum();
	  TLorentzVector momX  =mother->GetPosition();
	// Decay coordinates
	  Double_t X = momX.X() + (momP.Px()/tE)*tTime;
	  Double_t Y = momX.Y()  + (momP.Py()/tE)*tTime;
	  Double_t Z = momX.Z()  + (momP.Pz()/tE)*tTime;
	  Double_t T = momX.T()  + tTime;

	  Double_t tPxr2 = tP2 * TMath::Sqrt(1-tCos12*tCos12);
	  Double_t tPzr2 = tP2*tCos12;
	  Double_t tPxr3 = - tPxr2;
	  Double_t tPzr3 = - (tP1 + tPzr2);
	  Double_t tP3 = TMath::Hypot(tPxr3, tPzr3);
	  Double_t tES3 = TMath::Hypot(tM3, tP3);

	  // Generating Euler angles
	  Double_t tPhi = gRandom->Rndm() * 2 * TMath::Pi();
	  Double_t tKsi = gRandom->Rndm() * 2 * TMath::Pi();
	  Double_t tCosTh = gRandom->Rndm() * 2.0 - 1.0;

	  Double_t sp = TMath::Sin(tPhi);
	  Double_t cp = TMath::Cos(tPhi);
	  Double_t sk = TMath::Sin(tKsi);
	  Double_t ck = TMath::Cos(tKsi);
	  Double_t st = TMath::Sqrt(1.0-tCosTh*tCosTh);
	  Double_t ct = tCosTh;

	  // Rotating the whole system
	  Double_t tPxp1 = - st*ck * tP1;
	  Double_t tPyp1 = st*sk * tP1;
	  Double_t tPzp1 = ct * tP1;

	  Double_t tPxp2 = (cp*ct*ck - sp*sk)  * tPxr2 + (-st*ck) * tPzr2;
	  Double_t tPyp2 = (-cp*ct*sk - sp*ck) * tPxr2 + (st*sk)  * tPzr2;
	  Double_t tPzp2 = cp*st               * tPxr2 + ct       * tPzr2;

	  Double_t tPxp3 = (cp*ct*ck - sp*sk)  * tPxr3 + (-st*ck) * tPzr3;
	  Double_t tPyp3 = (-cp*ct*sk - sp*ck) * tPxr3 + (st*sk)  * tPzr3;
	  Double_t tPzp3 = cp*st               * tPxr3 + ct       * tPzr3;
	  TVector3 boost = momP.BoostVector();

	  tES1 = TMath::Sqrt(tM1*tM1 + tPxp1*tPxp1 + tPyp1*tPyp1 + tPzp1*tPzp1);
	  tES2 = TMath::Sqrt(tM2*tM2 + tPxp2*tPxp2 + tPyp2*tPyp2 + tPzp2*tPzp2);
	  tES3 = TMath::Sqrt(tM3*tM3 + tPxp3*tPxp3 + tPyp3*tPyp3 + tPzp3*tPzp3);

	  TLorentzVector p1,p2,p3;
	  p1.SetPxPyPzE(tPxp1,tPyp1,tPzp1,tES1);
	  p2.SetPxPyPzE(tPxp2,tPyp2,tPzp2,tES2);
	  p3.SetPxPyPzE(tPxp3,tPyp3,tPzp3,tES3);
	  p1.Boost(boost);
	  p2.Boost(boost);
	  p3.Boost(boost);
	  Int_t size = daughters->GetEntriesFast();
	  UParticle *daughter1 = (UParticle*)daughters->ConstructedAt(size);
	  UParticle *daughter2 = (UParticle*)daughters->ConstructedAt(size+1);
	  UParticle *daughter3 = (UParticle*)daughters->ConstructedAt(size+2);
	  daughter1->SetParent(mother->GetIndex());
	  daughter2->SetParent(mother->GetIndex());
	  daughter3->SetParent(mother->GetIndex());
	  daughter1->SetPdg(channel->GetDaughterPDG(0));
	  daughter2->SetPdg(channel->GetDaughterPDG(1));
	  daughter3->SetPdg(channel->GetDaughterPDG(2));
	  daughter1->SetMomentum(p1);
	  daughter2->SetMomentum(p2);
	  daughter3->SetMomentum(p3);
	  daughter1->SetPosition(X,Y,Z,T);
	  daughter2->SetPosition(X,Y,Z,T);
	  daughter3->SetPosition(X,Y,Z,T);
	  daughter1->SetIndex(shift+size);
	  daughter2->SetIndex(shift+size+1);
	  daughter3->SetIndex(shift+size+2);
	  daughter1->SetStatus(2);
	  daughter2->SetStatus(2);
	  daughter3->SetStatus(2);
	  mother->SetFirstChild(daughter1->GetIndex());
	  mother->SetLastChild(daughter3->GetIndex());
}

UDecayChannel* UDecayParticle::GetRandomChannel() const {
	Double_t randval = gRandom->Rndm();
	Int_t channel = 0;
	while(randval>fBranchRatio[channel]){
		channel++;
	}
	return fDecays[channel];
}
