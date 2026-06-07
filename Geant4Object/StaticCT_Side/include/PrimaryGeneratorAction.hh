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
/// \file medical/GammaTherapy/include/PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

//---------------------------------------------------------------------------
//
// ClassName:   PrimaryGeneratorAction
//
// Description: Generate primary beam
//
// Authors: V.Grichine, V.Ivanchenko
//
// Modified:
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4GeneralParticleSource.hh"
#include "DetectorConstruction.hh"
#include "G4UImessenger.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
class G4ParticleGun;
class G4Event;
class G4UImessenger;
class PrimaryGeneratorActionMessenger;
class DetectorConstruction;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction, public G4UImessenger
{
public:
  void SetDSO(G4double DSO);
  void SetUOffset(G4double uOffset);
  void SetVOffset(G4double vOffset);
  void SetOrder(G4double angle);
  void SetTotalsource(G4double angle);
  void SetTubeXRotationAngle(G4double angle);
  void SetTubeYRotationAngle(G4double angle);
  void SetTubeZRotationAngle(G4double angle);
  void SetTubeRotationAngle(G4double angleX, G4double angleY, G4double angleZ) ;
  void SetLeftAngle(G4double angle);
  void SetRightAngle(G4double angle);
  void SetLowTubeConeAngle(G4double angle);
  void SetHigTubeConeAngle(G4double angle);

  PrimaryGeneratorAction(DetectorConstruction* detector);
  virtual ~PrimaryGeneratorAction();

  //It reads the parameters of the primary particles.
  //Generates the primary event via the ParticleGun method.
  void GeneratePrimaries(G4Event* anEvent) override;
  void InitializeMe();
  //Get/Set methods
  void SetBeamEnergy(G4double val);

  inline void SetBeamSigmaE(G4double val) { fSigmaE = val; };
  inline void SetBeamX(G4double val) { fX0 = val;};
  inline void SetBeamY(G4double val) { fY0 = val;};
  inline void SetBeamZ(G4double val) { fZ0 = val;};
  inline void SetBeamSigmaX(G4double val) { fSigmaX = val;};
  inline void SetBeamSigmaY(G4double val) { fSigmaY = val;};
  inline void SetBeamSigmaZ(G4double val) { fSigmaY = val;};
  inline void SetBeamMinCosTheta(G4double val) { fMinCosTheta = val;};
  inline void SetSigmaTheta(G4double val) { fSigmaTheta = val;};
  inline void SetVerbose(G4int val) { fVerbose = val;};
  inline void SetRandom(const G4String& type) { fGauss = type;};

  G4bool GetVerbose() const { return fVerbose; }
  //
  void SetConeHalfAngle(G4double val); // 设置锥形束半张角
  void SetConeAxis(G4ThreeVector axis); // 设置锥形束轴线

  // G4double GetSourceX() const { return fSourceX; }
  // void SetNewValue(G4UIcommand* command, G4String newValue);
  //void SetRotationAngle(G4double angle);
  //
private:

  G4double fDSO;
  G4String fSpectrumPos;
  G4String fSpectrumEnergy;
  G4double fUOffset;
  G4double fVOffset;
  G4double fOrder;
  G4int fTotalsource;
  G4double fPolygonCount;
  G4double fdetLength;
  G4double fTubeRotationAngleX;
  G4double fTubeRotationAngleY;
  G4double fTubeRotationAngleZ;
  G4double fTubeXRotationAngle;
  G4double fTubeYRotationAngle;
  G4double fTubeZRotationAngle;
  G4double fleftAngle;
  G4double frightAngle;
  G4double flowConeAngle;
  G4double fhigConeAngle;
  PrimaryGeneratorActionMessenger* fMessenger;

  std::vector<G4double> energies;
  std::vector<G4double> intensities;
  std::vector<G4double> probabilities;
  std::vector<G4double> cumulativeProbabilities;

  //PrimaryGeneratorAction & operator=(const PrimaryGeneratorAction &right);
  //PrimaryGeneratorAction(const PrimaryGeneratorAction&);

  //G4int fVerbose;
  //PrimaryGeneratorMessenger* fMessenger;
  G4UIcmdWithADoubleAndUnit* fSetSourceXCmd;
  G4double fSourceX;
  std::vector<G4double> sourceCDFs;  // 射线源位置的 CDF
  std::vector<G4double> sourceX;     // 射线源 x 坐标
  std::vector<G4double> sourceY;     // 射线源 y 坐标

  G4ParticleGun* fParticleGun;
  DetectorConstruction* fDetector;

  std::vector<G4ThreeVector> fPoints;
  G4double fConeHalfAngle; // 锥形束半张角（弧度）
  G4ThreeVector fConeAxis; // 锥形束轴线方向


  G4int fCounter;
  G4double fX0, fY0, fZ0;
  G4double fSigmaX, fSigmaY, fSigmaZ;
  G4double fRMax2;
  G4double fSigmaE;
  G4double fSigmaTheta;
  G4double fEnergy;
  G4double fMinCosTheta;
  G4double fFanAngle;
  G4ThreeVector fPosition;
  G4ThreeVector fDirection;
  G4String fGauss;
  //
  struct SourceParams {
        G4ThreeVector position;
        G4double lefthalfAngle;
        G4double righthalAngle;
    };
  std::vector<SourceParams> fSources;
  G4GeneralParticleSource* fGPS;
  G4bool fVerbose;
  //
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#endif






