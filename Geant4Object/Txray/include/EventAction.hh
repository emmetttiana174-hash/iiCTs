#ifndef EVENT_ACTION_HH
#define EVENT_ACTION_HH
#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include <map>
#include "G4AnalysisManager.hh"
#include <fstream>
//class G4AnalysisManager;
class DetectorConstruction;
class EventAction : public G4UserEventAction {
 public:
  EventAction(DetectorConstruction* detector);
  //EventAction();
  virtual ~EventAction();
  virtual void BeginOfEventAction(const G4Event* event);
  virtual void EndOfEventAction(const G4Event* event);
  G4AnalysisManager* fAnalysisManager;
  
 private:
  ////
  DetectorConstruction* fDetector;
  ////
  //G4AnalysisManager* fAnalysisManager;
  std::map<G4int, G4double> fDetectorEdep;
  
  // 新增成员变量，用于输出能量文件
  std::ofstream* fEnergyOutputFile;
};
#endif // EVENT_ACTION_HH