/*
 * U2U.cxx
 *
 *  Created on: 11 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include <TParticlePDG.h>
#include "U2U.h"

U2U::U2U(TString name) :
fFlags(NULL),fFlagsSize(2000),fPDG(NULL),fTrashFile(NULL),
fInputFile(NULL),fEvent(NULL),fUrQMDEvent(NULL),fEventTrash(NULL),fFilename(name),
fTimeFlag(0),fMaxEvents(0),fStatus(0),fUseStatus(kFALSE),fFreezoutTime(0),fFreezoutHisto(NULL){
}

void U2U::Convert() {
	fFlags = new Int_t[fFlagsSize];
	fPDG = UPdgConvert::Instance();
	fInputFile = new UFile(fFilename);
	fTrashFile = new UFile("u2boot_temp/trash.root","recreate");
	fEvent = fInputFile->GetEvent();
	fEventTrash = fTrashFile->GetEvent();
	fUrQMDFile.open("u2boot_temp/test_U2boot");
	fUrQMDEvent = new UEvent();
	for(int i=0;i<60;i++){
		fCTO[i] = 0 ;
	}
	SetCTOs();
	if(fMaxEvents<=0||fMaxEvents>fInputFile->GetEntries()){
		fMaxEvents = fInputFile->GetEntries();
	}
	if(fTimeFlag==3){
		fFreezoutHisto = new TH1D("temp","temp",1000,0,1000);
		fFreezoutHisto->SetDirectory(0);
	}
	for(int i=0;i<fMaxEvents;i++){
		fInputFile->GetEntry(i);
		ReadUnigen();
		WriteUrQMD();
	}
	delete fInputFile;
	delete fTrashFile;
	fUrQMDFile.close();

}

void U2U::Interpolate(Double_t t_min) {
	for(int i=0;i<fUrQMDEvent->GetNpa();i++){
		UParticle *part = fUrQMDEvent->GetParticle(i);
		TLorentzVector mom = part->GetMomentum();
		TVector3 boost = mom.BoostVector();
		Double_t dt = part->T()-t_min;
		part->SetX(part->X()-boost.X()*dt);
		part->SetY(part->Y()-boost.Y()*dt);
		part->SetY(part->Z()-boost.Z()*dt);
		part->SetT(t_min);
	}
}

void U2U::SetCTOs() {
	//CTOP -1
	fCTO[21] = 1;//string mass excitation  FRITOF
	fCTO[23] = 1;//initialization mode - hard sphere
	fCTO[28] = 2;//pt for lat two particles in sting -bayon goes into formward hempispere pt=0
	fCTO[29] = 1;//frozen fermi approximation in cascade mode  enabled
	fCTO[33] = 1;//resonance lifemtysmes gamma = 1/gamm_pole
	fCTO[34] = 1;//gnerate hight precission tables file enabled
	fCTO[40] = 1;//extended output for f14
//	fCTO[43] = "1  ";//pytha call for hard scatterings - enabled

}

void U2U::ReadUnigen() {
	fUrQMDEvent->Clear();
	fUrQMDEvent->SetB(fEvent->GetB());
	fUrQMDEvent->SetPhi(fEvent->GetPhi());
	fUrQMDEvent->SetEventNr(fEvent->GetEventNr());
	fEventTrash->Clear();
	fEventTrash->SetB(fEvent->GetB());
	fEventTrash->SetPhi(fEvent->GetPhi());
	fEventTrash->SetEventNr(fEvent->GetEventNr());
	TParticlePDG *particle_pdg = NULL;
	if(fEvent->GetNpa()>fFlagsSize){
		fFlagsSize = fEvent->GetNpa()*2;
		delete []fFlags;
		fFlags = new Int_t[fFlagsSize];
	}
	//check flags
	Int_t ityp, ichg,i3;
	UItyp ITYP;
	Double_t bad_particles = 0;
	//check particle flags
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fUseStatus){
			if(particle->GetStatus()!=fStatus){
				fFlags[i]=kBad;
				bad_particles++;
				continue;
			}
		}
		Int_t pdg = particle->GetPdg();
		Int_t stat = fPDG->Status(pdg);
		switch(stat){
		case 2:
			fFlags[i]=kMeson;
			break;
		case 3:
			fFlags[i]=kBaryon;
			break;
		default:
			fFlags[i] = kUnknown;
			break;
		}
		fPDG->Pdg2U(pdg,ityp,ichg,i3);
		if(ityp==0){
			fFlags[i] = kUnknown;
			continue;
		}
		//std::cout<<"PDG"<<pdg<< " "<<ityp<<std::endl;
		if(TMath::Abs(ityp)>999){
			fFlags[i]=kUnknown;
			continue;
		}
	}
	Double_t t_min  = EstimateTime();

	//copy barions
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kBaryon){
			fUrQMDEvent->AddParticle(*particle);
		}
	}
	//copy mesons
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kMeson){
			fUrQMDEvent->AddParticle(*particle);
		}
	}
	for(int i=0;i<fEvent->GetNpa();i++){
		UParticle *particle = fEvent->GetParticle(i);
		if(fFlags[i]==kUnknown){
			fEventTrash->AddParticle(*particle);
		}
	}
	std::cout<<fEventTrash->GetNpa()<<std::endl;
	fTrashFile->Fill();
	Int_t lost = fEvent->GetNpa()-fUrQMDEvent->GetNpa();
	Interpolate(t_min);
	Double_t lost_frac = ((Double_t)lost)/((Double_t)fEvent->GetNpa());
	std::cout<<"Particles\tTotal/Good/Bad/Unknown\t"<<fEvent->GetNpa()<<"/"<<fUrQMDEvent->GetNpa()<<"/"<<bad_particles<<"/"<<fEventTrash->GetNpa()<<std::endl;

}

void U2U::WriteUrQMD() {
	fUrQMDFile<<"UQMD   version:       20030   1000  20030  output_file  14"<<std::endl;
		fUrQMDFile<<"projectile:  (mass, char)  102 100   target:  (mass, char)  102  100 "<<std::endl;
		fUrQMDFile<<"transformation betas (NN,lab,pro)     0.0000000  0.9634189 -0.9634189"<<std::endl;
		fUrQMDFile<<"impact_parameter_real/min/max(fm):   "<<Form("%4.2f",fUrQMDEvent->GetB())<<"  0.00 13.00  total_cross_section(mbarn):    5309.29"<<std::endl;
		fUrQMDFile<<"equation_of_state:    0  E_lab(GeV/u): 0.2424E+02  sqrt(s)(GeV): 0.7000E+01  p_lab(GeV/u): 0.2516E+02"<<std::endl;
		int evN = fUrQMDEvent->GetEventNr();
		TString number = Form("%i",evN);
		fUrQMDFile<<"event#"<<Form("%10i",evN)<<" random seed:  1486911827 (auto)   total_time(fm/c):        0 Delta(t)_O(fm/c):            0"<<std::endl;
	//	fUrQMDFIle<<"op  0    0    0    0    0    0    0    0    0    0    0    0    0    0    0  "<<std::endl;
	//	fUrQMDFIle<<"op  0    0    0    0    0    0    1    0    1    0    0    0    0    2    1  "<<std::endl;
	//	fUrQMDFIle<<"op  0    0    0    1    1    0    0    0    0    0    1 *  0    0    1    0  "<<std::endl;
		int lopt= 0;
		fUrQMDFile<<Form("%2s","op");
		for(int i=0;i<15;i++){
			fUrQMDFile<<Form("%3d%2s",fCTO[lopt+i],"  ");
		}
		fUrQMDFile<<std::endl;
		fUrQMDFile<<Form("%2s","op");
		lopt = 15;
		for(int i=0;i<15;i++){
			fUrQMDFile<<Form("%3d%2s",fCTO[lopt+i],"  ");
		}
		fUrQMDFile<<std::endl;
		fUrQMDFile<<Form("%2s","op");
		lopt = 30;
		for(int i=0;i<15;i++){
			fUrQMDFile<<Form("%3d%2s",fCTO[lopt+i],"  ");
		}
		fUrQMDFile<<std::endl;
		if(UQMD_VER>=3.4){
			lopt = 45;
			for(int i=0;i<15;i++){
				fUrQMDFile<<Form("%3d%2s",fCTO[lopt+i],"  ");
			}
			fUrQMDFile<<std::endl;
		}
	/*	fUrQMDFIle<<"op "<<fCTO[0+lopt]<<"  "<<fCTO[1+lopt]<<"  "<<fCTO[2+lopt]<<"  "<<fCTO[3+lopt]<<"  "<<fCTO[4+lopt]<<"  "<<fCTO[5+lopt]<<"  "
				<<fCTO[6+lopt]<<"  "<<fCTO[7+lopt]<<"  "<<fCTO[8+lopt]<<"  "<<fCTO[9+lopt]<<"  "<<fCTO[10+lopt]
				<<"  "<<fCTO[11+lopt]<<"  "<<fCTO[12+lopt]<<"  "<<fCTO[13+lopt]<<"  "<<fCTO[14+lopt]<<"  "<<std::endl;

		lopt=15;
		fUrQMDFIle<<"op "<<fCTO[0+lopt]<<"  "<<fCTO[1+lopt]<<"  "<<fCTO[2+lopt]<<"  "<<fCTO[3+lopt]<<"  "<<fCTO[4+lopt]<<"  "<<fCTO[5+lopt]<<"  "
				<<fCTO[6+lopt]<<"  "<<fCTO[7+lopt]<<"  "<<fCTO[8+lopt]<<"  "<<fCTO[9+lopt]<<"  "<<fCTO[10+lopt]
				<<"  "<<fCTO[11+lopt]<<"  "<<fCTO[12+lopt]<<"  "<<fCTO[13+lopt]<<"  "<<fCTO[14+lopt]<<"  "<<std::endl;
		lopt = 30;
		fUrQMDFIle<<"op "<<fCTO[0+lopt]<<"  "<<fCTO[1+lopt]<<"  "<<fCTO[2+lopt]<<"  "<<fCTO[3+lopt]<<"  "<<fCTO[4+lopt]<<"  "<<fCTO[5+lopt]<<"  "
				<<fCTO[6+lopt]<<"  "<<fCTO[7+lopt]<<"  "<<fCTO[8+lopt]<<"  "<<fCTO[9+lopt]<<"  "<<fCTO[10+lopt]
				<<"  "<<fCTO[11+lopt]<<"  "<<fCTO[12+lopt]<<"  "<<fCTO[13+lopt]<<"  "<<fCTO[14+lopt]<<"  "<<std::endl;
				*/

		fUrQMDFile<<"pa 0.1000E+01   0.5200E+00   0.2000E+01   0.3000E+00   0.0000E+00   0.3700E+00   0.0000E+00   0.9300E-01   0.3500E+00   0.2500E+00   0.0000E+00   0.5000E+00  "<<std::endl;
		fUrQMDFile<<"pa 0.2700E+00   0.4900E+00   0.2700E+00   0.1000E+01   0.1600E+01   0.8500E+00   0.1550E+01   0.0000E+00   0.0000E+00   0.0000E+00   0.0000E+00   0.0000E+00  "<<std::endl;
		fUrQMDFile<<"pa 0.9000E+00   0.5000E+02   0.1000E+01   0.1000E+01   0.1000E+01   0.1500E+01   0.1600E+01   0.0000E+00   0.2500E+01   0.1000E+00   0.3000E+01   0.2750E+00  "<<std::endl;
		fUrQMDFile<<"pa 0.4200E+00   0.1080E+01   0.8000E+00   0.5000E+00   0.0000E+00   0.5500E+00   0.5000E+01   0.8000E+00   0.5000E+00   0.8000E+06   0.1000E+01   0.2000E+01  "<<std::endl;
		if(UQMD_VER>=3.4){
			fUrQMDFile<<"pa 0.9000E+00   0.5000E+02   0.1000E+01   0.1000E+01   0.1000E+01   0.1500E+01   0.1600E+01   0.0000E+00   0.2500E+01   0.1000E+00   0.3000E+01   0.2750E+00  "<<std::endl;
			fUrQMDFile<<"pa 0.4200E+00   0.1080E+01   0.8000E+00   0.5000E+00   0.0000E+00   0.5500E+00   0.5000E+01   0.8000E+00   0.5000E+00   0.8000E+06   0.1000E+01   0.2000E+01  "<<std::endl;
		}
		fUrQMDFile<<"pvec: r0              rx              ry              rz              p0              px              py              pz              m          ityp 2i3 chg lcl#  ncl or "<<std::endl;
		fUrQMDFile<<"        "<<fUrQMDEvent->GetNpa()<<"         200"<<std::endl;
		fUrQMDFile<<"     0      0       0       0       0      0       0       0"<<std::endl;
		for(int i=0;i<fUrQMDEvent->GetNpa();i++){
			UParticle *part = fUrQMDEvent->GetParticle(i);
			TLorentzVector mom = part->GetMomentum();
			Int_t pdg = part->GetPdg();
			Int_t ichg, ityp,i3;
			fPDG->Pdg2U(pdg,ityp,ichg,i3);
			TString ityp_s = Form("%3.d",ityp);
			TString ichg_s = Form("%3.d",ichg);
			TString i3_s = Form("%3.d",i3);
			if(ityp==0)
				ityp_s = "  0";
			if(ichg==0)
				ichg_s = "  0";
			if(i3==0)
				i3_s="  0";
			TString spaces = "        ";
			if(ityp_s.Length()==4){
				spaces = "       ";
			}
			//std::cout<<"xPDG\t"<<pdg<< "|"<<ityp<<"|"<<ichg<<"|"<<i3<<std::endl;
			//std::cout<<"---\t"<<pdg<< "|"<<ityp_s<<"|"<<ichg_s<<"|"<<i3_s<<std::endl;
			fUrQMDFile<<Format(part->T())<<Format(part->X())<<Format(part->Y())<<Format(part->Z())<<Format(part->E())<<
					Format(part->Px())<<Format(part->Py())<<Format(part->Pz())<<Format(mom.M())<<spaces<<
					ityp_s<<i3_s<<ichg_s<<
					"        0    0         0  "<<"0.10000000E+32  0.10000000E+00  0.10000000E+01"<<Form("%8d",i)
					<<std::endl;
		}
}

