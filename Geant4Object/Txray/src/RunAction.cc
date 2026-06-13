//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file medical/GammaTherapy/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// -------------------------------------------------------------
//
//      GEANT4
//
// -------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "RunAction.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include "Run.hh"
#include "G4AnalysisManager.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4String.hh"
#include <sstream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::RunAction(DetectorConstruction* det,PrimaryGeneratorAction* prim)
  : G4UserRunAction()
  , fDetector(det)
  , fRun(nullptr)
  , fHistoManager(nullptr)
  , fPrimary(prim)
{

  // Book predefined histograms
  fHistoManager = new HistoManager();
  //fHistoManager->Update(det);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::~RunAction() { delete fHistoManager; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run* RunAction::GenerateRun()
{

  fRun = new Run(fDetector);
  //fRun = new Run(fDetector, fHistoManager);
  return fRun;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
  G4cout << "11111111111111111111111111111111111" << G4endl;
  G4cout << "Filling histogram..." << G4endl;
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  G4double angle = fDetector->GetCurrentAngle();
  G4double degrees = angle * (180.0 / CLHEP::pi);
  std::ostringstream oss;
  oss << "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/BeamCT9/output/2test10deg120kev" << degrees << ".root";
  G4String fileName = oss.str();  // 转换为 G4String
  analysisManager->OpenFile(fileName);
  //analysisManager->OpenFile("GammaTherapy"+std::to_string(angle)+".root");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run*)
{
  G4cout << "666666666666666666666666666666" << G4endl;
  // print Run summary
  //
  if(isMaster) fRun->EndOfRun();
  // save histograms

  // if(analysisManager->IsActive())
  // {
  //   analysisManager->Write();
  //   analysisManager->CloseFile();
  // }

  // show Rndm status
  if(isMaster) G4Random::showEngineStatus();
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
  G4cout << "File written and closed" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
