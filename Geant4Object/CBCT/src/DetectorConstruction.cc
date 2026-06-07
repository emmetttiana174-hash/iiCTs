/// \file medical/GammaTherapy/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include "G4NistManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4RunManager.hh"
#include "G4GeometryManager.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "globals.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4UIcmdWithADouble.hh"
#include "CLHEP/Units/PhysicalConstants.h"
#include "DetectorConstructionMessenger.hh"
#include "G4Types.hh"       // Geant4 基本类型（如 G4double）
#include "CLHEP/Units/PhysicalConstants.h"  // CLHEP 单位和常数（如 pi）
#include <iostream>
#include "G4SubtractionSolid.hh"
#include "G4Ellipsoid.hh"
#include "G4Cons.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4VisAttributes.hh"
// 参数化类
class PhantomParameterisation : public G4VPVParameterisation {
public:
    PhantomParameterisation(const PhantomData& data, std::vector<G4Material*> materials, double multiple)
        : fData(data), fMaterials(materials), fMultiple(multiple) {}

    void ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const override {
        int iz = copyNo / (fData.nx * fData.ny);
        int iy = (copyNo % (fData.nx * fData.ny)) / fData.nx;
        int ix = copyNo % fData.nx;

        double dx = (fData.maxX - fData.minX) / fData.nx;
        double dy = (fData.maxY - fData.minY) / fData.ny;
        double dz = (fData.maxZ - fData.minZ) / fData.nz;

        // 相对母体框中心的坐标
        double x = (ix - fData.nx / 2 ) * dx * fMultiple;
        double y = (iy - fData.ny / 2 ) * dy * fMultiple;
        double z = (iz - fData.nz / 2 ) * dz * fMultiple;
        // double x = fData.minX + (ix + 0.5) * dx;
        // double y = fData.minY + (iy + 0.5) * dy;
        // double z = fData.minZ + (iz + 0.5) * dz;
        physVol->SetTranslation(G4ThreeVector(x * mm, y * mm, z * mm));
    }

    G4Material* ComputeMaterial(const G4int copyNo, G4VPhysicalVolume* physVol, const G4VTouchable*) override {
        int matIndex = fData.materialIndices[copyNo];
        // 静态颜色映射（根据材料索引设置颜色）
        static const G4Colour colours[] = {
            G4Colour(0.5, 0.5, 1.0),  // Air - 浅蓝色
            G4Colour(0.0, 1.0, 0.0),  // LungInhale - 绿色
            G4Colour(0.0, 0.5, 0.0),  // LungExhale - 深绿色
            G4Colour(1.0, 1.0, 0.0),  // AdiposeTissue - 黄色
            G4Colour(1.0, 0.5, 0.0),  // Breast - 橙色
            G4Colour(0.0, 0.0, 1.0),  // Water - 蓝色
            G4Colour(0.5, 0.0, 0.5),  // Muscle - 紫色
            G4Colour(0.5, 0.25, 0.0), // Liver - 棕色
            G4Colour(0.5, 0.5, 0.5),  // TrabecularBone - 灰色
            G4Colour(1.0, 1.0, 1.0)   // DenseBone - 白色
        };
        if (matIndex >= 0 && matIndex < 10) { // 假设材料索引 0-9
            physVol->GetLogicalVolume()->SetVisAttributes(new G4VisAttributes(colours[matIndex]));
        } else {
            G4cout << "Warning: Invalid material index " << matIndex << " at copyNo " << copyNo << G4endl;
            physVol->GetLogicalVolume()->SetVisAttributes(new G4VisAttributes(G4Colour(1.0, 0.0, 0.0))); // 红色警告
        }
        return fMaterials[matIndex];
    }

private:
    const PhantomData& fData;
    std::vector<G4Material*> fMaterials;
    double fMultiple;
};

// 读取 .g4dcm 文件（保持不变）
PhantomData ReadG4DCM(const std::string& filename,G4double fMultiple) {
    PhantomData data;
    std::ifstream file(filename);
    if (!file.is_open()) {
        G4cerr << "Error: Cannot open " << filename << G4endl;
        return data;
    }

    int numMaterials;
    file >> numMaterials;
    data.materialNames.resize(numMaterials);
    for (int i = 0; i < numMaterials; ++i) {
        int idx;
        file >> idx >> data.materialNames[i];
    }

    file >> data.nx >> data.ny >> data.nz;
    file >> data.minX >> data.maxX;
    file >> data.minY >> data.maxY;
    file >> data.minZ >> data.maxZ;

    // data.minX *= fMultiple;
    // data.maxX *= fMultiple;
    // data.minY *= fMultiple;
    // data.maxY *= fMultiple;
    // data.minZ *= fMultiple;
    // data.maxZ *= fMultiple;

    int totalVoxels = data.nx * data.ny * data.nz;
    data.materialIndices.resize(totalVoxels);
    for (int i = 0; i < totalVoxels; ++i) {
        file >> data.materialIndices[i];
    }

    data.densities.resize(totalVoxels);
    for (int i = 0; i < totalVoxels; ++i) {
        file >> data.densities[i];
    }

    file.close();
    return data;
}

