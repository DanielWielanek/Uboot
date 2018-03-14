/*
 * UItyp.cxx
 *
 *  Created on: 16 lut 2017
 *      Author: Daniel Wielanek
 *		E-mail: daniel.wielanek@gmail.com
 *		Warsaw University of Technology, Faculty of Physics
 */
#include "UItyp.h"


UItyp::UItyp(Int_t ityp, Int_t isospin) :fItyp(ityp),fI3(isospin){
}

bool UItyp::operator <(const UItyp& other) const {
	if(fItyp==other.fItyp){
		return fI3<other.fI3;
	}else{
		return fItyp<other.fItyp;
	}
}
