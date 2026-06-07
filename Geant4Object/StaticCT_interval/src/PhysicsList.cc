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
/// \file medical/GammaTherapy/src/PhysicsList.cc
/// \brief Implementation of the PhysicsList class
//
//
//---------------------------------------------------------------------------
//
// ClassName:   PhysicsList
//
// Author:      V.Ivanchenko 03.05.2004
//
// Modified:
// 16.11.06 Use components from physics_lists subdirectory (V.Ivanchenko)
// 16.05.07 Use renamed EM components from physics_lists (V.Ivanchenko)
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"
#include "PhysicsListMessenger.hh"

#include "G4BiasingProcessInterface.hh"
#include "G4ParticleTable.hh"
#include "G4ios.hh"
#include "G4Gamma.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLowEPPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4HadronElasticPhysics.hh"
#include "G4HadronInelasticQBBC.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"
#include "G4UnitsTable.hh"
#include "G4LossTableManager.hh"
#include "G4EmParameters.hh"
#include "G4ProcessManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4EmLowEPPhysics.hh" 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList(): G4VModularPhysicsList(), fPhotoEffect(0.0),fComptonScatter(0.0),fRayleighScatter(0.0),fIonization(0.0),fMultipleScatter(0.0)
{
  fHelIsRegisted = false;
  fBicIsRegisted = false;
  fIonIsRegisted = false;
  fGnucIsRegisted = false;
  fStopIsRegisted = false;

  fVerbose = 1;

  SetDefaultCutValue(0.001*mm);
  
  fMessenger = new PhysicsListMessenger(this);

  G4cout << "PhysicsList constructor: fPhotoEffect = " << fPhotoEffect << G4endl;
  RegisterPhysics(new G4EmLivermorePhysics());
  RegisterPhysics(new G4DecayPhysics());
}
PhysicsList::~PhysicsList()
{
  delete fMessenger;
}
void PhysicsList::ConstructParticle()
{
  if(fVerbose > 0) {
  }
  G4VModularPhysicsList::ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{

  if(fVerbose > 0) {
  }
  G4VModularPhysicsList::ConstructProcess();

  G4EmParameters* param = G4EmParameters::Instance();
  
  // // 读取 physics_control.txt 文件
  // std::ifstream inputFile("/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/HelicalCT/build/physics.gwl");
  // if (!inputFile.is_open()) {
  //   G4cerr << "Error: Cannot open physics_control.txt" << G4endl;
  //   // 设置默认值（例如全开，0 表示不移除）
  //   fPhotoEffect = 0.0;
  //   fComptonScatter = 0.0;
  //   fRayleighScatter = 0.0;
  //   fIonization = 0.0;
  //   fMultipleScatter = 0.0;
  // } else {
  //   // 读取五行数据
  //   G4int value;
  //   inputFile >> value; fPhotoEffect = value;
  //   inputFile >> value; fComptonScatter = value;
  //   inputFile >> value; fRayleighScatter = value;
  //   inputFile >> value; fIonization = value;
  //   inputFile >> value; fMultipleScatter = value;
  //   // 检查文件是否正确读取
  //   if (inputFile.fail()) {
  //     G4cerr << "Error: Failed to read 5 values from physics_control.txt" << G4endl;
  //     // 设置默认值
  //     fPhotoEffect = 0.0;
  //     fComptonScatter = 0.0;
  //     fRayleighScatter = 0.0;
  //     fIonization = 0.0;
  //     fMultipleScatter = 0.0;
  //   }
  //   inputFile.close();
  // }

  // G4cout << "fPhotoEffect" << fPhotoEffect << G4endl;
  // G4cout << "fComptonScatter" << fComptonScatter << G4endl;
  // G4cout << "fRayleighScatter" << fRayleighScatter << G4endl;
  // G4cout << "fIonization" << fIonization << G4endl;
  // G4cout << "fMultipleScatter" << fMultipleScatter << G4endl;
  // param->SetNumberOfBinsPerDecade(10);
  // param->SetVerbose(1);
  // ///
  // // 获取伽马粒子的进程管理器
  // G4ProcessManager* pManager = G4Gamma::Gamma()->GetProcessManager();
  // G4ProcessVector* processList = pManager->GetProcessList();
  // G4cout << "Gamma processes:" << G4endl;
  // for (G4int i = 0; i < processList->size(); i++) {
  //   G4cout << "  Process " << i << ": " << (*processList)[i]->GetProcessName() << G4endl;
  // }
  //   // 移除康普顿散射
  //   if (fComptonScatter == 1.0) {
  //     for (G4int i = 0; i < processList->size(); i++) {
  //         if ((*processList)[i]->GetProcessName() == "compt") {
  //             pManager->RemoveProcess(i);
  //             G4cout << "Removed Compton scattering process" << G4endl;
  //             break;
  //         }
  //     }
  //   }
  //   // 移除光电效应
  //   if (fPhotoEffect == 1.0) {
  //     for (G4int i = 0; i < processList->size(); i++) {
  //         if ((*processList)[i]->GetProcessName() == "phot") {
  //             pManager->RemoveProcess(i);
  //             G4cout << "Removed Photoelectric effect process" << G4endl;
  //             break;
  //         }
  //     }
  //   }
  //   // 移除瑞利散射
  //   if (fRayleighScatter == 1.0) {
  //     for (G4int i = 0; i < processList->size(); i++) {
  //         if ((*processList)[i]->GetProcessName() == "rayl") {
  //             pManager->RemoveProcess(i);
  //             G4cout << "Removed Rayleigh scattering process" << G4endl;
  //             break;
  //         }
  //     }
  //   }
  //   // 处理电子的进程
  //   G4ProcessManager* electronManager = G4Electron::Electron()->GetProcessManager();
  //   G4ProcessVector* electronProcessList = electronManager->GetProcessList();

  //   if (fIonization == 1.0) {
  //     for (G4int i = 0; i < electronProcessList->size(); i++) {
  //       if ((*electronProcessList)[i]->GetProcessName() == "eIoni") {
  //         electronManager->RemoveProcess(i);
  //         G4cout << "Removed Ionization process for electron" << G4endl;
  //         break;
  //       }
  //     }
  //   }

  //   if (fMultipleScatter == 1.0) {
  //     for (G4int i = 0; i < electronProcessList->size(); i++) {
  //       if ((*electronProcessList)[i]->GetProcessName() == "msc") {
  //         electronManager->RemoveProcess(i);
  //         G4cout << "Removed Multiple Scattering process for electron" << G4endl;
  //         break;
  //       }
  //     }
  //   }
  ///
  // G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("gamma");
  // G4ProcessManager* pManager = particle->GetProcessManager();
  // G4ProcessVector* processList = pManager->GetProcessList();
}
void PhysicsList::UpdateProcesses()
{
    // G4cout << "Entering UpdateProcesses()" << G4endl;
    // G4cout << "fPhotoEffect = " << fPhotoEffect << G4endl;
    // G4cout << "fComptonScatter = " << fComptonScatter << G4endl;
    // G4cout << "fRayleighScatter = " << fRayleighScatter << G4endl;
    // G4cout << "fIonization = " << fIonization << G4endl;
    // G4cout << "fMultipleScatter = " << fMultipleScatter << G4endl;

    // 获取伽马粒子的进程管理器
    G4ProcessManager* pManager = G4Gamma::Gamma()->GetProcessManager();
    G4ProcessVector* processList = pManager->GetProcessList();
    G4cout << "Gamma processes before update:" << G4endl;
    // for (G4int i = 0; i < processList->size(); i++) {
    //     G4cout << "  Process " << i << ": " << (*processList)[i]->GetProcessName() << G4endl;
    // }

    // 移除康普顿散射
    if (fComptonScatter == 1.0) {
        for (G4int i = 0; i < processList->size(); i++) {
            if ((*processList)[i]->GetProcessName() == "compt") {
                pManager->RemoveProcess(i);
                G4cout << "Removed Compton scattering process" << G4endl;
                break;
            }
        }
    }

    // 移除光电效应
    if (fPhotoEffect == 1.0) {
        for (G4int i = 0; i < processList->size(); i++) {
            if ((*processList)[i]->GetProcessName() == "phot") {
                pManager->RemoveProcess(i);
                G4cout << "Removed Photoelectric effect process" << G4endl;
                break;
            }
        }
    }

    // 移除瑞利散射
    if (fRayleighScatter == 1.0) {
        for (G4int i = 0; i < processList->size(); i++) {
            if ((*processList)[i]->GetProcessName() == "rayl") {
                pManager->RemoveProcess(i);
                G4cout << "Removed Rayleigh scattering process" << G4endl;
                break;
            }
        }
    }

    // 处理电子的进程
    G4ProcessManager* electronManager = G4Electron::Electron()->GetProcessManager();
    G4ProcessVector* electronProcessList = electronManager->GetProcessList();

    if (fIonization == 1.0) {
        for (G4int i = 0; i < electronProcessList->size(); i++) {
            if ((*electronProcessList)[i]->GetProcessName() == "eIoni") {
                electronManager->RemoveProcess(i);
                G4cout << "Removed Ionization process for electron" << G4endl;
                break;
            }
        }
    }

    if (fMultipleScatter == 1.0) {
        for (G4int i = 0; i < electronProcessList->size(); i++) {
            if ((*electronProcessList)[i]->GetProcessName() == "msc") {
                electronManager->RemoveProcess(i);
                G4cout << "Removed Multiple Scattering process for electron" << G4endl;
                break;
            }
        }
    }

    // 通知运行管理器物理过程已更改
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddPhysicsList(const G4String& name)
{
  if(fVerbose > 0) {
    //G4cout << "### PhysicsList Add Physics <" << name 
           //<< "> " << G4endl;
  }
  if (name == "emstandard") {
    ReplacePhysics(new G4EmStandardPhysics());

  } else if (name == "emstandard_opt1") {
    ReplacePhysics(new G4EmStandardPhysics_option1());

  } else if (name == "emstandard_opt2") {
    ReplacePhysics(new G4EmStandardPhysics_option2());

  } else if (name == "emstandard_opt3") {
    ReplacePhysics(new G4EmStandardPhysics_option3());

  } else if (name == "emstandard_opt4") {
    ReplacePhysics(new G4EmStandardPhysics_option4());

  } else if (name == "emlivermore") {
    ReplacePhysics(new G4EmLivermorePhysics());

  } else if (name == "empenelope") {
    ReplacePhysics(new G4EmPenelopePhysics());

  } else if (name == "emlowenergy") {
    ReplacePhysics(new G4EmLowEPPhysics());

  } else if (name == "elastic" && !fHelIsRegisted) {
    RegisterPhysics(new G4HadronElasticPhysics());
    fHelIsRegisted = true;
    
  } else if (name == "binary" && !fBicIsRegisted) {
    RegisterPhysics(new G4HadronInelasticQBBC());
    fBicIsRegisted = true;
    
  } else if (name == "binary_ion" && !fIonIsRegisted) {
    RegisterPhysics(new G4IonBinaryCascadePhysics());
    fIonIsRegisted = true;

  } else if (name == "gamma_nuc" && !fGnucIsRegisted) {
    RegisterPhysics(new G4EmExtraPhysics());
    fGnucIsRegisted = true;

  } else if (name == "stopping" && !fStopIsRegisted) {
    RegisterPhysics(new G4StoppingPhysics());
    fStopIsRegisted = true;
    
  }
}
void PhysicsList::SetPhotoEffect(G4double PhotoEffect) {
    fPhotoEffect = PhotoEffect;
    UpdateProcesses();
    G4cout << "fPhotoEffect is " << fPhotoEffect << G4endl;
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
void PhysicsList::SetComptonScatter(G4double ComptonScatter) {
    fComptonScatter = ComptonScatter;
    UpdateProcesses();
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
void PhysicsList::SetRayleighScatter(G4double RayleighScatter) {
    fRayleighScatter = RayleighScatter;
    UpdateProcesses();
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
void PhysicsList::SetIonization(G4double Ionization) {
    fIonization = Ionization;
    UpdateProcesses();
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}
void PhysicsList::SetMSC(G4double MSC) {
    fMultipleScatter = MSC;
    UpdateProcesses();
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
}