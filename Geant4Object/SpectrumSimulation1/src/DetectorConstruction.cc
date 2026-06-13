/// \file medical/GammaTherapy/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
#include "DetectorConstruction.hh"
#include "DetectorSD.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
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
#include "DetectorConstructionMessenger.hh"
#include "DetectorMaterials.hh"
using CLHEP::cm;
using CLHEP::keV;
using CLHEP::mm;
using CLHEP::um;

DetectorConstruction::DetectorConstruction()
    : fVerbose(false), fMaxEnergy(120.0 * keV), fRotationMatrix(new G4RotationMatrix()),
      fAnodeRotationAngle(0.0), fAnodeMaterialName("W"), fPhysAnode(nullptr),
      fPhysDetector(nullptr), fAnodeLogical(nullptr),
      fFilterMaterialName("Al"), fPhysFilter(nullptr), fFilterLogical(nullptr),
      fFilter1MaterialName("Al"), fPhysFilter1(nullptr), fFilter1Logical(nullptr),
      fFilter2MaterialName("Cu"), fPhysFilter2(nullptr), fFilter2Logical(nullptr),
      fDetectorMaterialName("Galactic"), fDetectorLogical(nullptr)
{
    fNumZ = 60;
    fNumR = 80;
    fNumE = 200;
    fMaxEnergy = 120.0 * keV;
    DetectorMaterials materials;
    materials.DefineMaterials();
    fMessenger = new DetectorConstructionMessenger(this);

    G4NistManager *man = G4NistManager::Instance();
    fWorldMaterial = man->FindOrBuildMaterial("G4_Galactic");
    fObjectMaterial = man->FindOrBuildMaterial("G4_AIR");
    fDetectorMaterial = man->FindOrBuildMaterial("G4_Galactic");
    materials.DefineMaterials();

    G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

DetectorConstruction::~DetectorConstruction()
{
    delete fRotationMatrix;
    delete fMessenger;
}

void DetectorConstruction::InitialiseGeometryParameters()
{
    worldSize = 100. * cm;

    // Virtual detection plane: 4cm x 4cm square, very thin
    fDetectorX = 2 * cm;
    fDetectorY = 2 * cm;
    fDetectorZ = 0.00000002 * mm; // Very thin to act as a virtual plane
}

G4VPhysicalVolume *DetectorConstruction::Construct()
{
    InitialiseGeometryParameters();

    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    // World
    G4Box *solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    G4LogicalVolume *logicWorld = new G4LogicalVolume(solidWorld, fWorldMaterial, "World");
    fPhysWorld = new G4PVPlacement(0, G4ThreeVector(), "World", logicWorld, 0, false, 0);

    // ===============================
    // 阳极靶面建模 (X射线管阳极) - 支持动态材料和角度
    // ===============================

    // 1. 根据设定获取阳极靶材料
    G4NistManager *nist = G4NistManager::Instance();
    G4Material *anodeMaterial;
    if (fAnodeMaterialName == "Galactic")
    {
        anodeMaterial = nist->FindOrBuildMaterial("G4_Galactic");
    }
    else if (fAnodeMaterialName == "W")
    {
        anodeMaterial = nist->FindOrBuildMaterial("G4_W");
    }
    else if (fAnodeMaterialName == "Mo")
    {
        anodeMaterial = nist->FindOrBuildMaterial("G4_Mo");
    }
    else
    {
        anodeMaterial = nist->FindOrBuildMaterial("G4_W"); // 默认钨
        G4cout << "Warning: Unknown anode material " << fAnodeMaterialName
               << ", using tungsten (W) instead." << G4endl;
    }

    // Anode target geometry from the reference setup: 12cm x 12cm x 5um tungsten slab.
    G4double cubeHalfLengthXY = 6 * cm;
    G4double cubeHalfLengthZ = 2.5 * um;
    G4Box *anode_solid = new G4Box("AnodeSolid",
                                   cubeHalfLengthXY,
                                   cubeHalfLengthXY,
                                   cubeHalfLengthZ);

    fAnodeLogical = new G4LogicalVolume(anode_solid,
                                        anodeMaterial,
                                        "AnodeLogical");
    G4VisAttributes *anode_vis;
    if (fAnodeMaterialName == "Galactic")
    {
        anode_vis = new G4VisAttributes(G4Colour(0.2, 0.2, 0.8, 0.3)); // 蓝色半透明（真空）
    }
    else if (fAnodeMaterialName == "Mo")
    {
        anode_vis = new G4VisAttributes(G4Colour(0.6, 0.8, 0.6, 0.8)); // 绿色半透明（钼）
    }
    else
    {
        anode_vis = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.8)); // 灰色半透明（钨）
    }
    anode_vis->SetForceSolid(true);
    fAnodeLogical->SetVisAttributes(anode_vis);

    // Place the anode target while preserving the existing physical-volume name.
    G4RotationMatrix *anodeRotation = new G4RotationMatrix();

    fPhysAnode = new G4PVPlacement(anodeRotation,
                                   G4ThreeVector(0 * cm, 0 * cm, -0.1 * cm),
                                   fAnodeLogical,
                                   "AnodePhysical",
                                   logicWorld,
                                   false,
                                   0,
                                   true);

    G4cout << "Created Anode Target:" << G4endl;
    G4cout << "  - Material: " << fAnodeMaterialName << G4endl;
    G4cout << "  - Size: 12cm x 12cm x 5um" << G4endl;
    G4cout << "  - Position: (0, 0, -0.1cm)" << G4endl;
    G4cout << "  - Rotation angle: " << fAnodeRotationAngle / deg << " degrees" << G4endl;

    // ===============================
    // 探测器系统构建 - 支持动态材料和尺寸调整
    // ===============================

    // 虚拟探测平面 - 用于记录X射线光子的能量谱
    // 支持动态材料和尺寸配置

    // 获取探测器材料
    G4Material *detectorMaterial;
    if (fDetectorMaterialName == "Si")
    {
        detectorMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Si");
    }
    else if (fDetectorMaterialName == "Galactic")
    {
        detectorMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
    }
    else if (fDetectorMaterialName == "CESIUM_IODIDE")
    {
        detectorMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_CESIUM_IODIDE");
    }
    else
    {
        detectorMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"); // 默认为真空
        G4cout << "Warning: Unknown detector material " << fDetectorMaterialName
               << ", using Galactic instead." << G4endl;
    }

    G4Box *solidDetector = new G4Box("DetectorPlane", fDetectorX, fDetectorY, fDetectorZ);
    fDetectorLogical = new G4LogicalVolume(solidDetector, detectorMaterial, "DetectorPlane");

    // 根据材料设置可视化属性
    G4VisAttributes *detectorVisAtt;
    if (fDetectorMaterialName == "Si")
    {
        detectorVisAtt = new G4VisAttributes(G4Colour(0.2, 0.8, 0.2)); // 绿色（硅）
    }
    else if (fDetectorMaterialName == "CESIUM_IODIDE")
    {
        detectorVisAtt = new G4VisAttributes(G4Colour(0.8, 0.6, 0.2)); // 橙色（碘化铯）
    }
    else
    {
        detectorVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // 亮绿色（真空）
    }
    detectorVisAtt->SetForceSolid(true);
    fDetectorLogical->SetVisAttributes(detectorVisAtt);

    // 创建探测器旋转矩阵 - 与阳极靶保持相对平行
    G4RotationMatrix *detectorRotation = new G4RotationMatrix();

    // Place the detection plane at z = -4 cm
    G4double detectorPosX = 0 * cm;
    G4double detectorPosY = 0 * cm;
    G4double detectorPosZ = -4 * cm;

    fPhysDetector = new G4PVPlacement(
        detectorRotation,                             // 与阳极靶同步旋转
        G4ThreeVector(detectorPosX, detectorPosY, detectorPosZ),
        fDetectorLogical,                             // Logical volume
        "PhysDetectorPlane",                          // Name
        logicWorld,                                   // Mother volume
        false,                                        // No boolean operations
        0,                                            // Copy number
        true                                          // Check overlaps
    );

    // 铝过滤层 - 用于硬化X射线束，滤除低能X射线
    // 尺寸: 4cm x 4cm x 2mm，位于探测器前方，支持动态材料选择
    // 使用成员变量以支持运行时通过宏命令修改尺寸
    G4double filterHalfLengthX = fFilter1X; // 使用 fFilter1X（半长度）
    G4double filterHalfLengthY = fFilter1Y; // 使用 fFilter1Y（半长度）
    G4double filterHalfLengthZ = fFilter1Z; // 使用 fFilter1Z（半厚度）
    G4double filterPosX = 0 * cm;
    G4double filterPosY = 0 * cm;
    G4double filterPosZ = fFilter1PosZ; // 按设定位置

    // 根据设定获取过滤层材料
    G4Material *filterMaterial;

    // 智能材料识别：如果材料名不以G4_开头且不是特殊情况，自动添加G4_前缀
    G4String materialName = fFilterMaterialName;
    if (materialName != "Galactic" && materialName.find("G4_") != 0)
    {
        materialName = "G4_" + materialName;
    }

    // 尝试查找材料
    filterMaterial = nist->FindOrBuildMaterial(materialName);
    if (!filterMaterial)
    {
        G4cout << "Warning: Material " << materialName << " not found, using G4_Al instead." << G4endl;
        filterMaterial = nist->FindOrBuildMaterial("G4_Al");
        fFilterMaterialName = "G4_Al";
    }
    else
    {
        G4cout << "Filter material successfully set to: " << materialName << G4endl;
        fFilterMaterialName = materialName; // 更新为完整的材料名
    }

    G4cout << "Construct: Filter1 sizes (half-lengths) X=" << filterHalfLengthX / cm << " cm, Y=" << filterHalfLengthY / cm << " cm, Z=" << filterHalfLengthZ / mm << " mm" << G4endl;
    G4Box *solidFilter = new G4Box("FilterLayer", filterHalfLengthX, filterHalfLengthY, filterHalfLengthZ);
    fFilterLogical = new G4LogicalVolume(solidFilter, filterMaterial, "FilterLayer");

    // 根据材料设置可视化属性
    G4VisAttributes *filterVisAtt;
    if (fFilterMaterialName == "Al")
    {
        filterVisAtt = new G4VisAttributes(G4Colour(0.8, 0.8, 0.9)); // Light blue-grey for aluminum
    }
    else
    {
        filterVisAtt = new G4VisAttributes(G4Colour(0.2, 0.2, 0.8, 0.3)); // Blue transparent for vacuum
    }
    filterVisAtt->SetForceSolid(true);
    fFilterLogical->SetVisAttributes(filterVisAtt);

    // 过滤层也与阳极靶保持相对平行
    G4RotationMatrix *filterRotation = new G4RotationMatrix();

    fPhysFilter = new G4PVPlacement(
        filterRotation,                                    // 与阳极靶同步旋转
        G4ThreeVector(filterPosX, filterPosY, filterPosZ), // Position at configurable Z
        fFilterLogical,                                    // Logical volume
        "PhysFilterLayer1",                                // Name (更新为Layer1以区分)
        logicWorld,                                        // Mother volume
        false,                                             // No boolean operations
        0,                                                 // Copy number
        true                                               // Check overlaps
    );

    // 同步设置Filter1（保持向后兼容）
    fPhysFilter1 = fPhysFilter;
    fFilter1Logical = fFilterLogical;

    // 铜滤过层2 - 额外的滤过层，默认位于阳极靶和探测器之间的负Z轴
    // 材料默认为Cu，其余参数与滤过层1相同
    G4double filter2HalfLengthX = fFilter2X; // 使用成员变量
    G4double filter2HalfLengthY = fFilter2Y; // 使用成员变量
    G4double filter2HalfLengthZ = fFilter2Z; // 使用成员变量
    G4double filter2PosX = 0 * cm;
    G4double filter2PosY = 0 * cm;
    G4double filter2PosZ = fFilter2PosZ; // 使用成员变量设置位置

    // 根据设定获取滤过层2材料
    G4Material *filter2Material;

    // 智能材料识别：如果材料名不以G4_开头且不是特殊情况，自动添加G4_前缀
    G4String material2Name = fFilter2MaterialName;
    if (material2Name != "Galactic" && material2Name.find("G4_") != 0)
    {
        material2Name = "G4_" + material2Name;
    }

    // 尝试查找材料
    filter2Material = nist->FindOrBuildMaterial(material2Name);
    if (!filter2Material)
    {
        G4cout << "Warning: Material " << material2Name << " not found, using G4_Cu instead." << G4endl;
        filter2Material = nist->FindOrBuildMaterial("G4_Cu");
        fFilter2MaterialName = "G4_Cu";
    }
    else
    {
        G4cout << "Filter2 material successfully set to: " << material2Name << G4endl;
        fFilter2MaterialName = material2Name; // 更新为完整的材料名
    }

    G4cout << "Construct: Filter2 sizes (half-lengths) X=" << filter2HalfLengthX / cm << " cm, Y=" << filter2HalfLengthY / cm << " cm, Z=" << filter2HalfLengthZ / mm << " mm" << G4endl;
    G4Box *solidFilter2 = new G4Box("FilterLayer2", filter2HalfLengthX, filter2HalfLengthY, filter2HalfLengthZ);
    fFilter2Logical = new G4LogicalVolume(solidFilter2, filter2Material, "FilterLayer2");

    // 根据材料设置可视化属性
    G4VisAttributes *filter2VisAtt;
    if (fFilter2MaterialName.find("Cu") != G4String::npos)
    {
        filter2VisAtt = new G4VisAttributes(G4Colour(0.72, 0.45, 0.2)); // Copper color
    }
    else
    {
        filter2VisAtt = new G4VisAttributes(G4Colour(0.8, 0.2, 0.2, 0.3)); // Red transparent for other materials
    }
    filter2VisAtt->SetForceSolid(true);
    fFilter2Logical->SetVisAttributes(filter2VisAtt);

    // 滤过层2也与阳极靶保持相对平行
    G4RotationMatrix *filter2Rotation = new G4RotationMatrix();

    fPhysFilter2 = new G4PVPlacement(
        filter2Rotation,                                      // 与阳极靶同步旋转
        G4ThreeVector(filter2PosX, filter2PosY, filter2PosZ), // Position at configurable Z
        fFilter2Logical,                                      // Logical volume
        "PhysFilterLayer2",                                   // Name
        logicWorld,                                           // Mother volume
        false,                                                // No boolean operations
        0,                                                    // Copy number
        true                                                  // Check overlaps
    );

    // Store the logical volume for the sensitive detector
    cellLogical = fDetectorLogical;

    G4cout << "=== X射线光谱仿真系统构建完成 ===" << G4endl;
    G4cout << "阳极靶: 材料=" << fAnodeMaterialName << ", 旋转角度=" << fAnodeRotationAngle / deg << "度" << G4endl;
    G4cout << "探测器: 材料=" << fDetectorMaterialName << ", 尺寸=" << 2 * fDetectorX / cm << "×" << 2 * fDetectorY / cm << "×" << 2 * fDetectorZ / mm << "cm×cm×mm" << G4endl;
    G4cout << "滤过层1: 材料=" << fFilterMaterialName << ", 位置=(0,0,-3cm)" << G4endl;
    G4cout << "滤过层2: 材料=" << fFilter2MaterialName << ", 位置=(0,0,-2cm)" << G4endl;

    return fPhysWorld;
}

