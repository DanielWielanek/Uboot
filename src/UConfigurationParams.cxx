/*
 * UParameters.cxx
 *
 *  Created on: 21 lis 2018
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UConfigurationParams.h"

#include "TString.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>

UConfigurationParams::UConfigurationParams(int argc, char *argv[]):
fRemoveTemp(kFALSE),fNoDecay(kFALSE),
fAfterburner(kFALSE),fDecayOnly(kFALSE),
fFeedDown(kFALSE),fUseUrQMD(kTRUE),
fUseStatus(kFALSE),fSuppressUrQMD(kFALSE),
fTimeFlag(kMinimum),
fNevents(-1),fStatus(-1E+9),
fTau(-1),
fUrQMDTime_calc(200),
fUrQMDTime_out(200),
fUrQMDTime_dt(-1),
fInputFile(""),
fOutputFile("")
{
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
		std::cout<<"\t\t-t=XX start with fixed time XX fm/c"<<std::endl;
		std::cout<<"\t\t-t=av start with averaged time of freezout"<<std::endl;
		std::cout<<"\t\t-t=max start with time where most particles are created"<<std::endl;
		std::cout<<"urqmd simulation flags"<<std::endl;
		std::cout<<"\t-urqmd_out=X set urqmd output time tim[1]"<<std::endl;
		std::cout<<"\t-urqmd_calc=X set urqmd calculattion time tim[0]"<<std::endl;
		std::cout<<"\t-urqmd_dt=X set urqmd dleta time cdt"<<std::endl;
		std::cout<<"\t-sup - supress UrQMD output"<<std::endl;
		std::cout<<"\t-input=XX path to input file XX"<<std::endl;
//		std::cout<<"-aftberburner use afterburner not urqmd"<<std::endl;
		return;
	}
	fInputFile = argv[1];
	fOutputFile = argv[2];
	TString *par_array = new TString[argc-3];
	for(int i=3;i<argc;i++){
		par_array[i-3] = argv[i];
	}
	ReadParams(par_array,argc-3 );
}

void UConfigurationParams::ParCheck(TString par, TString flag,double &val)const {
	if(par.BeginsWith(flag)){
		if(par.Length()==flag.Length()){
			val = 1; //true value
		}else{
			TString sval = par(flag.Length(),par.Length());
			val  = sval.Atof();
		}
	}
}

void UConfigurationParams::ReadConfigFile(TString file) {
	std::ifstream input_file;
	input_file.open(file);
	if(input_file.good()){
		std::string line;
		Int_t nLines=0;
		while(std::getline(input_file,line)){
			nLines++;
		}
		if(nLines==0){
			std::cout<<"Something is wrong with input file "<<std::endl;
			exit(0);
		}else{
			TString *params = new TString[nLines-1];
			int i =0;
			input_file.clear();
			input_file.seekg(0);
			TString temp;
			input_file>>temp;
			if(temp!="#UBOOT_INPUT"){
				std::cout<<"Something is wrong with input file "<<std::endl;
				exit(0);
			}
			for(int i=0;i<nLines-1;i++){
				input_file>>params[i];
				if(!params[i].BeginsWith("-")){
					params[i]="-"+params[i];
				}
			}
			ReadParams(params, nLines-1);
			delete []params;
		}
	}else{
		std::cout<<"Something is wrong with input file "<<std::endl;
		exit(0);
	}
	input_file.close();
}
void UConfigurationParams::ReadParams(TString *params, Int_t size) {
	Double_t x=0;
	Double_t y = -1E+9;
	for(int i=0;i<size;i++){
		x = -1E+9;
		TString par = params[i];
		if(par.EqualTo("-feeddown")){
			fFeedDown = kTRUE;
			continue;
		}
		if(par.EqualTo("-decay")){
			fDecayOnly = kTRUE;
			continue;
		}
		if(par.EqualTo("-no-decay")){
			fNoDecay = kTRUE;
			continue;
		}
		/*if(par.EqualTo("-afterburner")){
			fAfterburner = kTRUE;
			continue;
		}*/
		ParCheck(par,"-f",x);
		if(x==1){
			fRemoveTemp = kTRUE;
			continue;
		}
		ParCheck(par,"-n=",x);
		if(x>0){
			fNevents = x;
			continue;
		}
		if(par.BeginsWith("-t=")){
			ParCheck(par,"-t=av",x);
			if(x!=y){
				fTimeFlag = kAverage;
				continue;
			}
			ParCheck(par,"-t=max",x);
			if(x!=y){
				fTimeFlag = kMaximum;
				continue;
			}
			ParCheck(par,"-t=",x);
			if(x!=y){
				fTau = x;
				if(fTau>=0) fTimeFlag = kFixedTime;
				continue;
			}
		}
		ParCheck(par,"-urqmd_out=",x);
		if(x!=y){
			fUrQMDTime_out=x;
			continue;
		}
		ParCheck(par,"-urqmd_calc=",x);
		if(x!=y){
			fUrQMDTime_calc=x;
			continue;
		}
		ParCheck(par,"-urqmd_dt=",x);
		if(x!=y){
			fUrQMDTime_dt=x;
			continue;
		}
		ParCheck(par,"-s=",x);
		if(x!=y){
			fStatus = x;
			fUseStatus = kTRUE;
			continue;
		}
		if(par.EqualTo("-sup")){
			fSuppressUrQMD = kTRUE;
			continue;
		}
		if(par.BeginsWith("-input=")){
			TString name = par(7,par.Length());
			ReadConfigFile(name);
			return;//skip this args load from input
		}
	}
}
void UConfigurationParams::PrintConfiguration() const {
	std::cout<<"***Configuration:***"<<std::endl;
	if(fRemoveTemp==kTRUE)
		std::cout<<"Removing temp files: ENABLED"<<std::endl;
	else
		std::cout<<"Removing temp files: DISABLED"<<std::endl;

	if(fDecayOnly){
		std::cout<<"Mode: DECAY ONLY"<<std::endl;
	}else{
		TString model = "URQMD";
		if(fAfterburner){
			model = "AFTERBURNER";
		}else{
			std::cout<<"URQMD time conf"<<std::endl;
			std::cout<<"\tcalculation time:\t"<<fUrQMDTime_calc<<std::endl;
			std::cout<<"\toutput time:\t"<<fUrQMDTime_out<<std::endl;
			if(fUrQMDTime_dt>0)
				std::cout<<"\tdelta time:\t"<<fUrQMDTime_dt<<std::endl;
			if(fFeedDown){
				std::cout<<"Feeddown:ENABLED"<<std::endl;
			}else{
				std::cout<<"Feeddown:DISABLED"<<std::endl;
			}
		}

		if(fNoDecay)
			std::cout<<"Mode: "<<model<<std::endl;
		else
			std::cout<<"Mode: "<<model<<"+DECAY"<<std::endl;
		if(fStatus==-1E+9)
			std::cout<<"Processing all particles"<<std::endl;
		else
			std::cout<<"Processing particles with status "<<(int)fStatus<<" only"<<std::endl;
		switch(fTimeFlag){
		case kMinimum:
			std::cout<<"tau mode : minimum "<<std::endl;
		break;
		case kFixedTime:
			std::cout<<Form("tau mode fixed : %4.2f fm/c ",fTau)<<std::endl;
			break;
		case kAverage:
			std::cout<<"tau mode : average "<<std::endl;
			break;
		case kMaximum:
			std::cout<<"tau mode : maximum "<<std::endl;
			break;
		}
	}
}

UConfigurationParams::~UConfigurationParams() {
	// TODO Auto-generated destructor stub
}

