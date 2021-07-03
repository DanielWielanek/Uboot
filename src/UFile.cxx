/*
 * UFile.cxx
 *
 *  Created on: 23 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UFile.h"

#include <TKey.h>
#include <TList.h>
#include <iostream>
UFile::UFile(TString file, TString option) {
  fFile  = new TFile(file, option);
  fEvent = new UEvent();
  if (option.Length() == 0) {
    fWriteMode = kFALSE;
    fTree      = (TTree*) fFile->Get("events");
    if (fTree == nullptr) { TryToFindTree(); }
    TBranch* branch = fTree->GetBranch("event");
    branch->SetAddress(&fEvent);
  } else {
    fWriteMode = kTRUE;
    fTree      = new TTree("events", "new_events");
    fTree->Branch("event", "UEvent", &fEvent);
  }
}

UFile::~UFile() {
  if (fWriteMode) fTree->Write();
  fFile->Close();
  delete fFile;
}

void UFile::TryToFindTree() {
  TList* keys = fFile->GetListOfKeys();
  for (int i = 0; i < keys->GetEntries(); i++) {
    TKey* key = (TKey*) keys->At(i);
    fTree     = dynamic_cast<TTree*>(fFile->Get(key->GetName()));
    if (fTree != nullptr) return;
  }
  std::cout << "UFIle::TryToFindTree: cannot find tree in file" << std::endl;
}
