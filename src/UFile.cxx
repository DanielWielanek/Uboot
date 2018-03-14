/*
 * UFile.cxx
 *
 *  Created on: 23 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UFile.h"
#include <iostream>
UFile::UFile(TString file, TString option) {
	fFile = new TFile(file,option);
	fEvent = new UEvent();
	if(option.Length()==0){
		fWriteMode= kFALSE;
		fTree = (TTree*)fFile->Get("events");
		TBranch * branch = fTree->GetBranch("event");
		branch->SetAddress(&fEvent);
	}else{
		fWriteMode = kTRUE;
		fTree = new TTree("events","new_events");
		fTree->Branch("event","UEvent",&fEvent);
	}
}

UFile::~UFile() {
	if(fWriteMode)
		fTree->Write();
	fFile->Close();
	delete fFile;
}

