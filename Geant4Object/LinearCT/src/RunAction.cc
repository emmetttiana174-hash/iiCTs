#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include "Run.hh"

#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include <sstream>

// [MODIFICATION] <<< --- Initialization of static member variables --- >>>
// This must be done outside of any function, in the global scope of the .cc file.
std::chrono::steady_clock::time_point RunAction::fGlobalStartTime;
bool RunAction::fIsBatchMode = false; // Default to interactive mode
// [MODIFICATION] <<< --- End of addition --- >>>

// [MODIFICATION] Simplified constructor to only require the detector
RunAction::RunAction(DetectorConstruction* det)
 : G4UserRunAction(),
   fDetector(det),
   fRun(nullptr),
   fHistoManager(nullptr),
   fPrimary(nullptr) // Primary can be retrieved later if needed
{
  // Book predefined histograms
  fHistoManager = new HistoManager(det);
}

RunAction::~RunAction()
{
  delete fHistoManager;
}

// [MODIFICATION] <<< --- Implementation of new static functions --- >>>
void RunAction::SetGlobalStartTime(std::chrono::steady_clock::time_point time)
{
  fGlobalStartTime = time;
}

void RunAction::SetBatchMode(bool mode)
{
  fIsBatchMode = mode;
}
// [MODIFICATION] <<< --- End of addition --- >>>

G4Run* RunAction::GenerateRun()
{
  fRun = new Run(fDetector);
  return fRun;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  if (fHistoManager) {
    fHistoManager->Update(fDetector, true);
    G4cout << "RunAction: HistoManager::Update() called" << G4endl;
  }
  if (fDetector){
    G4String outputpath = fDetector->GetOutputPath();
    fOutput = outputpath;
  }

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4double angle = fDetector->GetCurrentAngle();
  G4double degrees = angle * (180.0 / CLHEP::pi);
  std::ostringstream oss;
  G4cout << "fOutput: " << fOutput << G4endl;
  oss << fOutput << degrees << ".root";

  G4String fileName = oss.str();
  analysisManager->OpenFile(fileName);
}

void RunAction::EndOfRunAction(const G4Run*)
{
  if(isMaster) fRun->EndOfRun();

  // Show random number engine status
  if(isMaster) G4Random::showEngineStatus();

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  // [MODIFICATION] <<< --- Added timer output at the end of the run --- >>>
  // This logic is now part of RunAction and works for all modes.
  auto endTime = std::chrono::steady_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - fGlobalStartTime).count();
  
  if (isMaster) {
      G4cout << "\n===============================================================" << G4endl;
      G4cout << " Run finished." << G4endl;
      G4cout << " Total duration: " << duration << " ms" << G4endl;
      G4cout << "===============================================================\n" << G4endl;
  }
  // [MODIFICATION] <<< --- End of addition --- >>>
}