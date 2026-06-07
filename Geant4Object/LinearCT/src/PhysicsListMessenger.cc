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

PhysicsListMessenger::PhysicsListMessenger(PhysicsList* pPhys)
:fPhysicsList(pPhys)
{
  fPhysDir = new G4UIdirectory("/testem/phys/");
  fPhysDir->SetGuidance("physics list commands");
   
  fListCmd = new G4UIcmdWithAString("/testem/phys/addPhysics",this);  
  fListCmd->SetGuidance("Add modula physics list.");
  fListCmd->SetParameterName("PList",false);
  fListCmd->AvailableForStates(G4State_PreInit);  

  fVerbCmd = new G4UIcmdWithAnInteger("/testem/phys/verbose",this);  
  fVerbCmd->SetGuidance("Set verbose level for processes");
  fVerbCmd->SetParameterName("pVerb",false);
  fVerbCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fPhotoEffectCmd = new G4UIcommand("/testem/PhotoEffect", this);
  fPhotoEffectCmd->SetGuidance("Set detector size with one parameters: length in cm");
  G4UIparameter* PhotoEffect = new G4UIparameter("value", 'd', false);  // Double type, not optional
  fPhotoEffectCmd->SetParameter(PhotoEffect);

  fComptonScatterCmd = new G4UIcommand("/testem/ComptonScatter", this);
  fComptonScatterCmd->SetGuidance("Set detector size with one parameters: length in cm");
  G4UIparameter* ComptonScatter = new G4UIparameter("value", 'd', false);  // Double type, not optional
  fComptonScatterCmd->SetParameter(ComptonScatter);

  fRayleighScatterCmd = new G4UIcommand("/testem/RayleighScatter", this);
  fRayleighScatterCmd->SetGuidance("Set detector size with one parameters: length in cm");
  G4UIparameter* RayleighScatter = new G4UIparameter("value", 'd', false);  // Double type, not optional
  fRayleighScatterCmd->SetParameter(RayleighScatter);

  fIonizationCmd = new G4UIcommand("/testem/Ionization", this);
  fIonizationCmd->SetGuidance("Set detector size with one parameters: length in cm");
  G4UIparameter* Ionization = new G4UIparameter("value", 'd', false);  // Double type, not optional
  fIonizationCmd->SetParameter(Ionization);

  fMultipleScatteringCmd = new G4UIcommand("/testem/MultipleScattering", this);
  fMultipleScatteringCmd->SetGuidance("Set detector size with one parameters: length in cm");
  G4UIparameter* MultipleScattering = new G4UIparameter("value", 'd', false);  // Double type, not optional
  fMultipleScatteringCmd->SetParameter(MultipleScattering);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsListMessenger::~PhysicsListMessenger()
{
  delete fListCmd;
  delete fVerbCmd;
  delete fPhysDir;  
  delete fPhotoEffectCmd;
  delete fComptonScatterCmd;
  delete fRayleighScatterCmd;
  delete fIonizationCmd;
  delete fMultipleScatteringCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsListMessenger::SetNewValue(G4UIcommand* command,
                                          G4String newValue)
{       
  if( command == fVerbCmd ) { 
    G4LossTableManager::Instance()->SetVerbose(
      fVerbCmd->GetNewIntValue(newValue));
  }

  if( command == fListCmd ){ 
    fPhysicsList->AddPhysicsList(newValue);
   }else if (command == fPhotoEffectCmd) {
        std::istringstream iss(newValue);
        G4double PhotoEffect;
        iss >> PhotoEffect;
        fPhysicsList->SetPhotoEffect(PhotoEffect);
    }else if (command == fComptonScatterCmd) {
        std::istringstream iss(newValue);
        G4double ComptonScatter;
        iss >> ComptonScatter;
        fPhysicsList->SetComptonScatter(ComptonScatter);
    }else if (command == fRayleighScatterCmd) {
        std::istringstream iss(newValue);
        G4double RayleighScatter;
        iss >> RayleighScatter;
        fPhysicsList->SetRayleighScatter(RayleighScatter);
    }else if (command == fMultipleScatteringCmd) {
        std::istringstream iss(newValue);
        G4double MultipleScattering;
        iss >> MultipleScattering;
        fPhysicsList->SetMSC(MultipleScattering);
    }else if (command == fIonizationCmd) {
        std::istringstream iss(newValue);
        G4double Ionization;
        iss >> Ionization;
        fPhysicsList->SetIonization(Ionization);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
