#include "DetectorSD.hh"
#include "DetectorHit.hh"
#include "G4Step.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"

DetectorSD::DetectorSD(const G4String& name)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   fHCID(-1)
{
    collectionName.insert("DetectorHitsCollection");
}

DetectorSD::~DetectorSD() {}

void DetectorSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new G4THitsCollection<DetectorHit>(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4Track* track = step->GetTrack();
    if (!track || track->GetDefinition()->GetParticleName() != "gamma") {
        return false;
    }

    // 创建新的 DetectorHit 并记录信息
    DetectorHit* hit = new DetectorHit();
    hit->RecordStepInfo(step); // 使用 RecordStepInfo 设置动量、位置等
    hit->SetDetectorID(0);

    // 调试输出
    // G4cout << "Gamma Momentum: (" << hit->GetMomentum().x() << ", " 
    //        << hit->GetMomentum().y() << ", " << hit->GetMomentum().z() << ") MeV" << G4endl;

    fHitsCollection->insert(hit);
    return true;
}

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
    // G4int nHits = fHitsCollection->entries();
    // G4cout << "DetectorSD: Collected " << nHits << " gamma hits" << G4endl;
}