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

#include "DetectorSD.hh"
#include "G4VHitsCollection.hh"
#include "G4RunManager.hh"
#include "Run.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Track.hh"
#include "G4Positron.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "DetectorHit.hh"
#include "TrackInformation.hh"
G4ThreadLocal DetectorSD* DetectorSD::instance = nullptr;

DetectorSD::DetectorSD(const G4String& name)
    : G4VSensitiveDetector(name), fHitsCollection(nullptr), hcID(-1) {
    collectionName.push_back("DetectorHitsCollection");
}

DetectorSD::~DetectorSD()
{}

void DetectorSD::Initialize(G4HCofThisEvent* hce)
{
  if (!instance) { instance = this; }
  fHitsCollection = new G4THitsCollection<DetectorHit>(SensitiveDetectorName, collectionName[0]);
  
  if (hcID < 0) {  // ⚠️ 直接使用类成员变量
    hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  }

  hce->AddHitsCollection(hcID, fHitsCollection);
}

G4bool DetectorSD::ProcessHits(G4Step* aStep, G4TouchableHistory* /*history*/) {
    G4double edep = aStep->GetTotalEnergyDeposit();
    // G4String processName = aStep->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
    // G4String particleName = aStep->GetTrack()->GetParticleDefinition()->GetParticleName();
    // G4cout << "SD - Particle: " << particleName << ", Process: " << processName 
    //        << ", Edep: " << edep / CLHEP::keV << " keV" << G4endl;
    if (edep <= 0.) return false; 
    // 获取单元的物理信息 -----------------------------------------------
    G4TouchableHistory* touchable = (G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable());
    //const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int detectorID = touchable->GetCopyNumber();
    G4int copyNo = touchable->GetCopyNumber();
    // 可选：获取全局坐标（用于校验几何）
    //G4ThreeVector worldPos = aStep->GetPostStepPoint()->GetPosition();
    //G4ThreeVector localPos = touchable->GetHistory()->GetTopTransform().TransformPoint(worldPos);
    //G4ThreeVector pos = aStep->GetPostStepPoint()->GetPosition();
    
    // 设置必要参数
    DetectorHit* hit = new DetectorHit();
    //hit->SetEdep(edep);
    //hit->SetDetectorID(detectorID);
    hit->RecordStepInfo(aStep);
    //hit->SetCopyNo(copyNo);       // ⚠️ 必须标识单元
    //hit->SetWorldPos(worldPos);   // 世界坐标系位置（用于可视化校验）
    //hit->SetLocalPos(localPos);   // 本地坐标系位置（如需要）
    //hit->AddEnergy(edep);            // 累加能量沉积
    //hit->SetPosition(pos);
    ///
    hit->SetDetectorID(aStep->GetPreStepPoint()->GetTouchableHandle()->GetCopyNumber());
    hit->SetStep(aStep); // 存储 G4Step
    ///
    const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();
    if (process != nullptr) {
        hit->SetProcessName(process->GetProcessName());
    }
    hit->SetParticleName(aStep->GetTrack()->GetParticleDefinition()->GetParticleName());
    G4Track* track = aStep->GetTrack();
    TrackInformation* info = dynamic_cast<TrackInformation*>(track->GetUserInformation());
    // if (info) {
    // if (info->IsScattered()) {
    //     hit->SetScattered(true);  // 设置散射标志
    // }
    // if (info->IsPrimary()) {
    //     hit->SetPrimary(true);    // 设置主信号标志
    // }
    // }
    if (info && info->IsScattered()) {
        hit->SetScattered(true); // 假设 DetectorHit 有 SetScattered 方法
    } else {
        hit->SetScattered(false);
    }
    if (info && info->IsPrimary()) {
        hit->SetPrimary(true); // 假设 DetectorHit 有 SetScattered 方法
    } else {
        hit->SetPrimary(false);
    }
    ///
    fHitsCollection->insert(hit);

    return true;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorSD::clear()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void DetectorSD::PrintAll()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....