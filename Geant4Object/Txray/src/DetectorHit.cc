#include "DetectorHit.hh"
#include <G4UnitsTable.hh>
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"

// 线程局部存储的分配器
G4ThreadLocal G4Allocator<DetectorHit>* DetectorHitAllocator = nullptr;

// ========== DetectorHit 类的实现 ==========

DetectorHit::DetectorHit()
  : G4VHit(), 
    fPos(G4ThreeVector()), 
    fMomentum(G4ThreeVector()), 
    fEdep(0.),
    fIncidentEnergy(0.),
    fParticleName("")
{}

DetectorHit::DetectorHit(const DetectorHit& right)
  : G4VHit(right), 
    fPos(right.fPos), 
    fMomentum(right.fMomentum), 
    fEdep(right.fEdep),
    fIncidentEnergy(right.fIncidentEnergy),
    fParticleName(right.fParticleName)
{}

DetectorHit::~DetectorHit() {}

const DetectorHit& DetectorHit::operator=(const DetectorHit& right) {
    fPos = right.fPos;
    fMomentum = right.fMomentum;
    fEdep = right.fEdep;
    fIncidentEnergy = right.fIncidentEnergy;
    fParticleName = right.fParticleName;
    return *this;
}

void DetectorHit::SetParticleName(const G4String& name) {
    fParticleName = name;
}

G4String DetectorHit::GetParticleName() const {
    return fParticleName;
}

void DetectorHit::RecordStepInfo(const G4Step* step) {
    G4Track* track = step->GetTrack();
    if (track && track->GetDefinition()->GetParticleName() == "gamma") {
        fParticleName = track->GetDefinition()->GetParticleName();
        fMomentum = track->GetMomentum() * 1000; // GeV -> MeV
        fIncidentEnergy = track->GetKineticEnergy() * 1000; // GeV -> MeV
    }

    if (G4StepPoint* postPoint = step->GetPostStepPoint()) {
        fPos = postPoint->GetPosition();
    }

    AddEnergy(step->GetTotalEnergyDeposit());
}

void DetectorHit::SetIncidentEnergy(G4double energy) {
    fIncidentEnergy = energy;
}

G4double DetectorHit::GetIncidentEnergy() const {
    return fIncidentEnergy;
}

void DetectorHit::SetMomentum(const G4ThreeVector& momentum) {
    fMomentum = momentum;
}

G4ThreeVector DetectorHit::GetMomentum() const {
    return fMomentum;
}

void DetectorHit::SetStep(G4Step* step) {
    fStep = step;
}

G4Step* DetectorHit::GetStep() const {
    return fStep;
}

void DetectorHit::SetPos(const G4ThreeVector& pos) {
    fPos = pos;
}

G4ThreeVector DetectorHit::GetPos() const {
    return fPos;
}

void DetectorHit::AddEnergy(G4double edep) {
    fEdep += edep;
}

void DetectorHit::SetDetectorID(G4int id) {
    fDetectorID = id;
}

G4int DetectorHit::GetDetectorID() const {
    return fDetectorID;
}

void DetectorHit::Draw() {}

void DetectorHit::Print() {
    G4cout << "Hit: Edep = " << G4BestUnit(fEdep, "Energy")
           << ", Incident Energy = " << G4BestUnit(fIncidentEnergy, "Energy")
           << ", Position = " << G4BestUnit(fPos, "Length")
           << ", Momentum = (" << fMomentum.x() << ", " << fMomentum.y() << ", " << fMomentum.z() << ") MeV"
           << ", Particle = " << fParticleName
           << G4endl;
}