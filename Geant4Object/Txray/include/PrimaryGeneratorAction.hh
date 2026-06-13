#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

// Geant4 核心头
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

// STL
#include <vector>
#include <cstddef>

class G4Event;
class DetectorConstruction;

// 将 ParticleRecord 放在头文件中一次定义
struct ParticleRecord {
  G4ThreeVector position;     // 发射位置
  G4ThreeVector momentumDir;  // 归一化的动量方向
  G4double      energy;       // 能量 (MeV)
};

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
  PrimaryGeneratorAction(DetectorConstruction* detector);
  virtual ~PrimaryGeneratorAction();
  
  // 重载：每个事件调用
  virtual void GeneratePrimaries(G4Event* anEvent);
  
private:
  DetectorConstruction*           fDetector;      // 如需要可访问探测器几何
  G4ParticleGun*                  fParticleGun;   // 用来发射粒子
  std::vector<ParticleRecord>     fRecords;       // 存储从 source.txt 读取的数据
  size_t                          fCurrentIndex;  // 当前已发射到哪个记录
  G4long                          fTotalParticles; // 总计发射的粒子数
};

#endif // PrimaryGeneratorAction_h