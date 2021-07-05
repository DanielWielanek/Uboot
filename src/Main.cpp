//============================================================================
// Name        : Uboot.cpp
// Author      : Daniel WIelanek
// Version     : 1.1
// Copyright   : GPL 3
// U2boot - Unigen2urqmdBOOsTer
/**
 * Program for afterburning events by using UrQMD generator and or decay particles.
 * To process cascades this code call UrQMD code. To decay particles use decay tables derived from THERMINATOR 2 model
 */
//============================================================================

#include "U2U.h"
#include "UConfigurationParams.h"
#include "UEvent.h"
#include "UMerger.h"
#include "UParticle.h"
#include "UrQMDCall.h"
#include <TBranch.h>
#include <TFile.h>
#include <TTree.h>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
UPdgConvert* fPDG;
UConfigurationParams parameters;

void Print(TString text) {
  text          = Form("*              %s", text.Data());
  Int_t add     = 89 - text.Length();  // 20
  TString empty = "";
  for (int i = 0; i < add; i++) {
    empty += " ";
  }
  std::cout << text << empty << "*" << std::endl;
}
void Welcome() {
  std::cout << "******************************************************************************************" << std::endl;
  std::cout << "*                                                                                        *" << std::endl;
  std::cout << "*                   -----                                                                *" << std::endl;
  std::cout << "*                  |      |                                                              *" << std::endl;
  std::cout << "*                  |      |                                                              *" << std::endl;
  std::cout << "*            -----       -----------------------------------                             *" << std::endl;
  std::cout << "*          -                                                -----     *   *              *" << std::endl;
  std::cout << "*         \\            --                                        -----  *                *" << std::endl;
  std::cout << "*          -                                                -----     *   *              *" << std::endl;
  std::cout << "*            -----------------------------------------------                             *" << std::endl;
  std::cout << "*                                                                                        *" << std::endl;
  Print("  _    _ ___  _                 _   ");
  Print(" | |  | |__ \\| |               | |  ");
  Print(" | |  | |  ) | |__   ___   ___ | |_ ");
  Print(" | |  | | / /| '_ \\ / _ \\ / _ \\| __|");
  Print(" | |__| |/ /_| |_) | (_) | (_) | |_");
  Print("  \\____/|____|_.__/ \\___/ \\___/ \\__|");
  std::cout << "*                                                                                        *" << std::endl;
  std::cout << "*                                                                                        *" << std::endl;
  Print("        Urqmd2unigenBOOsTer v 2.3");
  std::cout << "******************************************************************************************" << std::endl;
}

void DecayEvent(UEvent* inEvent, UEvent* outEvent, TClonesArray* temp) {
  temp->Clear();
  *outEvent       = *inEvent;
  Int_t max_index = outEvent->GetNpa();
  for (int i = 0; i < max_index; i++) {
    UParticle* prim = outEvent->GetParticle(i);
    fPDG->DecayParticle(prim, temp, max_index);
  }
  for (int i = 0; i < temp->GetEntriesFast(); i++) {
    UParticle* part = (UParticle*) temp->UncheckedAt(i);
    outEvent->AddParticle(*part);
  }
}

void UseUrQMD(TString file_in, TString file_out) {
  UrQMDCall* call = new UrQMDCall(parameters);
  call->Convert();
  Print("*** merging files ***");
  UMerger* merger = new UMerger(file_out, parameters.Decay());
  Print("*** cleaning up ***");
  delete merger;
  delete call;  // dlete txt files if necessary;
}

void DecayOn(TString file_in, TString file_out) {
  std::cout << "*** working in decay mode only ***" << std::endl;
  fPDG                = UPdgConvert::Instance();
  TClonesArray* array = new TClonesArray("UParticle", 1000);
  UFile* fileIn       = new UFile(file_in, "");
  UFile* fileOut      = new UFile(file_out, "recreate");
  UEvent* outEvent    = fileOut->GetEvent();
  Int_t events        = fileIn->GetEntries();
  if (parameters.GetNevents() != -1) events = parameters.GetNevents();
  for (int i = 0; i < events; i++) {
    fileIn->GetEntry(i);
    UEvent* inEvent = fileIn->GetEvent();
    DecayEvent(inEvent, outEvent, array);
    fileOut->Fill();
  }
  delete fileIn;
  delete fileOut;
}
int main(int argc, char* argv[]) {
  Welcome();
  TString file_in  = argv[1];
  TString file_out = argv[2];
  parameters       = UConfigurationParams(argc, argv);
  if (argc < 3) return 0;
  parameters.PrintConfiguration();
  if (parameters.DecayOnly()) {  // work in decay mode
    DecayOn(file_in, file_out);
  } else {  // call rescatter
    gSystem->Load("libTree");
    gSystem->Exec("mkdir u2boot_temp");
    if (parameters.UseUrQMD()) {  // cal UrQMD
      UseUrQMD(file_in, file_out);
    } else {  // call other afterburner
      gSystem->Load("libTree");
      gSystem->Exec("mkdir u2boot_temp");
    }
  }
  return 0;
}
