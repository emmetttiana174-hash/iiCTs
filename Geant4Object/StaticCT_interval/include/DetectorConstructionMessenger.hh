#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "globals.hh"
#include "G4UIcmdWithAString.hh"
class DetectorConstruction;
class G4UIcmdWithADouble;

class DetectorConstructionMessenger : public G4UImessenger
{
public:
    DetectorConstructionMessenger(DetectorConstruction* detector);
    virtual ~DetectorConstructionMessenger();
    void SetNewValue(G4UIcommand* cmd, G4String newValue) override;

private:
    DetectorConstruction* fDetector;       // 指向 DetectorConstruction 的指针
    G4UIcmdWithADouble* fAngleCmd;         // 定义角度命令
    G4UIcommand* fPolygonCountCmd;
    G4UIcommand* fObjectAngleCmd;
    G4UIcommand* fDetectorAngle1Cmd;
    G4UIcommand* fDetectorAngle2Cmd;
    G4UIcommand* fDetectorAngle3Cmd;
    G4UIcommand* fDetectorAngle4Cmd;
    G4UIcommand* fDetectorAngle5Cmd;
    G4UIcommand* fDetectorModuleNumCmd;
    G4UIcommand* fDetectorBedDirectionCmd;
    G4UIcommand* fMultipleCmd;
    G4UIcommand* fDetectorFilterCmd;
    G4UIcommand* fDetectorRadiusCmd;
    G4UIcommand* fDetectorXCmd;  // 新增命令
    G4UIcommand* fDetectorYCmd;  // 新增命令
    G4UIcommand* fDetectorZCmd;  // 新增命令
    G4UIcommand* fFilterXCmd;
    G4UIcommand* fFilterYCmd;
    G4UIcommand* fFilterZCmd;
    G4UIcommand* fFilterXBiasCmd;
    G4UIcommand* fFilterYBiasCmd;
    G4UIcommand* fFilterPosCmd;
    G4UIcommand* fTubeFilterXCmd;
    G4UIcommand* fTubeFilterYCmd;
    G4UIcommand* fTubeFilterZCmd;
    G4UIcommand* fTubeFilterPosCmd;
    G4UIcommand* fTubeFilterXBiasCmd;
    G4UIcommand* fTubeFilterYBiasCmd;
    G4UIcommand* fObjectXBiasCmd;
    G4UIcommand* fObjectYBiasCmd;
    G4UIcommand* fObjectZBiasCmd;
    G4UIcommand* fDetectorXBiasCmd;  // 新增偏置命令
    G4UIcommand* fDetectorYBiasCmd;  // 新增偏置命令
    G4UIcommand* fDetectorZBiasCmd;  // 新增偏置命令
    G4UIcommand* fDDOCmd;
    G4UIcommand* fDSDcmd;
    G4UIcommand* fDSOcmd;
    G4UIcommand* fPixelYCountCmd;
    G4UIcommand* fPixelXCountCmd;
    G4UIcommand* fPixelXCmd;
    G4UIcommand* fPixelYCmd;
    G4UIcommand* fPixelZCmd;
    G4UIcommand* fPitchCmd;
    G4UIcmdWithAString* fG4dcmCmd;
    G4UIcmdWithAString* fSpectrumPosCmd;
    G4UIcmdWithAString* fSpectrumEnergyCmd;
    G4UIcmdWithAString* fOutputCmd;
    G4UIcmdWithAString* fMaterialCmd;  // 新增材料命令
    G4UIcmdWithAString* fMaterial1Cmd;
    G4UIcmdWithAString* fMaterial2Cmd;
};

#endif