/*
 * UMerger.cxx
 *
 *  Created on: 18 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UMerger.h"
#include "UFile.h"
void UMerger::ReadUrQMD() {
	ReadUrQMDIn();
	ReadUrQMDOut();
	MatchUrQMD();
}

UMerger::UMerger(TString outFile,Bool_t decay, eUMergerMode mode):
		fDecay(decay){
	fEvent = new UEvent();
	fEventIn = new UEvent();
	fPDG = UPdgConvert::Instance();
    fDecayedArray = new TClonesArray("UParticle",1000);
	UFile *trashFile = new UFile("u2boot_temp/trash.root");
	UEvent *trash_event  = trashFile->GetEvent();
	UFile *outFIle = new UFile(outFile,"recreate");
	fEvent = outFIle->GetEvent();
	Int_t nevents = trashFile->GetEntries();
	for(int i=0;i<nevents;i++){
		if(mode==kUrQMD){
			fUrQMDFile.open(Form("u2boot_temp/test.f13_%i",i));
			fUrQMDFileIn.open(Form("u2boot_temp/test_U2boot_%i",i));
		}
		fEvent->Clear();
		switch(mode){
		case kUrQMD:
			ReadUrQMD();
			break;
		case kAfterburner:
			ReadAfterburner(i);
			break;
		}
		trashFile->GetEntry(i);
		fEvent->SetEventNr(trash_event->GetEventNr());
		fEvent->SetB(trash_event->GetB());
		fEvent->SetPhi(trash_event->GetPhi());
		Int_t count = fEvent->GetNpa();
		// add "trash" particles
		for(int j=0;j<trash_event->GetNpa();j++){
			trash_event->GetParticle(j)->SetIndex(count+j);
			trash_event->GetParticle(j)->SetStatus(STATUS_TRASH);
			trash_event->GetParticle(j)->SetParent(-1);
			fEvent->AddParticle(*(trash_event->GetParticle(j)));
		}
		if(fDecay)
			DecayEvent();
		outFIle->Fill();
		if(mode==kUrQMD){
			fUrQMDFile.close();
			fUrQMDFileIn.close();
		}
	}
	delete outFIle;
	delete trashFile;

}

void UMerger::DecayEvent() {
	fDecayedArray->Clear();
	Int_t max_index = fEvent->GetNpa();
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *prim = fEvent->GetParticle(i);
		Int_t daughters = fPDG->DecayParticle(prim,fDecayedArray,max_index);
	}
	for(int i=0;i<fDecayedArray->GetEntriesFast();i++){
		UParticle *part = (UParticle *)fDecayedArray->UncheckedAt(i);
		fEvent->AddParticle(*part);
	}
}

UMerger::~UMerger() {
	if(fDecayedArray) delete fDecayedArray;
	delete fEvent;
	delete fEventIn;
}

void UMerger::ReadAfterburner(Int_t event) {
	fUrQMDFile.open(Form("u2boot_temp/test_U2after_%i",event));
	ReadUrQMD();
    fUrQMDFile.close();
}

void UMerger::ReadUrQMDIn() {
    int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
    int format;
    float b = 0.;
    float b_min = 0., b_max=0.;
    // ---> Read and check first event header line from input file
    Int_t URQMDVERSION[2] = {20030, 30400};
    char read[300];
    fUrQMDFileIn.getline(read,200);// line 1
    Int_t urqmdVersion = 0;
    sscanf(read, "UQMD   version:       %d   1000  %d  output_file  %d", &urqmdVersion, &urqmdVersion, &format);
    // ---> Read rest of event header
  // line 2

    fUrQMDFileIn.getline(read,300);
    fUrQMDFileIn.getline(read, 300); // line 3
    fUrQMDFileIn.getline(read, 300); // line 4
    fUrQMDFileIn.getline(read, 300); // line 5
    fUrQMDFileIn.getline(read, 300); // line 6
    if (urqmdVersion == URQMDVERSION[1])
        for (int iline = 0; iline < 11; iline++) { // is supressed //11
        	fUrQMDFileIn.getline(read, 300);
        }// line 7-17
    else if (urqmdVersion == URQMDVERSION[0])
        for (int iline = 0; iline < 8; iline++) {
        	fUrQMDFileIn.getline(read, 300);
        }
    fUrQMDFileIn.getline(read, 300); // line 18
    sscanf(read, "%d", &ntracks); // line 18
    fUrQMDFileIn.getline(read, 300); // line 19
    int ityp = 0, i3 = 0, ichg = 0, pid = 0;
    float ppx = 0., ppy = 0., ppz = 0., e = 0.,trash;
    float x,y,z,t;
    float tau;
    int child[2]={0,0};
    fEventIn->Clear();
    for (int itrack = 0; itrack < ntracks; itrack++) {
    	fUrQMDFileIn>>t>>x>>y>>z;
    	fUrQMDFileIn>>e>>ppx>>ppy>>ppz>>trash;
    	fUrQMDFileIn>>ityp>>i3>>ichg>>trash>>trash>>trash;
    	fUrQMDFileIn>>trash>>tau>>trash>>trash;
        // Convert UrQMD type and charge to unique pid identifier
    	UItyp typ(ityp,i3);
        Int_t pdgID = fPDG->Ityp2Pdg(typ);
        if(pdgID==0){
        	TLorentzVector x(ppz,ppy,ppz,e);
        	std::cout<<"EEEE "<<ityp<<" "<<i3<<" "<<x.M()<<std::endl;
        	continue;
        }
        fEventIn->AddParticle(itrack,pdgID,1,-1,0,0,0,child,ppx,ppy,ppz,e,x,y,z,t,tau);//for UrQMD particles
    }
}

void UMerger::ReadUrQMDOut() {
    int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
    int ncoll, parent_process;
    int format;
    float b = 0.;
    float b_min = 0., b_max=0.;
    // ---> Read and check first event header line from input file
    Int_t URQMDVERSION[2] = {20030, 30400};
    char read[300];
    fUrQMDFile.getline(read,200);// line 1
    Int_t urqmdVersion = 0;
    sscanf(read, "UQMD   version:       %d   1000  %d  output_file  %d", &urqmdVersion, &urqmdVersion, &format);
    // ---> Read rest of event header
  // line 2

    fUrQMDFile.getline(read,300);
    fUrQMDFile.getline(read, 300); // line 3
    fUrQMDFile.getline(read, 300); // line 4
    fUrQMDFile.getline(read, 300); // line 5
    fUrQMDFile.getline(read, 300); // line 6
    if (urqmdVersion == URQMDVERSION[1])
        for (int iline = 0; iline < 11; iline++) { // is supressed //11
        	fUrQMDFile.getline(read, 300);
        }// line 7-17
    else if (urqmdVersion == URQMDVERSION[0])
        for (int iline = 0; iline < 8; iline++) {
        	fUrQMDFile.getline(read, 300);
        }
    fUrQMDFile.getline(read, 300); // line 18
    sscanf(read, "%d", &ntracks); // line 18
    fUrQMDFile.getline(read, 300); // line 19
    int ityp = 0, i3 = 0, ichg = 0, pid = 0;
    float ppx = 0., ppy = 0., ppz = 0., e = 0.,trash;
    float x,y,z,t;
    int child[2]={0,0};
    for (int itrack = 0; itrack < ntracks; itrack++) {
    	fUrQMDFile>>t>>x>>y>>z;
    	fUrQMDFile>>e>>ppx>>ppy>>ppz>>trash;
    	fUrQMDFile>>ityp>>i3>>ichg>>trash>>ncoll>>parent_process;
    	fUrQMDFile>>t>>x>>y>>z;
    	fUrQMDFile>>e>>ppx>>ppy>>ppz;
        // Convert UrQMD type and charge to unique pid identifier
    	if(!(parent_process==91&&ncoll==0)){
    		parent_process= 1;
    	}
    	UItyp typ(ityp,i3);
        Int_t pdgID = fPDG->Ityp2Pdg(typ);
        if(pdgID==0){
        	TLorentzVector x(ppz,ppy,ppz,e);
        	std::cout<<"EEEE "<<ityp<<" "<<i3<<" "<<x.M()<<std::endl;
        	continue;
        }
        fEvent->AddParticle(itrack,pdgID,1,-1,0,0,0,child,ppx,ppy,ppz,e,x,y,z,t,parent_process);//for UrQMD particles
    }
    fUrQMDFile.getline(read,300);//read cin whitespace
}

void UMerger::MatchUrQMD() {
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *p = fEvent->GetParticle(i);
		p->SetStatus(STATUS_URQMD_SEC);
		if(p->GetWeight()==91){
			p->SetWeight(1);
			UParticle *fluid = GetMatched(p, i);
			if(fluid!=NULL){

				//copy freezout information
				p->SetPosition(Extrapolate(fluid));
				p->SetStatus(STATUS_URQMD_PRIM);
			}
		}
		p->SetWeight(1);
	}

}

UParticle* UMerger::GetMatched(UParticle* p, Int_t pos) {
	if(pos>fEventIn->GetNpa()){
		std::cout<<"ops"<<std::endl;
		return NULL;
	}
	for(int i=pos;i<fEventIn->GetNpa();i++){
		UParticle *fluid_candidate = fEventIn->GetParticle(i);
		if(AreSimilar(p, fluid_candidate)){
			return fluid_candidate;
		}
	}
	for(int i=pos-1;i>=0;i--){
		UParticle *fluid_candidate = fEventIn->GetParticle(i);
		if(AreSimilar(p, fluid_candidate)){
			return fluid_candidate;
		}
	}
	std::cout<<"not found parent"<<std::endl;
	return NULL;
}

Bool_t UMerger::AreSimilar(UParticle* p1, UParticle* p2) const {
	if(p1->GetPdg()!=p1->GetPdg()) return kFALSE;
	if(TMath::Abs(p1->Px()-p2->Px())>1E-6) return kFALSE;
	if(TMath::Abs(p1->Py()-p2->Py())>1E-6) return kFALSE;
	if(TMath::Abs(p1->Pz()-p2->Pz())>1E-6) return kFALSE;
	return kTRUE;
}

TLorentzVector UMerger::Extrapolate(UParticle* fluid) const {
	Double_t tau = fluid->GetWeight();
	Double_t dt = tau - fluid->T();
	TLorentzVector tau_pos = fluid->GetPosition();
	TLorentzVector mom = fluid->GetMomentum();
	TVector3 boost = mom.BoostVector();
	return TLorentzVector(
			tau_pos.X()+dt*boost.X(),
			tau_pos.Y()+dt*boost.Y(),
			tau_pos.Z()+dt*boost.Z(),
			tau_pos.T()+dt
	);
}
