#include "EventAction.hh"
#include "G4Event.hh"
#include "G4VHitsCollection.hh"
#include "G4THitsCollection.hh"
#include "TrackerHit.hh"
#include "DetectorHit.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4AnalysisManager.hh"
#include "DetectorConstruction.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include "G4VProcess.hh"
#include "G4HCofThisEvent.hh"
#include "G4Track.hh"
EventAction::EventAction(DetectorConstruction* detector)
  : G4UserEventAction(),
    fAnalysisManager(G4AnalysisManager::Instance()),
    fDetectorEdep(),
    fDetector(detector) {
}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event) { 
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  fDetectorEdep.clear();
}

void EventAction::EndOfEventAction(const G4Event* event) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    std::map<G4int, G4double> detectorEdep;  // 用于存储每个探测器单元的能量沉积
    std::map<G4int, G4double> comptEdep;
    std::map<G4int, G4double> photEdep;
    std::map<G4int, G4double> scatteredEdep; // 来自散射光子的能量
    std::map<G4int, G4double> primaryEdep;   // 主信号能量
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce) {
        return;
    }
    G4THitsCollection<DetectorHit>* hitsCollection = static_cast<G4THitsCollection<DetectorHit>*>(hce->GetHC(0));
    if (!hitsCollection) {
        return;
    }
    for (G4int i = 0; i < hitsCollection->entries(); ++i) {
        DetectorHit* hit = (*hitsCollection)[i];
        if (!hit) continue;
        G4int detectorID = hit->GetDetectorID();
        G4double edep = hit->GetEdep();
        edep *= 1000 ;
        if (edep > 0.) {
            //G4cout << 111 << G4endl;
            detectorEdep[detectorID] += edep;
            ///
            // if (hit->IsScattered()) {
            //     scatteredEdep[detectorID] += edep; // 添加到散射能量
            // }
            // if (hit->IsPrimary()) {
            //     primaryEdep[detectorID] += edep;
            // }
            ///
            // 检查是否为散射粒子（需要从 hit 中获取创建过程）
            const G4Step* step = hit->GetStep(); // 假设 DetectorHit 存储了 G4Step
            // G4cout << "Event - Particle: " << hit->GetParticleName() << ", Process: " << hit->GetProcessName() 
            //        << ", Edep: " << edep / CLHEP::keV << " keV" << G4endl;
            // if (step) {
            //     //G4cout << 111 << G4endl;
            //     // const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
            //     // G4Track* track = step->GetTrack();
            //     // const G4VProcess* creatorProcess = track->GetCreatorProcess();
            //     // G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
            //     //G4String processName = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
            //     // G4cout << "particle" << particleName << "process" << processName << G4endl;
            //     // G4cout << "Event - Particle: " << hit->GetParticleName() << ", Process: " << hit->GetProcessName() << G4endl;
            //     //直接过程：伽马光子的相互作用
            //     G4String processName = hit->GetProcessName();
            //     //G4cout << processName << G4endl;
            //     //comptEdep[detectorID] += edep;
            //     if (processName == "compt") {
            //         comptEdep[detectorID] += edep;
            //     } 
            //     else if (processName == "phot") {
            //         photEdep[detectorID] += edep;
            //     }
            //     // if (process) {
            //     //     G4String processName = process->GetProcessName();
            //     //     //G4cout << processName << G4endl;
            //     //     //comptEdep[detectorID] += edep;
            //     //     if (processName == "compt") {
            //     //         comptEdep[detectorID] += edep;
            //     //     } else if (processName == "phot") {
            //     //         photEdep[detectorID] += edep;
            //     //     }
            //     // }
            // }
            //     // if (creatorProcess) {
            //     //     G4String creatorName = creatorProcess->GetProcessName();
            //     //     G4cout << creatorName << G4endl;
            //     //     if (creatorName == "compt") {
            //     //         comptEdep[detectorID] += edep; // 反冲电子的后续沉积
            //     //     } else if (creatorName == "phot") {
            //     //         photEdep[detectorID] += edep; // 光电子的后续沉积
            //     //     }
            //     // }
            // }
        }
    for (const auto& entry : detectorEdep) {
        G4int detectorID = entry.first;
        G4double totalEdep = entry.second;
        //totalEdep *= 100;
        analysisManager->FillH1(0, detectorID, totalEdep);
    }
    // for (const auto& entry : comptEdep) {
    //     G4int detectorID = entry.first;
    //     G4double comptEnergy = entry.second ; 
    //     //comptEnergy *= 100.;
    //     analysisManager->FillH1(1, detectorID, comptEnergy); // 散射能量沉积
    // }
    // for (const auto& entry : photEdep) {
    //     G4int detectorID = entry.first;
    //     G4double photEnergy = entry.second ; 
    //     //photEnergy *= 100.;
    //     analysisManager->FillH1(2, detectorID, photEnergy); // 散射能量沉积
    // }
    // for (const auto& entry : scatteredEdep) {
    //     G4int detectorID = entry.first;
    //     G4double scatteredEnergy = entry.second;
    //     analysisManager->FillH1(1, detectorID, scatteredEnergy); // 散射能量的新直方图
    // }
    // // 填充主信号直方图
    // for (const auto& entry : primaryEdep) {
    //     G4int detectorID = entry.first;
    //     G4double primaryEnergy = entry.second;
    //     analysisManager->FillH1(2, detectorID, primaryEnergy);
    // }
}
}