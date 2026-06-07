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
/// \file medical/GammaTherapy/src/PhysicsListMessenger.cc
/// \brief Implementation of the PhysicsListMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsListMessenger.hh"

#include "PhysicsList.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4LossTableManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::PhysicsListMessenger(PhysicsList *pPhys)
    : fPhysicsList(pPhys)
{
  fPhysDir = new G4UIdirectory("/testem/phys/");
  fPhysDir->SetGuidance("physics list commands");

  fListCmd = new G4UIcmdWithAString("/testem/phys/addPhysics", this);
  fListCmd->SetGuidance("Add modula physics list.");
  fListCmd->SetParameterName("PList", false);
  fListCmd->AvailableForStates(G4State_PreInit);

  fVerbCmd = new G4UIcmdWithAnInteger("/testem/phys/verbose", this);
  fVerbCmd->SetGuidance("Set verbose level for processes");
  fVerbCmd->SetParameterName("pVerb", false);
  fVerbCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  // 物理过程开关控制命令
  fIonisationCmd = new G4UIcmdWithAnInteger("/testem/phys/EnableIonisation", this);
  fIonisationCmd->SetGuidance("Enable/disable electron ionisation process (G4eIonisation)");
  fIonisationCmd->SetGuidance("1 = enable, 0 = disable");
  fIonisationCmd->SetParameterName("enableIon", false);
  fIonisationCmd->SetRange("enableIon>=0 && enableIon<=1");
  fIonisationCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fBremsstrahlungCmd = new G4UIcmdWithAnInteger("/testem/phys/EnableBremsstrahlung", this);
  fBremsstrahlungCmd->SetGuidance("Enable/disable bremsstrahlung process (G4eBremsstrahlung)");
  fBremsstrahlungCmd->SetGuidance("1 = enable, 0 = disable");
  fBremsstrahlungCmd->SetParameterName("enableBrem", false);
  fBremsstrahlungCmd->SetRange("enableBrem>=0 && enableBrem<=1");
  fBremsstrahlungCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fScatterCmd = new G4UIcmdWithAnInteger("/testem/phys/EnableScatter", this);
  fScatterCmd->SetGuidance("Enable/disable multiple scattering process (G4eMultipleScattering)");
  fScatterCmd->SetGuidance("1 = enable, 0 = disable");
  fScatterCmd->SetParameterName("enableScatter", false);
  fScatterCmd->SetRange("enableScatter>=0 && enableScatter<=1");
  fScatterCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

  fAnnihilationCmd = new G4UIcmdWithAnInteger("/testem/phys/EnableAnnihilation", this);
  fAnnihilationCmd->SetGuidance("Enable/disable positron annihilation process (G4eplusAnnihilation)");
  fAnnihilationCmd->SetGuidance("1 = enable, 0 = disable");
  fAnnihilationCmd->SetParameterName("enableAnn", false);
  fAnnihilationCmd->SetRange("enableAnn>=0 && enableAnn<=1");
  fAnnihilationCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fListCmd;
  delete fVerbCmd;
  delete fPhysDir;
  delete fIonisationCmd;
  delete fBremsstrahlungCmd;
  delete fScatterCmd;
  delete fAnnihilationCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand *command,
                                       G4String newValue)
{
  if (command == fVerbCmd)
  {
    G4LossTableManager::Instance()->SetVerbose(
        fVerbCmd->GetNewIntValue(newValue));
  }
  else if (command == fListCmd)
  {
    fPhysicsList->AddPhysicsList(newValue);
  }
  else if (command == fIonisationCmd)
  {
    G4bool enable = (fIonisationCmd->GetNewIntValue(newValue) == 1);
    fPhysicsList->SetEnableIonisation(enable);
    G4cout << "Ionisation process " << (enable ? "enabled" : "disabled") << G4endl;
  }
  else if (command == fBremsstrahlungCmd)
  {
    G4bool enable = (fBremsstrahlungCmd->GetNewIntValue(newValue) == 1);
    fPhysicsList->SetEnableBremsstrahlung(enable);
    G4cout << "Bremsstrahlung process " << (enable ? "enabled" : "disabled") << G4endl;
  }
  else if (command == fScatterCmd)
  {
    G4bool enable = (fScatterCmd->GetNewIntValue(newValue) == 1);
    fPhysicsList->SetEnableScatter(enable);
    G4cout << "Multiple scattering process " << (enable ? "enabled" : "disabled") << G4endl;
  }
  else if (command == fAnnihilationCmd)
  {
    G4bool enable = (fAnnihilationCmd->GetNewIntValue(newValue) == 1);
    fPhysicsList->SetEnableAnnihilation(enable);
    G4cout << "Positron annihilation process " << (enable ? "enabled" : "disabled") << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