void DetectorConstruction::SetFilter1Position(G4double posZ)
{
    fFilter1PosZ = posZ;
    G4cout << "Filter1 position Z set to: " << fFilter1PosZ / mm << " mm" << G4endl;
    if (fPhysFilter)
    {
        fPhysFilter->SetTranslation(G4ThreeVector(0, 0, fFilter1PosZ));
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}

G4double DetectorConstruction::GetFilter1Position() const
{
    return fFilter1PosZ;
}

void DetectorConstruction::SetFilter2Position(G4double posZ)
{
    fFilter2PosZ = posZ;
    G4cout << "Filter2 position Z set to: " << fFilter2PosZ / mm << " mm" << G4endl;
    if (fPhysFilter2)
    {
        fPhysFilter2->SetTranslation(G4ThreeVector(0, 0, fFilter2PosZ));
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}

G4double DetectorConstruction::GetFilter2Position() const
{
    return fFilter2PosZ;
}

void DetectorConstruction::SetObjectRotationAngle(G4double angle)
{
    fCurrentAngle = angle;
    if (fPhysObject)
    {
        *fRotationMatrix = G4RotationMatrix();
        fRotationMatrix->rotateY(angle);
        fPhysObject->SetRotation(fRotationMatrix);
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}

G4double DetectorConstruction::GetCurrentAngle() const
{
    return fCurrentAngle;
}

// ===============================
// 阳极靶控制方法实现
// ===============================

void DetectorConstruction::SetAnodeRotationAngle(G4double angle)
{
    fAnodeRotationAngle = angle;

    // 如果几何已经构建，则更新旋转
    if (fPhysAnode && fPhysDetector && fPhysFilter && fPhysFilter2)
    {
        // 更新阳极靶旋转：绕 X 轴自转
        G4RotationMatrix *anodeRotation = new G4RotationMatrix();
        //anodeRotation->rotateZ(-90.0 * deg);         // 基础Z轴旋转
        fPhysAnode->SetRotation(anodeRotation);

        // 更新探测器旋转（保持与阳极靶相对平行）
        G4RotationMatrix *detectorRotation = new G4RotationMatrix();
        //detectorRotation->rotateX(-5.0 * deg);          // 基础X轴旋转
        fPhysDetector->SetRotation(detectorRotation);

        // 更新过滤层1旋转（保持与阳极靶相对平行）
        G4RotationMatrix *filterRotation = new G4RotationMatrix();
        //filterRotation->rotateX(-5.0 * deg);          // 基础X轴旋转
        fPhysFilter->SetRotation(filterRotation);

        // 更新过滤层2旋转（保持与阳极靶相对平行）
        G4RotationMatrix *filter2Rotation = new G4RotationMatrix();
        //filter2Rotation->rotateX(-5.0 * deg);          // 基础X轴旋转
        fPhysFilter2->SetRotation(filter2Rotation);

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();

        G4cout << "Rotation macro accepted (" << fAnodeRotationAngle / deg
               << " degrees), but anode, detector, and filters are forced perpendicular to Z axis." << G4endl;
    }
}

void DetectorConstruction::SetAnodeMaterial(G4String material)
{
    fAnodeMaterialName = material;

    // 如果几何已经构建，需要重建几何
    if (fAnodeLogical)
    {
        G4cout << "Anode material changed to: " << fAnodeMaterialName << G4endl;
        G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;

        // 获取新材料
        G4NistManager *nist = G4NistManager::Instance();
        G4Material *newMaterial;
        if (fAnodeMaterialName == "Galactic")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Galactic");
        }
        else if (fAnodeMaterialName == "W")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_W");
        }
        else if (fAnodeMaterialName == "Mo")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Mo");
        }
        else
        {
            newMaterial = nist->FindOrBuildMaterial("G4_W");
            G4cout << "Warning: Unknown material " << fAnodeMaterialName
                   << ", using tungsten (W) instead." << G4endl;
        }

        // 更新逻辑体的材料
        fAnodeLogical->SetMaterial(newMaterial);

        // 更新可视化属性
        G4VisAttributes *anode_vis;
        if (fAnodeMaterialName == "Galactic")
        {
            anode_vis = new G4VisAttributes(G4Colour(0.2, 0.2, 0.8, 0.3)); // 蓝色半透明（真空）
        }
        else if (fAnodeMaterialName == "Mo")
        {
            anode_vis = new G4VisAttributes(G4Colour(0.6, 0.8, 0.6, 0.8)); // 绿色半透明（钼）
        }
        else
        {
            anode_vis = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7, 0.8)); // 灰色半透明（钨）
        }
        anode_vis->SetForceSolid(true);
        fAnodeLogical->SetVisAttributes(anode_vis);

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();
    }
}

