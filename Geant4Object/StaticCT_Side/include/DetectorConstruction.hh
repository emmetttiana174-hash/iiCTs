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
#include "G4VisAttributes.hh"
#include <vector>
#include <string>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class CheckVolumeSD;
class PhantomSD;
class TargetSD;
class G4Material;
class DetectorSD;
class DetectorMessenger;
class G4LogicalVolume;
class G4GlobalMagFieldMessenger;
class G4SensitiveDetector;
class G4VPhysicalVolume;
class G4UImessenger;
class DetectorConstructionMessenger;
class PrimaryGeneratorAction;
class PhysicsList;
struct PhantomData {
    int nx, ny, nz;  // 体素网格尺寸
    double minX, maxX, minY, maxY, minZ, maxZ;  // 物理范围（mm）
    std::vector<int> materialIndices;  // 材料索引
    std::vector<double> densities;  // 密度数据
    std::vector<std::string> materialNames;  // 材料名称
    G4double fMultiple;
};
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  //std::vector<G4LogicalVolume*> CreateDetectorUnits(); // 新增声明
  
  DetectorConstruction();
  virtual ~DetectorConstruction();

  //G4VPhysicalVolume* Construct();

  ////
  virtual G4VPhysicalVolume* Construct();
  void UpdateGeometry();
  void ConstructSDandField() override;
  void SetPolygonCount(G4double count);
  void SetObjectRotationAngle(G4double angle);
  void SetObjectRotationAngle1(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorModuleNumber(G4double num);
  void SetDetectorBedDirection(G4double Bed);
  void SetDetectorFilter(G4double filter);
  void SetG4dcm(const G4String& G4dcm);
  void SetSpectrumPos(const G4String& spectrumpos);
  void SetSpectrumEnergy(const G4String& spectrumenergy);
  void SetOutputPath(const G4String& outputPath);
  void SetFilterMaterial(const G4String& materialName);
  void SetTubeFilterMaterial(const G4String& materialName);
  void SetMultiple(G4double multiple);
  void SetDetectorRadius(G4double radius);
  void SetDDO(G4double DDO);
  void SetDetectorRotationAngle1(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorRotationAngle2(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorRotationAngle3(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorRotationAngle4(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorRotationAngle5(G4double angleX, G4double angleY, G4double angleZ);
  void SetDetectorXRotationAngle(G4double angle);
  void SetDetectorYRotationAngle(G4double angle);
  void SetDetectorZRotationAngle(G4double angle);
  void SetDetectorXSize(G4double length);
  void SetDetectorYSize(G4double width);
  void SetDetectorZSize(G4double height);
  void SetDetectorXBias(G4double length);
  void SetDetectorYBias(G4double width);
  void SetDetectorZBias(G4double height);
  void SetFilterXSize(G4double length);
  void SetFilterYSize(G4double width);
  void SetFilterZSize(G4double height);
  void SetFilterXBias(G4double length);
  void SetFilterYBias(G4double width);
  void SetObjectXBias(G4double length);
  void SetObjectYBias(G4double width);
  void SetObjectZBias(G4double height);
  void SetFilterPos(G4double pos);
  void SetTubeFilterXSize(G4double length);
  void SetTubeFilterYSize(G4double width);
  void SetTubeFilterZSize(G4double height);
  void SetTubeFilterXBias(G4double length);
  void SetTubeFilterYBias(G4double width);
  void SetTubeFilterPos(G4double pos);
  void SetDSD(G4double DSD);
  void SetDSO(G4double DSO);
  void SetPixelXCount(G4double XCount);
  void SetPixelYCount(G4double YCount);
  void SetPixelXSize(G4double length);
  void SetPixelYSize(G4double width);
  void SetPixelZSize(G4double height);
  void SetPitch(G4double pitch);
  void SetDetectorMaterial(const G4String& materialName);
  void UpdateGeometryIfNeeded();
  void CreateCustomMaterial(const G4String& materialInfo); // 新增方法用于创建自定义材料
  G4double GetDSO() const { return fDSO; }
  G4String GetOutputPath() const { return fOutput; }
  G4String GetSpectrumPos() const { return fSpectrumPos; }
  G4String GetSpectrumEnergy() const { return fSpectrumEnergy; }
  G4double GetPixelXCount() const { return fXCount; }
  G4double GetPixelYCount() const { return fYCount; }
  
  G4double GetfPolygonCount() const { return fPolygonCount; }
  G4double GetDetectorLength() const { return fDetectorLength; }
  G4double fMultiple;

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
  //void ConstructSDandField();

  G4bool fVerbose;
  G4double fPolygonCount;
  G4double fModuleNum;
  G4double fBedDirection;
  G4double fDetectorRadius;
  G4double fDetectorFilter;
  G4Material* fFilterMaterial;
  G4double fDetectorLength;
  G4double fDetectorWidth;
  G4double fDetectorHeight;
  G4double fFilterLength;
  G4double fFilterWidth;
  G4double fFilterHeight;
  G4double fFilterPos;
  G4double fDetectorBiasLength;
  G4double fDetectorBiasWidth;
  G4double fDetectorBiasHeight;
  G4double fDDO;
  G4double fDSD;
  G4double fDSO;
  G4double fXCount;
  G4double fYCount;
  G4double fPixelLength;
  G4double fPixelWidth;
  G4double fPixelHeight;
  G4double fFilterXBias;
  G4double fFilterYBias;
  G4double fTubeFilterLength;
  G4double fTubeFilterWidth;
  G4double fTubeFilterHeight;
  G4double fTubeFilterPos;
  G4double fTubeFilterXBias;
  G4double fTubeFilterYBias;
  G4double fObjectXBias;
  G4double fObjectYBias;
  G4double fObjectZBias;
  G4double fDetectorAngle;
  G4double fPitch;
  G4double fdegree;
  G4String fG4dcm;
  G4String fSpectrumPos;
  G4String fSpectrumEnergy;
  G4String fOutput;

  G4VPhysicalVolume* fPhysDetector;
  G4VPhysicalVolume* fPhysDetector1;
  G4VPhysicalVolume* fPhysDetector2;
  G4VPhysicalVolume* fPhscell;
  G4VPhysicalVolume* fPhsbig;
  DetectorSD* fDetectorSD;
  G4double fDetectorX, fDetectorY, fDetectorZ;
  bool fIsSensitiveDetectorRegistered;
  bool fGeometryNeedsUpdate;
  bool fIsMaterialSet;
  PhantomData fPhantomData;
  std::vector<G4Material*> fMaterials;
  std::vector<PhantomData> fPhantomDatas;
  G4int fNumZ;
  G4int fNumR;
  G4int worldSize;
  G4int fNumE;
  G4double fMaxEnergy;

  G4LogicalVolume* cellLogical;
  G4LogicalVolume* fLogicDetector;
  G4LogicalVolume* fLogicFilter1;
  G4LogicalVolume* fLogicFilter2;
  G4LogicalVolume* logicObject;
  G4LogicalVolume* logicWorld;
  //G4SensitiveDetector* sensitiveDetector;
  G4VSensitiveDetector* sensitiveDetector;

  G4double fWorldX, fWorldY, fWorldZ;
  G4double fObjectRmin,fObjectRmax,fObjectDz;
  

  G4double fObjectPosz, fDetectorPosz;

  G4Material* fWorldMaterial;
  G4Material* fObjectMaterial;
  G4Material* fDetectorMaterial;
  G4Material* fTubeFilterMaterial;
  //DetectorMessenger* fMessenger;
  G4VPhysicalVolume* fPhysWorld;

  ////
  G4Material* PMMA;
  G4Material* Water;
  G4Material* IodineSolution;
  G4Material* Calcium;
  G4Material* Polyethylene;
  G4Material* fMetalWire;
  G4LogicalVolume* fLogicFilter;
  G4LogicalVolume* fLogicTubeFilter;
  G4LogicalVolume* fLogicTubeFilter1;
  G4LogicalVolume* fLogicTubeFilter2;
  G4LogicalVolume* flogicalimiter1;
  G4LogicalVolume* cellLogical1;
  G4LogicalVolume* logicsource;
  G4LogicalVolume* fLogicDetector1;
  G4LogicalVolume* fLogicDetector2;
  G4double fCurrentAngle = 0.0;
  G4VPhysicalVolume* fPhysObject;      // 物体的物理体积
  G4VPhysicalVolume* fPhyssource; 
  G4VPhysicalVolume* fPhysFilter;
  G4VPhysicalVolume* fPhysTubeFilter;
  G4RotationMatrix* fRotationMatrix2;
  G4RotationMatrix* fRotationMatrix3;
  G4RotationMatrix* fRotationMatrix4;
  G4RotationMatrix* fRotationMatrix5;
  G4RotationMatrix* fRotationMatrix6;
  G4RotationMatrix* fRotationMatrix7;
  DetectorConstructionMessenger* fMessenger;
  ////
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif