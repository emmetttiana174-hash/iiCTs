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
/// \file medical/GammaTherapy/include/DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

// -------------------------------------------------------------
//      GEANT4  test  IBREM
//
// Authors: V.Grichine, V.Ivanchenko
//
// Modified:
//
// 18-02-03 V.Ivanchenko create
//
// -------------------------------------------------------------

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"
#include "G4VSensitiveDetector.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CheckVolumeSD;
class PhantomSD;
class TargetSD;
class DetectorMessenger;
class G4LogicalVolume;
class G4GlobalMagFieldMessenger;
class G4SensitiveDetector;
class G4VPhysicalVolume;
class G4UImessenger;
class DetectorConstructionMessenger;
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  //std::vector<G4LogicalVolume*> CreateDetectorUnits(); // 新增声明

  DetectorConstruction();
  virtual ~DetectorConstruction();

  //G4VPhysicalVolume* Construct();

  ////
  virtual G4VPhysicalVolume* Construct();
  void SetObjectRotationAngle(G4double angle);
  G4double GetCurrentAngle() const ;
  ////
  inline void SetNumberDivZ(G4int val)        { fNumZ = val; };
  inline void SetNumberDivR(G4int val)        { fNumR = val; };
  G4int GetNumberDivZ() const                 { return fNumZ; }
  G4int GetNumberDivR() const                 { return fNumR; }

  void SetMaxEnergy(G4double e) { fMaxEnergy = e; }
  inline G4double GetMaxEnergy() const { return fMaxEnergy; }
  G4int GetNumberDivE() const          { return fNumE; }
  inline void SetNumberDivE(G4int val)        { fNumE = val; };

  void SetVerbose(G4bool v) { fVerbose = v; }
  inline G4bool GetVerbose() const { return fVerbose; }

  //void DumpGeometryParameters();

private:  
  //static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
  void InitialiseGeometryParameters();

  DetectorConstruction & operator=(const DetectorConstruction &right);
  DetectorConstruction(const DetectorConstruction&);
  void ConstructSDandField();

  G4bool fVerbose;

  G4int fNumZ;
  G4int fNumR;
  G4int worldSize;
  G4int fNumE;
  G4double fMaxEnergy;

  G4LogicalVolume* cellLogical;
  G4LogicalVolume* fLogicDetector;
  G4LogicalVolume* logicObject;
  G4LogicalVolume* logicWorld;
  //G4SensitiveDetector* sensitiveDetector;
  G4VSensitiveDetector* sensitiveDetector;

  G4double fWorldX, fWorldY, fWorldZ;
  G4double fObjectRmin,fObjectRmax,fObjectDz;
  G4double fDetectorX, fDetectorY, fDetectorZ;

  G4double fObjectPosz, fDetectorPosz;

  G4Material* fWorldMaterial;
  G4Material* fObjectMaterial;
  G4Material* fDetectorMaterial;

  //DetectorMessenger* fMessenger;
  G4VPhysicalVolume* fPhysWorld;

  ////
  G4double fCurrentAngle = 0.0;
  G4VPhysicalVolume* fPhysObject;      // 物体的物理体积
  G4RotationMatrix* fRotationMatrix;   // 旋转矩阵
  DetectorConstructionMessenger* fMessenger;
  ////
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