U2U::~U2U() {
	if(fFreezoutHisto)
		delete fFreezoutHisto;
	delete []fFlags;
}

TString U2U::Format(Double_t val) {
	return Form("%16.8E",val);
}

Double_t U2U::EstimateTime() {
	Double_t time = 0;
	switch(fTimeFlag){
	case 0:{//default method  - first freezout time
		time= 1E+9;
		for(int i=0;i<fEvent->GetNpa();i++){
				UParticle *particle = fEvent->GetParticle(i);
				if(fFlags[i]==kMeson||fFlags[i]==kBaryon) //ignore unknown particles
				time = TMath::Min(time,particle->T());
		}
		return time;
	}break;
	case 1:{//fixed time
		return fFreezoutTime;
	}break;
	case 2:{//average value
		Double_t particles = fEvent->GetNpa();
		for(int i=0;i<fEvent->GetNpa();i++){
				UParticle *particle = fEvent->GetParticle(i);
				time +=particle->T();
		}
		return time/particles;
	}break;
	case 3:{//maximum value
		fFreezoutHisto->Reset();
		for(int i=0;i<fEvent->GetNpa();i++){
				UParticle *particle = fEvent->GetParticle(i);
				fFreezoutHisto->Fill(particle->T());
		}
		return fFreezoutHisto->GetBinCenter(fFreezoutHisto->GetMaximumBin());
	}break;
	default:
		return 0;
		break;
	}

}
