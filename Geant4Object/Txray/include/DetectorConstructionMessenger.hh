#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

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
};

#endif