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

    fOrdercmd = new G4UIcmdWithADouble("/tube/setOrder", this);
    fOrdercmd->SetGuidance("Set U offset in cm");
    fOrdercmd->SetParameterName("Order", false);

    fTotalsourcecmd = new G4UIcmdWithADouble("/tube/setTotalsource", this);
    fTotalsourcecmd->SetGuidance("Set U offset in cm");
    fTotalsourcecmd->SetParameterName("Totalsource", false);

    fSourceLengthcmd = new G4UIcmdWithADouble("/tube/setSourceLength", this);
    fSourceLengthcmd->SetGuidance("Set U offset in cm");
    fSourceLengthcmd->SetParameterName("SourceLength", false);

    fOffcentercmd = new G4UIcmdWithADouble("/tube/setOffcenter", this);
    fOffcentercmd->SetGuidance("Set U offset in cm");
    fOffcentercmd->SetParameterName("Offcenter", false);
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
    delete fOrdercmd;
    delete fTotalsourcecmd;
    delete fSourceLengthcmd;
    delete fOffcentercmd;
}
void PrimaryGeneratorActionMessenger::SetNewValue(G4UIcommand* command, G4String newValue) {
    if (command == fDSOcmd) {
        G4double dso = fDSOcmd->GetNewDoubleValue(newValue);
        fGenerator->SetDSO(dso* cm);
    } else if (command == fUOffsetcmd) {
        G4double offset = fUOffsetcmd->GetNewDoubleValue(newValue);
        fGenerator->SetUOffset(offset* mm);
    } else if (command == fVOffsetcmd) {
        G4double vOffset = fVOffsetcmd->GetNewDoubleValue(newValue);
        fGenerator->SetVOffset(vOffset* mm);
    } else if (command == fOrdercmd) {
        G4double Order = fOrdercmd->GetNewDoubleValue(newValue);
        fGenerator->SetOrder(Order);
    } else if (command == fTotalsourcecmd) {
        G4int Totalsource = fTotalsourcecmd->GetNewDoubleValue(newValue);
        fGenerator->SetTotalsource(Totalsource);
    } else if (command == fSourceLengthcmd) {
        G4double SourceLength = fSourceLengthcmd->GetNewDoubleValue(newValue);
        fGenerator->SetSourceLength(SourceLength* mm);
    } else if (command == fOffcentercmd) {
        G4double Offcenter = fOffcentercmd->GetNewDoubleValue(newValue);
        fGenerator->SetOffcenter(Offcenter* mm);
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