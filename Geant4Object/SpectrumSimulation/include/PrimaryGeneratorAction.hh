#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include <cstddef>
#include <vector>

class DetectorConstruction;
class G4Event;

struct ParticleRecord
{
  G4ThreeVector position;
  G4ThreeVector momentumDir;
  G4double energy;
};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(DetectorConstruction* detector);
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event* anEvent);
  void LoadParticleRecords();

private:
  void ReloadIfInputPathChanged();

  DetectorConstruction* fDetector;
  G4ParticleGun* fParticleGun;
  std::vector<ParticleRecord> fRecords;
  size_t fCurrentIndex;
  G4long fTotalParticles;
  G4String fInputFilePath;
};

#endif // PrimaryGeneratorAction_h
