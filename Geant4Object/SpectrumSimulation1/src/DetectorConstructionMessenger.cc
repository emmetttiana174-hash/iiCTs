#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "FilePathManager.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4SystemOfUnits.hh"
#include <cmath>

using CLHEP::deg;
using CLHEP::mm;

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction *detector)
    : fDetector(detector)
{
    // 定义 /detector/setObjectAngle 命令（原有）
    fAngleCmd = new G4UIcmdWithADouble("/detector/setObjectAngle", this);
    fAngleCmd->SetGuidance("Set object rotation angle in degrees");
    fAngleCmd->SetParameterName("angle", false); // 参数名为 angle，不可省略

    // 定义 /Anode/setAnodeAngle 命令（修改后）
    fAnodeAngleCmd = new G4UIcmdWithADouble("/Anode/setAnodeAngle", this);
    fAnodeAngleCmd->SetGuidance("Set anode target rotation angle in degrees");
    fAnodeAngleCmd->SetParameterName("angle", false);
    fAnodeAngleCmd->SetRange("angle>=-180 && angle<=180");

    // 定义 /Anode/setAnodeMaterial 命令（修改后）
    fAnodeMaterialCmd = new G4UIcmdWithAString("/Anode/setAnodeMaterial", this);
    fAnodeMaterialCmd->SetGuidance("Set anode target material");
    fAnodeMaterialCmd->SetGuidance("Available materials: Galactic, W, Mo");
    fAnodeMaterialCmd->SetParameterName("material", false);
    fAnodeMaterialCmd->SetCandidates("Galactic W Mo");

    // 定义 /Filter/setFilterMaterial 命令（保持向后兼容，等同于Filter1）
    fFilterMaterialCmd = new G4UIcmdWithAString("/Filter/setFilterMaterial", this);
    fFilterMaterialCmd->SetGuidance("Set filter layer material (equivalent to Filter1, for backward compatibility)");
    fFilterMaterialCmd->SetGuidance("Material name will be automatically prefixed with 'G4_' if needed");
    fFilterMaterialCmd->SetGuidance("Examples: Cu -> G4_Cu, Al -> G4_Al, Fe -> G4_Fe");
    fFilterMaterialCmd->SetParameterName("material", false);

    // 定义 /Filter/setFilterX 命令（保持向后兼容，等同于Filter1）
    fFilterXCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilterX", this);
    fFilterXCmd->SetGuidance("Set filter X dimension (half-length, equivalent to Filter1, for backward compatibility)");
    fFilterXCmd->SetParameterName("sizeX", false);
    fFilterXCmd->SetUnitCategory("Length");
    fFilterXCmd->SetDefaultUnit("cm");
    fFilterXCmd->SetRange("sizeX>0");

    // 定义 /Filter/setFilterY 命令（保持向后兼容，等同于Filter1）
    fFilterYCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilterY", this);
    fFilterYCmd->SetGuidance("Set filter Y dimension (half-length, equivalent to Filter1, for backward compatibility)");
    fFilterYCmd->SetParameterName("sizeY", false);
    fFilterYCmd->SetUnitCategory("Length");
    fFilterYCmd->SetDefaultUnit("cm");
    fFilterYCmd->SetRange("sizeY>0");

    // 定义 /Filter/setFilterZ 命令（保持向后兼容，等同于Filter1）
    fFilterZCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilterZ", this);
    fFilterZCmd->SetGuidance("Set filter Z dimension (half-length/thickness, equivalent to Filter1, for backward compatibility)");
    fFilterZCmd->SetParameterName("sizeZ", false);
    fFilterZCmd->SetUnitCategory("Length");
    fFilterZCmd->SetDefaultUnit("mm");
    fFilterZCmd->SetRange("sizeZ>0");

    // 定义滤过层1控制命令
    fFilter1MaterialCmd = new G4UIcmdWithAString("/Filter/setFilter1Material", this);
    fFilter1MaterialCmd->SetGuidance("Set filter layer 1 material");
    fFilter1MaterialCmd->SetGuidance("Material name will be automatically prefixed with 'G4_' if needed");
    fFilter1MaterialCmd->SetGuidance("Examples: Cu -> G4_Cu, Al -> G4_Al, Fe -> G4_Fe");
    fFilter1MaterialCmd->SetParameterName("material", false);

    fFilter1XCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter1X", this);
    fFilter1XCmd->SetGuidance("Set filter layer 1 X dimension (half-length)");
    fFilter1XCmd->SetParameterName("sizeX", false);
    fFilter1XCmd->SetUnitCategory("Length");
    fFilter1XCmd->SetDefaultUnit("cm");
    fFilter1XCmd->SetRange("sizeX>0");

    fFilter1YCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter1Y", this);
    fFilter1YCmd->SetGuidance("Set filter layer 1 Y dimension (half-length)");
    fFilter1YCmd->SetParameterName("sizeY", false);
    fFilter1YCmd->SetUnitCategory("Length");
    fFilter1YCmd->SetDefaultUnit("cm");
    fFilter1YCmd->SetRange("sizeY>0");

    fFilter1ZCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter1Z", this);
    fFilter1ZCmd->SetGuidance("Set filter layer 1 Z dimension (half-length/thickness)");
    fFilter1ZCmd->SetParameterName("sizeZ", false);
    fFilter1ZCmd->SetUnitCategory("Length");
    fFilter1ZCmd->SetDefaultUnit("mm");
    fFilter1ZCmd->SetRange("sizeZ>0");

    // 定义滤过层2控制命令
    fFilter2MaterialCmd = new G4UIcmdWithAString("/Filter/setFilter2Material", this);
    fFilter2MaterialCmd->SetGuidance("Set filter layer 2 material");
    fFilter2MaterialCmd->SetGuidance("Material name will be automatically prefixed with 'G4_' if needed");
    fFilter2MaterialCmd->SetGuidance("Examples: Cu -> G4_Cu, Al -> G4_Al, Fe -> G4_Fe");
    fFilter2MaterialCmd->SetParameterName("material", false);

    fFilter2XCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter2X", this);
    fFilter2XCmd->SetGuidance("Set filter layer 2 X dimension (half-length)");
    fFilter2XCmd->SetParameterName("sizeX", false);
    fFilter2XCmd->SetUnitCategory("Length");
    fFilter2XCmd->SetDefaultUnit("cm");
    fFilter2XCmd->SetRange("sizeX>0");

    fFilter2YCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter2Y", this);
    fFilter2YCmd->SetGuidance("Set filter layer 2 Y dimension (half-length)");
    fFilter2YCmd->SetParameterName("sizeY", false);
    fFilter2YCmd->SetUnitCategory("Length");
    fFilter2YCmd->SetDefaultUnit("cm");
    fFilter2YCmd->SetRange("sizeY>0");

    fFilter2ZCmd = new G4UIcmdWithADoubleAndUnit("/Filter/setFilter2Z", this);
    fFilter2ZCmd->SetGuidance("Set filter layer 2 Z dimension (half-length/thickness)");
    fFilter2ZCmd->SetParameterName("sizeZ", false);
    fFilter2ZCmd->SetUnitCategory("Length");
    fFilter2ZCmd->SetDefaultUnit("mm");
    fFilter2ZCmd->SetRange("sizeZ>0");

    // 定义探测器控制命令
    fDetectorMaterialCmd = new G4UIcmdWithAString("/detector/setDetectorMaterial", this);
    fDetectorMaterialCmd->SetGuidance("Set detector material");
    fDetectorMaterialCmd->SetGuidance("Available materials: Si, Galactic, CESIUM_IODIDE");
    fDetectorMaterialCmd->SetParameterName("material", false);
    fDetectorMaterialCmd->SetCandidates("Si Galactic CESIUM_IODIDE");

    fDetectorXCmd = new G4UIcmdWithADoubleAndUnit("/detector/setDetectorX", this);
    fDetectorXCmd->SetGuidance("Set detector X dimension (half-length)");
    fDetectorXCmd->SetParameterName("sizeX", false);
    fDetectorXCmd->SetUnitCategory("Length");
    fDetectorXCmd->SetDefaultUnit("cm");
    fDetectorXCmd->SetRange("sizeX>0");

    fDetectorYCmd = new G4UIcmdWithADoubleAndUnit("/detector/setDetectorY", this);
    fDetectorYCmd->SetGuidance("Set detector Y dimension (half-length)");
    fDetectorYCmd->SetParameterName("sizeY", false);
    fDetectorYCmd->SetUnitCategory("Length");
    fDetectorYCmd->SetDefaultUnit("cm");
    fDetectorYCmd->SetRange("sizeY>0");

    fDetectorZCmd = new G4UIcmdWithADoubleAndUnit("/detector/setDetectorZ", this);
    fDetectorZCmd->SetGuidance("Set detector Z dimension (half-length)");
    fDetectorZCmd->SetParameterName("sizeZ", false);
    fDetectorZCmd->SetUnitCategory("Length");
    fDetectorZCmd->SetDefaultUnit("mm");
    fDetectorZCmd->SetRange("sizeZ>0");

    // 定义输入文件路径命令
    fInputFilePathCmd = new G4UIcmdWithAString("/Input/setInputFilePath", this);
    fInputFilePathCmd->SetGuidance("Set input file path for particle source");
    fInputFilePathCmd->SetParameterName("inputPath", false);

    // 定义输出文件路径命令
    fOutputFilePathCmd = new G4UIcmdWithAString("/Output/setOutputFilePath", this);
    fOutputFilePathCmd->SetGuidance("Set output file path for energy spectrum");
    fOutputFilePathCmd->SetParameterName("outputPath", false);

    // 新增滤过层位置控制命令
    fFilter1PosCmd = new G4UIcmdWithADoubleAndUnit("/Filter/Filter1Position", this);
    fFilter1PosCmd->SetGuidance("Set filter1 distance on the negative Z axis.\nExample: /Filter/Filter1Position 3 mm sets position to (0,0,-3mm)");
    fFilter1PosCmd->SetParameterName("posZ", false);
    fFilter1PosCmd->SetUnitCategory("Length");
    fFilter1PosCmd->SetDefaultUnit("mm");

    fFilter2PosCmd = new G4UIcmdWithADoubleAndUnit("/Filter/Filter2Position", this);
    fFilter2PosCmd->SetGuidance("Set filter2 distance on the negative Z axis.\nExample: /Filter/Filter2Position 1 mm sets position to (0,0,-1mm)");
    fFilter2PosCmd->SetParameterName("posZ", false);
    fFilter2PosCmd->SetUnitCategory("Length");
    fFilter2PosCmd->SetDefaultUnit("mm");
}
DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete fAngleCmd;
    delete fAnodeAngleCmd;
    delete fAnodeMaterialCmd;
    delete fFilterMaterialCmd;
    delete fFilterXCmd;
    delete fFilterYCmd;
    delete fFilterZCmd;
    delete fFilter1MaterialCmd;
    delete fFilter1XCmd;
    delete fFilter1YCmd;
    delete fFilter1ZCmd;
    delete fFilter1PosCmd;
    delete fFilter2MaterialCmd;
    delete fFilter2XCmd;
    delete fFilter2YCmd;
    delete fFilter2ZCmd;
    delete fFilter2PosCmd;
    delete fDetectorMaterialCmd;
    delete fDetectorXCmd;
    delete fDetectorYCmd;
    delete fDetectorZCmd;
    delete fInputFilePathCmd;
    delete fOutputFilePathCmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
    if (cmd == fAngleCmd)
    {
        // 原有的待测物体旋转命令
        G4double angle_deg = G4UIcmdWithADouble::GetNewDoubleValue(newValue);
        G4double angle_rad = angle_deg * CLHEP::deg;
        fDetector->SetObjectRotationAngle(angle_rad);
    }
    else if (cmd == fAnodeAngleCmd)
    {
        // 新增的阳极靶旋转命令
        G4double angle_deg = G4UIcmdWithADouble::GetNewDoubleValue(newValue);
        G4double angle_rad = angle_deg * CLHEP::deg;
        fDetector->SetAnodeRotationAngle(angle_rad);
    }
    else if (cmd == fAnodeMaterialCmd)
    {
        // 新增的阳极靶材料命令
        G4String material = newValue;
        fDetector->SetAnodeMaterial(material);
    }
    else if (cmd == fFilterMaterialCmd)
    {
        // 过滤层材料命令（保持向后兼容，等同于Filter1）
        G4String material = newValue;

        // 如果材料名称不以"G4_"开头，自动添加前缀
        if (material.find("G4_") != 0 && material != "Galactic")
        {
            material = "G4_" + material;
        }

        fDetector->SetFilterMaterial(material);
        G4cout << "Filter material set to: " << material << G4endl;
    }
    else if (cmd == fFilterXCmd)
    {
        // 过滤层X尺寸命令（保持向后兼容，等同于Filter1）
        G4double sizeX = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilterX(sizeX);
    }
    else if (cmd == fFilter1PosCmd)
    {
        G4double posZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        G4double negativePosZ = -std::abs(posZ);
        G4cout << "Messenger: received /Filter/Filter1Position " << posZ / mm
               << " mm, applying Z=" << negativePosZ / mm << " mm" << G4endl;
        fDetector->SetFilter1Position(negativePosZ);
    }
    else if (cmd == fFilterYCmd)
    {
        // 过滤层Y尺寸命令（保持向后兼容，等同于Filter1）
        G4double sizeY = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilterY(sizeY);
    }
    else if (cmd == fFilter2PosCmd)
    {
        G4double posZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        G4double negativePosZ = -std::abs(posZ);
        G4cout << "Messenger: received /Filter/Filter2Position " << posZ / mm
               << " mm, applying Z=" << negativePosZ / mm << " mm" << G4endl;
        fDetector->SetFilter2Position(negativePosZ);
    }
    else if (cmd == fFilterZCmd)
    {
        // 过滤层Z尺寸命令（保持向后兼容，等同于Filter1）
        G4double sizeZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilterZ(sizeZ);
    }
    // 滤过层1控制命令
    else if (cmd == fFilter1MaterialCmd)
    {
        G4String material = newValue;
        if (material.find("G4_") != 0 && material != "Galactic")
        {
            material = "G4_" + material;
        }
        fDetector->SetFilter1Material(material);
        G4cout << "Filter1 material set to: " << material << G4endl;
    }
    else if (cmd == fFilter1XCmd)
    {
        G4double sizeX = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilter1X(sizeX);
    }
    else if (cmd == fFilter1YCmd)
    {
        G4double sizeY = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilter1Y(sizeY);
    }
    else if (cmd == fFilter1ZCmd)
    {
        G4double sizeZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        G4cout << "Messenger: received /Filter/setFilter1Z " << sizeZ / mm << " mm" << G4endl;
        fDetector->SetFilter1Z(sizeZ);
    }
    // 滤过层2控制命令
    else if (cmd == fFilter2MaterialCmd)
    {
        G4String material = newValue;
        if (material.find("G4_") != 0 && material != "Galactic")
        {
            material = "G4_" + material;
        }
        fDetector->SetFilter2Material(material);
        G4cout << "Filter2 material set to: " << material << G4endl;
    }
    else if (cmd == fFilter2XCmd)
    {
        G4double sizeX = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilter2X(sizeX);
    }
    else if (cmd == fFilter2YCmd)
    {
        G4double sizeY = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetFilter2Y(sizeY);
    }
    else if (cmd == fFilter2ZCmd)
    {
        G4double sizeZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        G4cout << "Messenger: received /Filter/setFilter2Z " << sizeZ / mm << " mm" << G4endl;
        fDetector->SetFilter2Z(sizeZ);
    }
    else if (cmd == fDetectorMaterialCmd)
    {
        // 新增的探测器材料命令
        G4String material = newValue;
        fDetector->SetDetectorMaterial(material);
    }
    else if (cmd == fDetectorXCmd)
    {
        // 探测器X尺寸命令
        G4double sizeX = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetDetectorX(sizeX);
    }
    else if (cmd == fDetectorYCmd)
    {
        // 探测器Y尺寸命令
        G4double sizeY = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetDetectorY(sizeY);
    }
    else if (cmd == fDetectorZCmd)
    {
        // 探测器Z尺寸命令
        G4double sizeZ = G4UIcmdWithADoubleAndUnit::GetNewDoubleValue(newValue);
        fDetector->SetDetectorZ(sizeZ);
    }
    else if (cmd == fInputFilePathCmd)
    {
        // 输入文件路径命令
        FilePathManager::GetInstance()->SetInputFilePath(newValue);
        G4cout << "Input file path set to: " << newValue << G4endl;
    }
    else if (cmd == fOutputFilePathCmd)
    {
        // 输出文件路径命令
        FilePathManager::GetInstance()->SetOutputFilePath(newValue);
        G4cout << "Output file path set to: " << newValue << G4endl;
    }
}