G4double DetectorConstruction::GetAnodeRotationAngle() const
{
    return fAnodeRotationAngle;
}

G4String DetectorConstruction::GetAnodeMaterial() const
{
    return fAnodeMaterialName;
}

// ===============================
// 过滤层控制方法实现
// ===============================

void DetectorConstruction::SetFilterMaterial(G4String material)
{
    fFilterMaterialName = material;
    fFilter1MaterialName = material; // 同步更新Filter1（保持向后兼容）

    // 如果几何已经构建，需要更新材料
    if (fFilterLogical)
    {
        G4cout << "Filter material changed to: " << fFilterMaterialName << G4endl;

        // 获取新材料
        G4NistManager *nist = G4NistManager::Instance();
        G4Material *newMaterial;
        if (fFilterMaterialName == "Al")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Al");
        }
        else if (fFilterMaterialName == "Galactic")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Galactic");
        }
        else
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Al");
            G4cout << "Warning: Unknown filter material " << fFilterMaterialName
                   << ", using aluminum (Al) instead." << G4endl;
        }

        // 更新逻辑体的材料
        fFilterLogical->SetMaterial(newMaterial);
        if (fFilter1Logical)
            fFilter1Logical->SetMaterial(newMaterial); // 同步更新

        // 更新可视化属性
        G4VisAttributes *filter_vis;
        if (fFilterMaterialName == "Al")
        {
            filter_vis = new G4VisAttributes(G4Colour(0.8, 0.8, 0.9)); // Light blue-grey for aluminum
        }
        else
        {
            filter_vis = new G4VisAttributes(G4Colour(0.2, 0.2, 0.8, 0.3)); // Blue transparent for vacuum
        }
        filter_vis->SetForceSolid(true);
        fFilterLogical->SetVisAttributes(filter_vis);

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();

        G4cout << "Filter material updated to: " << fFilterMaterialName << G4endl;
    }
}

