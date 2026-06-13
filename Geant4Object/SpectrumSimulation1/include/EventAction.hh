#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH

#include "G4AnalysisManager.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4UserEventAction.hh"

#include <fstream>
#include <map>

class DetectorConstruction;
class G4Event;

class EventAction : public G4UserEventAction
{
public:
  EventAction(DetectorConstruction* detector);
  virtual ~EventAction();

  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);

  G4AnalysisManager* fAnalysisManager;

private:
  void OpenEnergyOutputFile();

  DetectorConstruction* fDetector;
  std::map<G4int, G4double> fDetectorEdep;
  std::ofstream* fEnergyOutputFile;
  G4String fCurrentEnergyOutputPath;
};

#endif // EVENT_ACTION_HH
