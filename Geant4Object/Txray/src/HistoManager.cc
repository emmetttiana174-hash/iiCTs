//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
  : fFileName("GammaTherapy.root")
{

  fNBinsZ    = 60;
  fNBinsR    = 80;
  fNBinsE    = 200;

  fAbsorberZ = 300.*mm;
  fAbsorberR = 200.*mm;
  fScoreZ    = 100.*mm;
  fMaxEnergy = 120.*keV;

  fStepZ = fStepR = fStepE = 0.0;

  Book();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::~HistoManager()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{ 
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(1);


  G4int histId = analysisManager->CreateH1("0","Detector unit energy deposition",1,0,1);
  //G4int histId = analysisManager->CreateH2("EnergyDeposition","2D Energy Deposition: Angle vs Detector Unit",
                              //180, 0, 180,250, 0, 250);
  if (histId >= 0) {
    // 创建成功
    std::cout << "Histogram created successfully" << std::endl;
  } else {
    // 创建失败
    std::cout << "Histogram creation failed" << std::endl;
  }
  // Create or get analysis manager
  analysisManager->SetDefaultFileType("root");

  //analysisManager->SetFileName(fFileName);
  analysisManager->SetActivation(true);

}




