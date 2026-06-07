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

  // 新增阳极靶相关成员变量
  G4double fAnodeRotationAngle = 0.0; // 阳极靶旋转角度
  G4String fAnodeMaterialName = "W";  // 阳极靶材料名称（默认钨）
  G4VPhysicalVolume *fPhysAnode;      // 阳极靶物理体积
  G4VPhysicalVolume *fPhysDetector;   // 探测器物理体积
  G4LogicalVolume *fAnodeLogical;     // 阳极靶逻辑体积

  // 新增过滤层相关成员变量
  G4String fFilterMaterialName = "Al"; // 过滤层1材料名称（默认铝，保持向后兼容）
  G4VPhysicalVolume *fPhysFilter;      // 过滤层1物理体积（保持向后兼容）
  G4LogicalVolume *fFilterLogical;     // 过滤层1逻辑体积（保持向后兼容）
  G4double fFilterX = 2.1 * cm;        // 过滤层1 X方向半长度（默认2.1cm，保持向后兼容）
  G4double fFilterY = 2.1 * cm;        // 过滤层1 Y方向半长度（默认2.1cm，保持向后兼容）
  G4double fFilterZ = 1.25 * mm;       // 过滤层1 Z方向半长度（默认1.25mm，保持向后兼容）

  // 滤过层位置（Z 坐标，单位内置为 cm/mm，可用命令修改）
  G4double fFilter1PosZ = 0.3 * cm; // 滤过层1默认 Z 位置
  G4double fFilter2PosZ = 0.4 * cm; // 滤过层2默认 Z 位置

  // 新增滤过层1相关成员变量
  G4String fFilter1MaterialName = "Al"; // 滤过层1材料名称（默认铝）
  G4VPhysicalVolume *fPhysFilter1;      // 滤过层1物理体积
  G4LogicalVolume *fFilter1Logical;     // 滤过层1逻辑体积
  G4double fFilter1X = 2.1 * cm;        // 滤过层1 X方向半长度（默认2.1cm）
  G4double fFilter1Y = 2.1 * cm;        // 滤过层1 Y方向半长度（默认2.1cm）
  G4double fFilter1Z = 1.25 * mm;       // 滤过层1 Z方向半长度（默认1.25mm）

  // 新增滤过层2相关成员变量
  G4String fFilter2MaterialName = "Cu"; // 滤过层2材料名称（默认铜）
  G4VPhysicalVolume *fPhysFilter2;      // 滤过层2物理体积
  G4LogicalVolume *fFilter2Logical;     // 滤过层2逻辑体积
  G4double fFilter2X = 2.1 * cm;        // 滤过层2 X方向半长度（默认2.1cm）
  G4double fFilter2Y = 2.1 * cm;        // 滤过层2 Y方向半长度（默认2.1cm）
  G4double fFilter2Z = 1.25 * mm;       // 滤过层2 Z方向半长度（默认1.25mm）

  // 新增探测器相关成员变量
  G4String fDetectorMaterialName = "Galactic"; // 探测器材料名称（默认真空）
  G4LogicalVolume *fDetectorLogical;           // 探测器逻辑体积
  ////
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