DetectorConstruction::DetectorConstruction():fVerbose(false),fDetectorFilter(0.0),fMultiple(1.0),fMaxEnergy(120.0 * keV),fRotationMatrix2(new G4RotationMatrix()),fRotationMatrix(new G4RotationMatrix()),fRotationMatrix1(new G4RotationMatrix()),fDetectorLength(5.0 * mm), fDetectorWidth(5.0 * mm), fDetectorHeight(1.0 * mm),fFilterLength(5.0*mm),fFilterWidth(5.0*mm),fFilterHeight(5.0*mm),fPixelLength(5.0 * mm), fPixelWidth(5.0 * mm), fPixelHeight(1.0 * mm),fDSD(1.0*mm),fDSO(1.0*mm),fDetectorBiasLength(0.0*mm),fDetectorBiasWidth(0.0*mm),fPitch(1),fdegree(10*rad)//,fDetectorXRotationAngle(0*deg),fDetectorYRotationAngle(0*deg),fDetectorZRotationAngle(0*deg)
{
    fXCount = 1;
    fYCount = 1;
    fG4dcm = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/SSS/build///";
    fSpectrumPos = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/SSS/build///";
    fSpectrumEnergy = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/SSS/build";
    fOutput = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/SSS/";
    fBedDirection = 1;
    fDetectorRadius = 0;
    fFilterPos=1.0*mm;
    fFilterXBias = 0.0*mm;
    fFilterYBias = 0.0*mm;
    fObjectXBias = 0.0*mm;
    fObjectYBias = 0.0*mm;
    fObjectZBias = 0.0*mm;
    fTubeFilterLength = 5.0 * mm;
    fTubeFilterWidth = 5.0 * mm;
    fTubeFilterHeight = 5.0 * mm;
    fTubeFilterPos = 1.0 * mm;
    fTubeFilterXBias = 0.0 * mm;
    fTubeFilterYBias = 0.0 * mm;


    //fPhantomData = ReadG4DCM("/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/Script/build/out2.g4dcm");

    // 定义材料
    G4NistManager* nist = G4NistManager::Instance();
    // const std::vector<G4String>& materialNames = nist->GetNistMaterialNames();
    // for (const auto& name : materialNames) {
    //     G4Material* material = nist->FindOrBuildMaterial(name);
    //     if (material) {
    //         G4cout << " - " << name << " (Density: " << material->GetDensity() / (g/cm3) << " g/cm3)" << G4endl;
    //     }
    // }
    fMaterials.clear();

    G4Material* air = new G4Material("Air", 0.207 * g/cm3, 2);
    air->AddElement(nist->FindOrBuildElement("N"), 0.7);
    air->AddElement(nist->FindOrBuildElement("O"), 0.3);
    fMaterials.push_back(air);

    G4Material* lungInhale = new G4Material("LungInhale", 0.481 * g/cm3, 2);
    lungInhale->AddElement(nist->FindOrBuildElement("O"), 0.8);
    lungInhale->AddElement(nist->FindOrBuildElement("C"), 0.2);
    fMaterials.push_back(lungInhale);

    G4Material* lungExhale = new G4Material("LungExhale", 0.919 * g/cm3, 2);
    lungExhale->AddElement(nist->FindOrBuildElement("O"), 0.8);
    lungExhale->AddElement(nist->FindOrBuildElement("C"), 0.2);
    fMaterials.push_back(lungExhale);

    G4Material* adiposeTissue = new G4Material("AdiposeTissue", 0.979 * g/cm3, 2);
    adiposeTissue->AddElement(nist->FindOrBuildElement("C"), 0.6);
    adiposeTissue->AddElement(nist->FindOrBuildElement("H"), 0.4);
    fMaterials.push_back(adiposeTissue);

    G4Material* breast = new G4Material("Breast", 1.004 * g/cm3, 3);
    breast->AddElement(nist->FindOrBuildElement("C"), 0.5);
    breast->AddElement(nist->FindOrBuildElement("H"), 0.3);
    breast->AddElement(nist->FindOrBuildElement("O"), 0.2);
    fMaterials.push_back(breast);

    G4Material* water = new G4Material("Water", 1.043 * g/cm3, 2);
    water->AddElement(nist->FindOrBuildElement("H"), 2);
    water->AddElement(nist->FindOrBuildElement("O"), 1);
    fMaterials.push_back(water);

    G4Material* muscle = new G4Material("Muscle", 1.109 * g/cm3, 3);
    muscle->AddElement(nist->FindOrBuildElement("O"), 0.7);
    muscle->AddElement(nist->FindOrBuildElement("C"), 0.2);
    muscle->AddElement(nist->FindOrBuildElement("H"), 0.1);
    fMaterials.push_back(muscle);

    G4Material* liver = new G4Material("Liver", 1.113 * g/cm3, 3);
    liver->AddElement(nist->FindOrBuildElement("O"), 0.7);
    liver->AddElement(nist->FindOrBuildElement("C"), 0.2);
    liver->AddElement(nist->FindOrBuildElement("H"), 0.1);
    fMaterials.push_back(liver);

    G4Material* trabecularBone = new G4Material("TrabecularBone", 1.496 * g/cm3, 3);
    trabecularBone->AddElement(nist->FindOrBuildElement("Ca"), 0.4);
    trabecularBone->AddElement(nist->FindOrBuildElement("O"), 0.4);
    trabecularBone->AddElement(nist->FindOrBuildElement("P"), 0.2);
    fMaterials.push_back(trabecularBone);

    G4Material* denseBone = new G4Material("DenseBone", 1.654 * g/cm3, 3);
    denseBone->AddElement(nist->FindOrBuildElement("Ca"), 0.5);
    denseBone->AddElement(nist->FindOrBuildElement("O"), 0.3);
    denseBone->AddElement(nist->FindOrBuildElement("P"), 0.2);
    fMaterials.push_back(denseBone);
  fNumZ    = 60;
  fNumR    = 80;

  fNumE    = 200;
  fMaxEnergy = 120.0*keV;
  ////
  fMessenger = new DetectorConstructionMessenger(this);
  ////
  //fVerbose = false;

  //G4NistManager* nist = G4NistManager::Instance();

  fWorldMaterial   = nist->FindOrBuildMaterial("G4_AIR");
  fObjectMaterial  = nist->FindOrBuildMaterial("G4_WATER");
  fMetalWire       = nist->FindOrBuildMaterial("G4_W");
  fFilterMaterial = nist->FindOrBuildMaterial("G4_AIR");
  fTubeFilterMaterial = nist->FindOrBuildMaterial("G4_AIR");
  //fDetectorMaterial= man->FindOrBuildMaterial("G4_Si");
  // 定义基本元素
  G4Element* elCs = new G4Element("Cesium", "Cs", 55., 132.90545*g/mole);
  G4Element* elI  = new G4Element("Iodine", "I", 53., 126.90447*g/mole);
  G4Element* elTl = new G4Element("Thallium", "Tl", 81., 204.3833*g/mole);

  // 定义纯CsI材料
  G4Material* CsI = new G4Material("CsI", 4.51*g/cm3, 2);
  CsI->AddElement(elCs, 1);
  CsI->AddElement(elI, 1);

  // 定义掺杂了Tl的 CsI_Tl 材料
  G4double density = 4.51*g/cm3;
  G4int ncomponents = 3;
  G4Material* CsI_Tl = new G4Material("CsI_Tl", density, ncomponents);

  G4double wTl  = 0.002;     // Tl 质量分数
  G4double wCsI = 1.0 - wTl; // CsI 质量分数
  G4double wCs  = 0.5 * wCsI; // Cs 占比
  G4double wI   = 0.5 * wCsI; // I 占比

  CsI_Tl->AddElement(elCs, wCs);
  CsI_Tl->AddElement(elI,  wI);
  CsI_Tl->AddElement(elTl, wTl);
  // 获取定义好的 CsI_Tl 材料
  fDetectorMaterial = nist->FindOrBuildMaterial("CsI_Tl");
  if (!fDetectorMaterial) {
      G4cerr << "Error: Material CsI_Tl not found!" << G4endl;
  }
  G4Element* elC  = new G4Element("Carbon",   "C",  6.,  12.01 * g/mole);
  G4Element* elH  = new G4Element("Hydrogen", "H",  1.,   1.008 * g/mole);
  G4Element* elO  = new G4Element("Oxygen",   "O",  8.,  16.00 * g/mole);
  //G4Element* elI  = new G4Element("Iodine",   "I", 53., 126.90447 * g/mole);
  G4Element* elCa = new G4Element("Calcium",  "Ca", 20.,  40.08 * g/mole);
  // PMMA (C5H8O2)
  PMMA = new G4Material("PMMA", 1.18 * g/cm3, 3);
  PMMA->AddElement(elC, 5);
  PMMA->AddElement(elH, 8);
  PMMA->AddElement(elO, 2);

  // 水 (H2O)
  Water = new G4Material("Water", 1.0 * g/cm3, 2);
  Water->AddElement(elH, 2);
  Water->AddElement(elO, 1);

  // 聚乙烯 (C2H4)
  Polyethylene = new G4Material("Polyethylene", 0.93 * g/cm3, 2);
  Polyethylene->AddElement(elC, 2);
  Polyethylene->AddElement(elH, 4);

  // 含碘溶液 (简化为 98% 水 + 2% 碘，按质量分数)
  // IodineSolution = new G4Material("IodineSolution", 1.05 * g/cm3, 3);
  // IodineSolution->AddElement(elH, 0.01984); // H 的质量分数 ~ 0.01984 (基于 H2O 和 98% 水)
  // IodineSolution->AddElement(elO, 0.07936); // O 的质量分数 ~ 0.07936
  // IodineSolution->AddElement(elI, 0.02);    // I 的质量分数 2%

  // 钙 (碳酸钙 CaCO3)
  Calcium = new G4Material("CalciumCarbonate", 2.71 * g/cm3, 3);
  Calcium->AddElement(elCa, 1);
  Calcium->AddElement(elC,  1);
  Calcium->AddElement(elO,  3);
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}


