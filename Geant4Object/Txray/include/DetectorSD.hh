//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file medical/GammaTherapy/src/TargetSD.cc
/// \brief Implementation of the TargetSD class
//
// -------------------------------------------------------------
//
//
//      ---------- TargetSD -------------
//              
//  Modified:
//
// -------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
/// \file medical/GammaTherapy/include/DetectorSD.hh
/// \brief Definition of the DetectorSD class
//

// -------------------------------------------------------------
//
//  Modified:
//
// -------------------------------------------------------------
#ifndef DetectorSD_h
#define DetectorSD_h 1

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "DetectorHit.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/// 探测器灵敏区域类
class DetectorSD : public G4VSensitiveDetector
{
  public:
    DetectorSD(const G4String& name);
    virtual ~DetectorSD();
    
    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history);
    virtual void EndOfEvent(G4HCofThisEvent* hce);
    
  private:
    G4THitsCollection<DetectorHit>* fHitsCollection;
    G4int fHCID;  // 添加此变量声明
};

#endif