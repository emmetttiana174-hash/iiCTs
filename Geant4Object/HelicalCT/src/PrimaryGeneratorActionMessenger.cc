#include "PrimaryGeneratorActionMessenger.hh"
#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithADouble.hh"
PrimaryGeneratorActionMessenger::PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* generator)
    : fGenerator(generator) {
    fTubeDir = new G4UIdirectory("/tube/");
    fTubeDir->SetGuidance("Commands to control tube parameters");
    
    fDSOcmd = new G4UIcmdWithADouble("/tube/setDSO", this);
    fDSOcmd->SetGuidance("Set DSO in cm");
    fDSOcmd->SetParameterName("length", false);


    fUOffsetcmd = new G4UIcmdWithADouble("/tube/setUOffset", this);
    fUOffsetcmd->SetGuidance("Set U offset in cm");
    fUOffsetcmd->SetParameterName("offset", false);


    fVOffsetcmd = new G4UIcmdWithADouble("/tube/setVOffset", this);
    fVOffsetcmd->SetGuidance("Set V offset in cm");
    fVOffsetcmd->SetParameterName("vOffset", false);

    // fTubeAngleCmd = new G4UIcmdWith3VectorAndUnit("/tube/setTubeRotationAngle", this);
    // fTubeAngleCmd->SetGuidance("Set Tube rotation angles in radians (angleX angleY angleZ)");
    // fTubeAngleCmd->SetParameterName("angleX", "angleY", "angleZ", false);

    fTubeXAngleCmd = new G4UIcmdWithADouble("/tube/setTubeXRotationAngle", this);
    fTubeXAngleCmd->SetGuidance("Set Tube rotation angles in radians (angleX angleY angleZ)");
    fTubeXAngleCmd->SetParameterName("angleX", false);

    fTubeYAngleCmd = new G4UIcmdWithADouble("/tube/setTubeYRotationAngle", this);
    fTubeYAngleCmd->SetGuidance("Set Tube rotation angles in radians (angleX angleY angleZ)");
    fTubeYAngleCmd->SetParameterName("angleY", false);

    fTubeZAngleCmd = new G4UIcmdWithADouble("/tube/setTubeZRotationAngle", this);
    fTubeZAngleCmd->SetGuidance("Set Tube rotation angles in radians (angleX angleY angleZ)");
    fTubeZAngleCmd->SetParameterName("angleZ", false);

    fLeftAngleCmd = new G4UIcmdWithADouble("/tube/setLeftAngle", this);
    fLeftAngleCmd->SetGuidance("Set left angle in degrees");
    fLeftAngleCmd->SetParameterName("leftAngle", false);


    fRightAngleCmd = new G4UIcmdWithADouble("/tube/setRightAngle", this);
    fRightAngleCmd->SetGuidance("Set right angle in degrees");
    fRightAngleCmd->SetParameterName("rightAngle", false);

    fLowConeAngleCmd = new G4UIcmdWithADouble("/tube/setLowConeAngle", this);
    fLowConeAngleCmd->SetGuidance("Set Tube cone angle in degrees");
    fLowConeAngleCmd->SetParameterName("coneAngle", false);

    fHigConeAngleCmd = new G4UIcmdWithADouble("/tube/setHigConeAngle", this);
    fHigConeAngleCmd->SetGuidance("Set Tube cone angle in degrees");
    fHigConeAngleCmd->SetParameterName("coneAngle", false);
    // fDSOcmd = new G4UIcommand("/tube/setDSO", this);
    // fDSOcmd->SetGuidance("Set DSO with one parameter: length in cm");
    // G4UIparameter* lengthParam = new G4UIparameter("length", 'd', false);  // Double type, not optional
    // fDSOcmd->SetParameter(lengthParam);

    // fUOffsetcmd = new G4UIcommand("/tube/setUOffset", this);
    // fUOffsetcmd->SetGuidance("Set U offset with one parameter: offset in cm");
    // G4UIparameter* offsetParam = new G4UIparameter("offset", 'd', false);  // Double type, not optional
    // fUOffsetcmd->SetParameter(offsetParam);

    // fVOffsetcmd = new G4UIcommand("/tube/setVOffset", this);
    // fVOffsetcmd->SetGuidance("Set V offset with one parameter: offset in cm");
    // G4UIparameter* vOffsetParam = new G4UIparameter("vOffset", 'd', false);  // Double type, not optional
    // fVOffsetcmd->SetParameter(vOffsetParam);

    // fTubeAngleCmd = new G4UIcommand("/tube/settubeRotationAngle", this);
    // fTubeAngleCmd->SetGuidance("Set Tube rotation angles in radians (angleX angleY angleZ)");
    // G4UIparameter* angleXParam = new G4UIparameter("angleX", 'd', false);
    // G4UIparameter* angleYParam = new G4UIparameter("angleY", 'd', false);
    // G4UIparameter* angleZParam = new G4UIparameter("angleZ", 'd', false);
    // fTubeAngleCmd->SetParameter(angleXParam);
    // fTubeAngleCmd->SetParameter(angleYParam);
    // fTubeAngleCmd->SetParameter(angleZParam);

    // fleftAngleCmd = new G4UIcommand("/tube/setLeftAngle", this);
    // fleftAngleCmd->SetGuidance("Set left angle with one parameter: angle in degrees");
    // G4UIparameter* leftAngleParam = new G4UIparameter("leftAngle", 'd', false);  // Double type, not optional
    // fleftAngleCmd->SetParameter(leftAngleParam);

    // frightAngleCmd = new G4UIcommand("/tube/setRightAngle", this);
    // frightAngleCmd->SetGuidance("Set right angle with one parameter: angle in degrees");
    // G4UIparameter* rightAngleParam = new G4UIparameter("rightAngle", 'd', false);  // Double type, not optional
    // frightAngleCmd->SetParameter(rightAngleParam);

    // fConeAngleCmd = new G4UIcommand("/tube/setTubeConeAngle", this);
    // fConeAngleCmd->SetGuidance("Set Tube cone angle with one parameter: angle in degrees");
    // G4UIparameter* coneAngleParam = new G4UIparameter("coneAngle", 'd', false);  // Double type, not optional
    // fConeAngleCmd->SetParameter(coneAngleParam);
}

