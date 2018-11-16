/*
 * U2A.cxx
 *
 *  Created on: 21 paź 2018
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "U2A.h"

U2A::U2A(TString name):fFlags(NULL),fFlagsSize(2000),fPDG(NULL),fTrashFile(NULL),
fInputFile(NULL),fEvent(NULL),fUrQMDEvent(NULL),fEventTrash(NULL),fFilename(name),
fStatus(0),fUseStatus(kFALSE) {
	fInputFile = new UFile(fFilename);
	fFlags = new Int_t[fFlagsSize];
	fPDG = UPdgConvert::Instance();
	fTrashFile = new UFile("u2boot_temp/trash.root","recreate");
	fEvent = fInputFile->GetEvent();
	fEventTrash = fTrashFile->GetEvent();
	fUrQMDEvent = new UEvent();
}

void U2A::ReadUnigen() {
	fUrQMDEvent->Clear();
	fUrQMDEvent->SetB(fEvent->GetB());
	fUrQMDEvent->SetPhi(fEvent->GetPhi());
	fUrQMDEvent->SetEventNr(fEvent->GetEventNr());
	fEventTrash->Clear();
	fEventTrash->SetB(fEvent->GetB());
	fEventTrash->SetPhi(fEvent->GetPhi());
	fEventTrash->SetEventNr(fEvent->GetEventNr());
	TParticlePDG *particle_pdg = NULL;
	if(fEvent->GetNpa()>fFlagsSize){
		fFlagsSize = fEvent->GetNpa()*2;
		delete []fFlags;
		fFlags = new Int_t[fFlagsSize];
	}
	//check flags
	Int_t ityp, ichg,i3;
	UItyp ITYP;
	Double_t bad_particles = 0;
	//check particle flags
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fUseStatus){
			if(particle->GetStatus()!=fStatus){
				fFlags[i]=kBad;
				bad_particles++;
				continue;
			}
		}
		Int_t pdg = particle->GetPdg();
		Int_t stat = fPDG->Status(pdg);
		switch(stat){
		case 2:
			fFlags[i]=kMeson;
			break;
		case 3:
			fFlags[i]=kBaryon;
			break;
		case 4:
			fFlags[i]=kOther;
			break;
		default:
			fFlags[i] = kUnknown;
			break;
		}
		fPDG->Pdg2U(pdg,ityp,ichg,i3);
		if(ityp==0){
			fFlags[i] = kUnknown;
			continue;
		}
		if(TMath::Abs(ityp)>999){
			fFlags[i]=kUnknown;
			continue;
		}
	}

	//copy barions
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]!=kUnknown&&fFlags[i]!=kBad){
			fUrQMDEvent->AddParticle(*particle);
		}
	}
	/*
	//copy mesons
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kMeson){
			fUrQMDEvent->AddParticle(*particle);
		}
	}
	//copy others (photons)
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kOther){
			fUrQMDEvent->AddParticle(*particle);
		}
	}
*/
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kUnknown){
			fEventTrash->AddParticle(*particle);
		}
	}
	std::cout<<fEventTrash->GetNpa()<<std::endl;
	fTrashFile->Fill();
	Int_t lost = fEvent->GetNpa()-fUrQMDEvent->GetNpa();
	Double_t lost_frac = ((Double_t)lost)/((Double_t)fEvent->GetNpa());
	std::cout<<"Particles\tTotal/Good/Bad/Unknown\t"<<fEvent->GetNpa()<<"/"<<fUrQMDEvent->GetNpa()<<"/"<<bad_particles<<"/"<<fEventTrash->GetNpa()<<std::endl;

}

void U2A::WriteUrQMD() {
	fUrQMDFile<<"# "<<fUrQMDEvent->GetNpa()<<std::endl;
	for(int i=0;i<fUrQMDEvent->GetNpa();i++){
		UParticle *p = fUrQMDEvent->GetParticle(i);
		fUrQMDFile<<p->GetPdg()<<" "<<Format(p->T())<<" "<<Format(p->X())<<" "<<Format(p->Y())<<" "<<Format(p->Z())<<" "<<Format(p->E())
			<<Format(p->Px())<<" "<<Format(p->Py())<<" "<<Format(p->Pz())<<std::endl;
	}
}

TString U2A::Format(Double_t val) {
	return Form("%16.8E",val);
}

void U2A::Convert(Int_t event) {
	fUrQMDFile.open(Form("u2boot_temp/test_U2boot_%i",event));
	fInputFile->GetEntry(event);
	ReadUnigen();
	WriteUrQMD();
	fUrQMDFile.close();
}

U2A::~U2A() {
	delete fTrashFile;
	delete fInputFile;
	delete []fFlags;
}

