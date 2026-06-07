#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "FilePathManager.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include <fstream>
#include <iostream>
#include <sstream>

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
    : G4VUserPrimaryGeneratorAction(),
      fDetector(detector),
      fParticleGun(nullptr),
      fCurrentIndex(0),
      fTotalParticles(0)
{
  fParticleGun = new G4ParticleGun(1);
  auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particle);

  fInputFilePath = FilePathManager::GetInstance()->GetInputFilePath();
  LoadParticleRecords();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  G4cout << "[iiCTs Debug][PrimaryGenerator] Total generated primaries: "
         << fTotalParticles << G4endl;
}

void PrimaryGeneratorAction::ReloadIfInputPathChanged()
{
  const G4String currentPath = FilePathManager::GetInstance()->GetInputFilePath();
  if (currentPath != fInputFilePath) {
    G4cout << "[iiCTs Debug][PrimaryGenerator] Input path changed from '"
           << fInputFilePath << "' to '" << currentPath << "'. Reloading source." << G4endl;
    fInputFilePath = currentPath;
    LoadParticleRecords();
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  ReloadIfInputPathChanged();

  if (fRecords.empty()) {
    static G4int s_noRecordErrCount = 0;
    if (s_noRecordErrCount < 5) {
      G4cerr << "[iiCTs Debug][PrimaryGenerator] ERROR: no particle records available; "
             << "no primary vertex generated for this event." << G4endl;
      ++s_noRecordErrCount;
    }
    return;
  }

  if (fCurrentIndex >= fRecords.size()) {
    fCurrentIndex = 0;
    G4cout << "[iiCTs Debug][PrimaryGenerator] Recycling source particles." << G4endl;
  }

  const auto& rec = fRecords[fCurrentIndex++];
  fParticleGun->SetParticlePosition(rec.position);
  fParticleGun->SetParticleMomentumDirection(rec.momentumDir);
  fParticleGun->SetParticleEnergy(rec.energy);
  fParticleGun->GeneratePrimaryVertex(anEvent);

  ++fTotalParticles;
  if (fTotalParticles <= 5) {
    G4cout << "[iiCTs Debug][PrimaryGenerator] Primary " << fTotalParticles
           << ": E(MeV)=" << rec.energy / MeV
           << " pos(mm)=(" << rec.position.x() / mm << ", "
           << rec.position.y() / mm << ", " << rec.position.z() / mm << ")"
           << " dir=(" << rec.momentumDir.x() << ", "
           << rec.momentumDir.y() << ", " << rec.momentumDir.z() << ")"
           << G4endl;
  } else if (fTotalParticles % 1000000 == 0) {
    G4cout << "Generated " << fTotalParticles << " particles so far" << G4endl;
  }
}

void PrimaryGeneratorAction::LoadParticleRecords()
{
  fRecords.clear();
  fCurrentIndex = 0;

  std::ifstream infile(fInputFilePath);
  if (!infile) {
    G4cerr << "[iiCTs Debug][PrimaryGenerator] Cannot open input file: "
           << fInputFilePath << G4endl;
    G4cerr << "[iiCTs Debug][PrimaryGenerator] Check that the macro command "
           << "/Input/setInputFilePath points to a readable file." << G4endl;
    return;
  }

  G4cout << "[iiCTs Debug][PrimaryGenerator] Reading particle source file: "
         << fInputFilePath << G4endl;

  G4double x, y, z, px, py, pz, E;
  std::string line;
  G4int previewCount = 0;

  while (std::getline(infile, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    std::istringstream iss(line);
    if (!(iss >> x >> y >> z >> px >> py >> pz >> E)) {
      continue;
    }

    ParticleRecord rec;
    rec.position = G4ThreeVector(x * mm, y * mm, z * mm);
    rec.momentumDir = G4ThreeVector(px, py, pz).unit();
    rec.energy = E * MeV;
    fRecords.push_back(rec);

    if (previewCount < 3) {
      G4cout << "[iiCTs Debug][PrimaryGenerator] Source preview " << previewCount + 1
             << ": E(MeV)=" << E
             << " pos(mm)=(" << x << ", " << y << ", " << z << ")"
             << " dir=(" << rec.momentumDir.x() << ", "
             << rec.momentumDir.y() << ", " << rec.momentumDir.z() << ")"
             << G4endl;
      ++previewCount;
    }
  }

  infile.close();
  G4cout << "[iiCTs Debug][PrimaryGenerator] Loaded " << fRecords.size()
         << " particle records from input file" << G4endl;

  if (fRecords.empty()) {
    G4cerr << "[iiCTs Debug][PrimaryGenerator] WARNING: no valid particle records loaded." << G4endl;
  }
}
