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
#include "G4SystemOfUnits.hh"

using CLHEP::cm;
using CLHEP::mm;
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
  // std::vector<G4LogicalVolume*> CreateDetectorUnits(); // 新增声明

  DetectorConstruction();
  virtual ~DetectorConstruction();

  // G4VPhysicalVolume* Construct();

  ////
  virtual G4VPhysicalVolume *Construct();
  void SetObjectRotationAngle(G4double angle);
  G4double GetCurrentAngle() const;

  // 新增阳极靶控制方法
  void SetAnodeRotationAngle(G4double angle);
  void SetAnodeMaterial(G4String material);
  G4double GetAnodeRotationAngle() const;
  G4String GetAnodeMaterial() const;

  // 新增过滤层控制方法
  void SetFilterMaterial(G4String material); // 保持向后兼容，等同于SetFilter1Material
  void SetFilterX(G4double sizeX);           // 保持向后兼容，等同于SetFilter1X
  void SetFilterY(G4double sizeY);           // 保持向后兼容，等同于SetFilter1Y
  void SetFilterZ(G4double sizeZ);           // 保持向后兼容，等同于SetFilter1Z
  G4String GetFilterMaterial() const;        // 保持向后兼容，等同于GetFilter1Material
  G4double GetFilterX() const;               // 保持向后兼容，等同于GetFilter1X
  G4double GetFilterY() const;               // 保持向后兼容，等同于GetFilter1Y
  G4double GetFilterZ() const;               // 保持向后兼容，等同于GetFilter1Z

  // 新增滤过层1控制方法
  void SetFilter1Material(G4String material);
  void SetFilter1X(G4double sizeX);
  void SetFilter1Y(G4double sizeY);
  void SetFilter1Z(G4double sizeZ);
  void SetFilter1Position(G4double posZ);
  G4double GetFilter1Position() const;
  G4String GetFilter1Material() const;
  G4double GetFilter1X() const;
  G4double GetFilter1Y() const;
  G4double GetFilter1Z() const;

  // 新增滤过层2控制方法
  void SetFilter2Material(G4String material);
  void SetFilter2X(G4double sizeX);
  void SetFilter2Y(G4double sizeY);
  void SetFilter2Z(G4double sizeZ);
  void SetFilter2Position(G4double posZ);
  G4double GetFilter2Position() const;
  G4String GetFilter2Material() const;
  G4double GetFilter2X() const;
  G4double GetFilter2Y() const;
  G4double GetFilter2Z() const;

  // 新增探测器控制方法
  void SetDetectorMaterial(G4String material);
  void SetDetectorX(G4double sizeX);
  void SetDetectorY(G4double sizeY);
  void SetDetectorZ(G4double sizeZ);
  G4String GetDetectorMaterial() const;
  G4double GetDetectorX() const;
  G4double GetDetectorY() const;
  G4double GetDetectorZ() const;
  ////
  inline void SetNumberDivZ(G4int val) { fNumZ = val; };
  inline void SetNumberDivR(G4int val) { fNumR = val; };
  G4int GetNumberDivZ() const { return fNumZ; }
  G4int GetNumberDivR() const { return fNumR; }

  void SetMaxEnergy(G4double e) { fMaxEnergy = e; }
  inline G4double GetMaxEnergy() const { return fMaxEnergy; }
  G4int GetNumberDivE() const { return fNumE; }
  inline void SetNumberDivE(G4int val) { fNumE = val; };

  void SetVerbose(G4bool v) { fVerbose = v; }
  inline G4bool GetVerbose() const { return fVerbose; }

  // void DumpGeometryParameters();

private:
  // static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger;
  void InitialiseGeometryParameters();

  DetectorConstruction &operator=(const DetectorConstruction &right);
  DetectorConstruction(const DetectorConstruction &);
  void ConstructSDandField();

  G4bool fVerbose;

  G4int fNumZ;
  G4int fNumR;
  G4int worldSize;
  G4int fNumE;
  G4double fMaxEnergy;

  G4LogicalVolume *cellLogical;
  G4LogicalVolume *fLogicDetector;
  G4LogicalVolume *logicObject;
  G4LogicalVolume *logicWorld;
  // G4SensitiveDetector* sensitiveDetector;
  G4VSensitiveDetector *sensitiveDetector;

  G4double fWorldX, fWorldY, fWorldZ;
  G4double fObjectRmin, fObjectRmax, fObjectDz;
  G4double fDetectorX, fDetectorY, fDetectorZ;

  G4double fObjectPosz, fDetectorPosz;

  G4Material *fWorldMaterial;
  G4Material *fObjectMaterial;
  G4Material *fDetectorMaterial;

  // DetectorMessenger* fMessenger;
  G4VPhysicalVolume *fPhysWorld;

  ////
  G4double fCurrentAngle = 0.0;
  G4VPhysicalVolume *fPhysObject;    // 物体的物理体积
  G4RotationMatrix *fRotationMatrix; // 旋转矩阵
  DetectorConstructionMessenger *fMessenger;
  // Anode members.
  G4double fAnodeRotationAngle = 0.0;
  G4String fAnodeMaterialName = "W";
  G4VPhysicalVolume *fPhysAnode;
  G4VPhysicalVolume *fPhysDetector;
  G4LogicalVolume *fAnodeLogical;

  // Filter layer 1 members (legacy Filter* commands map to Filter1).
  G4String fFilterMaterialName = "Al";
  G4VPhysicalVolume *fPhysFilter;
  G4LogicalVolume *fFilterLogical;
  G4double fFilterX = 2.5 * cm;
  G4double fFilterY = 2.5 * cm;
  G4double fFilterZ = 1 * mm;

  G4double fFilter1PosZ = -3 * cm;
  G4double fFilter2PosZ = -2 * cm;

  G4String fFilter1MaterialName = "Al";
  G4VPhysicalVolume *fPhysFilter1;
  G4LogicalVolume *fFilter1Logical;
  G4double fFilter1X = 2.5 * cm;
  G4double fFilter1Y = 2.5 * cm;
  G4double fFilter1Z = 1 * mm;

  // Filter layer 2 members are preserved for existing macro commands.
  G4String fFilter2MaterialName = "Cu";
  G4VPhysicalVolume *fPhysFilter2;
  G4LogicalVolume *fFilter2Logical;
  G4double fFilter2X = 2.1 * cm;
  G4double fFilter2Y = 2.1 * cm;
  G4double fFilter2Z = 1.25 * mm;

  // Detector members.
  G4String fDetectorMaterialName = "Galactic";
  G4LogicalVolume *fDetectorLogical;
  ////
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
