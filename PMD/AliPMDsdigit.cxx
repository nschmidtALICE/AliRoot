/***************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/
//-----------------------------------------------------//
//                                                     //
//  Date   : August 05 2003                            //
//  used to store the info into TreeS                  //
//                                                     //
//-----------------------------------------------------//
#include "Riostream.h"
#include "Rtypes.h"
#include "AliPMDsdigit.h"
#include <stdio.h>

ClassImp(AliPMDsdigit)

AliPMDsdigit::AliPMDsdigit()
{
  // Default Constructor
  fTrNumber   = 0;
  fDet        = 0;
  fSMN        = 0;
  fCellNumber = 0;
  fEdep       = 0.;
}

AliPMDsdigit::AliPMDsdigit(Int_t trnumber, Int_t det, Int_t smn,
			   Int_t cellnumber, Float_t edep)
{
  // Constructor
  fTrNumber   = trnumber;
  fDet        = det;
  fSMN        = smn;
  fCellNumber = cellnumber;
  fEdep       = edep;
}
AliPMDsdigit::AliPMDsdigit(const AliPMDsdigit& pmdsdigit):TObject(pmdsdigit) {
  //Copy Constructor 
  if(&pmdsdigit == this) return;
  this->fTrNumber   = pmdsdigit.fTrNumber;
  this->fDet        = pmdsdigit.fDet;
  this->fSMN        = pmdsdigit.fSMN;
  this->fCellNumber = pmdsdigit.fCellNumber;
  this->fEdep       = pmdsdigit.fEdep;
  return;
}
AliPMDsdigit & AliPMDsdigit::operator=(const AliPMDsdigit& pmdsdigit) {
  //Assignment operator 
  if(&pmdsdigit == this) return *this;
  this->fTrNumber   = pmdsdigit.fTrNumber;
  this->fDet        = pmdsdigit.fDet;
  this->fSMN        = pmdsdigit.fSMN;
  this->fCellNumber = pmdsdigit.fCellNumber;
  this->fEdep       = pmdsdigit.fEdep;
  return *this;
}


AliPMDsdigit::~AliPMDsdigit()
{
  // Default Destructor
}
Int_t AliPMDsdigit::GetTrackNumber() const
{
  return fTrNumber;
}
Int_t AliPMDsdigit::GetDetector() const
{
  return fDet;
}
Int_t AliPMDsdigit::GetSMNumber() const
{
  return fSMN;
}
Int_t AliPMDsdigit::GetCellNumber() const
{
  return fCellNumber;
}

Float_t AliPMDsdigit::GetCellEdep() const
{
  return fEdep;
}
