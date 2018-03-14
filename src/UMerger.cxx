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
    int evnr = 0, ntracks = 0, aProj = 0, zProj = 0, aTarg = 0, zTarg = 0;
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
        for (int iline = 0; iline < 11; iline++) {
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
    	fUrQMDFile>>ityp>>i3>>ichg>>trash>>trash>>trash;
    	fUrQMDFile>>t>>x>>y>>z;
    	fUrQMDFile>>e>>ppx>>ppy>>ppz;
        // Convert UrQMD type and charge to unique pid identifier
    	UItyp typ(ityp,i3);
        Int_t pdgID = fPDG->Ityp2Pdg(typ);
        if(pdgID==0){
        	std::cout<<"EEEE "<<ityp<<" "<<i3<<std::endl;
        }
        fEvent->AddParticle(itrack,pdgID,1,-1,0,0,0,child,ppx,ppy,ppz,e,x,y,z,t,1);//for UrQMD particles
    }
    fUrQMDFile.getline(read,300);//read cin whitespace
}

UMerger::UMerger(TString outFile,Bool_t decay):fDecay(decay) {
	fEvent = new UEvent();
	fUrQMDFile.open("u2boot_temp/test.f13");
	fPDG = UPdgConvert::Instance();
    fDecayedArray = new TClonesArray("UParticle",1000);
	UFile *trashFile = new UFile("u2boot_temp/trash.root");
	UEvent *trash_event  = trashFile->GetEvent();
	UFile *outFIle = new UFile(outFile,"recreate");
	fEvent = outFIle->GetEvent();
	Int_t nevents = trashFile->GetEntries();
	for(int i=0;i<nevents;i++){
		fEvent->Clear();
		ReadUrQMD();
		trashFile->GetEntry(i);
		fEvent->SetEventNr(trash_event->GetEventNr());
		fEvent->SetB(trash_event->GetB());
		fEvent->SetPhi(trash_event->GetPhi());
		Int_t count = fEvent->GetNpa();
		// add "trash" particles
		for(int j=0;j<trash_event->GetNpa();j++){
			trash_event->GetParticle(j)->SetIndex(count+j);
			trash_event->GetParticle(j)->SetStatus(0);
			trash_event->GetParticle(j)->SetParent(-1);
			fEvent->AddParticle(*(trash_event->GetParticle(j)));
		}
		if(fDecay)
			DecayEvent();
		outFIle->Fill();
	}
	delete outFIle;
	delete trashFile;
	fUrQMDFile.close();
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
}

