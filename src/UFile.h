/*
 * UFile.h
 *
 *  Created on: 23 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UFILE_H_
#define SRC_UFILE_H_

#include "UEvent.h"
#include <TFile.h>
#include <TTree.h>
class UFile : public TObject {
  TFile* fFile;
  TTree* fTree;
  UEvent* fEvent;
  Bool_t fWriteMode;
  void TryToFindTree();

public:
  UFile(TString file = "data.root", TString option = "");
  UEvent* GetEvent() const { return fEvent; };
  Int_t GetEntries() const { return fTree->GetEntries(); };
  void GetEntry(Long64_t ent) { fTree->GetEntry(ent); };
  void Fill() { fTree->Fill(); };
  virtual ~UFile();
  ClassDef(UFile, 1)
};

#endif /* SRC_UFILE_H_ */
