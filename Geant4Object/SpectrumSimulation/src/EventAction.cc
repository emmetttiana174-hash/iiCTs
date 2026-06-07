#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "DetectorHit.hh"
#include "FilePathManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4THitsCollection.hh"
#include "G4Threading.hh"
#include "G4VHitsCollection.hh"
#include "G4ios.hh"
#include "TrackerHit.hh"

#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

std::mutex outputFileMutex;

EventAction::EventAction(DetectorConstruction* detector)
    : G4UserEventAction(),
      fAnalysisManager(G4AnalysisManager::Instance()),
      fDetector(detector),
      fEnergyOutputFile(nullptr)
{
}

EventAction::~EventAction()
{
    if (fEnergyOutputFile) {
        if (fEnergyOutputFile->is_open()) {
            fEnergyOutputFile->close();
        }
        delete fEnergyOutputFile;
    }
}

void EventAction::OpenEnergyOutputFile()
{
    if (fEnergyOutputFile && fEnergyOutputFile->is_open()) {
        return;
    }

    std::ostringstream pathStream;
    pathStream << FilePathManager::GetInstance()->GetOutputFilePath()
               << ".txt";
    fCurrentEnergyOutputPath = pathStream.str();

    const G4int threadId = G4Threading::G4GetThreadId();
    const bool writeHeader = (threadId <= 0);
    std::ios_base::openmode mode = std::ios::out;
    mode |= writeHeader ? std::ios::trunc : std::ios::app;

    {
        std::lock_guard<std::mutex> lock(outputFileMutex);
        fEnergyOutputFile = new std::ofstream(fCurrentEnergyOutputPath, mode);
        if (!fEnergyOutputFile->is_open()) {
            G4cerr << "Error: unable to open energy output file "
                   << fCurrentEnergyOutputPath << G4endl;
            return;
        }

        G4cout << "[iiCTs Debug][EventAction] Energy TXT output opened: "
               << fCurrentEnergyOutputPath
               << (writeHeader ? " (truncate/header)" : " (append)")
               << G4endl;

        if (writeHeader) {
            *fEnergyOutputFile << "Energy(KeV) X(cm) Y(cm) Z(cm) px py pz" << G4endl;
            fEnergyOutputFile->flush();
        }
    }
}

void EventAction::BeginOfEventAction(const G4Event*)
{
    fDetectorEdep.clear();
    OpenEnergyOutputFile();
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    G4HCofThisEvent* hce = event->GetHCofThisEvent();
    if (!hce) {
        static G4int s_noHceCount = 0;
        if (s_noHceCount < 10) {
            G4cerr << "[iiCTs Debug][EventAction] Event " << event->GetEventID()
                   << " has no hits collection container." << G4endl;
            ++s_noHceCount;
        }
        return;
    }

    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    G4int HCID = -1;
    if (sdManager) {
        HCID = sdManager->GetCollectionID("DetectorHitsCollection");
    }

    if (HCID >= 0) {
        G4THitsCollection<DetectorHit>* hitsCollection =
            static_cast<G4THitsCollection<DetectorHit>*>(hce->GetHC(HCID));
        if (!hitsCollection) {
            static G4int s_nullCollectionCount = 0;
            if (s_nullCollectionCount < 10) {
                G4cerr << "[iiCTs Debug][EventAction] Event " << event->GetEventID()
                       << " collection ID " << HCID << " is null." << G4endl;
                ++s_nullCollectionCount;
            }
            return;
        }

        static G4int s_printedEventSummaries = 0;
        if (s_printedEventSummaries < 20) {
            G4cout << "[iiCTs Debug][EventAction] Event " << event->GetEventID()
                   << " detector hits = " << hitsCollection->entries() << G4endl;
            ++s_printedEventSummaries;
        }

        std::stringstream buffer;
        for (G4int i = 0; i < hitsCollection->entries(); ++i) {
            DetectorHit* hit = (*hitsCollection)[i];
            if (!hit) continue;

            G4double incidentEnergy = hit->GetIncidentEnergy();
            G4ThreeVector position = hit->GetPos();
            G4ThreeVector momentum = hit->GetMomentum();
            if (incidentEnergy > 0. && hit->GetParticleName() == "gamma") {
                fAnalysisManager->FillH1(0, 0.5);
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
    } else {
        static G4int s_badHcidCount = 0;
        if (s_badHcidCount < 10) {
            G4cerr << "[iiCTs Debug][EventAction] DetectorHitsCollection ID not found. HCID="
                   << HCID << G4endl;
            ++s_badHcidCount;
        }
    }
}
