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
#include "FilePathManager.hh"
#include "G4AnalysisManager.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4String.hh"
#include <sstream>
#include <chrono>
#include <iomanip>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::RunAction(DetectorConstruction *det, PrimaryGeneratorAction *prim)
    : G4UserRunAction(), fDetector(det), fRun(nullptr), fHistoManager(nullptr), fPrimary(prim)
{

  // Book predefined histograms
  fHistoManager = new HistoManager();
  // fHistoManager->Update(det);
}

// 静态变量定义
std::chrono::steady_clock::time_point RunAction::fGlobalStartTime;
bool RunAction::fIsBatchMode = false;

// 静态方法实现
void RunAction::SetGlobalStartTime(std::chrono::steady_clock::time_point time)
{
  fGlobalStartTime = time;
}

void RunAction::SetBatchMode(bool isBatch)
{
  fIsBatchMode = isBatch;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

RunAction::~RunAction() { delete fHistoManager; }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4Run *RunAction::GenerateRun()
{

  fRun = new Run(fDetector);
  // fRun = new Run(fDetector, fHistoManager);
  return fRun;
}

void RunAction::BeginOfRunAction(const G4Run *)
{
  // 记录开始时间
  fStartTime = std::chrono::steady_clock::now();

  // 打印开始提示
  G4cout << "=====================================" << G4endl;
  G4cout << "Starting simulation..." << G4endl;
  G4cout << "=====================================" << G4endl;

  G4cout << "Filling histogram..." << G4endl;
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  std::ostringstream oss;
  G4String outputPath = FilePathManager::GetInstance()->GetOutputFilePath();
  oss << outputPath << ".root";
  std::ostringstream txtOss;
  txtOss << outputPath << ".txt";
  G4String fileName = oss.str(); // 转换为 G4String

  // 保存当前输出文件名
  fCurrentOutputFile = fileName;

  analysisManager->OpenFile(fileName);
  G4cout << "Output file: " << fileName << G4endl;
  G4cout << "[iiCTs Debug][RunAction] Output prefix from FilePathManager: "
         << outputPath << G4endl;
  G4cout << "[iiCTs Debug][RunAction] Paired energy TXT file should be: "
         << txtOss.str() << G4endl;
  // analysisManager->OpenFile("GammaTherapy"+std::to_string(angle)+".root");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run *)
{
  // print Run summary
  //
  if (isMaster)
    fRun->EndOfRun();
  // save histograms

  // if(analysisManager->IsActive())
  // {
  //   analysisManager->Write();
  //   analysisManager->CloseFile();
  // }

  // show Rndm status
  if (isMaster)
    G4Random::showEngineStatus();
  G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

  // 计算仿真运行时间
  auto endTime = std::chrono::steady_clock::now();
  auto simulationDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - fStartTime);

  // 计算总运行时间
  auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - fGlobalStartTime).count();

  // 转换仿真时间为秒
  double simulationSeconds = simulationDuration.count() / 1000.0;

  // 最终输出信息 - 确保在所有输出的最后显示
  if (isMaster)
  {
    G4cout << "=====================================" << G4endl;
    G4cout << "Simulation complete!" << G4endl;
    G4cout << "Output file saved: " << fCurrentOutputFile << G4endl;
    G4cout << "Simulation elapsed time: " << std::fixed << std::setprecision(3)
           << simulationSeconds << "s" << G4endl;
    G4cout << "=====================================" << G4endl;

    if (fIsBatchMode)
    {
      G4cout << "=== The macro file execution is complete ===" << G4endl;
      G4cout << "=== The simulation timer ends ===" << G4endl;
      G4cout << "Total Time: " << totalDuration << " ms" << G4endl;
    }
  }
}
