/*
 * UrQMDCall.cxx
 *
 *  Created on: 12 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UrQMDCall.h"
#include <TString.h>
UrQMDCall::UrQMDCall(Bool_t remove):
fRemove(remove),
fCalls(0),
fCalculationTime(200),
fOutputTime(200),
fDeltaTime(-1)
{
}

void UrQMDCall::GenerateInput() {
	std::ofstream input;
	input.open("u2boot_temp/inputfile_U2boot");
//	input<<"pro 208 82"<<std::endl;
///	input<<"tar 208 82"<<std::endl;
	input<<"nev "<<1<<" "<<std::endl;
//	input<<"imp 13. 14.0"<<std::endl;
//	input<<"ecm 7"<<std::endl;
	input<<"tim "<<fCalculationTime<<" "<<fOutputTime<<""<<std::endl;
	if(fDeltaTime>0)
		input<<"cdt "<<Form("%4.2f",fDeltaTime)<<std::endl;
//	input<<"eos 0"<<std::endl;
	input<<"cto 40 2"<<std::endl;
	input<<"#f13"<<std::endl;
	input<<"f14"<<std::endl;
	input<<"f15"<<std::endl;
	input<<"f16"<<std::endl;
	input<<"f19"<<std::endl;
	input<<"f20 "<<std::endl;
	input<<"xxx"<<std::endl;
	input.close();
}

void UrQMDCall::GenerateRunQMD(Int_t event) {
	std::ofstream runqmd;
	runqmd.open("u2boot_temp/runqmd.bash_U2boot");
	runqmd<<"#!/bin/sh"<<std::endl;
	runqmd<<"export ftn09=u2boot_temp/inputfile_U2boot"<<std::endl;
	runqmd<<"export ftn10=u2boot_temp/test_U2boot_"<<event<<std::endl;
	runqmd<<"export ftn13=u2boot_temp/test.f13_"<<event<<std::endl;
	runqmd<<"export ftn14=u2boot_temp/test.f14"<<std::endl;
	runqmd<<"export ftn15=u2boot_temp/test.f15"<<std::endl;
	runqmd<<"export ftn16=u2boot_temp/test.f16"<<std::endl;
	runqmd<<"export ftn19=u2boot_temp/test.f19"<<std::endl;
	runqmd<<"export ftn20=u2boot_temp/test.f20"<<std::endl;
	TString urqmd_path = gSystem->Getenv("URQMD");
	if(urqmd_path.Length()==1){
		runqmd<<"time ./urqmd.$(uname -m) "<<std::endl;
	}else{
		runqmd<<Form("time %s/urqmd.$(uname -m) ",urqmd_path.Data())<<std::endl;
	}
	runqmd.close();
}

void UrQMDCall::RunQMD(Int_t event) {
	gSystem->Exec("chmod a+x u2boot_temp/runqmd.bash_U2boot");
	gSystem->Exec("./u2boot_temp/runqmd.bash_U2boot");
	std::ifstream test;
	test.open(Form("u2boot_temp/test.f13_%i",event));
	std::string line;
	Int_t nLines=0;
	fCalls++;
	while(std::getline(test,line)){
		nLines++;
	}
	test.close();
	if(nLines<20){
		std::cout<<"\033[1;31m UrQMD problem with event "<<event <<", trying again..."<<fCalls<<"\033[0m\n"<<std::endl;
		RunQMD(event);
	}
}

UrQMDCall::~UrQMDCall() {
	if(fRemove){
		gSystem->Exec("rm -rf u2boot_temp");
		gSystem->Exec("rm tables.dat");
	}
	std::cout<<"UQMD DONE"<<std::endl;
}

void UrQMDCall::Convert(Int_t nevents) {
	GenerateInput();
	for(int i=0;i<nevents;i++){
		GenerateRunQMD(i);
		fCalls = 0;
		RunQMD(i);
	}
}
