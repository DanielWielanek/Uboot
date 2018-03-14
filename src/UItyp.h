/*
 * UItyp.h
 *
 *  Created on: 16 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#ifndef SRC_UITYP_H_
#define SRC_UITYP_H_
#include <TObject.h>
#include <map>
/**
 * class that hold UrQMD typ and isospin
 */
class UItyp : public TObject{
	Int_t fItyp;
	Int_t fI3;
public:
	UItyp(Int_t ityp=0, Int_t isospin=0);
	void Swap(){fItyp = -fItyp; fI3 = -fI3;};
	Int_t GetItyp()const{return fItyp;};
	Int_t GetI3()const{return fI3;};
	virtual bool operator<(const UItyp &other)const;
	virtual ~UItyp(){};
	ClassDef(UItyp,1)
};

#endif /* SRC_UITYP_H_ */
