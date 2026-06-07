#ifndef PRIMARY_MESSENGER_HH
#define PRIMARY_MESSENGER_HH

#include "G4UImessenger.hh"
#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "globals.hh"
#include "G4UIcmdWithAString.hh"
class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithADouble;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWith2VectorAndUnit;

class PrimaryGeneratorActionMessenger : public G4UImessenger {
public:
    PrimaryGeneratorActionMessenger(PrimaryGeneratorAction* generator);
    ~PrimaryGeneratorActionMessenger();

    void SetNewValue(G4UIcommand* command, G4String newValue) override;

private:
    PrimaryGeneratorAction* fGenerator;
    G4UIdirectory* fTubeDir;
    //G4UIcmdWithADouble* fDSOcmd; 
    //G4UIdirectory* fTubeDir;
    G4UIcmdWithADouble* fDSOcmd;
    G4UIcmdWithADouble* fUOffsetcmd;
    G4UIcmdWithADouble* fVOffsetcmd;
    G4UIcmdWith3VectorAndUnit* fTubeAngleCmd; // 保持为 3 向量命令
    G4UIcmdWithADouble* fTubeXAngleCmd;
    G4UIcmdWithADouble* fTubeYAngleCmd;
    G4UIcmdWithADouble* fTubeZAngleCmd;
    G4UIcmdWithADouble* fLeftAngleCmd;
    G4UIcmdWithADouble* fRightAngleCmd;
    G4UIcmdWithADouble* fLowConeAngleCmd;
    G4UIcmdWithADouble* fHigConeAngleCmd;
    // G4UIcommand* fUOffsetcmd; 
    // G4UIcommand* fVOffsetcmd;
    // G4UIcommand* fTubeAngleCmd;
    // G4UIcommand* fleftAngleCmd;
    // G4UIcommand* frightAngleCmd;
    // G4UIcommand* fConeAngleCmd;
};

#endif