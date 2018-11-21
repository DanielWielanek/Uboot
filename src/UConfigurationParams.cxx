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
#include <iostream>
UConfigurationParams::UConfigurationParams(int argc, char *argv[]):
fRemoveTemp(kFALSE),fNoDecay(kFALSE),
fAfterburner(kFALSE),fDecayOnly(kFALSE),
fFeedDown(kFALSE),fUseUrQMD(kTRUE),
fUseStatus(kFALSE),
fTimeFlag(0),
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
		std::cout<<"\t\t-t=fmcXX start with fixed time XX fm/c"<<std::endl;
		std::cout<<"\t\t-t=av start with averaged time of freezout"<<std::endl;
		std::cout<<"\t\t-t=max start with time where most particles are created"<<std::endl;
		std::cout<<"urqmd simulation flags"<<std::endl;
		std::cout<<"\t-urqmd_out=X set urqmd output time tim[1]"<<std::endl;
		std::cout<<"\t-urqmd_calc=X set urqmd calculattion time tim[0]"<<std::endl;
		std::cout<<"\t-urqmd_dt=X set urqmd dleta time cdt"<<std::endl;

//		std::cout<<"-aftberburner use afterburner not urqmd"<<std::endl;
		return;
	}
	Double_t x;
	Double_t y = -1E+9;
	fInputFile = argv[1];
	fOutputFile = argv[2];
	for(int i=3;i<argc;i++){
		x = -1E+9;
		ParCheck(argv[i],"-f",x);
		if(x==1){
			fRemoveTemp = kTRUE;
			continue;
		}
		ParCheck(argv[i],"-n=",x);
		if(x>0){
			fNevents = x;
			continue;
		}
		ParCheck(argv[i],"-t=fmc",x);
		if(x!=y){
			fTau = x;
			if(fTau>=0) fTimeFlag = 1;
			continue;
		}
		ParCheck(argv[i],"-urqmd_out=",x);
		if(x!=y){
			fUrQMDTime_out=x;
			continue;
		}
		ParCheck(argv[i],"-urqmd_calc=",x);
		if(x!=y){
			fUrQMDTime_calc=x;
			continue;
		}
		ParCheck(argv[i],"-urqmd_dt=",x);
		if(x!=y){
			fUrQMDTime_dt=x;
			continue;
		}

		ParCheck(argv[i],"-t=av",x);
		if(x!=y){
			fTimeFlag = 2;
			continue;
		}
		ParCheck(argv[i],"-t=max",x);
		if(x!=y){
			fTimeFlag = 3;
			continue;
		}
		ParCheck(argv[i],"-s=",x);
		if(x!=y){
			fStatus = x;
			fUseStatus = kTRUE;
			continue;
		}
		TString par = argv[i];
		if(par.EqualTo("-decay")){
			fDecayOnly = kTRUE;
			continue;
		}
		if(par.EqualTo("-no-decay")){
			fNoDecay = kTRUE;
			continue;
		}
		if(par.EqualTo("-afterburner")){
			fAfterburner = kTRUE;
			continue;
		}
		if(par.EqualTo("-feeddiwb")){
			fFeedDown = kTRUE;
			continue;
		}
	}
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
		}

		if(fNoDecay)
			std::cout<<"Mode: "<<model<<std::endl;
		else
			std::cout<<"Mode: "<<model<<"+DECAY"<<std::endl;
		if(fStatus==-1E+9)
			std::cout<<"Processing all particles"<<std::endl;
		else
			std::cout<<"Processing particles with status "<<(int)fStatus<<" only"<<std::endl;
		switch((int)fTimeFlag){
		case 0:
			std::cout<<"tau mode : minimum "<<std::endl;
			break;
		case 1:
			std::cout<<Form("tau mode : %4.2f fm/c ",fTau)<<std::endl;
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

UConfigurationParams::~UConfigurationParams() {
	// TODO Auto-generated destructor stub
}