G4String DetectorConstruction::GetFilterMaterial() const
{
    return fFilterMaterialName;
}

// ===============================
// 探测器控制方法实现
// ===============================

void DetectorConstruction::SetDetectorMaterial(G4String material)
{
    fDetectorMaterialName = material;

    // 如果几何已经构建，需要更新材料
    if (fDetectorLogical)
    {
        G4cout << "Detector material changed to: " << fDetectorMaterialName << G4endl;

        // 获取新材料
        G4NistManager *nist = G4NistManager::Instance();
        G4Material *newMaterial;
        if (fDetectorMaterialName == "Si")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Si");
        }
        else if (fDetectorMaterialName == "Galactic")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Galactic");
        }
        else if (fDetectorMaterialName == "CESIUM_IODIDE")
        {
            newMaterial = nist->FindOrBuildMaterial("G4_CESIUM_IODIDE");
        }
        else
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Galactic");
            G4cout << "Warning: Unknown detector material " << fDetectorMaterialName
                   << ", using Galactic instead." << G4endl;
        }

        // 更新逻辑体的材料
        fDetectorLogical->SetMaterial(newMaterial);

        // 更新可视化属性
        G4VisAttributes *detector_vis;
        if (fDetectorMaterialName == "Si")
        {
            detector_vis = new G4VisAttributes(G4Colour(0.2, 0.8, 0.2)); // 绿色（硅）
        }
        else if (fDetectorMaterialName == "CESIUM_IODIDE")
        {
            detector_vis = new G4VisAttributes(G4Colour(0.8, 0.6, 0.2)); // 橙色（碘化铯）
        }
        else
        {
            detector_vis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0)); // 亮绿色（真空）
        }
        detector_vis->SetForceSolid(true);
        fDetectorLogical->SetVisAttributes(detector_vis);

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();

        G4cout << "Detector material updated to: " << fDetectorMaterialName << G4endl;
    }
}

