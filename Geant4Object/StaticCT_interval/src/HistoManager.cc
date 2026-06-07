//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"
#include "G4SystemOfUnits.hh"
#include "DetectorConstruction.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager(DetectorConstruction* detConstruction)
  : fFileName("GammaTherapy.root"),fDetector(detConstruction)
{

  fNBinsZ    = 60;
  fNBinsR    = 80;
  fNBinsE    = 200;
  if (fDetector){
    G4double xcount = fDetector->GetPixelXCount();
    G4double ycount = fDetector->GetPixelYCount();
    G4double PolygonCount = fDetector->GetfPolygonCount();
    fPolygonCount = PolygonCount;
    fXCount = xcount;
    fYCount = ycount;
  } 
  G4cout << "fXcount: " << fXCount << G4endl;
  G4cout << "fYcount: " << fYCount << G4endl;
  fAbsorberZ = 300.*mm;
  fAbsorberR = 200.*mm;
  fScoreZ    = 100.*mm;
  fMaxEnergy = 120.*MeV;

  fStepZ = fStepR = fStepE = 0.0;

  //Book();

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

  if (fDetector){
    G4double xcount = fDetector->GetPixelXCount();
    G4double ycount = fDetector->GetPixelYCount();
    G4double PolygonCount = fDetector->GetfPolygonCount();
    fPolygonCount = PolygonCount;
    fXCount = xcount;
    fYCount = ycount;
  } 
  analysisManager->Clear();
  G4int histId0 = analysisManager->CreateH1("TotalEdep","Detector unit energy deposition",fPolygonCount*fXCount*fYCount,0,fPolygonCount*fXCount*fYCount);

  analysisManager->SetDefaultFileType("root");
  analysisManager->SetActivation(true);
}

void HistoManager::Update(DetectorConstruction* det, bool bForceActivation)
{
    if (det) {
        fDetector = det;
        fXCount = fDetector->GetPixelXCount();
        fYCount = fDetector->GetPixelYCount();
        G4cout << "Update: fXCount = " << fXCount << ", fYCount = " << fYCount << G4endl;

        if (bForceActivation || !histogramsCreated) {
            Book();
        }
    }
}


