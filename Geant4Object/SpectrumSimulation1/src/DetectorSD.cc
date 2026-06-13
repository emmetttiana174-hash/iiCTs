#include "DetectorSD.hh"
#include "DetectorHit.hh"
#include "G4HCofThisEvent.hh"
#include "G4ParticleDefinition.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4ios.hh"

DetectorSD::DetectorSD(const G4String& name)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   fHCID(-1)
{
    collectionName.insert("DetectorHitsCollection");
    G4cout << "[iiCTs Debug][DetectorSD] Created SD '" << name
           << "' with collection '" << collectionName[0] << "'" << G4endl;
}

DetectorSD::~DetectorSD() {}

void DetectorSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new G4THitsCollection<DetectorHit>(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
        G4cout << "[iiCTs Debug][DetectorSD] Collection ID = " << fHCID << G4endl;
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool DetectorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4Track* track = step->GetTrack();
    if (!track) {
        return false;
    }

    if (track->GetDefinition()->GetParticleName() != "gamma") {
        return false;
    }

    DetectorHit* hit = new DetectorHit();
    hit->RecordStepInfo(step);
    hit->SetDetectorID(0);
    fHitsCollection->insert(hit);

    static G4int s_printedHits = 0;
    if (s_printedHits < 10) {
        const G4ThreeVector pos = hit->GetPos();
        const G4ThreeVector mom = hit->GetMomentum();
        G4cout << "[iiCTs Debug][DetectorSD] Gamma hit recorded: E="
               << hit->GetIncidentEnergy()
               << " pos(cm)=(" << pos.x() / cm << ", " << pos.y() / cm << ", " << pos.z() / cm << ")"
               << " mom=(" << mom.x() << ", " << mom.y() << ", " << mom.z() << ")"
               << G4endl;
        ++s_printedHits;
    }

    return true;
}

void DetectorSD::EndOfEvent(G4HCofThisEvent*)
{
    static G4int s_printedEvents = 0;
    if (s_printedEvents < 20 && fHitsCollection) {
        G4cout << "[iiCTs Debug][DetectorSD] EndOfEvent hits = "
               << fHitsCollection->entries() << G4endl;
        ++s_printedEvents;
    }
}