void DetectorConstruction::SetDetectorX(G4double sizeX)
{
    fDetectorX = sizeX;
    G4cout << "Detector X dimension set to: " << fDetectorX / cm << " cm (half-length)" << G4endl;
    G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;
}

void DetectorConstruction::SetDetectorY(G4double sizeY)
{
    fDetectorY = sizeY;
    G4cout << "Detector Y dimension set to: " << fDetectorY / cm << " cm (half-length)" << G4endl;
    G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;
}

void DetectorConstruction::SetDetectorZ(G4double sizeZ)
{
    fDetectorZ = sizeZ;
    G4cout << "Detector Z dimension set to: " << fDetectorZ / mm << " mm (half-length)" << G4endl;
    G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;
}

G4String DetectorConstruction::GetDetectorMaterial() const
{
    return fDetectorMaterialName;
}

G4double DetectorConstruction::GetDetectorX() const
{
    return fDetectorX;
}

G4double DetectorConstruction::GetDetectorY() const
{
    return fDetectorY;
}

G4double DetectorConstruction::GetDetectorZ() const
{
    return fDetectorZ;
}

void DetectorConstruction::ConstructSDandField()
{
    G4SDManager *sdManager = G4SDManager::GetSDMpointer();
    DetectorSD *fDetectorSD = new DetectorSD("detectorSD");
    sdManager->AddNewDetector(fDetectorSD);

    if (fDetectorLogical)
    {
        fDetectorLogical->SetSensitiveDetector(fDetectorSD);
        G4cout << "[iiCTs Debug][DetectorConstruction] Sensitive detector attached to "
               << fDetectorLogical->GetName() << G4endl;
    }
    else
    {
        G4cerr << "[iiCTs Debug][DetectorConstruction] ERROR: fDetectorLogical is null; "
               << "detector hits will not be recorded." << G4endl;
    }
}
// 滤过器尺寸设置方法的实现（保持向后兼容，等同于Filter1）
void DetectorConstruction::SetFilterX(G4double sizeX)
{
    fFilterX = sizeX;
    fFilter1X = sizeX; // 同步更新Filter1
    G4cout << "Filter X size set to: " << fFilterX / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilterY(G4double sizeY)
{
    fFilterY = sizeY;
    fFilter1Y = sizeY; // 同步更新Filter1
    G4cout << "Filter Y size set to: " << fFilterY / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilterZ(G4double sizeZ)
{
    fFilterZ = sizeZ;
    fFilter1Z = sizeZ; // 同步更新Filter1
    G4cout << "Filter Z size set to: " << fFilterZ / mm << " mm" << G4endl;
}

// 滤过器尺寸获取方法的实现（保持向后兼容）
G4double DetectorConstruction::GetFilterX() const
{
    return fFilterX;
}

G4double DetectorConstruction::GetFilterY() const
{
    return fFilterY;
}

G4double DetectorConstruction::GetFilterZ() const
{
    return fFilterZ;
}

// ===============================
// 滤过层1控制方法实现
// ===============================

void DetectorConstruction::SetFilter1Material(G4String material)
{
    fFilter1MaterialName = material;
    fFilterMaterialName = material; // 同步更新原Filter（保持向后兼容）

    // 如果几何已经构建，需要更新材料
    if (fFilter1Logical)
    {
        G4cout << "Filter1 material changed to: " << fFilter1MaterialName << G4endl;

        // 获取新材料
        G4NistManager *nist = G4NistManager::Instance();
        G4Material *newMaterial = nist->FindOrBuildMaterial(material);
        if (!newMaterial)
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Al");
            G4cout << "Warning: Unknown filter1 material " << fFilter1MaterialName
                   << ", using aluminum (Al) instead." << G4endl;
        }

        // 更新逻辑体的材料
        fFilter1Logical->SetMaterial(newMaterial);
        if (fFilterLogical)
            fFilterLogical->SetMaterial(newMaterial); // 同步更新

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();

        G4cout << "Filter1 material updated to: " << fFilter1MaterialName << G4endl;
    }
}

void DetectorConstruction::SetFilter1X(G4double sizeX)
{
    fFilter1X = sizeX;
    fFilterX = sizeX; // 同步更新原Filter（保持向后兼容）
    G4cout << "Filter1 X size set to: " << fFilter1X / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilter1Y(G4double sizeY)
{
    fFilter1Y = sizeY;
    fFilterY = sizeY; // 同步更新原Filter（保持向后兼容）
    G4cout << "Filter1 Y size set to: " << fFilter1Y / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilter1Z(G4double sizeZ)
{
    fFilter1Z = sizeZ;
    fFilterZ = sizeZ; // 同步更新原Filter（保持向后兼容）
    G4cout << "Filter1 Z size set to: " << fFilter1Z / mm << " mm" << G4endl;
    G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4String DetectorConstruction::GetFilter1Material() const
{
    return fFilter1MaterialName;
}

G4double DetectorConstruction::GetFilter1X() const
{
    return fFilter1X;
}

G4double DetectorConstruction::GetFilter1Y() const
{
    return fFilter1Y;
}

G4double DetectorConstruction::GetFilter1Z() const
{
    return fFilter1Z;
}

// ===============================
// 滤过层2控制方法实现
// ===============================

void DetectorConstruction::SetFilter2Material(G4String material)
{
    fFilter2MaterialName = material;

    // 如果几何已经构建，需要更新材料
    if (fFilter2Logical)
    {
        G4cout << "Filter2 material changed to: " << fFilter2MaterialName << G4endl;

        // 获取新材料
        G4NistManager *nist = G4NistManager::Instance();
        G4Material *newMaterial = nist->FindOrBuildMaterial(material);
        if (!newMaterial)
        {
            newMaterial = nist->FindOrBuildMaterial("G4_Cu");
            G4cout << "Warning: Unknown filter2 material " << fFilter2MaterialName
                   << ", using copper (Cu) instead." << G4endl;
        }

        // 更新逻辑体的材料
        fFilter2Logical->SetMaterial(newMaterial);

        // 通知运行管理器几何已被修改
        G4RunManager::GetRunManager()->GeometryHasBeenModified();

        G4cout << "Filter2 material updated to: " << fFilter2MaterialName << G4endl;
    }
}

void DetectorConstruction::SetFilter2X(G4double sizeX)
{
    fFilter2X = sizeX;
    G4cout << "Filter2 X size set to: " << fFilter2X / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilter2Y(G4double sizeY)
{
    fFilter2Y = sizeY;
    G4cout << "Filter2 Y size set to: " << fFilter2Y / cm << " cm" << G4endl;
}

void DetectorConstruction::SetFilter2Z(G4double sizeZ)
{
    fFilter2Z = sizeZ;
    G4cout << "Filter2 Z size set to: " << fFilter2Z / mm << " mm" << G4endl;
    G4cout << "Note: Geometry will be reconstructed on next run." << G4endl;
    G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

G4String DetectorConstruction::GetFilter2Material() const
{
    return fFilter2MaterialName;
}

G4double DetectorConstruction::GetFilter2X() const
{
    return fFilter2X;
}

G4double DetectorConstruction::GetFilter2Y() const
{
    return fFilter2Y;
}

G4double DetectorConstruction::GetFilter2Z() const
{
    return fFilter2Z;
}
