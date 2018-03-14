/*
 * UPdgConvert.cxx
 *
 *  Created on: 17 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UPdgConvert.h"
UPdgConvert *UPdgConvert::fgInstance =NULL;
UPdgConvert::UPdgConvert() {
	fPDG = TDatabasePDG::Instance();
	AddDummyParticles();
	TString path = Form("%s/ityp.data",gSystem->Getenv("UBOOT"));
	std::ifstream* pdgconv = new std::ifstream(path.Data());
	Int_t ityp = 0;
	Int_t pdgId = 0;
	Int_t isospin = 0;
	while (!pdgconv->eof()) {
		ityp = isospin = pdgId = 0;
		*pdgconv >> pdgId>>isospin>>ityp;
		UItyp typ(ityp,isospin);
		fPDG2Ityp[pdgId] = typ;;//pgd ti ityp
		fItyp2PDG[typ]=pdgId;
	}
	pdgconv->close();
	delete pdgconv;
	path = Form("%s/decays.data",gSystem->Getenv("UBOOT"));

	std::ifstream* decays = new std::ifstream(path.Data());
	Int_t ndaughters;
	Double_t gamma,br;
	Int_t daughterspdg;
	Int_t count=0;
	while (!decays->eof()) {
		*decays>>pdgId>>gamma>>br>>ndaughters;
		UDecayParticle p;
		if(fPDG2Decay.find(pdgId)!=fPDG2Decay.end()){//such pdg already exist
			p = fPDG2Decay[pdgId];
		}else{
			p = UDecayParticle();
			p.SetPdg(pdgId);
			p.SetGamma(gamma);
		}

		UDecayChannel *decay_c = new UDecayChannel();
		decay_c->SetBranchingRatio(br);
		for(int i=0;i<ndaughters;i++){
			*decays>>daughterspdg;
			decay_c->AddDaughter(daughterspdg);
		}
		p.SetPdg(pdgId);
		p.AddDecayChannel(decay_c);

		fPDG2Decay[pdgId] =p;
		ityp = isospin = pdgId = 0;

	}

	  for (std::map<Int_t,UDecayParticle>::iterator it=fPDG2Decay.begin(); it!=fPDG2Decay.end(); ++it)
	     it->second.ScaleDecays();

	decays->close();
	delete decays;
	fgInstance = this;
}

UItyp UPdgConvert::Pdg2Ityp(Int_t pid) {
	if(fPDG2Ityp.find(pid)!=fPDG2Ityp.end()){
		return fPDG2Ityp[pid];
	}else{
		return UItyp(0,0);
	}
}

UPdgConvert* UPdgConvert::Instance() {
	if(fgInstance) return fgInstance;
	return new UPdgConvert();
}

Char_t UPdgConvert::GetQuarkCode(Char_t quark, Bool_t anti){
	switch(quark){
	case 1:
		quark = 'd';
		if(anti)quark='D';
		break;
	case 2:
		quark = 'u';
		if(anti)quark='U';
		break;
	case 3:
		quark = 's';
		if(anti)quark='S';
		break;
	case 4:
		quark = 'c';
		if(anti)quark='C';
		break;
	case 5:
		quark = 'b';
		if(anti)quark='B';
		break;
	case 6:
		quark = 't';
		if(anti)quark='T';
		break;
	default:
		quark = 'x';
		break;
	}
	return quark;
}

TString UPdgConvert::GetQuarks(Int_t pdg){
	if(TMath::Abs(pdg)>999999) return "";
	if(TMath::Abs(pdg)<=99) return "";
	TString name = Form("%i",pdg);
	Bool_t meson = kFALSE;
	if(name.Length()==3)
		meson = kTRUE;
	else if(name[name.Length()-4]=='0')
		meson=kTRUE;
	Bool_t anti = kFALSE;
	if(pdg<0){
		anti = kTRUE;
	}
	TString quarks = "";
	if(meson){//meson
		Char_t quarkA = name[name.Length()-3];
		Char_t quarkB = name[name.Length()-2];
		quarkA = GetQuarkCode(quarkA,anti);
		quarkB = GetQuarkCode(quarkB,!anti);
		quarks = Form("%c%c",quarkA,quarkB);
		quarks = quarkA+quarkB;
	}else{//baryon
		Int_t isospin = 0;
		Char_t quarkA = name[name.Length()-4];
		Char_t quarkB = name[name.Length()-3];
		Char_t quarkC = name[name.Length()-2];
		quarkA = GetQuarkCode(quarkA,anti);
		quarkB = GetQuarkCode(quarkB,anti);
		quarkC = GetQuarkCode(quarkC,anti);
		quarks = Form("%c%c%c",quarkA,quarkB,quarkC);
	}
	return quarks;
}

Int_t UPdgConvert::CalcS(Int_t pdg){
	TString qq = GetQuarks(pdg);
	Int_t s = 0;
	s -=qq.CountChar('s');
	s+=qq.CountChar('s');
	return s;
}

Int_t UPdgConvert::CalcI3(Int_t pdg) {
	TString qq= GetQuarks(pdg);
	Int_t isospin = 0;
	isospin += qq.CountChar('u');
	isospin +=qq.CountChar('D');
	isospin -=qq.CountChar('U');
	isospin -=qq.CountChar('d');
	return isospin;
}

Int_t UPdgConvert::Ityp2Pdg(UItyp pid) {
	if(fItyp2PDG.find(pid)!=fItyp2PDG.end()){
		return fItyp2PDG[pid];
	}else{
		return 0;
	}
}

void UPdgConvert::Pdg2U(Int_t pdg_code, Int_t& ityp, Int_t& ichg, Int_t& i3){
	TParticlePDG *part  = fPDG->GetParticle(pdg_code);
	if(part){
		ichg = GetCharge(pdg_code);
		UItyp typ = Pdg2Ityp(pdg_code);
		if(typ.GetItyp()==0){
			typ = Pdg2Ityp(-pdg_code);
			ityp = -typ.GetItyp();
			i3 = -typ.GetI3();
		}else{
			ityp = typ.GetItyp();
			i3 = typ.GetI3();
		}
	}else{
		ityp = 9999;
	}
}

Int_t UPdgConvert::GetCharge(Int_t pdg_code) {
	TParticlePDG *part  = fPDG->GetParticle(pdg_code);
	if(part){
		return part->Charge()/3;
	}
	return 0;
}

Bool_t UPdgConvert::Stable(Int_t pdg_code) {
	if(fPDG2Decay.find(pdg_code)==fPDG2Decay.end())return kTRUE;
	return kFALSE;
}

Int_t UPdgConvert::DecayParticle(UParticle* mother, TClonesArray* daughters,Int_t shift) {
	if(fPDG2Decay.find(mother->GetPdg())==fPDG2Decay.end()){
		Int_t mpid = TMath::Abs(mother->GetPdg());
		mother->SetDecay(-1);
		return 0;
	}else{
		UDecayParticle decay = fPDG2Decay[mother->GetPdg()];
		mother->SetDecay(0);
		Int_t size = daughters->GetEntriesFast();
		Int_t products =  decay.Decay(mother,daughters,shift);
		Int_t count_daughters = products;
		for(int i=0;i<products;i++){
			count_daughters+=DecayParticle(((UParticle*)daughters->UncheckedAt(size+i)),daughters,shift);
		}
		return count_daughters;
	}
}

void UPdgConvert::AddDummyParticles() {
	TString path = Form("%s/particles.data",gSystem->Getenv("UBOOT"));
	std::ifstream particles;
	particles.open(path);


	TString first;
	int i = 0;
	int pdg;
	Double_t t;
	TString name;
	Double_t mass, gamma;
	while(i<391){
		particles>>first;
		if(first=="#"){
			particles.ignore ( 10000, '\n' );
		}else{
			name = first;
			particles>>mass>>gamma>>t>>t>>t>>t>>t>>t>>t>>t>>t>>pdg;
			TParticlePDG *part = fPDG->GetParticle(pdg);
			if(part==NULL){
				TString pClass = "unknown";
				TString quarks = GetQuarks(pdg);
				if(quarks.Length()==2){
				//	pClass = "Meson";
				}else{
				//	pClass = "Baryon";
				}
		//		std::cout<<" ADDING\t"<<pdg<<"\t"<<quarks<<std::endl;
				Int_t type = -1;
				if(pdg<0){
					fPDG->AddAntiParticle(name,pdg);
				}else{
					fPDG->AddParticle(name,name,mass,false,gamma,0,pClass,pdg,type);
				}
			}
		}
		i++;
	}
	particles.close();

}

UPdgConvert::~UPdgConvert() {
	// TODO Auto-generated destructor stub
}

Int_t UPdgConvert::Status(Int_t pdg_code) {
	TParticlePDG *particle = fPDG->GetParticle(pdg_code);
	if(particle==NULL) return 0;
	TString particle_class = particle->ParticleClass();
	if(particle_class.Contains("Baryon")){
		return 3;
	}else if(particle_class.Contains("Meson")){
		return 2;
	}
	return 0;
}
