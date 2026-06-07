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
/// \file medical/GammaTherapy/include/PhysicsList.hh
/// \brief Definition of the PhysicsList class
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"

class PhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PhysicsList: public G4VModularPhysicsList
{
public:
  PhysicsList();
  virtual ~PhysicsList();

  void ConstructParticle();
  void ConstructProcess();

  void AddPhysicsList(const G4String&);
  inline void SetVerbose(G4int val);

  // 物理过程开关控制方法
  void SetEnableIonisation(G4bool enable) { fEnableIonisation = enable; }
  void SetEnableBremsstrahlung(G4bool enable) { fEnableBremsstrahlung = enable; }
  void SetEnableScatter(G4bool enable) { fEnableScatter = enable; }
  void SetEnableAnnihilation(G4bool enable) { fEnableAnnihilation = enable; }
  
  G4bool GetEnableIonisation() const { return fEnableIonisation; }
  G4bool GetEnableBremsstrahlung() const { return fEnableBremsstrahlung; }
  G4bool GetEnableScatter() const { return fEnableScatter; }
  G4bool GetEnableAnnihilation() const { return fEnableAnnihilation; }

private:

  void ApplyPhysicsProcessControls();  // 应用物理过程控制

  PhysicsList & operator=(const PhysicsList &right);
  PhysicsList(const PhysicsList&);

  G4int    fVerbose;
  G4bool   fHelIsRegisted;
  G4bool   fBicIsRegisted;
  G4bool   fIonIsRegisted;
  G4bool   fGnucIsRegisted;
  G4bool   fStopIsRegisted;

  // 物理过程开关状态
  G4bool   fEnableIonisation;      // 电离过程
  G4bool   fEnableBremsstrahlung;  // 韧致辐射过程
  G4bool   fEnableScatter;         // 多重散射过程
  G4bool   fEnableAnnihilation;    // 正电子湮灭过程

  PhysicsListMessenger* fMessenger;
};

inline void PhysicsList::SetVerbose(G4int value)
{
  fVerbose = value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