DetectorConstruction::~DetectorConstruction()
{   
  ////
  delete fRotationMatrix;
  //if (fDetectorSD) delete fDetectorSD;
  for (G4Material* mat : fMaterials) {
        delete mat;
    }
  ////
  delete fMessenger;
}


void DetectorConstruction::InitialiseGeometryParameters()
{
  // Volumee sizes
  worldSize     = 500. * cm;
  fDetectorX     = 5.*cm;
  fDetectorY     = 5.*cm;
  fDetectorZ     = 0.02*cm;
  //DumpGeometryParameters();
}


G4VPhysicalVolume* DetectorConstruction::Construct()
{
    InitialiseGeometryParameters();

    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();
    // World
    // 读取 .g4dcm 文件
    std::vector<std::string> filenames;
    G4cout << "fG4dcm" << fG4dcm << G4endl;
    for (int i = 1; ; i++) {
        std::string filename = fG4dcm + std::to_string(i) + ".g4dcm";
        std::ifstream file(filename); // 尝试打开文件
        if (file.good()) {
            filenames.push_back(filename);
            file.close();
        } else {
            break; // 文件不存在，退出循环
        }
    }
    // 如果 filenames 为空，添加默认 g4dcm 文件
    // if (filenames.empty()) {
    //     std::string defaultFile = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/HelicalCT/build/1.g4dcm";
    //     std::ifstream file(defaultFile);
    //     if (file.good()) {
    //         filenames.push_back(defaultFile);
    //         G4cout << "No .g4dcm files found in " << fG4dcm << ", using default: " << defaultFile << G4endl;
    //         file.close();
    //     } else {
    //         G4cerr << "Error: Default file " << defaultFile << " does not exist" << G4endl;
    //     }
    // }
    // 打印找到的文件以便调试
    G4cout << "Found " << filenames.size() << " .g4dcm files:" << G4endl;
    for (const auto& filename : filenames) {
        G4cout << "  " << filename << G4endl;
    }
    //std::vector<std::string> filenames = {"/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/Script/build/1.g4dcm","/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/Script/build/2.g4dcm","/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/Script/build/3.g4dcm"}; // 替换为你的文件名
    for (const auto& filename : filenames) {
        fPhantomDatas.push_back(ReadG4DCM(filename,fMultiple));
    }

    //G4cout << "World volume created" << G4endl;
    G4Material* air = fMaterials[0];
    G4Box* solidWorld = new G4Box("World",worldSize,worldSize,worldSize);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,air,"World");
    fPhysWorld = new G4PVPlacement(0,G4ThreeVector(),"World",logicWorld,0,false,0);
    if (fPhantomDatas.empty()) {
            //TubeFilter
    G4Box* TubeFilter = new G4Box("TubeFilter", fTubeFilterLength/2.0, fTubeFilterWidth/2.0, fTubeFilterHeight/2.0);
    fLogicTubeFilter = new G4LogicalVolume(TubeFilter, fTubeFilterMaterial, "TubeFilter");
    G4ThreeVector detPos3( fTubeFilterXBias, fTubeFilterYBias,  -fDSO+fTubeFilterPos+fTubeFilterHeight/2.0 );
    fPhysTubeFilter = new G4PVPlacement(0, detPos3, fLogicTubeFilter, "TubeFilter", logicWorld, false, 0, false);

    //DetectorFilter
    G4Tubs* Filter = new G4Tubs("Filter",fDSD,fDSD+fFilterHeight,fFilterWidth/2.0, 90*deg-90*fFilterLength/(CLHEP::pi*fDSD)*deg, 180*fFilterLength/(CLHEP::pi*fDSD)*deg);
    fLogicFilter = new G4LogicalVolume(Filter, fFilterMaterial, "Filter");
    G4ThreeVector detPos2( fFilterXBias, fFilterYBias,  -fFilterPos-fDSO-fDetectorHeight/2.0 );
    G4RotationMatrix* rotation = new G4RotationMatrix();
    rotation->rotateX(-90 * CLHEP::pi / 180.0);
    fPhysFilter = new G4PVPlacement(rotation, detPos2, fLogicFilter, "Filter", logicWorld, false, 0, false);

    // MicroCT
    G4Tubs* solidDetector1 = new G4Tubs("Detector",fDSD,fDSD+fDetectorHeight,fYCount*fPixelWidth/2.0, 90*deg-90*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg, 180*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg);
    fLogicDetector = new G4LogicalVolume(solidDetector1, fWorldMaterial, "Detector");
    G4ThreeVector detPos1( fDetectorBiasLength, fDetectorBiasWidth, -fDSO+fDetectorHeight/2.0 );
    // 添加旋转：绕 x 轴旋转 90 度，使轴线从 z 轴变为 y 轴
    // G4RotationMatrix* rotation = new G4RotationMatrix();
    // rotation->rotateX(-90 * CLHEP::pi / 180.0);  // 绕 x 轴旋转 90 度
    fPhysDetector = new G4PVPlacement(fRotationMatrix, detPos1, fLogicDetector, "Detector", logicWorld, false, 0, false);

    //G4double x = fDetectorLength/fPixelLength,y = fDetectorWidth/fPixelWidth;
    G4double x = fXCount,y = fYCount;
    G4Tubs* cellSolid = new G4Tubs("cell",fDSD,fDSD+fDetectorHeight,fPixelWidth/2.0, 90*deg-90*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg, 180*fPixelLength*fXCount/((CLHEP::pi*fDSD)*(x)) * deg);
    cellLogical = new G4LogicalVolume(cellSolid, fDetectorMaterial, "cell");
    for (int i = 0;i <=y-1;i += 1){
      for (int j = 0;j <=x-1;j += 1){
      // G4Tubs* cellSolid1 = new G4Tubs("cell",11 * cm,11.02 * cm,0.2 * cm, 0, 1 * CLHEP::pi / 180.0);
      // cellLogical = new G4LogicalVolume(cellSolid1, fDetectorMaterial, "cell");
      G4RotationMatrix* rot = new G4RotationMatrix();
      rot->rotateZ((-j*180*fDetectorLength/((CLHEP::pi*fDSD)*(x))) * CLHEP::pi / 180.0);
      new G4PVPlacement(rot, G4ThreeVector(0, 0, ((i * fPixelWidth) - (fDetectorWidth/2.0-fPixelWidth/2.0))), cellLogical, "cell", fLogicDetector, false, i*y + j, false);
      }
    }
        G4cerr << "Warning: No .g4dcm files loaded, skipping phantom construction" << G4endl;
        return fPhysWorld; // 假设 fPhysWorld 是世界体积
    }
    // 计算所有体模的总 Z 范围
    double totalMinZ = fMultiple*fPhantomDatas[0].minZ;
    double totalMaxZ = fMultiple*fPhantomDatas[0].maxZ;
    for (size_t i = 1; i < fPhantomDatas.size(); ++i) {
        totalMinZ = std::min(totalMinZ, fMultiple*fPhantomDatas[i].minZ);
        totalMaxZ = std::max(totalMaxZ, fMultiple*fPhantomDatas[i].maxZ);
    }
    double totalZCenter = (totalMinZ + totalMaxZ) / 2; // 整体 Z 中心

    // 为每个体模创建几何体并放置
    for (size_t i = 0; i < fPhantomDatas.size(); ++i) {
        const PhantomData& data = fPhantomDatas[i];
        G4cout << "fMultiple" << fMultiple << G4endl;
        // 创建母体框
        double phantomSizeX = (data.maxX - data.minX) / 2 * fMultiple * mm;
        double phantomSizeY = (data.maxY - data.minY) / 2 * fMultiple * mm;
        double phantomSizeZ = (data.maxZ - data.minZ) / 2 * fMultiple * mm;
        G4cout << "phantomSizeX" << phantomSizeX << "phantomSizeY" << phantomSizeY << "phantomSizeZ" << phantomSizeZ << G4endl;
        G4Box* phantomBox = new G4Box("Phantom" + std::to_string(i), phantomSizeX, phantomSizeY, phantomSizeZ);
        G4LogicalVolume* logicPhantom = new G4LogicalVolume(phantomBox, air, "Phantom" + std::to_string(i));
        logicPhantom->SetVisAttributes(new G4VisAttributes(G4Colour(0.9, 0.9, 0.9)));

        // 创建体素逻辑体积
        double voxelSizeX = (data.maxX - data.minX) / data.nx * fMultiple * mm;
        double voxelSizeY = (data.maxY - data.minY) / data.ny * fMultiple * mm;
        double voxelSizeZ = (data.maxZ - data.minZ) / data.nz * fMultiple * mm;
        G4Box* voxelBox = new G4Box("Voxel" + std::to_string(i), voxelSizeX / 2, voxelSizeY / 2, voxelSizeZ / 2);
        G4LogicalVolume* logicVoxel = new G4LogicalVolume(voxelBox, air, "Voxel" + std::to_string(i));
        G4cout << "Voxel Size - X: " << voxelSizeX / mm << " mm, Y: " << voxelSizeY / mm
           << " mm, Z: " << voxelSizeZ / mm << " mm" << G4endl;
        // 创建参数化对象
        PhantomParameterisation* param = new PhantomParameterisation(data, fMaterials, fMultiple);

        // 将参数化体素放置到母体框中
        new G4PVParameterised("Voxels" + std::to_string(i), logicVoxel, logicPhantom, kUndefined, 
                              data.nx * data.ny * data.nz, param);

        // 计算体模的 Z 位置（相对于整体中心）
        double phantomZ = (data.minZ + data.maxZ) / 2 * fMultiple - totalZCenter; // 偏移量使整体居中
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateX(90 * CLHEP::pi / 180.0);
        // 设置体模位置
        // G4ThreeVector phantomPos((data.minX + data.maxX) / 2 * mm,
        //                          (data.minY + data.maxY) / 2 * mm,
        //                          phantomZ * mm);



        // //Decom可视化
        // if (fBedDirection == 1){
        //     G4ThreeVector phantomPos((data.minX + data.maxX) / 2  * mm +fObjectXBias,
        //                 phantomZ * mm+fdegree*fDetectorWidth*fPitch/360 +fObjectYBias,
        //                 (data.minY + data.maxY) / 2  * mm+ fObjectZBias);
        //     fPhysObject=new G4PVPlacement(fRotationMatrix2, phantomPos, logicPhantom, "Phantom" + std::to_string(i), logicWorld, false, 0);
        // }else{
        //     G4ThreeVector phantomPos((data.minX + data.maxX) / 2  * mm +fObjectXBias,
        //                 -(phantomZ * mm+fdegree*fDetectorWidth*fPitch/360 +fObjectYBias),
        //                 (data.minY + data.maxY) / 2  * mm+ fObjectZBias);
        //    fPhysObject=new G4PVPlacement(fRotationMatrix2, phantomPos, logicPhantom, "Phantom" + std::to_string(i), logicWorld, false, 0);
        // } 






        // G4ThreeVector phantomPos((data.minX + data.maxX) / 2  * mm +fObjectXBias,
        //                         phantomZ * mm+fdegree*fDetectorWidth*fPitch/360 +fObjectYBias,
        //                         (data.minY + data.maxY) / 2  * mm+ fObjectZBias);
        // fPhysObject=new G4PVPlacement(fRotationMatrix2, phantomPos, logicPhantom, "Phantom" + std::to_string(i), logicWorld, false, 0);

        // G4cout << "Phantom " << i << " Position: " << phantomPos / mm << " mm" << G4endl;
        // G4cout << "Total Voxels: " << data.nx * data.ny * data.nz << G4endl;
    }
    
    //TubeFilter
    G4Box* TubeFilter = new G4Box("TubeFilter", fTubeFilterLength/2.0, fTubeFilterWidth/2.0, fTubeFilterHeight/2.0);
    fLogicTubeFilter = new G4LogicalVolume(TubeFilter, fTubeFilterMaterial, "TubeFilter");
    G4ThreeVector detPos3( fTubeFilterXBias, fTubeFilterYBias,  -fDSO+fTubeFilterPos+fTubeFilterHeight/2.0 );
    fPhysTubeFilter = new G4PVPlacement(0, detPos3, fLogicTubeFilter, "TubeFilter", logicWorld, false, 0, false);
    
    if (fDetectorFilter == 1){
        //DetectorFilter
        G4Tubs* Filter = new G4Tubs("Filter",fDSD,fDSD+fFilterHeight,fFilterWidth/2.0, 90*deg-90*fFilterLength/(CLHEP::pi*fDSD)*deg, 180*fFilterLength/(CLHEP::pi*fDSD)*deg);
        fLogicFilter = new G4LogicalVolume(Filter, fFilterMaterial, "Filter");
        G4ThreeVector detPos2( fFilterXBias, fFilterYBias,  -fFilterPos-fDSO-fDetectorHeight/2.0 );
        G4RotationMatrix* rotation = new G4RotationMatrix();
        rotation->rotateX(-90 * CLHEP::pi / 180.0);
        fPhysFilter = new G4PVPlacement(rotation, detPos2, fLogicFilter, "Filter", logicWorld, false, 0, false);
    }
    if (fDetectorRadius == -1){
        // MicroCT
        G4Tubs* solidDetector1 = new G4Tubs("Detector",fDSD,fDSD+fDetectorHeight,fYCount*fPixelWidth/2.0, 90*deg-90*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg, 180*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg);
        fLogicDetector = new G4LogicalVolume(solidDetector1, fWorldMaterial, "Detector");
        G4ThreeVector detPos1( fDetectorBiasLength, fDetectorBiasWidth, -fDSO+fDetectorHeight/2.0 );
        // 添加旋转：绕 x 轴旋转 90 度，使轴线从 z 轴变为 y 轴
        // G4RotationMatrix* rotation = new G4RotationMatrix();
        // rotation->rotateX(-90 * CLHEP::pi / 180.0);  // 绕 x 轴旋转 90 度
        fPhysDetector = new G4PVPlacement(fRotationMatrix, detPos1, fLogicDetector, "Detector", logicWorld, false, 0, false);

        //G4double x = fDetectorLength/fPixelLength,y = fDetectorWidth/fPixelWidth;
        G4cout << "Detector Pixel: " << fXCount << "*" << fYCount << G4endl;
        G4double x = fXCount,y = fYCount;
        G4Tubs* cellSolid = new G4Tubs("cell",fDSD,fDSD+fDetectorHeight,fPixelWidth/2.0, 90*deg-90*fPixelLength*fXCount/(CLHEP::pi*fDSD)*deg, 180*fPixelLength*fXCount/((CLHEP::pi*fDSD)*(x)) * deg);
        cellLogical = new G4LogicalVolume(cellSolid, fDetectorMaterial, "cell");
        for (int i = 0;i <=y-1;i += 1){
        for (int j = 0;j <=x-1;j += 1){
        G4RotationMatrix* rot = new G4RotationMatrix();
        rot->rotateZ((-j*180*fDetectorLength/((CLHEP::pi*fDSD)*(x))) * CLHEP::pi / 180.0);
        new G4PVPlacement(rot, G4ThreeVector(0, 0, ((i * fPixelWidth) - (fDetectorWidth/2.0-fPixelWidth/2.0))), cellLogical, "cell", fLogicDetector, false, i*y + j, false);
        }
        }
    }else{
        // 定义探测器，使用动态尺寸
        G4Box* solidDetector = new G4Box("Detector", fDetectorLength/2, fDetectorWidth/2, fDetectorHeight/2);
        fLogicDetector = new G4LogicalVolume(solidDetector, fWorldMaterial, "Detector");
        fLogicDetector->SetVisAttributes(new G4VisAttributes(G4Colour(0.9, 0.9, 0.9)));
        if (!fLogicDetector) {
            G4Exception("DetectorConstruction::Construct", "GeometryError", FatalException,
                        "Failed to create Detector logical volume!");
        }
        G4ThreeVector detPos( fDetectorBiasLength, fDetectorBiasWidth, fDSD-fDSO);
        fRotationMatrix->rotateX(90 * CLHEP::pi / 180.0);
        fPhysDetector = new G4PVPlacement(fRotationMatrix, detPos, fLogicDetector, "Detector", logicWorld, false, 0, false);

        G4Box* cellSolid = new G4Box("cell", fPixelLength/2, fPixelWidth/2, fPixelHeight/2);
        cellLogical = new G4LogicalVolume(cellSolid, fDetectorMaterial, "cell");
        cellLogical->SetVisAttributes(new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)));
        G4double x = fDetectorLength/fPixelLength,y = fDetectorWidth/fPixelWidth;
        for (G4int i = 0; i < x; ++i) {
        for (G4int j = 0; j < y; ++j){
            G4double xPos = (-fDetectorLength/2 + fPixelLength/2) + i * fPixelLength;
            G4double yPos = (-fDetectorWidth/2 + fPixelWidth/2) + j * fPixelWidth;
            G4double zPos = 0.;
            new G4PVPlacement(0, G4ThreeVector(xPos, yPos, zPos), cellLogical, "cell", fLogicDetector,
                            false, i*y + j , false);
        }
        }
    }
  return fPhysWorld;
}

