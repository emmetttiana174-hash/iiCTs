#include "EventAction.hh"
#include "G4Event.hh"
#include "G4VHitsCollection.hh"
#include "G4THitsCollection.hh"
#include "TrackerHit.hh"
#include "DetectorHit.hh"
#include "G4StepPoint.hh"
#include "G4Step.hh"
#include "G4AnalysisManager.hh"
#include "DetectorConstruction.hh"
#include "G4SDManager.hh"
#include <CLHEP/Units/PhysicalConstants.h>
#include <fstream>
#include <iostream>
#include <string>
#include <mutex>

// 创建一个全局互斥锁用于文件写入
std::mutex outputFileMutex;

EventAction::EventAction(DetectorConstruction *detector)
    : G4UserEventAction(),
      fAnalysisManager(G4AnalysisManager::Instance()),
      fDetectorEdep(),
      fDetector(detector),
      fEnergyOutputFile(nullptr)
{
    std::string outputPath = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/BeamCT9/xenergyoutput/1W_8_100keV_8mm_2500K_15mm_edit.txt";
    if (G4Threading::G4GetThreadId() == 0) {
        fEnergyOutputFile = new std::ofstream(outputPath);
        if (!fEnergyOutputFile->is_open()) {
            G4cerr << "错误: 无法打开输出文件 " << outputPath << G4endl;
        } else {
            *fEnergyOutputFile << "Energy(KeV) X(cm) Y(cm) Z(cm) px py pz" << G4endl;
            fEnergyOutputFile->flush();
        }
    } else {
        fEnergyOutputFile = new std::ofstream(outputPath, std::ios::app);
        if (!fEnergyOutputFile->is_open()) {
            G4cerr << "错误: 工作线程无法打开输出文件 " << outputPath << G4endl;
        }
    }
}

EventAction::~EventAction() {
    if (fEnergyOutputFile) {
        if (fEnergyOutputFile->is_open()) {
            fEnergyOutputFile->close();
        }
        delete fEnergyOutputFile;
    }
}

void EventAction::BeginOfEventAction(const G4Event *event) {
    fDetectorEdep.clear();
}

void EventAction::EndOfEventAction(const G4Event *event) {
    G4HCofThisEvent *hce = event->GetHCofThisEvent();
    if (!hce) return;
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    G4int HCID = -1;
    if (sdManager) {
        HCID = sdManager->GetCollectionID("DetectorHitsCollection");
    }
    if (HCID >= 0) {
        G4THitsCollection<DetectorHit>* hitsCollection = 
            static_cast<G4THitsCollection<DetectorHit>*>(hce->GetHC(HCID));
        if (!hitsCollection) return;
        std::stringstream buffer;
        for (G4int i = 0; i < hitsCollection->entries(); ++i) {
            DetectorHit *hit = (*hitsCollection)[i];
            if (!hit) continue;
            G4double incidentEnergy = hit->GetIncidentEnergy();
            G4ThreeVector position = hit->GetPos();
            G4ThreeVector momentum = hit->GetMomentum();
            if (incidentEnergy > 0. && hit->GetParticleName() == "gamma") {
                // 由于DetectorHit中已将能量和动量单位转为MeV
                // 这里直接输出即可，无需再除以CLHEP::MeV
                buffer << incidentEnergy << " "
                       << position.x() / CLHEP::cm << " "
                       << position.y() / CLHEP::cm << " "
                       << position.z() / CLHEP::cm << " "
                       << momentum.x() << " "
                       << momentum.y() << " "
                       << momentum.z() << G4endl;
            }
        }
        if (fEnergyOutputFile && fEnergyOutputFile->is_open() && !buffer.str().empty()) {
            std::lock_guard<std::mutex> lock(outputFileMutex);
            *fEnergyOutputFile << buffer.str();
        }
    }
}