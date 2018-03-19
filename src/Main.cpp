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

#include <stdio.h>
#include <stdlib.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <iostream>
#include <cassert>
#include "UrQMDCall.h"
#include "UEvent.h"
#include "UParticle.h"
#include "U2U.h"
#include "UMerger.h"
UPdgConvert	*fPDG;
void Print(TString text){
	text = Form("*              %s",text.Data());
	Int_t add = 89 - text.Length();//20
	TString empty="";
	for(int i=0;i<add;i++){
		empty+=" ";
	}
	std::cout<<text<<empty<<"*"<<std::endl;
}
void Welcome(){
	std::cout<<"******************************************************************************************"<<std::endl;
	std::cout<<"*                                                                                        *"<<std::endl;
	std::cout<<"*                   -----                                                                *"<<std::endl;
	std::cout<<"*                  |      |                                                              *"<<std::endl;
	std::cout<<"*                  |      |                                                              *"<<std::endl;
	std::cout<<"*            -----       -----------------------------------                             *"<<std::endl;
	std::cout<<"*          -                                                -----     *   *              *"<<std::endl;
	std::cout<<"*         \\            --                                        -----  *                *"<<std::endl;
	std::cout<<"*          -                                                -----     *   *              *"<<std::endl;
	std::cout<<"*            -----------------------------------------------                             *"<<std::endl;
	std::cout<<"*                                                                                        *"<<std::endl;
	Print("  _    _ ___  _                 _   ");
	Print(" | |  | |__ \\| |               | |  ");
	Print(" | |  | |  ) | |__   ___   ___ | |_ ");
	Print(" | |  | | / /| '_ \\ / _ \\ / _ \\| __|");
	Print(" | |__| |/ /_| |_) | (_) | (_) | |_");
	Print("  \\____/|____|_.__/ \\___/ \\___/ \\__|");
	std::cout<<"*                                                                                        *"<<std::endl;
	std::cout<<"*                                                                                        *"<<std::endl;
	Print("        Urqmd2unigenBOOsTer v 1.1");
	std::cout<<"******************************************************************************************"<<std::endl;
}
void ParCheck(TString par, TString flag,double &val){
	if(par.BeginsWith(flag)){
		if(par.Length()==flag.Length()){
			val = 1; //true value
		}else{
			TString sval = par(flag.Length(),par.Length());
			val  = sval.Atof();
		}
	}
}
void DecayEvent(UEvent *inEvent, UEvent *outEvent, TClonesArray *temp){
	temp->Clear();
	Int_t max_index = inEvent->GetNpa();
	*outEvent = *inEvent;
	for(int i=0;i<max_index;i++){
		UParticle *prim = outEvent->GetParticle(i);
		Int_t daughters = fPDG->DecayParticle(prim,temp,max_index);
	}
	for(int i=0;i<temp->GetEntriesFast();i++){
		UParticle *part = (UParticle *)temp->UncheckedAt(i);
		outEvent->AddParticle(*part);
	}
}
int main(int argc, char *argv[]) {
	if(argc<3){
		std::cout<<"usage :"<<std::endl;
		std::cout<<"U2boot unigen_file_in unigen_file_out  -flag=value"<<std::endl;
		std::cout<<"avaiable flags"<<std::endl;
		std::cout<<"\t-f clean all temporary files"<<std::endl;
		std::cout<<"\t-decay only decay unstable particles (don't call UrQMD)"<<std::endl;
		std::cout<<"\t-no-decay disable decayer after UrQMD"<<std::endl;
		std::cout<<"\t-s=N where N is staus of processed particles -  particles with different status are ingored completely"<<std::endl;
		std::cout<<"\t-n=N where N is maximun number of events to process"<<std::endl;
		std::cout<<"\t-t=tau_flag, where tau_flag can  have possible values:"<<std::endl;
		std::cout<<"\t\t-t=min (default) star with first particle"<<std::endl;
		std::cout<<"\t\t-t=fmcXX start with fixed time XX fm/c"<<std::endl;
		std::cout<<"\t\t-t=av start with averaged time of freezout"<<std::endl;
		std::cout<<"\t\t-t=max start with time where most particles are created"<<std::endl;
		return 0;
	}
	Welcome();
	TString file_in = argv[1];
	TString file_out = argv[2];
	double remove_i = 0;
	double nevents = -1;
	double time_flag = 0;
	Bool_t no_decay = kFALSE;
	double_t tau  = 0;
	double status=-1E+9;
	Bool_t decay_only =kFALSE;
	for(int i=3;i<argc;i++){
		double dummy  = 0;
		ParCheck(argv[i],"-f",remove_i);
		ParCheck(argv[i],"-n=",nevents);
		ParCheck(argv[i],"-t=fmc",tau);
		ParCheck(argv[i],"-t=av",dummy);
		if(dummy) time_flag = 2;
		ParCheck(argv[i],"-t=max",dummy);
		if(dummy) time_flag  =3;
		ParCheck(argv[i],"-s=",status);
		TString par = argv[i];
		if(par.EqualTo("-decay")){
			decay_only = kTRUE;
			break;
		}
		if(par.EqualTo("-no-decay"))
			no_decay = kTRUE;
	}
	if(decay_only){// work in decay mode
		std::cout<<"*** working in decay mode only ***"<<std::endl;
		fPDG = UPdgConvert::Instance();
		TClonesArray *array = new TClonesArray("UParticle",1000);
		TString file_in = argv[1];
		TString file_out = argv[2];
		UFile *fileIn = new UFile(file_in,"");
		UFile *fileOut = new UFile(file_out,"recreate");
		UEvent *outEvent = fileOut->GetEvent();
		for(int i=0;i<fileIn->GetEntries();i++){
			fileIn->GetEntry(i);
			UEvent *inEvent = fileIn->GetEvent();
			DecayEvent(inEvent,outEvent,array);
			fileOut->Fill();
		}
		delete fileIn;
		delete fileOut;
		return 1;
	}else{//call UrQMD
		if(tau>0) time_flag = 1;
		bool remove = false;
		if(remove_i==1.0)remove = true;
		gSystem->Load("libTree");
		gSystem->Exec("mkdir u2boot_temp");
		Print("*** reading unigen file ***");
		U2U *convert = new U2U(file_in);
		convert->SetNEvents(nevents);
		convert->SetTimeFlag(time_flag);
		if(status!=1E+9){
			convert->SetStatus(status);
		}
		convert->SetFreezoutTime(tau);
		convert->Convert();
		Int_t total_events = convert->GetEvents();
		delete convert;
		Print("*** staring UrQMD ***");
		UrQMDCall *call = new UrQMDCall(total_events,remove);
		Print("*** merging files ***");
		UMerger *merger = new UMerger(file_out,!no_decay);
		Print("*** cleaning up ***");
		delete merger;
		delete call;//dlete txt files if necessary
		//ok now file is converted
		return 1;
	}

}