////    bool sizeChanged = (fDetectorLength != length);
void DetectorConstruction::SetObjectRotationAngle(G4double angle) {
    fCurrentAngle = angle;
    fdegree = angle * 180.0 / CLHEP::pi;
    if (fPhysObject) {
        *fRotationMatrix2 = G4RotationMatrix();
        fRotationMatrix2->rotateY(angle+90 * CLHEP::pi / 180.0); // 绕 Z 轴旋转指定的角度
        fPhysObject->SetRotation(fRotationMatrix2); // 更新物体的旋转
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}
void DetectorConstruction::SetObjectRotationAngle1(G4double angleX, G4double angleY, G4double angleZ) {
    fCurrentAngle = angleX;
    fdegree = angleZ * 180.0 / CLHEP::pi;
    if (fPhysDetector) {
        *fRotationMatrix = G4RotationMatrix();
        fRotationMatrix2->rotateX(angleX-90 * CLHEP::pi / 180.0);
        fRotationMatrix2->rotateY(angleY);
        fRotationMatrix2->rotateZ(angleZ);
        G4cout << "fRotationMatrix2" << fRotationMatrix2 << G4endl;
        G4cout << angleZ << G4endl;
        fPhysDetector->SetRotation(fRotationMatrix2);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }
}
void DetectorConstruction::SetDetectorModuleNumber(G4double Num) {
    fModuleNum = Num;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}

void DetectorConstruction::SetDetectorBedDirection(G4double Bed) {
    fBedDirection = Bed;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetMultiple(G4double mul) {
    fMultiple = mul;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorFilter(G4double filter) {
    fDetectorFilter = filter;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterMaterial(const G4String& materialName) {
    G4cout << "SetDetectorMaterial called with: " << materialName << G4endl;
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* newMaterial = nist->FindOrBuildMaterial(materialName);
    if (!newMaterial) {
        // 如果未找到材料，在前面添加 "G4_" 后再次查找
        G4String modifiedMaterialName = "G4_" + materialName;
        G4cout << "Material " << materialName << " not found, trying " << modifiedMaterialName << G4endl;
        newMaterial = nist->FindOrBuildMaterial(modifiedMaterialName);
    }
    if (!newMaterial) {
        CreateCustomMaterial(materialName);
        newMaterial = fFilterMaterial; // 使用创建的自定义材料
    }
    if (newMaterial) {
        fFilterMaterial = newMaterial;
        fIsMaterialSet = true;
        G4cout << "Material set to: " << materialName << G4endl;
        if (fPhysWorld) {
            G4RunManager::GetRunManager()->ReinitializeGeometry(); // 重新构建几何
        }
    } else {
        G4cout << "Warning: Material " << materialName << " not found, using default (G4_AIR)." << G4endl;
        fFilterMaterial = fWorldMaterial; // 默认使用空气
    }
}
void DetectorConstruction::SetTubeFilterMaterial(const G4String& materialName) {
    G4cout << "SetDetectorMaterial called with: " << materialName << G4endl;
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* newMaterial = nist->FindOrBuildMaterial(materialName);
    if (!newMaterial) {
        // 如果未找到材料，在前面添加 "G4_" 后再次查找
        G4String modifiedMaterialName = "G4_" + materialName;
        G4cout << "Material " << materialName << " not found, trying " << modifiedMaterialName << G4endl;
        newMaterial = nist->FindOrBuildMaterial(modifiedMaterialName);
    }
    if (!newMaterial) {
        CreateCustomMaterial(materialName);
        newMaterial = fDetectorMaterial; // 使用创建的自定义材料
    }
    if (newMaterial) {
        fTubeFilterMaterial = newMaterial;
        fIsMaterialSet = true;
        G4cout << "Material set to: " << materialName << G4endl;
        if (fPhysWorld) {
            G4RunManager::GetRunManager()->ReinitializeGeometry(); // 重新构建几何
        }
    } else {
        G4cout << "Warning: Material " << materialName << " not found, using default (G4_AIR)." << G4endl;
        fTubeFilterMaterial = fWorldMaterial; // 默认使用空气
    }
}
void DetectorConstruction::SetDetectorRadius(G4double Radius) {
    fDetectorRadius = Radius;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDDO(G4double ddo) {
    fDDO = ddo;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorRotationAngle(G4double angleX, G4double angleY, G4double angleZ) {
    if (fPhysDetector) {
        *fRotationMatrix = G4RotationMatrix();
        G4cout << angleX-90 * CLHEP::pi / 180.0 << G4endl;
        fRotationMatrix->rotateX(angleX-90 * CLHEP::pi / 180.0);
        fRotationMatrix->rotateY(angleY);
        fRotationMatrix->rotateZ(angleZ);
        fPhysDetector->SetRotation(fRotationMatrix);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }
}
void DetectorConstruction::SetDetectorXRotationAngle(G4double angle) {
    if (fPhysDetector) {
        *fRotationMatrix1 = G4RotationMatrix();
        G4cout << angle << G4endl;
        G4cout << 90*deg << G4endl;
        fRotationMatrix1->rotateX(angle);   //-90 * CLHEP::pi / 180.0
        fPhysDetector->SetRotation(fRotationMatrix1);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }
}
void DetectorConstruction::SetDetectorYRotationAngle(G4double angle) {
    if (fPhysDetector) {
        *fRotationMatrix1 = G4RotationMatrix();
        G4cout << angle << G4endl;
        fRotationMatrix1->rotateY(angle-90 * CLHEP::pi / 180.0);
        G4cout << fRotationMatrix1 << G4endl;
        fPhysDetector->SetRotation(fRotationMatrix1);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }
}
void DetectorConstruction::SetDetectorZRotationAngle(G4double angle) {
    if (fPhysDetector) {
        *fRotationMatrix1 = G4RotationMatrix();
        G4cout << angle << G4endl;
        fRotationMatrix1->rotateZ(angle);
        fPhysDetector->SetRotation(fRotationMatrix1);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
        if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
        }
    }
}
void DetectorConstruction::SetDetectorXSize(G4double length) {
    fDetectorLength = length;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorYSize(G4double width) {
    fDetectorWidth = width;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorZSize(G4double height) {
    fDetectorHeight = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetObjectXBias(G4double XBias) {
    fObjectXBias = XBias;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetObjectYBias(G4double YBias) {
    fObjectYBias = YBias;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetObjectZBias(G4double ZBias) {
    fObjectZBias = ZBias;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterXSize(G4double length) {
    fFilterLength = length;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterYSize(G4double width) {
    fFilterWidth = width;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterZSize(G4double height) {
    fFilterHeight = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterXBias(G4double height) {
    fFilterXBias = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterYBias(G4double height) {
    fFilterYBias = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetFilterPos(G4double pos) {
    fFilterPos = pos;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterXSize(G4double length) {
    fTubeFilterLength = length;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterYSize(G4double width) {
    fTubeFilterWidth = width;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterZSize(G4double height) {
    fTubeFilterHeight = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterXBias(G4double height) {
    fTubeFilterXBias = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterYBias(G4double height) {
    fTubeFilterYBias = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetTubeFilterPos(G4double pos) {
    fTubeFilterPos = pos;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorXBias(G4double length) {
    fDetectorBiasLength = length;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorYBias(G4double width) {
    fDetectorBiasWidth = width;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorZBias(G4double height) {
    fDetectorBiasHeight = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDSD(G4double distance) {
    fDSD = distance;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDSO(G4double distance) {
    fDSO = distance;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}

void DetectorConstruction::SetG4dcm(const G4String& g4dcm) {
    fG4dcm = g4dcm;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetSpectrumPos(const G4String& spectrumpos) {
    fSpectrumPos = spectrumpos;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetSpectrumEnergy(const G4String& spectrumenergy) {
    fSpectrumEnergy = spectrumenergy;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetOutputPath(const G4String& output) {
    fOutput = output;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetDetectorMaterial(const G4String& materialName) {
    G4cout << "SetDetectorMaterial called with: " << materialName << G4endl;
    G4NistManager* nist = G4NistManager::Instance();
    G4Material* newMaterial = nist->FindOrBuildMaterial(materialName);
    if (!newMaterial) {
        // 如果未找到材料，在前面添加 "G4_" 后再次查找
        G4String modifiedMaterialName = "G4_" + materialName;
        G4cout << "Material " << materialName << " not found, trying " << modifiedMaterialName << G4endl;
        newMaterial = nist->FindOrBuildMaterial(modifiedMaterialName);
    }
    if (!newMaterial) {
        CreateCustomMaterial(materialName);
        newMaterial = fDetectorMaterial; // 使用创建的自定义材料
    }
    if (newMaterial) {
        fDetectorMaterial = newMaterial;
        fIsMaterialSet = true;
        G4cout << "Material set to: " << materialName << G4endl;
        if (fPhysWorld) {
            G4RunManager::GetRunManager()->ReinitializeGeometry(); // 重新构建几何
        }
    } else {
        G4cout << "Warning: Material " << materialName << " not found, using default (G4_AIR)." << G4endl;
        fDetectorMaterial = fWorldMaterial; // 默认使用空气
    }
}
void DetectorConstruction::CreateCustomMaterial(const G4String& materialInfo) {
    G4cout << "Creating custom material from: " << materialInfo << G4endl;
    std::istringstream iss(materialInfo);
    G4String materialName;
    G4double density;
    G4int ncomponents;
    if (!(iss >> materialName >> density >> ncomponents)) {
        G4cout << "Error: Invalid custom material format. Expected: name density(g/cm3) ncomponents" << G4endl;
        return;
    }

    G4Material* customMaterial = new G4Material(materialName, density, ncomponents);
    for (G4int i = 0; i < ncomponents; ++i) {
        G4String elementName;
        G4int z;
        G4double a;
        G4int numAtoms;
        //G4double fraction;
        //if (!(iss >> elementName >> z >> a >> fraction)) {
        if (!(iss >> elementName >> z >> a >> numAtoms)) {
            G4cout << "Error: Invalid element data for component " << i + 1 << G4endl;
            delete customMaterial;
            return;
        }
        G4Element* element = new G4Element(elementName, elementName, z, a * g/mole);
        //customMaterial->AddElement(element, fraction);
        customMaterial->AddElement(element, numAtoms);
    }

    fDetectorMaterial = customMaterial;
    G4cout << "Custom material " << materialName << " created with density " << density << " g/cm3" << G4endl;
}

void DetectorConstruction::SetPixelXCount(G4double XCount) {
    fXCount = XCount;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetPixelYCount(G4double YCount) {
    fYCount = YCount;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetPixelXSize(G4double length) {
    bool sizeChanged = (fPixelLength != length);
    fPixelLength = length;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld && sizeChanged) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetPixelYSize(G4double width) {
    bool sizeChanged = (fPixelWidth != width);
    fPixelWidth = width;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld && sizeChanged) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetPixelZSize(G4double height) {
    bool sizeChanged = (fPixelHeight != height);
    fPixelHeight = height;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld && sizeChanged) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
void DetectorConstruction::SetPitch(G4double pitch) {
    bool sizeChanged = (fPitch != pitch);
    fPitch = pitch;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();  // Notify geometry change
    if (fPhysWorld && sizeChanged) {
        G4RunManager::GetRunManager()->ReinitializeGeometry();
    }
}
G4double DetectorConstruction::GetCurrentAngle() const {
    return fCurrentAngle;
}

////
// void DetectorConstruction::ConstructSDandField()
// {   static bool isSDAdded = false;
//     if (!isSDAdded) {
//     G4SDManager* sdManager = G4SDManager::GetSDMpointer();
//     DetectorSD* fDetectorSD = new DetectorSD("detectorSD");
//     sdManager->AddNewDetector(fDetectorSD);
//     cellLogical->SetSensitiveDetector(fDetectorSD);
//     isSDAdded = true;
//     }
//     //cellLogical1->SetSensitiveDetector(fDetectorSD);
//     //logicObject->SetSensitiveDetector(fDetectorSD);
// }
void DetectorConstruction::ConstructSDandField() {
    G4SDManager* sdManager = G4SDManager::GetSDMpointer();
    if (!fIsSensitiveDetectorRegistered) {
        fDetectorSD = new DetectorSD("detectorSD");
        sdManager->AddNewDetector(fDetectorSD);
        if (cellLogical) {
            cellLogical->SetSensitiveDetector(fDetectorSD);
        }
        fIsSensitiveDetectorRegistered = true;
        G4cout << "Sensitive detector registered at " << fDetectorSD << G4endl;
    } else {
        G4cout << "Sensitive detector already registered at " << fDetectorSD << ", reusing." << G4endl;
        if (cellLogical && fDetectorSD) {
            G4VSensitiveDetector* currentSD = cellLogical->GetSensitiveDetector();
            if (currentSD != fDetectorSD) {
                cellLogical->SetSensitiveDetector(fDetectorSD);
                G4cout << "Reassociated sensitive detector." << G4endl;
            }
        } else {
            G4cout << "Warning: cellLogical or fDetectorSD is null during reuse." << G4endl;
        }
    }
}