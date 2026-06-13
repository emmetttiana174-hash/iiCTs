#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"
#include "G4UIcmdWithADouble.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction* detector)
    : fDetector(detector) {
    // 定义 /detector/setObjectAngle 命令
    fAngleCmd = new G4UIcmdWithADouble("/detector/setObjectAngle", this);
    fAngleCmd->SetGuidance("Set object rotation angle in radians");
    fAngleCmd->SetParameterName("angle", false);  // 参数名为 angle，不可省略
}

DetectorConstructionMessenger::~DetectorConstructionMessenger() {
    delete fAngleCmd;
}

void DetectorConstructionMessenger::SetNewValue(G4UIcommand* cmd, G4String newValue) {
    if (cmd == fAngleCmd) {
        // 将宏文件中输入的字符串转换为 double 类型
        G4double angle_deg = G4UIcmdWithADouble::GetNewDoubleValue(newValue);
        G4double angle_rad = angle_deg * CLHEP::deg;
        fDetector->SetObjectRotationAngle(angle_rad);  // 调用 DetectorConstruction 的方法
    }
}