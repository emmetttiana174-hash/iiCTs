// PrimaryGeneratorAction.cc
// 使用 source.txt 文件进行粒子发射
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
// STL 文件读取
#include <fstream>
#include <sstream>
#include <iostream>

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
 : G4VUserPrimaryGeneratorAction(),
   fDetector(detector),
   fParticleGun(nullptr),
   fCurrentIndex(0),
   fTotalParticles(0)
{
  // 创建 ParticleGun
  fParticleGun = new G4ParticleGun(1);
  // 设置默认粒子种类（可根据需要改成宏命令）
  auto particle = G4ParticleTable::GetParticleTable()->FindParticle("e-");
  fParticleGun->SetParticleDefinition(particle);
  // 读取 source.txt
  std::ifstream infile("/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/xray/txray2/input/W_0_100keV_8mm_2500K_20mm.txt");
  if (!infile) {
    G4cerr << "Cannot open source.txt" << G4endl;
    return;
  }
  G4cout << "Reading source.txt ..." << G4endl;
  G4double x,y,z, px,py,pz, E;
  std::string line;
  while (std::getline(infile, line)) {
    if (line.empty() || line[0]=='#') continue;
    std::istringstream iss(line);
    if (!(iss >> x >> y >> z >> px >> py >> pz >> E)) continue;
    ParticleRecord rec;
    rec.position     = G4ThreeVector(x*mm, y*mm, z*mm);
    rec.momentumDir  = G4ThreeVector(px,py,pz).unit();
    rec.energy       = E * MeV;
    fRecords.push_back(rec);
  }
  infile.close();
  G4cout << "Loaded " << fRecords.size() << " records from source.txt" << G4endl;
  
  if (fRecords.empty()) {
    G4cerr << "Warning: No valid particle records loaded from source file!" << G4endl;
  }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  
  // 输出统计信息
  G4cout << "Total particles generated: " << fTotalParticles << G4endl;
  if (!fRecords.empty()) {
    G4cout << "Source file records used: " << fRecords.size() << G4endl;
    G4cout << "Source file recycled: " << fTotalParticles / fRecords.size() 
           << " times with " << fTotalParticles % fRecords.size() 
           << " extra particles" << G4endl;
  }
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // 检查是否有粒子记录
  if (fRecords.empty()) {
    G4cerr << "Error: No particle records available!" << G4endl;
    return;
  }
  
  // 当所有粒子记录都用完时，重新从头开始
  if (fCurrentIndex >= fRecords.size()) {
    fCurrentIndex = 0;
    G4cout << "Recycling source particles. Starting from the beginning again." << G4endl;
  }
  
  // 使用当前索引的粒子记录
  const auto& rec = fRecords[fCurrentIndex++];
  fParticleGun->SetParticlePosition(rec.position);
  fParticleGun->SetParticleMomentumDirection(rec.momentumDir);
  fParticleGun->SetParticleEnergy(rec.energy);
  fParticleGun->GeneratePrimaryVertex(anEvent);
  
  // 更新计数器
  fTotalParticles++;
  
  // 每100万个粒子打印一次进度信息（可根据需要调整）
  if (fTotalParticles % 1000000 == 0) {
    G4cout << "Generated " << fTotalParticles << " particles so far" << G4endl;
  }
}