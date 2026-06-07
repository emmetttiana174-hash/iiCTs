#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <chrono> // [MODIFICATION] Added for std::chrono::steady_clock

// Forward declarations
class G4Run;
class DetectorConstruction;
class PrimaryGeneratorAction;
class HistoManager;
class Run;

class RunAction : public G4UserRunAction
{
public:
  // [MODIFICATION] Simplified constructor
  RunAction(DetectorConstruction* det);
  virtual ~RunAction();

  virtual G4Run* GenerateRun();
  virtual void BeginOfRunAction(const G4Run*);
  virtual void EndOfRunAction(const G4Run*);

  // [MODIFICATION] <<< --- Added static functions for external control --- >>>
  // These allow main.cpp or EmbeddingHelper.cc to set global states
  static void SetGlobalStartTime(std::chrono::steady_clock::time_point time);
  static void SetBatchMode(bool mode);
  // [MODIFICATION] <<< --- End of addition --- >>>

private:
  G4String fOutput;
  DetectorConstruction* fDetector;
  Run* fRun;
  HistoManager* fHistoManager;
  PrimaryGeneratorAction* fPrimary; // This can be kept if used elsewhere

  // [MODIFICATION] <<< --- Added static members to store global states --- >>>
  static std::chrono::steady_clock::time_point fGlobalStartTime;
  static bool fIsBatchMode;
  // [MODIFICATION] <<< --- End of addition --- >>>
};

#endif