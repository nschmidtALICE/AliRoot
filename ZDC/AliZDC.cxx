/**************************************************************************
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

/*
$Log$
Revision 1.23  2001/05/15 13:44:57  coppedis
Changes in AddHit method

Revision 1.22  2001/05/14 09:53:32  coppedis
Adding functions ZMin and ZMax

Revision 1.21  2001/04/20 10:05:02  coppedis
Minor changes

Revision 1.20  2001/03/26 13:39:20  coppedis
Comment prints

Revision 1.19  2001/03/26 09:10:23  coppedis
Corrected bug in constructor (fIshunt has to be =1)

Revision 1.18  2001/03/20 08:21:55  coppedis
ZDC needs PIPE, ABSO, DIPO and SHIL

Revision 1.17  2001/03/16 16:18:03  coppedis
Correction for superposition of ZDC volumes with MUON arm one

Revision 1.16  2001/03/15 16:01:11  coppedis
Code review

Revision 1.15  2001/01/26 19:56:27  hristov
Major upgrade of AliRoot code

Revision 1.14  2000/12/12 13:17:01  coppedis
Minor corrections suggested by P. Hristov

Revision 1.12  2000/12/01 08:19:01  coppedis
Adding a message error if ZDC is constructed without DIPO

Revision 1.11  2000/11/30 17:21:03  coppedis
Introduce hit array fStHits reset only at the end of the event (for digitization)

Revision 1.10  2000/11/22 11:32:58  coppedis
Major code revision

Revision 1.9  2000/10/02 21:28:20  fca
Removal of useless dependecies via forward declarations

Revision 1.8  2000/07/10 13:58:01  fca
New version of ZDC from E.Scomparin & C.Oppedisano

Revision 1.7  2000/01/19 17:17:40  fca

Revision 1.6  1999/09/29 09:24:35  fca
Introduction of the Copyright and cvs Log

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  			Zero Degree Calorimeter			             //
//  	     This class contains the basic functions for the ZDCs;           //
//            functions specific to one particular geometry are              //
//                      contained in the derived classes                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream.h>

// --- ROOT system
#include <TBRIK.h>
#include <TNode.h>
#include <TGeometry.h>
#include <TFile.h>
#include <TTree.h>

// --- AliRoot header files
#include "AliZDC.h"
#include "AliZDCHit.h"
#include "AliZDCDigit.h"
#include "AliZDCMerger.h"
#include "AliDetector.h"
#include "AliCallf77.h"
#include "AliConst.h"
#include "AliMC.h"
#include "AliRun.h"
#include "AliHeader.h"

 
ClassImp(AliZDC)
 
//_____________________________________________________________________________
AliZDC::AliZDC()
{
  //
  // Default constructor for the Zero Degree Calorimeter base class
  //
  
  fIshunt   = 1;
  fNoShower = 0;
  fMerger   = 0;

  fHits     = 0;
  fNhits    = 0;

  fDigits   = 0;
  fNdigits  = 0;
  
}
 
//_____________________________________________________________________________
AliZDC::AliZDC(const char *name, const char *title)
  : AliDetector(name,title)
{
  //
  // Standard constructor for the Zero Degree Calorimeter base class
  //

  fIshunt   = 1;
  fNoShower = 0;
  fMerger   = 0;

  // Allocate the hits array  
  fHits   = new TClonesArray("AliZDCHit",1000);
  gAlice->AddHitList(fHits);

  // Allocate the digits array  
  fDigits = new TClonesArray("AliZDCDigit",1000);
  

}
//____________________________________________________________________________ 
AliZDC::~AliZDC()
{
  //
  // ZDC destructor
  //

  fIshunt   = 0;
  
//  if(fMerger) delete fMerger;

//  if(fHits){
//    fHits->Delete();
//    delete fHits;
//  }

//  if(fDigits){
//    fDigits->Delete();
//    delete fDigits;
//  }

}
//_____________________________________________________________________________
void AliZDC::AddHit(Int_t track, Int_t *vol, Float_t *hits)
{
  //
  // 		Add a ZDC hit to the hit list.
  // -> We make use of 2 array of hits:
  // [1]  fHits (the usual one) that contains hits for each PRIMARY
  // [2]  fStHits that contains hits for each EVENT and is used to
  //	  obtain digits at the end of each event
  //
  
  static Float_t primKinEn, xImpact, yImpact, sFlag;

  AliZDCHit *newquad, *curprimquad;
  newquad = new AliZDCHit(fIshunt, track, vol, hits);
  TClonesArray &lhits = *fHits;
  
  if(fNhits==0){
      // First hit -> setting flag for primary or secondary particle
      Int_t primary = gAlice->GetPrimary(track);     
      if(track != primary){
        newquad->fSFlag = 1;  // SECONDARY particle entering the ZDC
      }
      else if(track == primary){
        newquad->fSFlag = 0;  // PRIMARY particle entering the ZDC
      }  
//      fNPrimaryHits += 1;
      sFlag 	= newquad->fSFlag;
      primKinEn = newquad->fPrimKinEn;
      xImpact 	= newquad->fXImpact;
      yImpact 	= newquad->fYImpact;
   }
   else{       
      newquad->fPrimKinEn = primKinEn;
      newquad->fXImpact	= xImpact;
      newquad->fYImpact = yImpact;
      newquad->fSFlag 	= sFlag;
   }
 
  Int_t j;
  for(j=0; j<fNhits; j++){
    // If hits are equal (same track, same volume), sum them.
     curprimquad = (AliZDCHit*) lhits[j];
     if(*curprimquad == *newquad){
        *curprimquad = *curprimquad+*newquad;
	delete newquad;
	return;
     } 
  }

    //Otherwise create a new hit
    new(lhits[fNhits]) AliZDCHit(newquad);
    fNhits++;
    
    delete newquad;
}

//_____________________________________________________________________________
void  AliZDC::AddDigit(Int_t *sect, Int_t digit)
{
//
  AliZDCDigit *newdigit;
  newdigit = new AliZDCDigit(sect, digit);

//  AliZDCDigit *curdigit;
//  TClonesArray &ldigits = *fDigits;
//
//  Int_t j;
//  for(j=0; j<fNdigits; j++){
//     curdigit = (AliZDCDigit*) ldigits[j];
//     if(*curdigit == *newdigit){
//	*curdigit = *curdigit+*newdigit;
//      delete newdigit;
//      return;
//     } 
//  } 
//
  
//  printf("\n	AddDigit -> sector[0] = %d, sector[1] = %d, digit = %d",
//         sect[0], sect[1], digit);
  new((*fDigits)[fNdigits]) AliZDCDigit(*newdigit);
  fNdigits++;
  delete newdigit;
}
      
//_____________________________________________________________________________
void AliZDC::BuildGeometry()
{
  //
  // Build the ROOT TNode geometry for event display 
  // in the Zero Degree Calorimeter
  // This routine is dummy for the moment
  //

  TNode *node, *top;
  TBRIK *brik;
  const int kColorZDC  = kBlue;
  
  //
  top=gAlice->GetGeometry()->GetNode("alice");
  
  // ZDC
    brik = new TBRIK("S_ZDC","ZDC box","void",300,300,5);
    top->cd();
    node = new TNode("ZDC","ZDC","S_ZDC",0,0,600,"");
    node->SetLineColor(kColorZDC);
    fNodes->Add(node);
}

//_____________________________________________________________________________
Int_t AliZDC::DistancetoPrimitive(Int_t , Int_t )
{
  //
  // Distance from the mouse to the Zero Degree Calorimeter
  // Dummy routine
  //
  return 9999;
}

//____________________________________________________________________________
Float_t AliZDC::ZMin(void) const
{
  // Minimum dimension of the ZDC module in z
  return 11600.;
}

//____________________________________________________________________________
Float_t AliZDC::ZMax(void) const
{
  // Maximum dimension of the ZDC module in z
  return  11750.;
}
  

//_____________________________________________________________________________
 void AliZDC::MakeBranch(Option_t *opt, const char *file)
{
  //
  // Create Tree branches for the ZDC
  //

  char branchname[10];
  sprintf(branchname,"%s",GetName());
  
  AliDetector::MakeBranch(opt);

  const char *cS = strstr(opt,"S");

  if (gAlice->TreeS() && cS) {
    MakeBranchInTree(gAlice->TreeS(), 
                     branchname, &fHits, fBufferSize, file) ;
    printf("* AliZDC::MakeBranch    * Making Branch %s for SDigits\n\n",branchname);
  }

    
  const char *cD = strstr(opt,"D");

  if (gAlice->TreeD() && cD) {
    if(fDigits!=0) fDigits->Clear();
    else fDigits = new TClonesArray ("AliZDCDigit",1000);
    MakeBranchInTree(gAlice->TreeD(), 
                     branchname, &fDigits, fBufferSize, file) ;
    printf("* AliZDC::MakeBranch    * Making Branch %s for Digits\n\n",branchname);
  }

  
  const char *cR = strstr(opt,"R");

  if (gAlice->TreeR() && cR) {
    MakeBranchInTree(gAlice->TreeR(), 
		     branchname, &fRecPoints, fBufferSize, file) ;
    printf("* AliZDC::MakeBranch    * Making Branch %s for RecPoints\n\n",branchname);   }
          
}

//_____________________________________________________________________________
void AliZDC::Hits2SDigits()
{

  if(!fMerger){ 
    // ### Copy of TreeH in TreeS
    // Pointer to TreeH
//    printf("\n 	ZDC digitization (without merging)\n");
    TTree *treeH = gAlice->TreeH();
    Int_t ntracks = (Int_t) treeH->GetEntries();
    Int_t fNhits = 0;
    gAlice->ResetHits();
  
    // Tracks loop
    for(Int_t itrack=0; itrack<ntracks; itrack++){
       treeH->GetEvent(itrack);
       // Hits loop
       for(AliZDCHit* zdcHit=(AliZDCHit*)this->FirstHit(-1); zdcHit;
           zdcHit = (AliZDCHit*)this->NextHit()){ 
	  TClonesArray &sdigits = *fHits;
	  new (sdigits[fNhits++]) AliZDCHit(zdcHit);
       }
    }
  
    gAlice->TreeS()->Fill();
    gAlice->TreeS()->Write(0,TObject::kOverwrite);  
    gAlice->TreeS()->Reset();  
  }
  
}

//_____________________________________________________________________________
void AliZDC::SDigits2Digits()
{
  if(!fMerger){ // Only digitization
//    printf("\n        ZDC digitization (without merging) \n");
    fMerger = new AliZDCMerger();    
    fMerger->Digitize();
  }
  else{	// Merging and digitization
    printf("\n        ZDC merging and digitization\n");
    fMerger -> InitMerging();
  }

  char hname[30];
  sprintf(hname,"TreeD%d",gAlice->GetHeader()->GetEvent());

  gAlice->TreeD()->Fill();
  gAlice->TreeD()->Write(0,TObject::kOverwrite);
  gAlice->TreeD()->Reset();  
  
}
//_____________________________________________________________________________
void AliZDC::Hits2Digits()
{
    gAlice->Hits2SDigits();
    gAlice->SDigits2Digits();
}

//_____________________________________________________________________________
void AliZDC::Digits2Reco()
{
    
}

 
//_____________________________________________________________________________
void   AliZDC::SetMerger(AliZDCMerger* merger)
{
// Set pointer to merger 
    fMerger = merger;
}

//_____________________________________________________________________________
AliZDCMerger*  AliZDC::Merger()
{
// Return pointer to merger
    return fMerger;
}