PrimaryGeneratorActionMessenger::~PrimaryGeneratorActionMessenger() {
    delete fTubeDir;
    delete fDSOcmd;
    delete fUOffsetcmd;
    delete fVOffsetcmd;
    delete fTubeAngleCmd;
    delete fTubeXAngleCmd;
    delete fTubeYAngleCmd;
    delete fTubeZAngleCmd;
    delete fLeftAngleCmd;
    delete fRightAngleCmd;
    delete fLowConeAngleCmd;
    delete fHigConeAngleCmd;
}
void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fDSOcmd) {
        G4double dso = fDSOcmd->GetNewDoubleValue(newValue);
        fGenerator->SetDSO(dso* cm);
    } else if (command == fUOffsetcmd) {
        G4double offset = fUOffsetcmd->GetNewDoubleValue(newValue);
        fGenerator->SetUOffset(offset* cm);
    } else if (command == fVOffsetcmd) {
        G4double vOffset = fVOffsetcmd->GetNewDoubleValue(newValue);
        fGenerator->SetVOffset(vOffset* cm);
    } else if (command == fTubeAngleCmd) {
        G4ThreeVector angles = fTubeAngleCmd->GetNew3VectorValue(newValue);
        fGenerator->SetTubeRotationAngle(angles.x()* deg, angles.y()* deg, angles.z()* deg);
    }else if (command == fTubeXAngleCmd) {
        G4double angleX = fTubeXAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetTubeXRotationAngle(angleX* deg);
    } else if (command == fTubeYAngleCmd) {
        G4double angleY = fTubeYAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetTubeYRotationAngle(angleY* deg);
    } else if (command == fTubeZAngleCmd) {
        G4double angleZ = fTubeZAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetTubeZRotationAngle(angleZ* deg);
    } else if (command == fLeftAngleCmd) {
        G4double leftAngle = fLeftAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetLeftAngle(leftAngle* deg);
    } else if (command == fRightAngleCmd) {
        G4double rightAngle = fRightAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetRightAngle(rightAngle* deg);
    } else if (command == fLowConeAngleCmd) {
        G4double lowconeAngle = fLowConeAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetLowTubeConeAngle(lowconeAngle* deg);
    } else if (command == fHigConeAngleCmd) {
        G4double HigconeAngle = fHigConeAngleCmd->GetNewDoubleValue(newValue);
        fGenerator->SetHigTubeConeAngle(HigconeAngle* deg);
    } else {
        G4cerr << "Unknown command: " << command->GetCommandName() << G4endl;
    }
}
// void PrimaryMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
//     // if(command == fDSOcmd){
//     //     std::istringstream iss(newValue);
//     //     G4double length;
//     //     iss >> length ;
//     //     fGenerator->SetDSO(length * cm);
//     // }
//     if (command == fDSOcmd) {
//         G4double dso = fDSOcmd->GetNewDoubleValue(newValue);
//         fGenerator->SetDSO(dso);
//     }else if(command == fUOffsetcmd){
//         std::istringstream iss(newValue);
//         G4double offset;
//         iss >> offset ;
//         fGenerator->SetUOffset(offset * cm);
//     }else if(command == fVOffsetcmd){
//         std::istringstream iss(newValue);
//         G4double vOffset;
//         iss >> vOffset ;
//         fGenerator->SetVOffset(vOffset * cm);
//     }else if(command == fTubeAngleCmd){
//         std::istringstream iss(newValue);
//         G4double angleX, angleY, angleZ;
//         iss >> angleX >> angleY >> angleZ;
//         fGenerator->SetTubeRotationAngle(angleX * rad, angleY * rad, angleZ * rad);
//     }else if(command == fleftAngleCmd){
//         std::istringstream iss(newValue);
//         G4double leftAngle;
//         iss >> leftAngle;
//         fGenerator->SetLeftAngle(leftAngle * deg);
//     }else if(command == frightAngleCmd){
//         std::istringstream iss(newValue);
//         G4double rightAngle;
//         iss >> rightAngle;
//         fGenerator->SetRightAngle(rightAngle * deg);
//     } else if(command == fConeAngleCmd){
//         std::istringstream iss(newValue);
//         G4double coneAngle;
//         iss >> coneAngle;
//         fGenerator->SetTubeConeAngle(coneAngle * deg);
//     } else {
//         G4cerr << "Unknown command: " << command->GetCommandName() << G4endl;
//     }

// }