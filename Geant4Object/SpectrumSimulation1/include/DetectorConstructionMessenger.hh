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
    DetectorConstruction *fDetector;
    G4UIcmdWithADouble *fAngleCmd;
    G4UIcmdWithADouble *fAnodeAngleCmd;
    G4UIcmdWithAString *fAnodeMaterialCmd;

    G4UIcmdWithAString *fFilterMaterialCmd;
    G4UIcmdWithADoubleAndUnit *fFilterXCmd;
    G4UIcmdWithADoubleAndUnit *fFilterYCmd;
    G4UIcmdWithADoubleAndUnit *fFilterZCmd;

    G4UIcmdWithAString *fFilter1MaterialCmd;
    G4UIcmdWithADoubleAndUnit *fFilter1XCmd;
    G4UIcmdWithADoubleAndUnit *fFilter1YCmd;
    G4UIcmdWithADoubleAndUnit *fFilter1ZCmd;
    G4UIcmdWithADoubleAndUnit *fFilter1PosCmd;

    G4UIcmdWithAString *fFilter2MaterialCmd;
    G4UIcmdWithADoubleAndUnit *fFilter2XCmd;
    G4UIcmdWithADoubleAndUnit *fFilter2YCmd;
    G4UIcmdWithADoubleAndUnit *fFilter2ZCmd;
    G4UIcmdWithADoubleAndUnit *fFilter2PosCmd;

    G4UIcmdWithAString *fDetectorMaterialCmd;
    G4UIcmdWithADoubleAndUnit *fDetectorXCmd;
    G4UIcmdWithADoubleAndUnit *fDetectorYCmd;
    G4UIcmdWithADoubleAndUnit *fDetectorZCmd;

    G4UIcmdWithAString *fInputFilePathCmd;
    G4UIcmdWithAString *fOutputFilePathCmd;
};

#endif
