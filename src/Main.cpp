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
#include "U2A.h"
#include "UMerger.h"
UPdgConvert	*fPDG;
Bool_t Remove;
Bool_t NoDecay;
Bool_t Afterburner;
Bool_t DecayOnly;
Bool_t FeedDown;
Int_t TimeFlag;
Int_t Nevents;
Int_t Status;
Double_t Tau;
Int_t UrQMDTime_out;
Int_t UrQMDTime_calc;
Double_t UrQMDTime_dt;
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
	Print("        Urqmd2unigenBOOsTer v 1.4");
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
	*outEvent = *inEvent;
	Int_t max_index = outEvent->GetNpa();
	for(int i=0;i<max_index;i++){
		UParticle *prim = outEvent->GetParticle(i);
		Int_t daughters = fPDG->DecayParticle(prim,temp,max_index);
	}
	for(int i=0;i<temp->GetEntriesFast();i++){
		UParticle *part = (UParticle *)temp->UncheckedAt(i);
		outEvent->AddParticle(*part);
	}
}

void UseUrQMD(TString file_in, TString file_out){
	gSystem->Load("libTree");
	gSystem->Exec("mkdir u2boot_temp");
	Print("*** reading unigen file ***");
	U2U *convert = new U2U(file_in);
	if(FeedDown)
		convert->TryDecay();
	convert->SetNEvents(Nevents);
//	std::cout<<"TIME FLAG"<<TimeFlag<<std::end;
	convert->SetTimeFlag(TimeFlag);
	if(Status!=-1E+9){
		convert->SetStatus(Status);
	}
	convert->SetFreezoutTime(Tau);
	convert->Convert();
	Nevents = convert->GetEvents();// is number of events ok?
	Int_t total_events = convert->GetEvents();
	delete convert;
	// al text files ready time to convert
	Print("*** staring UrQMD ***");

	UrQMDCall *call = new UrQMDCall(Remove);
	call->SetCalculationTime(UrQMDTime_calc);
	call->SetOutputTime(UrQMDTime_out);
	call->SetDeltaTime(UrQMDTime_dt);
	call->Convert(Nevents);
	Print("*** merging files ***");
	UMerger *merger = new UMerger(file_out,!NoDecay);
	Print("*** cleaning up ***");
	delete merger;
	delete call;//dlete txt files if necessary;
}
void UseAfterBurner(TString file_in, TString file_out){
	gSystem->Load("libTree");
	gSystem->Exec("mkdir u2boot_temp");
	Print("*** reading unigen file ***");
	U2A *convert = new U2A(file_in);
	Int_t total_events = convert->GetTotalEvents();
	if(Nevents>0)
		total_events = Nevents;
	if(Status!=-1E+9){
		convert->SetStatus(Status);
	}
	gSystem->Exec("mkdir u2boot_temp");
	TString path = gSystem->Getenv("URQMD_AFTERBURNER");
	TString pwd = gSystem->Getenv("PWD");
	std::cout<<"Processing afterburner events"<<std::endl;
	gSystem->Exec(Form("cd %s",path.Data()));
	for(int i=0;i<total_events;i++){
		convert->Convert(i);
		TString inp = Form("%s/u2boot_temp/test_U2boot_%i",pwd.Data(),i);
		TString out = Form("%s/u2boot_temp/test_U2after_%i",pwd.Data(),i);
		TString osc = Form("%s/u2boot_temp/test_U2oscar_%i",pwd.Data(),i);
		gSystem->Exec(Form("%s/./afterburner %s %s %s",path.Data(),inp.Data(),osc.Data(),out.Data()));
	}
	gSystem->Exec(Form("cd %s",pwd.Data()));
	delete convert;
	if(Remove){
		gSystem->Exec("rm -rf u2boot_temp");
	}

	Print("*** merging files ***");
	UMerger *merger = new UMerger(file_out,!NoDecay,UMerger::kAfterburner);
	Print("*** cleaning up ***");
	delete merger;
}
void CheckParams(Int_t argc, char*argv[]){
	TimeFlag  = 0;
	Remove = kFALSE;
	Nevents = -1;
	NoDecay = kFALSE;
	Afterburner = kFALSE;
	DecayOnly = kFALSE;
	FeedDown = kFALSE;
	UrQMDTime_calc = 200;//total UrQMD time calc
	UrQMDTime_out =200;//final UrQMD time
	UrQMDTime_dt = -1;//UrQMD timestep
	Tau = -1;
	Status = -1E+9;
	Double_t x;
	Double_t y = -1E+9;
	for(int i=3;i<argc;i++){
		x = -1E+9;
		ParCheck(argv[i],"-f",x);
		if(x==1){
			Remove = kTRUE;
			continue;
		}
		ParCheck(argv[i],"-n=",x);
		if(x>0){
			Nevents = x;
			continue;
		}
		ParCheck(argv[i],"-t=fmc",x);
		if(x!=y){
			Tau = x;
			std::cout<<"\t************"<<x<<std::endl;
			if(Tau>=0) TimeFlag = 1;
			continue;
		}
		ParCheck(argv[i],"-urqmd_out=",x);
		if(x!=y){
			UrQMDTime_out=x;
			continue;
		}
		ParCheck(argv[i],"-urqmd_calc=",x);
		if(x!=y){
			UrQMDTime_calc=x;
			continue;
		}
		ParCheck(argv[i],"-urqmd_dt=",x);
		if(x!=y){
			UrQMDTime_dt=x;
			continue;
		}

		ParCheck(argv[i],"-t=av",x);
		if(x!=y){
			TimeFlag = 2;
			continue;
		}
		ParCheck(argv[i],"-t=max",x);
		if(x!=y){
			TimeFlag = 3;
			continue;
		}
		ParCheck(argv[i],"-s=",x);
		if(x!=y){
			Status = x;
			continue;
		}
		TString par = argv[i];
		if(par.EqualTo("-decay")){
			DecayOnly = kTRUE;
			continue;
		}
		if(par.EqualTo("-no-decay")){
			NoDecay = kTRUE;
			continue;
		}
		if(par.EqualTo("-afterburner")){
			Afterburner = kTRUE;
			continue;
		}
		if(par.EqualTo("-feeddiwb")){
			FeedDown = kTRUE;
			continue;
		}
	}

	//- configuration print
	std::cout<<"***Configuration:***"<<std::endl;
	if(Remove==kTRUE)
		std::cout<<"Removing temp files: ENABLED"<<std::endl;
	else
		std::cout<<"Removing temp files: DISABLED"<<std::endl;

	if(DecayOnly){
		std::cout<<"Mode: DECAY ONLY"<<std::endl;
	}else{
		TString model = "URQMD";
		if(Afterburner){
			model = "AFTERBURNER";
		}else{
			std::cout<<"URQMD time conf"<<std::endl;
			std::cout<<"\tcalculation time:\t"<<UrQMDTime_calc<<std::endl;
			std::cout<<"\toutput time:\t"<<UrQMDTime_out<<std::endl;
			if(UrQMDTime_dt>0)
				std::cout<<"\tdelta time:\t"<<UrQMDTime_dt<<std::endl;
		}

		if(NoDecay)
			std::cout<<"Mode: "<<model<<std::endl;
		else
			std::cout<<"Mode: "<<model<<"+DECAY"<<std::endl;
		if(Status==-1E+9)
			std::cout<<"Processing all particles"<<std::endl;
		else
			std::cout<<"Processing particles with status "<<(int)Status<<" only"<<std::endl;
		switch((int)TimeFlag){
		case 0:
			std::cout<<"tau mode : minimum "<<std::endl;
			break;
		case 1:
			std::cout<<Form("tau mode : %4.2f fm/c ",Tau)<<std::endl;
			break;
		case 2:
			std::cout<<"tau mode : average "<<std::endl;
			break;
		case 3:
			std::cout<<"tau mode : maximum "<<std::endl;
			break;
		}
	}
}
void DecayOn(TString file_in, TString file_out){
	std::cout<<"*** working in decay mode only ***"<<std::endl;
	fPDG = UPdgConvert::Instance();
	TClonesArray *array = new TClonesArray("UParticle",1000);
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
		std::cout<<"-feeddown try decay highly exotic particles before UrQMD"<<std::endl;
		std::cout<<"time flags:"<<std::endl;
		std::cout<<"\t-t=tau_flag, where tau_flag can  have possible values:"<<std::endl;
		std::cout<<"\t\t-t=min (default) star with first particle"<<std::endl;
		std::cout<<"\t\t-t=fmcXX start with fixed time XX fm/c"<<std::endl;
		std::cout<<"\t\t-t=av start with averaged time of freezout"<<std::endl;
		std::cout<<"\t\t-t=max start with time where most particles are created"<<std::endl;
		std::cout<<"urqmd simulation flags"<<std::endl;
		std::cout<<"\t-urqmd_out=X set urqmd output time tim[1]"<<std::endl;
		std::cout<<"\t-urqmd_calc=X set urqmd calculattion time tim[0]"<<std::endl;
		std::cout<<"\t-urqmd_dt=X set urqmd dleta time cdt"<<std::endl;

//		std::cout<<"-aftberburner use afterburner not urqmd"<<std::endl;
		return 0;
	}
	Welcome();
	TString file_in = argv[1];
	TString file_out = argv[2];

	CheckParams(argc,argv);

	if(DecayOnly){// work in decay mode
		DecayOn(file_in, file_out);
	}else{//call rescatter
		gSystem->Load("libTree");
		gSystem->Exec("mkdir u2boot_temp");
		if(Afterburner==kFALSE){//cal UrQMD
			UseUrQMD(file_in,  file_out);
		}else{// call other afterburner
			gSystem->Load("libTree");
			gSystem->Exec("mkdir u2boot_temp");
			UseAfterBurner(file_in, file_out);

		}
	}

}
