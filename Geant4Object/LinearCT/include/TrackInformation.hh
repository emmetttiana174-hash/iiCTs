#include "G4VUserTrackInformation.hh"

class TrackInformation : public G4VUserTrackInformation {
public:
    TrackInformation() : scattered(false) ,passedObject(false),primary(false){}
    virtual ~TrackInformation() {}
    void SetScattered(bool scat) { scattered = scat; }
    bool IsScattered() const { return scattered; }

    void SetParentScattered(bool scat) { parentScattered = scat; }
    bool IsParentScattered() const { return parentScattered; }

    void SetPassedObject(bool passed) { passedObject = passed; }
    bool HasPassedObject() const { return passedObject; }

    void SetPrimary(bool prim) { primary = prim; }
    bool IsPrimary() const { return primary; }

    void SetScatteredInDetector(bool scat) { scatteredInDetector = scat; }
    bool IsScatteredInDetector() const { return scatteredInDetector; }
    
private:
    bool scattered;
    bool parentScattered;   // 父轨迹是否在物体内散射（仅光子）
    bool scatteredInDetector; // 是否是探测器内散射光子产生的次级粒子
    bool passedObject;   // 是否穿过物体
    bool primary;        // 是否为主信号
};