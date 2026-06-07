#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithADouble.hh"
#include <sstream>
#include "G4UIcommand.hh"
#include "G4SystemOfUnits.hh"
DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* detector)
    : fDetector(detector) {
    // 定义 /detector/setObjectAngle 命令
    fAngleCmd = new G4UIcmdWithADouble("/detector/setObjectAngle", this);
    fAngleCmd->SetGuidance("Set object rotation angle in radians");
    fAngleCmd->SetParameterName("angle", false);  // 参数名为 angle，不可省略

    fPolygonCountCmd = new G4UIcommand("/detector/PolygonCount", this);
    fPolygonCountCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* PolygonCount = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fPolygonCountCmd->SetParameter(PolygonCount);

    fObjectAngleCmd = new G4UIcommand("/detector/setObjectAngle1", this);
    fObjectAngleCmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* ObjectangleXParam = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* ObjectangleYParam = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* ObjectangleZParam = new G4UIparameter("angleZ", 'd', false);
    fObjectAngleCmd->SetParameter(ObjectangleXParam);
    fObjectAngleCmd->SetParameter(ObjectangleYParam);
    fObjectAngleCmd->SetParameter(ObjectangleZParam);

    fDetectorBedDirectionCmd = new G4UIcommand("/detector/BedDirection", this);
    fDetectorBedDirectionCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* BedDirection = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDetectorBedDirectionCmd->SetParameter(BedDirection);

    fDetectorModuleNumCmd = new G4UIcommand("/detector/setDetectorModuleNum", this);
    fDetectorModuleNumCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* ModuleNum = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDetectorModuleNumCmd->SetParameter(ModuleNum);

    fDetectorFilterCmd = new G4UIcommand("/detector/setFilter", this);
    fDetectorFilterCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* Filter = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDetectorFilterCmd->SetParameter(Filter);

    fDetectorRadiusCmd = new G4UIcommand("/detector/setDetectorRadius", this);
    fDetectorRadiusCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* Radius = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDetectorRadiusCmd->SetParameter(Radius);

    fFilterXCmd = new G4UIcommand("/detector/setFilterXSize", this);
    fFilterXCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* FilterX = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fFilterXCmd->SetParameter(FilterX);

    fFilterYCmd = new G4UIcommand("/detector/setFilterYSize", this);
    fFilterYCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* FilterY = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fFilterYCmd->SetParameter(FilterY);

    fFilterZCmd = new G4UIcommand("/detector/setFilterZSize", this);
    fFilterZCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* FilterZ = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fFilterZCmd->SetParameter(FilterZ);

    fFilterPosCmd = new G4UIcommand("/detector/setFilterPos", this);
    fFilterPosCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* pos = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fFilterPosCmd->SetParameter(pos);

    fFilterXBiasCmd = new G4UIcommand("/detector/setFilterXBias", this);
    fFilterXBiasCmd->SetGuidance("Set detector Bias with one parameters: length in cm");
    G4UIparameter* FilterXBias = new G4UIparameter("length", 'd', false); 
    fFilterXBiasCmd->SetParameter(FilterXBias);

    fFilterYBiasCmd = new G4UIcommand("/detector/setFilterYBias", this);
    fFilterYBiasCmd->SetGuidance("Set detector Bias with one parameters: width in cm");
    G4UIparameter* FilterYBias = new G4UIparameter("width", 'd', false);
    fFilterYBiasCmd->SetParameter(FilterYBias);

    fDDOCmd = new G4UIcommand("/detector/setDDO", this);
    fDDOCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* ddo = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDDOCmd->SetParameter(ddo);

    fObjectXBiasCmd = new G4UIcommand("/detector/setObjectXBias", this);
    fObjectXBiasCmd->SetGuidance("Set detector Bias with one parameters: length in cm");
    G4UIparameter* ObjectXBias = new G4UIparameter("length", 'd', false); 
    fObjectXBiasCmd->SetParameter(ObjectXBias);

    fObjectYBiasCmd = new G4UIcommand("/detector/setObjectYBias", this);
    fObjectYBiasCmd->SetGuidance("Set detector Bias with one parameters: width in cm");
    G4UIparameter* ObjectYBias = new G4UIparameter("width", 'd', false);
    fObjectYBiasCmd->SetParameter(ObjectYBias);

    fObjectZBiasCmd = new G4UIcommand("/detector/setObjectZBias", this);
    fObjectZBiasCmd->SetGuidance("Set detector Bias with one parameters: width in cm");
    G4UIparameter* ObjectZBias = new G4UIparameter("width", 'd', false);
    fObjectZBiasCmd->SetParameter(ObjectZBias);

    fDetectorAngle1Cmd = new G4UIcommand("/detector/setDetectorRotationAngle1", this);
    fDetectorAngle1Cmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* angleXParam1 = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* angleYParam1 = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* angleZParam1 = new G4UIparameter("angleZ", 'd', false);
    fDetectorAngle1Cmd->SetParameter(angleXParam1);
    fDetectorAngle1Cmd->SetParameter(angleYParam1);
    fDetectorAngle1Cmd->SetParameter(angleZParam1);

    fDetectorAngle2Cmd = new G4UIcommand("/detector/setDetectorRotationAngle2", this);
    fDetectorAngle2Cmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* angleXParam2 = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* angleYParam2 = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* angleZParam2 = new G4UIparameter("angleZ", 'd', false);
    fDetectorAngle2Cmd->SetParameter(angleXParam2);
    fDetectorAngle2Cmd->SetParameter(angleYParam2);
    fDetectorAngle2Cmd->SetParameter(angleZParam2);

    fDetectorAngle3Cmd = new G4UIcommand("/detector/setDetectorRotationAngle3", this);
    fDetectorAngle3Cmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* angleXParam3 = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* angleYParam3 = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* angleZParam3 = new G4UIparameter("angleZ", 'd', false);
    fDetectorAngle3Cmd->SetParameter(angleXParam3);
    fDetectorAngle3Cmd->SetParameter(angleYParam3);
    fDetectorAngle3Cmd->SetParameter(angleZParam3);

    fDetectorAngle4Cmd = new G4UIcommand("/detector/setDetectorRotationAngle4", this);
    fDetectorAngle4Cmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* angleXParam4 = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* angleYParam4 = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* angleZParam4 = new G4UIparameter("angleZ", 'd', false);
    fDetectorAngle4Cmd->SetParameter(angleXParam4);
    fDetectorAngle4Cmd->SetParameter(angleYParam4);
    fDetectorAngle4Cmd->SetParameter(angleZParam4);

    fDetectorAngle5Cmd = new G4UIcommand("/detector/setDetectorRotationAngle5", this);
    fDetectorAngle5Cmd->SetGuidance("Set Detector rotation angles in radians (angleX angleY angleZ)");
    G4UIparameter* angleXParam5 = new G4UIparameter("angleX", 'd', false);
    G4UIparameter* angleYParam5 = new G4UIparameter("angleY", 'd', false);
    G4UIparameter* angleZParam5 = new G4UIparameter("angleZ", 'd', false);
    fDetectorAngle5Cmd->SetParameter(angleXParam5);
    fDetectorAngle5Cmd->SetParameter(angleYParam5);
    fDetectorAngle5Cmd->SetParameter(angleZParam5);

    fMultipleCmd = new G4UIcommand("/detector/setmultiple", this);
    fMultipleCmd->SetGuidance("Set Detector rotation angles in radians (angleX)");
    G4UIparameter* Multiple = new G4UIparameter("angle", 'd', false);  // Double type, not optional
    fMultipleCmd->SetParameter(Multiple);

    fDetectorXCmd = new G4UIcommand("/detector/setDetectorXSize", this);
    fDetectorXCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* lengthParamX = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fDetectorXCmd->SetParameter(lengthParamX);

    fDetectorYCmd = new G4UIcommand("/detector/setDetectorYSize", this);
    fDetectorYCmd->SetGuidance("Set detector size with one parameters: width in cm");
    G4UIparameter* widthParamY = new G4UIparameter("width", 'd', false);
    fDetectorYCmd->SetParameter(widthParamY);
    
    fDetectorZCmd = new G4UIcommand("/detector/setDetectorZSize", this);
    fDetectorZCmd->SetGuidance("Set detector size with one parameters: height in cm");
    G4UIparameter* heightParamZ = new G4UIparameter("height", 'd', false);
    fDetectorZCmd->SetParameter(heightParamZ);

    fDetectorXBiasCmd = new G4UIcommand("/detector/setDetectorXBias", this);
    fDetectorXBiasCmd->SetGuidance("Set detector Bias with one parameters: length in cm");
    G4UIparameter* lengthParam1 = new G4UIparameter("length", 'd', false); 
    fDetectorXBiasCmd->SetParameter(lengthParam1);

    fDetectorYBiasCmd = new G4UIcommand("/detector/setDetectorYBias", this);
    fDetectorYBiasCmd->SetGuidance("Set detector Bias with one parameters: width in cm");
    G4UIparameter* widthParam1 = new G4UIparameter("width", 'd', false);
    fDetectorYBiasCmd->SetParameter(widthParam1);

    // fDetectorZBiasCmd = new G4UIcommand("/detector/setDetectorZBias", this);
    // fDetectorZBiasCmd->SetGuidance("Set detector Bias with one parameters: height in cm");
    // G4UIparameter* heightParam1 = new G4UIparameter("height", 'd', false);
    // fDetectorZBiasCmd->SetParameter(heightParam1);
    fDSDcmd = new G4UIcommand("/detector/setDSD", this);
    fDSDcmd->SetGuidance("Set DSD with one parameters: height in cm");
    G4UIparameter* DSD = new G4UIparameter("height", 'd', false);
    fDSDcmd->SetParameter(DSD);

    fDSOcmd = new G4UIcommand("/detector/setDSO", this);
    fDSOcmd->SetGuidance("Set DSD witDetectorh one parameters: height in cm");
    G4UIparameter* DSO = new G4UIparameter("height", 'd', false);
    fDSOcmd->SetParameter(DSO);

    fPixelXCountCmd = new G4UIcommand("/detector/setPixelXCount", this);
    fPixelXCountCmd->SetGuidance("Set Pixel size with one parameters: length in cm");
    G4UIparameter* XCount = new G4UIparameter("length", 'd', false);
    fPixelXCountCmd->SetParameter(XCount);

    fPixelYCountCmd = new G4UIcommand("/detector/setPixelYCount", this);
    fPixelYCountCmd->SetGuidance("Set Pixel size with one parameters: length in cm");
    G4UIparameter* YCount = new G4UIparameter("length", 'd', false);
    fPixelYCountCmd->SetParameter(YCount);

    fPixelXCmd = new G4UIcommand("/detector/setPixelXSize", this);
    fPixelXCmd->SetGuidance("Set Pixel size with one parameters: length in cm");
    G4UIparameter* lengthParam2 = new G4UIparameter("length", 'd', false);
    fPixelXCmd->SetParameter(lengthParam2);

    fPixelYCmd = new G4UIcommand("/detector/setPixelYSize", this);
    fPixelYCmd->SetGuidance("Set Pixel size with one parameters: width in cm");
    G4UIparameter* widthParam2= new G4UIparameter("width", 'd', false);
    fPixelYCmd->SetParameter(widthParam2);

    fPixelZCmd = new G4UIcommand("/detector/setPixelZSize", this);
    fPixelZCmd->SetGuidance("Set Pixel size with one parameters: height in cm");
    G4UIparameter* heightParam2 = new G4UIparameter("height", 'd', false);
    fPixelZCmd->SetParameter(heightParam2);

    fPitchCmd = new G4UIcommand("/detector/setPitch", this);
    fPitchCmd->SetGuidance("Set Pixel size with one parameters: height in cm");
    G4UIparameter* pitch = new G4UIparameter("height", 'd', false);
    fPitchCmd->SetParameter(pitch);

    fG4dcmCmd = new G4UIcmdWithAString("/detector/G4dcm", this);
    fG4dcmCmd->SetGuidance("Set folder path as parameter");
    fG4dcmCmd->SetParameterName("path", false);

    fSpectrumPosCmd = new G4UIcmdWithAString("/detector/SpectrumPos", this);
    fSpectrumPosCmd->SetGuidance("Set folder path as parameter");
    fSpectrumPosCmd->SetParameterName("path", false);

    fSpectrumEnergyCmd = new G4UIcmdWithAString("/detector/SpectrumEnergy", this);
    fSpectrumEnergyCmd->SetGuidance("Set folder path as parameter");
    fSpectrumEnergyCmd->SetParameterName("path", false);

    fOutputCmd = new G4UIcmdWithAString("/detector/Output", this);
    fOutputCmd->SetGuidance("Set folder path as parameter");
    fOutputCmd->SetParameterName("path", false);

    fMaterialCmd = new G4UIcmdWithAString("/detector/material", this);
    fMaterialCmd->SetGuidance("Set detector material (e.g., CsI_Tl or custom: name density(g/cm3) ncomponents elementName1 z1 a1(g/mole) fraction1 ...)");
    fMaterialCmd->SetParameterName("materialName", false);

    fMaterial1Cmd = new G4UIcmdWithAString("/detector/setFilterMaterial", this);
    fMaterial1Cmd->SetGuidance("Set detector material (e.g., CsI_Tl or custom: name density(g/cm3) ncomponents elementName1 z1 a1(g/mole) fraction1 ...)");
    fMaterial1Cmd->SetParameterName("materialName", false);
    
    fMaterial2Cmd = new G4UIcmdWithAString("/detector/setTubeFilterMaterial", this);
    fMaterial2Cmd->SetGuidance("Set detector material (e.g., CsI_Tl or custom: name density(g/cm3) ncomponents elementName1 z1 a1(g/mole) fraction1 ...)");
    fMaterial2Cmd->SetParameterName("materialName", false);   

    fTubeFilterXCmd = new G4UIcommand("/detector/setTubeFilterXSize", this);
    fTubeFilterXCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* TubeFilterX = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fTubeFilterXCmd->SetParameter(TubeFilterX);

    fTubeFilterYCmd = new G4UIcommand("/detector/setTubeFilterYSize", this);
    fTubeFilterYCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* TubeFilterY = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fTubeFilterYCmd->SetParameter(TubeFilterY);

    fTubeFilterZCmd = new G4UIcommand("/detector/setTubeFilterZSize", this);
    fTubeFilterZCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* TubeFilterZ = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fTubeFilterZCmd->SetParameter(TubeFilterZ);

    fTubeFilterPosCmd = new G4UIcommand("/detector/setTubeFilterPos", this);
    fTubeFilterPosCmd->SetGuidance("Set detector size with one parameters: length in cm");
    G4UIparameter* pos1 = new G4UIparameter("length", 'd', false);  // Double type, not optional
    fTubeFilterPosCmd->SetParameter(pos1);

    fTubeFilterXBiasCmd = new G4UIcommand("/detector/setTubeFilterXBias", this);
    fTubeFilterXBiasCmd->SetGuidance("Set detector Bias with one parameters: length in cm");
    G4UIparameter* TubeFilterXBias = new G4UIparameter("length", 'd', false); 
    fTubeFilterXBiasCmd->SetParameter(TubeFilterXBias);

    fTubeFilterYBiasCmd = new G4UIcommand("/detector/setTubeFilterYBias", this);
    fTubeFilterYBiasCmd->SetGuidance("Set detector Bias with one parameters: width in cm");
    G4UIparameter* TubeFilterYBias = new G4UIparameter("width", 'd', false);
    fTubeFilterYBiasCmd->SetParameter(TubeFilterYBias);
}

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete fPolygonCountCmd;
    delete fAngleCmd;
    delete fObjectAngleCmd;
    delete fDetectorModuleNumCmd;
    delete fDetectorBedDirectionCmd;
    delete fDetectorFilterCmd;
    delete fDetectorRadiusCmd;
    delete fMultipleCmd;
    delete fDDOCmd;
    delete fDetectorAngle1Cmd;
    delete fDetectorAngle2Cmd;
    delete fDetectorAngle3Cmd;
    delete fDetectorAngle4Cmd;
    delete fDetectorAngle5Cmd;
    delete fDetectorXCmd;
    delete fDetectorYCmd;
    delete fDetectorZCmd;
    delete fFilterXCmd;
    delete fFilterYCmd;
    delete fFilterZCmd;
    delete fFilterXBiasCmd;
    delete fFilterYBiasCmd;
    delete fFilterPosCmd;
    delete fTubeFilterXCmd;
    delete fTubeFilterYCmd;
    delete fTubeFilterZCmd;
    delete fTubeFilterPosCmd;
    delete fTubeFilterXBiasCmd;
    delete fTubeFilterYBiasCmd;
    delete fDetectorXBiasCmd;
    delete fDetectorYBiasCmd;
    delete fDetectorZBiasCmd;
    delete fObjectXBiasCmd;
    delete fObjectYBiasCmd;
    delete fObjectZBiasCmd;
    delete fDSDcmd;
    delete fDSOcmd;
    delete fPixelXCountCmd;
    delete fPixelYCountCmd;
    delete fPixelXCmd;
    delete fPixelYCmd;
    delete fPixelZCmd;
    delete fG4dcmCmd;
    delete fSpectrumPosCmd;
    delete fSpectrumEnergyCmd;
    delete fOutputCmd;
    delete fMaterialCmd;
    delete fMaterial1Cmd;
    delete fMaterial2Cmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    if (cmd == fAngleCmd) {
        G4double angle_deg = G4UIcmdWithADouble::GetNewDoubleValue(newValue);
        G4double angle_rad = angle_deg * CLHEP::deg;
        fDetector->SetObjectRotationAngle(angle_rad);
    }else if (cmd == fObjectAngleCmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetObjectRotationAngle1(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fPolygonCountCmd) {
        std::istringstream iss(newValue);
        G4double PolygonCount;
        iss >> PolygonCount;
        fDetector->SetPolygonCount(PolygonCount);
    }else if (cmd == fObjectXBiasCmd) {
        std::istringstream iss(newValue);
        G4double xBias;
        iss >> xBias;
        fDetector->SetObjectXBias(xBias * mm);
    }else if (cmd == fObjectYBiasCmd) {
        std::istringstream iss(newValue);
        G4double yBias;
        iss >> yBias;
        fDetector->SetObjectYBias(yBias * mm);
    }else if (cmd == fObjectZBiasCmd) {
        std::istringstream iss(newValue);
        G4double zBias;
        iss >> zBias;
        fDetector->SetObjectZBias(zBias * mm);
    }else if (cmd == fDetectorModuleNumCmd) {
        std::istringstream iss(newValue);
        G4double Num;
        iss >> Num;
        fDetector->SetDetectorModuleNumber(Num);
    }else if (cmd == fDetectorBedDirectionCmd) {
        std::istringstream iss(newValue);
        G4double Bed;
        iss >> Bed;
        fDetector->SetDetectorBedDirection(Bed);
    }else if (cmd == fDetectorFilterCmd) {
        std::istringstream iss(newValue);
        G4double filter;
        iss >> filter;
        fDetector->SetDetectorFilter(filter);
    }else if (cmd == fDetectorRadiusCmd) {
        std::istringstream iss(newValue);
        G4double radius;
        iss >> radius;
        fDetector->SetDetectorRadius(radius);
    }else if (cmd == fDDOCmd) {
        std::istringstream iss(newValue);
        G4double ddo;
        iss >> ddo;
        fDetector->SetDDO(ddo * mm);
    }else if (cmd == fDetectorAngle1Cmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetDetectorRotationAngle1(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fDetectorAngle2Cmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetDetectorRotationAngle2(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fDetectorAngle3Cmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetDetectorRotationAngle3(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fDetectorAngle4Cmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetDetectorRotationAngle4(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fDetectorAngle5Cmd) {
        std::istringstream iss(newValue);
        G4double angleX, angleY, angleZ;
        iss >> angleX >> angleY >> angleZ;
        fDetector->SetDetectorRotationAngle5(angleX* deg, angleY* deg, angleZ* deg);
    }else if (cmd == fMultipleCmd) {
        std::istringstream iss(newValue);
        G4double multiple;
        iss >> multiple;
        fDetector->SetMultiple(multiple);
    }else if (cmd == fDetectorXCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length ;
        fDetector->SetDetectorXSize(length * mm);
    } else if (cmd == fDetectorYCmd) {
        std::istringstream iss(newValue);
        G4double width;
        iss >> width;
        fDetector->SetDetectorYSize(width * mm);
    } else if (cmd == fDetectorZCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetDetectorZSize(height * mm);
    } else if (cmd == fFilterXCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length ;
        fDetector->SetFilterXSize(length * mm);
    } else if (cmd == fFilterYCmd) {
        std::istringstream iss(newValue);
        G4double width;
        iss >> width;
        fDetector->SetFilterYSize(width * mm);
    } else if (cmd == fFilterZCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetFilterZSize(height * mm);
    }else if (cmd == fFilterXBiasCmd){
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetFilterXBias(height * mm);
    }else if (cmd == fFilterYBiasCmd){
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetFilterYBias(height * mm);
    }else if (cmd == fFilterPosCmd){
        std::istringstream iss(newValue);
        G4double pos;
        iss >> pos;
        fDetector->SetFilterPos(pos * mm);
    }else if (cmd == fTubeFilterXCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length ;
        fDetector->SetTubeFilterXSize(length * mm);
    } else if (cmd == fTubeFilterYCmd) {
        std::istringstream iss(newValue);
        G4double width;
        iss >> width;
        fDetector->SetTubeFilterYSize(width * mm);
    } else if (cmd == fTubeFilterZCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetTubeFilterZSize(height * mm);
    }else if (cmd == fTubeFilterXBiasCmd){
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetTubeFilterXBias(height * mm);
    }else if (cmd == fTubeFilterYBiasCmd){
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetTubeFilterYBias(height * mm);
    }else if (cmd == fTubeFilterPosCmd){
        std::istringstream iss(newValue);
        G4double pos;
        iss >> pos;
        fDetector->SetTubeFilterPos(pos * mm);
    }else if (cmd == fPixelXCountCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length;
        fDetector->SetPixelXCount(length);
    }else if (cmd == fPixelYCountCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length;
        fDetector->SetPixelYCount(length);
    }else if (cmd == fPixelXCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length;
        fDetector->SetPixelXSize(length * mm);
    } else if (cmd == fPixelYCmd) {
        std::istringstream iss(newValue);
        G4double width;
        iss >> width;
        fDetector->SetPixelYSize(width * mm);
    } else if (cmd == fPixelZCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetPixelZSize(height * mm);
    }else if (cmd == fG4dcmCmd) {
        fDetector->SetG4dcm(newValue);
    }else if (cmd == fSpectrumPosCmd) {
        fDetector->SetSpectrumPos(newValue);
    }else if (cmd == fSpectrumEnergyCmd) {
        fDetector->SetSpectrumEnergy(newValue);
    }else if (cmd == fOutputCmd) {
        fDetector->SetOutputPath(newValue);
    }else if (cmd == fMaterialCmd) {
        G4cout << "Received material command with: " << newValue << G4endl;
        fDetector->SetDetectorMaterial(newValue);
    }else if (cmd == fMaterial1Cmd) {
        G4cout << "Received filter material command with: " << newValue << G4endl;
        fDetector->SetFilterMaterial(newValue);
    }else if (cmd == fMaterial2Cmd) {
        G4cout << "Received filter material command with: " << newValue << G4endl;
        fDetector->SetTubeFilterMaterial(newValue);
    }else if (cmd == fDetectorXBiasCmd) {
        std::istringstream iss(newValue);
        G4double length;
        iss >> length;
        fDetector->SetDetectorXBias(length * mm);
    }else if (cmd == fDetectorYBiasCmd) {
        std::istringstream iss(newValue);
        G4double width;
        iss >> width;
        fDetector->SetDetectorYBias(width * mm);
    }else if (cmd == fDetectorZBiasCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetDetectorZBias(height * mm);
    }else if (cmd == fDSDcmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetDSD(height * mm);
    }else if (cmd == fDSOcmd){
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetDSO(height * mm);
    }else if (cmd == fPitchCmd) {
        std::istringstream iss(newValue);
        G4double height;
        iss >> height;
        fDetector->SetPitch(height * mm);
    }
}