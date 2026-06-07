#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIcmdWithADouble;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

class DetectorConstructionMessenger : public G4UImessenger
{
public:
    DetectorConstructionMessenger(DetectorConstruction *detector);
    virtual ~DetectorConstructionMessenger();
    void SetNewValue(G4UIcommand *cmd, G4String newValue) override;

private:
    DetectorConstruction *fDetector;        // 指向 DetectorConstruction 的指针
    G4UIcmdWithADouble *fAngleCmd;          // 原有的待测物体角度命令
    G4UIcmdWithADouble *fAnodeAngleCmd;     // 阳极靶旋转角度命令
    G4UIcmdWithAString *fAnodeMaterialCmd;  // 阳极靶材料命令
    G4UIcmdWithAString *fFilterMaterialCmd; // 过滤层材料命令（保持向后兼容）
    G4UIcmdWithADoubleAndUnit *fFilterXCmd; // 过滤层X尺寸命令（保持向后兼容）
    G4UIcmdWithADoubleAndUnit *fFilterYCmd; // 过滤层Y尺寸命令（保持向后兼容）
    G4UIcmdWithADoubleAndUnit *fFilterZCmd; // 过滤层Z尺寸命令（保持向后兼容）

    // 新增滤过层1控制命令
    G4UIcmdWithAString *fFilter1MaterialCmd;   // 滤过层1材料命令
    G4UIcmdWithADoubleAndUnit *fFilter1XCmd;   // 滤过层1 X尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter1YCmd;   // 滤过层1 Y尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter1ZCmd;   // 滤过层1 Z尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter1PosCmd; // 滤过层1位置命令 (/Filter/Filter1Position)

    // 新增滤过层2控制命令
    G4UIcmdWithAString *fFilter2MaterialCmd;   // 滤过层2材料命令
    G4UIcmdWithADoubleAndUnit *fFilter2XCmd;   // 滤过层2 X尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter2YCmd;   // 滤过层2 Y尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter2ZCmd;   // 滤过层2 Z尺寸命令
    G4UIcmdWithADoubleAndUnit *fFilter2PosCmd; // 滤过层2位置命令 (/Filter/Filter2Position)

    // 新增探测器控制命令
    G4UIcmdWithAString *fDetectorMaterialCmd; // 探测器材料命令
    G4UIcmdWithADoubleAndUnit *fDetectorXCmd; // 探测器X尺寸命令
    G4UIcmdWithADoubleAndUnit *fDetectorYCmd; // 探测器Y尺寸命令
    G4UIcmdWithADoubleAndUnit *fDetectorZCmd; // 探测器Z尺寸命令

    // 文件路径控制命令
    G4UIcmdWithAString *fInputFilePathCmd;  // 输入文件路径命令
    G4UIcmdWithAString *fOutputFilePathCmd; // 输出文件路径命令
};

#endif