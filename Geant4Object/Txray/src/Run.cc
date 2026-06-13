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
/// \file medical/GammaTherapy/src/Run.cc
/// \brief Implementation of the Run class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "Run.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "HistoManager.hh"
#include "G4Track.hh"
#include "G4VPhysicalVolume.hh"
#include "G4DataVector.hh"
#include "G4EmCalculator.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Run::Run(DetectorConstruction* det, HistoManager* histoMgr)
//   : G4Run()
//   , fDetector(det)
//   , fHistoMgr(histoMgr)
Run::Run(DetectorConstruction* det)
  : G4Run(), fDetector(det)
{
  G4cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
  fSumR = 0;
  fNevt = 0;

  fAnalysisManager = G4AnalysisManager::Instance();

  //fHistoId = fHistoMgr->GetHistoIdentifiers();
  fNHisto  = fHistoId.size();

  fNBinsR = fDetector->GetNumberDivR();
  fNBinsZ = fDetector->GetNumberDivZ();

  fMaxEnergy = fDetector->GetMaxEnergy();

  fNBinsE    = fDetector->GetNumberDivE();
  fMaxEnergy = 120.0 * MeV;

  fStepE    = fMaxEnergy / (G4double) fNBinsE;
  fScoreBin = (G4int)(fScoreZ / fStepZ + 0.5);

  fVerbose = fDetector->GetVerbose();
  fVerbose = true;

  fGamma    = G4Gamma::Gamma();
  fElectron = G4Electron::Electron();
  fPositron = G4Positron::Positron();

  fVolumeR.clear();
  fEdep.clear();
  fGammaE.clear();

  fVolumeR.resize(fNBinsR, 0.0);
  fEdep.resize(fNBinsR, 0.0);
  //fEdep.resize(62500,0.0);
  fGammaE.resize(fNBinsE, 0.0);
  //fGammaE.resize(62500, 0.0);

  G4double r1 = 0.0;
  G4double r2 = fStepR;
  for(G4int i = 0; i < fNBinsR; ++i)
  {
    fVolumeR[i] = cm * cm / (CLHEP::pi * (r2 * r2 - r1 * r1));
    r1          = r2;
    r2 += fStepR;
  }

  //if(fAnalysisManager->GetFileName() != "")
    //fHistoMgr->Update(det, true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Run::~Run() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::Merge(const G4Run* run)
{
  G4cout << "222222222222222222222222222222222222" << G4endl;
  const Run* localRun = static_cast<const Run*>(run);

  fNevt += localRun->fNevt;

  fNelec += localRun->fNelec;
  fNgam += localRun->fNgam;
  fNposit += localRun->fNposit;

  fNgamTar += localRun->fNgamTar;
  fNgamPh += localRun->fNgamPh;
  fNeTar += localRun->fNeTar;
  fNePh += localRun->fNePh;

  fNstep += localRun->fNstep;
  fNstepTarget += localRun->fNstepTarget;

  fSumR += localRun->fSumR;

  for(int i = 0; i < (int) fEdep.size(); i++)
    fEdep[i] += localRun->fEdep[i];
  for(int i = 0; i < (int) fGammaE.size(); i++)
    fGammaE[i] += localRun->fGammaE[i];

  G4Run::Merge(run);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Run::EndOfRun()
{
  G4cout << "33333333333333333333333333333333333" << G4endl;
  G4cout << "Histo: End of run actions are started" << G4endl;

  // average
  G4cout << "========================================================"
         << G4endl;

}
