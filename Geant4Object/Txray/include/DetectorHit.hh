#ifndef DetectorHit_h
#define DetectorHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4Threading.hh"

class G4Step;

/// 简化版的探测器hit类，专为虚拟探测平面设计
class DetectorHit : public G4VHit
{
  public:
    DetectorHit();
    DetectorHit(const DetectorHit& right);
    virtual ~DetectorHit();

    const DetectorHit& operator=(const DetectorHit& right);
    inline void* operator new(size_t);
    inline void operator delete(void* hit);

    // 记录每个步骤的信息
    void RecordStepInfo(const G4Step* step);

    // 设置和获取入射能量
    void SetIncidentEnergy(G4double energy);
    G4double GetIncidentEnergy() const;

    // 设置和获取粒子名称
    void SetParticleName(const G4String& name);
    G4String GetParticleName() const;

    // 设置和获取step指针
    void SetStep(G4Step* step);
    G4Step* GetStep() const;

    // 设置和获取位置
    void SetPos(const G4ThreeVector& pos);
    G4ThreeVector GetPos() const;

    // 设置和获取动量
    void SetMomentum(const G4ThreeVector& momentum);
    G4ThreeVector GetMomentum() const;

    // 累加能量沉积（对于虚拟探测器通常为0）
    void AddEnergy(G4double edep);
    G4double GetEdep() const;

    // 设置和获取探测器ID
    void SetDetectorID(G4int id);
    G4int GetDetectorID() const;

    // 绘制和打印函数
    virtual void Draw();
    virtual void Print();

  private:
    G4ThreeVector fPos;       // 位置
    G4ThreeVector fMomentum;  // 动量
    G4double fEdep;           // 能量沉积
    G4double fIncidentEnergy; // 入射能量
    G4String fParticleName;   // 粒子名称
    G4int fDetectorID;        // 探测器ID
    G4Step* fStep;            // Step指针
};

// 线程局部存储分配器的定义
typedef G4THitsCollection<DetectorHit> DetectorHitsCollection;
extern G4ThreadLocal G4Allocator<DetectorHit>* DetectorHitAllocator;

inline void* DetectorHit::operator new(size_t)
{
  if (!DetectorHitAllocator) {
    DetectorHitAllocator = new G4Allocator<DetectorHit>;
  }
  return (void*)DetectorHitAllocator->MallocSingle();
}

inline void DetectorHit::operator delete(void* hit)
{
  if (!DetectorHitAllocator) {
    DetectorHitAllocator = new G4Allocator<DetectorHit>;
  }
  DetectorHitAllocator->FreeSingle((DetectorHit*) hit);
}

#endif
