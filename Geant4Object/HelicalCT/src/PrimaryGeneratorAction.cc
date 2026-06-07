//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file medical/GammaTherapy/src/PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//

//---------------------------------------------------------------------------
//
// ClassName:   PrimaryGeneratorAction
//
// Description: Generate primary beam
//
// Authors: V.Grichine, V.Ivanchenko
//
// Modified:
//
//----------------------------------------------------------------------------
//
#include "G4GeneralParticleSource.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "Randomize.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4Event.hh"
#include <fstream>
#include <vector>
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "PrimaryGeneratorActionMessenger.hh"
// PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector): fGPS(new G4GeneralParticleSource())
// //PrimaryGeneratorAction::PrimaryGeneratorAction():fDetector(detector)
//   //fDetector(pDet)
// {
//   fDetector = detector;
//   InitializeMe();
// }

// PrimaryGeneratorAction::~PrimaryGeneratorAction()
// {
//   delete fGPS;
//   //delete fParticleGun;
//   //delete fMessenger;
// }  

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void PrimaryGeneratorAction::InitializeMe()
// {
//     fVerbose = true;
//     fGPS = new G4GeneralParticleSource();
//     G4ThreeVector dir = fGPS->GetParticleMomentumDirection();
//     fCounter = 0;
// }


// void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
// {
//     fCounter++;
//     fGPS->GeneratePrimaryVertex(anEvent);
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void PrimaryGeneratorAction::SetBeamEnergy(G4double val)
// {
//   fEnergy = val;
//   //if(fEnergy<fDetector->GetMaxEnergy()) fDetector->SetMaxEnergy(fEnergy);  

//   // 确保能量单位为 keV
//   if(fEnergy < 10*keV) {
//     //G4cout << "Warning: Beam energy is too low for X-ray source." << G4endl;
//   }
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....


/////////////读取不同能谱文件
// PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
//     : fDetector(detector)
// {
//     InitializeMe();
// }

// PrimaryGeneratorAction::~PrimaryGeneratorAction()
// {
// }

// void PrimaryGeneratorAction::InitializeMe()
// {
//     fVerbose = true;
//     fCounter = 0;

//     // 加载能谱文件
//     std::ifstream file("/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/ScatterAnalysis/build/xcist_kVp120_tar7_bin1.dat");
//     if (!file.is_open()) {
//         G4cout << "Error: Cannot open spectrum file xcist_kVp120_tar7_bin1.dat! Please check the file path." << G4endl;
//         // 设置默认单能（120 keV）
//         energies.push_back(120 * keV);
//         probabilities.push_back(1.0);
//         cumulativeProbabilities.push_back(1.0);
//     } else {
//         energies.clear();
//         intensities.clear();

//         // 跳过第一行
//         std::string line;
//         if (!std::getline(file, line)) {
//             G4cout << "Error: Failed to read the first line of spectrum file!" << G4endl;
//             file.close();
//             energies.push_back(120 * keV);
//             probabilities.push_back(1.0);
//             cumulativeProbabilities.push_back(1.0);
//             return;
//         }

//         // 读取数据（处理逗号分隔）
//         while (std::getline(file, line)) {
//             std::istringstream iss(line);
//             std::string energyStr, intensityStr;

//             // 用逗号分隔字段
//             if (!std::getline(iss, energyStr, ',') || !std::getline(iss, intensityStr)) {
//                 // 忽略无效行（例如最后一行 "7.000000e+00"）
//                 continue;
//             }

//             // 将字符串转换为浮点数
//             std::istringstream energyStream(energyStr);
//             std::istringstream intensityStream(intensityStr);
//             G4double energy, intensity;
//             if (energyStream >> energy && intensityStream >> intensity) {
//                 energies.push_back(energy * keV); // 转换为 Geant4 单位 (keV)
//                 intensities.push_back(intensity);
//             } else {
//                 // 忽略无法解析的行
//                 continue;
//             }

//             if (energies.size() >= 120) break; // 读取前 120 行
//         }
//         file.close();

//         // 检查是否成功读取数据
//         if (energies.empty()) {
//             G4cout << "Error: No valid data loaded from spectrum file!" << G4endl;
//             energies.push_back(120 * keV);
//             probabilities.push_back(1.0);
//             cumulativeProbabilities.push_back(1.0);
//         } else {
//             // 计算总粒子数
//             G4double totalIntensity = 0.0;
//             for (const auto& intensity : intensities) {
//                 totalIntensity += intensity;
//             }

//             if (totalIntensity <= 0.0) {
//                 G4cout << "Error: Total intensity is zero or negative!" << G4endl;
//                 energies.clear();
//                 energies.push_back(120 * keV);
//                 probabilities.push_back(1.0);
//                 cumulativeProbabilities.push_back(1.0);
//             } else {
//                 // 计算概率并构建累积分布函数 (CDF)
//                 probabilities.clear();
//                 cumulativeProbabilities.clear();
//                 G4double cumulative = 0.0;
//                 for (const auto& intensity : intensities) {
//                     G4double probability = intensity / totalIntensity;
//                     probabilities.push_back(probability);
//                     cumulative += probability;
//                     cumulativeProbabilities.push_back(cumulative);
//                 }
//             }
//         }
//     }

//     // 调试输出：打印概率和 CDF
//     // for (size_t i = 0; i < energies.size(); ++i) {
//     //     G4cout << "Energy: " << energies[i] / keV << " keV, Probability: " << probabilities[i]
//     //            << ", CDF: " << cumulativeProbabilities[i] << G4endl;
//     // }
//     // 确保 CDF 的最后一个值接近 1.0
//     // if (!cumulativeProbabilities.empty() && std::abs(cumulativeProbabilities.back() - 1.0) > 1e-6) {
//     //     G4cout << "Warning: CDF final value is " << cumulativeProbabilities.back() << ", should be 1.0!" << G4endl;
//     // }
// }

// void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
// {
//     fCounter++;

//     // 自定义方锥束参数
//     G4double leftFanAngle = 16.7 * deg;    // 左边扇角
//     G4double rightFanAngle = 35 * deg;   // 右边扇角
//     //G4double fanAngle = 53 * deg;  // 扇角
//     G4double coneAngle = 10 * deg; // 锥角
//     G4double distance = 30.0 * cm;   // 射源到探测器距离
//     G4ThreeVector centre(6 * cm, 0, -15 * cm);  // 射源位置

//     // 计算 x 和 y 范围
//     G4double x_min = distance * std::tan(leftFanAngle);  // 左边范围
//     G4double x_max = -distance * std::tan(rightFanAngle);  // 右边范围
//     //G4double xMax = distance * std::tan(fanAngle / 2);
//     G4double yMax = distance * std::tan(coneAngle / 2);

//     // 生成均匀分布的点
//     G4double x = G4UniformRand() * (x_max - x_min) + x_min;  // 不对称 x
//     //G4double x = G4UniformRand() * 2 * xMax - xMax;
//     G4double y = G4UniformRand() * 2 * yMax - yMax;
//     G4double z = distance;  // z 从 -15 到 0

//     // 计算方向向量
//     G4ThreeVector direction(x, y, z);
//     direction = direction.unit();  // 归一化

//     // 设置发射位置
//     G4ThreeVector position = centre;
//     G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);

//     // 设置粒子类型（gamma）
//     G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//     G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
//     G4PrimaryParticle* primaryParticle = new G4PrimaryParticle(particle);

//     // 随机选择能量
//     G4double rand = G4UniformRand();
//     G4double selectedEnergy = 0.0;
//     for (size_t i = 0; i < cumulativeProbabilities.size(); ++i) {
//         if (rand <= cumulativeProbabilities[i]) {
//             selectedEnergy = energies[i];
//             break;
//         }
//     }

//     primaryParticle->SetKineticEnergy(selectedEnergy);
//     primaryParticle->SetMomentumDirection(direction);

//     vertex->SetPrimary(primaryParticle);
//     anEvent->AddPrimaryVertex(vertex);
// }

// void PrimaryGeneratorAction::SetBeamEnergy(G4double val)
// {
//     fEnergy = val;
//     if(fEnergy < 10*keV) {
//         //G4cout << "Warning: Beam energy is too low for X-ray source." << G4endl;
//     }
// }

////////定义方锥角
// PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
//     : fGPS(new G4GeneralParticleSource()), fDetector(detector)
// {
//     // 创建自定义命令 /control/setSourceX
//     // fSetSourceXCmd = new G4UIcmdWithADoubleAndUnit("/control/setSourceX", this);
//     // fSetSourceXCmd->SetGuidance("Set the x position of the source.");
//     // fSetSourceXCmd->SetParameterName("x", false);
//     // fSetSourceXCmd->SetDefaultUnit("cm");
//     // fSetSourceXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
//     InitializeMe();
// }

// PrimaryGeneratorAction::~PrimaryGeneratorAction()
// {
//     delete fGPS;
//     delete fSetSourceXCmd;
// }
// // void PrimaryGeneratorAction::SetNewValue(G4UIcommand* command, G4String newValue)
// // {
// //     if (command == fSetSourceXCmd)
// //     {
// //         fSourceX = fSetSourceXCmd->GetNewDoubleValue(newValue);
// //         //G4cout << "Source x position set to " << fSourceX / cm << " cm" << G4endl;
// //     }
// // }
// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void PrimaryGeneratorAction::InitializeMe()
// {
//     fVerbose = true;
//     fCounter = 0;

//     // 设置粒子类型（gamma）
//     G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//     G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
//     fGPS->SetParticleDefinition(particle);

//     // 设置能量（120 keV）
//     fGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
//     fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(120 * keV);
// }

// void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
// {
//     fCounter++;

//     // 自定义方锥束参数
//     G4double leftFanAngle  = 10 * deg;    // 左边扇角
//     G4double rightFanAngle = 10 * deg;   // 右边扇角
//     //G4double fanAngle = 53 * deg;  // 扇角
//     G4double coneAngle = 10 * deg; // 锥角
//     G4double distance = 29.77 * cm;   // 射源到探测器距离
//     G4ThreeVector centre(0 * cm, 0, -17.929 * cm);  // 射源位置

//     // 计算 x 和 y 范围
//     G4double x_min = -distance * std::tan(leftFanAngle);  // 左边范围
//     G4double x_max = distance * std::tan(rightFanAngle);  // 右边范围
//     //G4double xMax = distance * std::tan(fanAngle / 2);
//     G4double yMax = distance * std::tan(coneAngle / 2);

//     // 生成均匀分布的点
//     G4double x = G4UniformRand() * (x_max - x_min) + x_min;  // 不对称 x
//     //G4double x = G4UniformRand() * 2 * xMax - xMax;
//     G4double y = G4UniformRand() * 2 * yMax - yMax;
//     G4double z = distance;  // z 从 -40 到 0

//     // 计算方向向量
//     G4ThreeVector direction(x, y, z);
//     direction = direction.unit();  // 归一化
//     // 逆时针旋转60度（绕 y 轴）
//     // G4double theta = 60 * deg;
//     // G4double cosTheta = std::cos(theta);  // 0.5
//     // G4double sinTheta = std::sin(theta);  // 0.866
//     // G4ThreeVector direction_rotated(
//     //     cosTheta * direction.x() - sinTheta * direction.z(),
//     //     direction.y(),
//     //     sinTheta * direction.x() + cosTheta * direction.z()
//     // );
//     // direction = direction_rotated;  // 更新方向向量
//     // 设置发射位置
//     G4ThreeVector position = centre;
//     G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);

//     // 生成粒子
//     G4PrimaryParticle* particle = new G4PrimaryParticle(fGPS->GetParticleDefinition());
//     particle->SetKineticEnergy(fGPS->GetCurrentSource()->GetEneDist()->GenerateOne(fGPS->GetParticleDefinition()));
//     particle->SetMomentumDirection(direction);

//     vertex->SetPrimary(particle);
//     anEvent->AddPrimaryVertex(vertex);
//     // 可选：调试输出新射线源位置
//     //G4cout << "Event " << fCounter << ": Source position = (" << centre / cm << G4endl;
// }

// //....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// void PrimaryGeneratorAction::SetBeamEnergy(G4double val)
// {
//     fEnergy = val;
//     //if(fEnergy<fDetector->GetMaxEnergy()) fDetector->SetMaxEnergy(fEnergy);  

//     // 确保能量单位为 keV
//     if(fEnergy < 10*keV) {
//         //G4cout << "Warning: Beam energy is too low for X-ray source." << G4endl;
//     }

//     // 更新 GPS 的能量
//     fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fEnergy);
// }


/////////////不同形状的焦点
// PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
//     : fDetector(detector), fGPS(new G4GeneralParticleSource()), fCounter(0), fVerbose(true),
//       fConeHalfAngle(18 * deg), fConeAxis(G4ThreeVector(0, 0, 1)) // 默认：30度半张角，沿 z 轴
// {
//     InitializeMe();
// }

// PrimaryGeneratorAction::~PrimaryGeneratorAction()
// {
//     delete fGPS;
// }

// void PrimaryGeneratorAction::InitializeMe()
// {
//     // 读取 triangle_points.txt 文件
//     std::ifstream file("triangle_points.txt");
//     if (!file.is_open()) {
//         G4cerr << "Error: Cannot open triangle_points.txt" << G4endl;
//         return;
//     }
//     G4double x, y, z;
//     while (file >> x >> y >> z) {
//         fPoints.push_back(G4ThreeVector(x * mm, y * mm, z * mm));
//     }
//     file.close();
//     if (fPoints.empty()) {
//         G4cerr << "Error: No points loaded from triangle_points.txt" << G4endl;
//         return;
//     }
//     // 配置 GPS
//     G4SingleParticleSource* particleSource = fGPS->GetCurrentSource();
//     // 设置粒子类型
//     G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
//     G4ParticleDefinition* particle = particleTable->FindParticle("gamma"); // 假设使用伽马射线
//     particleSource->SetParticleDefinition(particle);
//     // 设置能量
//     particleSource->GetEneDist()->SetMonoEnergy(100 * keV); // 默认能量
//     // 设置位置类型为点（稍后动态修改具体位置）
//     particleSource->GetPosDist()->SetPosDisType("Point");
//     // 设置锥形束角度分布
//     particleSource->GetAngDist()->SetAngDistType("iso");
//     particleSource->GetAngDist()->SetMinTheta(0 * deg);
//     particleSource->GetAngDist()->SetMaxTheta(fConeHalfAngle); // 锥形束半张角
//     particleSource->GetAngDist()->SetBeamSigmaInAngR(0); // 确保均匀分布
//     particleSource->GetAngDist()->DefineAngRefAxes("angref1", G4ThreeVector(-1, 0, 0)); // 默认 z 轴
//     particleSource->GetAngDist()->DefineAngRefAxes("angref2", G4ThreeVector(0, 1, 0));
// }

// void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
// {
//     fCounter++;
//     // 随机选择发射位置
//     if (fPoints.empty()) {
//         G4cerr << "Error: No points available for particle generation" << G4endl;
//         return;
//     }
//     int idx = (int)(G4UniformRand() * fPoints.size());
//     G4ThreeVector position = fPoints[idx];
//     // 动态设置 GPS 发射位置
//     G4SingleParticleSource* particleSource = fGPS->GetCurrentSource();
//     particleSource->GetPosDist()->SetCentreCoords(position);
//     // 更新锥形束轴线（如果不是默认 z 轴）
//     if (fConeAxis != G4ThreeVector(0, 0, 1)) {
//         G4ThreeVector zAxis(0, 0, 1);
//         G4ThreeVector ref1 = zAxis.cross(fConeAxis).unit();
//         G4ThreeVector ref2 = fConeAxis.cross(ref1).unit();
//         particleSource->GetAngDist()->DefineAngRefAxes("angref1", ref1);
//         particleSource->GetAngDist()->DefineAngRefAxes("angref2", ref2);
//     }
//     // 生成粒子
//     fGPS->GeneratePrimaryVertex(anEvent);
// }
// void PrimaryGeneratorAction::SetBeamEnergy(G4double val)
// {
//     fEnergy = val;
//     fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(fEnergy);
// }
// void PrimaryGeneratorAction::SetConeHalfAngle(G4double val)
// {
//     fConeHalfAngle = val;
//     fGPS->GetCurrentSource()->GetAngDist()->SetMaxTheta(fConeHalfAngle);
// }
// void PrimaryGeneratorAction::SetConeAxis(G4ThreeVector axis)
// {
//     fConeAxis = axis.unit();
// }


///读取实际仿真能谱文件
PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* detector)
    : fDetector(detector)
{
    fMessenger = new PrimaryGeneratorActionMessenger(this);
    InitializeMe();
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fMessenger;
}

void PrimaryGeneratorAction::InitializeMe()
{
    fVerbose = true;
    fCounter = 0;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    fCounter++;
    if (fDetector){
        G4String spectrumpos = fDetector->GetSpectrumPos();
        G4double dso = fDetector->GetDSO();
        G4String spectrumenergy = fDetector->GetSpectrumEnergy();
        fDSO = dso;
        fSpectrumPos = spectrumpos;
        fSpectrumEnergy = spectrumenergy;
    }
    // 加载射线源位置文件
    std::ostringstream oss1;
    oss1 << fSpectrumPos << "position_cdf.dat";
    G4String fileName = oss1.str();  // 转换为 G4String
    std::ifstream sourceFile(fileName);
    //std::ifstream sourceFile("/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/ScatterAnalysis/build/position_cdf.dat");
    if (!sourceFile.is_open()) {
        G4cout << "Error: Cannot open source positions file! Please check the file path." << G4endl;
        // 设置默认位置
        sourceCDFs.push_back(1.0);
        sourceX.push_back(0.0 * cm);
        sourceY.push_back(0.0 * cm);
    } else {
        sourceCDFs.clear();
        sourceX.clear();
        sourceY.clear();
        std::string line;
        // 跳过第一行（如果有标题）
        if (!std::getline(sourceFile, line)) {
            G4cout << "Warning: Source file is empty, using default position." << G4endl;
            sourceCDFs.push_back(1.0);
            sourceX.push_back(0.0 * cm);
            sourceY.push_back(0.0 * cm);
        } else {
            do {
                std::istringstream iss(line);
                G4int id;
                G4double cdf, x, y;
                if (iss >> id >> cdf >> x >> y) {
                    sourceCDFs.push_back(cdf);
                    sourceX.push_back(x * cm); // 转换为 Geant4 单位 (cm)
                    sourceY.push_back(y * cm); // 转换为 Geant4 单位 (cm)
                }
            } while (std::getline(sourceFile, line) && sourceCDFs.size() < 400);
        }
        sourceFile.close();

        if (sourceCDFs.empty()) {
            G4cout << "Error: No valid source positions loaded!" << G4endl;
            sourceCDFs.push_back(1.0);
            sourceX.push_back(0.0 * cm);
            sourceY.push_back(0.0 * cm);
        }
    }
    // 自定义方锥束参数
    // G4double leftFanAngle = 5 * deg;    // 左边扇角
    // G4double rightFanAngle =5 * deg;   // 右边扇角
    G4double leftFanAngle   = fleftAngle;    
    G4double rightFanAngle  = frightAngle;   
    G4double lowerConeAngle = flowConeAngle; // 下锥角
    G4double upperConeAngle = fhigConeAngle * deg; // 上锥角
    // G4double coneAngle = 53.12 * deg; // 锥角
    //G4double coneAngle = fConeAngle;
    G4double distance = 30.0 * cm;   // 射源到探测器距离
    //G4ThreeVector centre(6 * cm, 0, -15 * cm);  // 射源位置

    // 计算 x 和 y 范围
    G4double x_min = -distance * std::tan(leftFanAngle);  // 左边范围
    G4double x_max = distance * std::tan(rightFanAngle);  // 右边范围
    G4double y_min = -distance * std::tan(lowerConeAngle); // 下部范围（负 y 方向）
    G4double y_max = distance * std::tan(upperConeAngle);  // 上部范围（正 y 方向）
    //G4double yMax = distance * std::tan(coneAngle / 2);

    // 生成均匀分布的点
    G4double x = G4UniformRand() * (x_max - x_min) + x_min;  // 不对称 x
    G4double y = G4UniformRand() * (y_max - y_min) + y_min;  // y 方向分布
    //G4double y = G4UniformRand() * 2 * yMax - yMax;
    G4double z = distance;  // z 从 -15 到 0

    // 计算方向向量
    G4ThreeVector direction(x, y, z);
    direction = direction.unit();  // 归一化
    G4RotationMatrix rot;
    rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
    rot.rotateY(fTubeYRotationAngle); // 绕 Y 轴旋转
    rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转

    // 应用旋转到方向向量
    direction = rot * direction;
    // 逆时针旋转60度（绕 y 轴）
    // G4double theta = -90 * deg;
    // G4double cosTheta = std::cos(theta);  // 0.5
    // G4double sinTheta = std::sin(theta);  // 0.866
    // G4ThreeVector direction_rotated(
    //     cosTheta * direction.x() - sinTheta * direction.z(),
    //     direction.y(),
    //     sinTheta * direction.x() + cosTheta * direction.z()
    // );
    // direction = direction_rotated;  // 更新方向向量
    // 设置发射位置
    //G4ThreeVector position = centre;
    //G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);

    // 设置粒子类型（gamma）
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("gamma");
    G4PrimaryParticle* primaryParticle = new G4PrimaryParticle(particle);

    // 随机选择射线源位置
    G4double rand = G4UniformRand();
    size_t sourceIndex = 0;
    for (size_t i = 0; i < sourceCDFs.size(); ++i) {
        if (rand <= sourceCDFs[i]) {
            sourceIndex = i;
            break;
        }
    }
    G4double sourceXPos = (sourceX[sourceIndex]/10 ) ; // 单位设为 cm 并加 5 cm
    G4double sourceYPos = (sourceY[sourceIndex]/10 ) ; // 单位设为 cm 并加 5 cm
    //G4cout << sourceXPos << sourceYPos << G4endl;
    //G4double sourceZPos = -10.0 * cm; // 您定义的 z 值
    //G4ThreeVector centre(sourceXPos,sourceYPos,-15 * cm);  // 射源位置
    //G4cout <<  "TubeDSO" << fDSO << G4endl;
    G4ThreeVector centre(sourceXPos+fUOffset,sourceYPos+fVOffset, -fDSO);
    G4ThreeVector position = centre;
    G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
    // 根据编号读取对应的能量文件
    std::ostringstream oss;
    oss << (sourceIndex + 1) << ".dat"; // 编号从 1 开始
    std::string energyFilePath = fSpectrumEnergy + oss.str();
    //std::string energyFilePath = "/home/day/Geant4/geant4-install/share/Geant4/examples/extended/medical/HelicalCT/2-3/" + oss.str();
    std::ifstream energyFile(energyFilePath.c_str());
    if (!energyFile.is_open()) {
        G4cout << "Error: Cannot open energy file " << oss.str() << "! Using default energy." << G4endl;
        primaryParticle->SetKineticEnergy(120 * keV);
    } else {
        std::vector<G4double> energyCDFs;
        std::vector<G4double> energies;
        std::string line;
        while (std::getline(energyFile, line)) {
            std::istringstream iss(line);
            G4double cdf, energy;
            if (iss >> cdf >> energy) {
                energyCDFs.push_back(cdf);
                energies.push_back(energy * keV); // 转换为 Geant4 单位 (keV)
            }
        }
        energyFile.close();

        if (energyCDFs.empty()) {
            G4cout << "Error: No valid energy data loaded from " << oss.str() << "! Using default energy." << G4endl;
            primaryParticle->SetKineticEnergy(120 * keV);
        } else {
            G4double rand2 = G4UniformRand();
            G4double selectedEnergy = 0.0;
            for (size_t i = 0; i < energyCDFs.size(); ++i) {
                if (rand2 <= energyCDFs[i]) {
                    selectedEnergy = energies[i];
                    break;
                }
            }
            if (selectedEnergy <= 0.0) {
                G4cout << "Warning: Selected energy is 0, using default 120 keV for file " << oss.str() << G4endl;
                selectedEnergy = 120 * keV;
            }
            primaryParticle->SetKineticEnergy(selectedEnergy);
        }
    }

    primaryParticle->SetMomentumDirection(direction);

    vertex->SetPrimary(primaryParticle);
    anEvent->AddPrimaryVertex(vertex);
}

void PrimaryGeneratorAction::SetBeamEnergy(G4double val)
{
    fEnergy = val;
    if(fEnergy < 10*keV) {
        //G4cout << "Warning: Beam energy is too low for X-ray source." << G4endl;
    }
}
void PrimaryGeneratorAction::SetDSO(G4double DSO)
{
    fDSO = DSO;
    G4cout << fDSO << G4endl;
}
void PrimaryGeneratorAction::SetUOffset(G4double uOffset)
{
    fUOffset = uOffset;
}
void PrimaryGeneratorAction::SetVOffset(G4double vOffset)
{
    fVOffset = vOffset;
}
void PrimaryGeneratorAction::SetTubeRotationAngle(G4double angleX, G4double angleY, G4double angleZ) {
    fTubeRotationAngleX = angleX;
    fTubeRotationAngleY = angleY;
    fTubeRotationAngleZ = angleZ;
}
void PrimaryGeneratorAction::SetTubeXRotationAngle(G4double angle) {
    fTubeXRotationAngle = angle;
}
void PrimaryGeneratorAction::SetTubeYRotationAngle(G4double angle) {
    fTubeYRotationAngle = angle;
}
void PrimaryGeneratorAction::SetTubeZRotationAngle(G4double angle) {
    fTubeZRotationAngle = angle;
}
void PrimaryGeneratorAction::SetLeftAngle(G4double angle) {
    fleftAngle = angle;
}
void PrimaryGeneratorAction::SetRightAngle(G4double angle) {
    fFanAngle = angle;
}
void PrimaryGeneratorAction::SetLowTubeConeAngle(G4double angle) {
    flowConeAngle = angle;
}
void PrimaryGeneratorAction::SetHigTubeConeAngle(G4double angle) {
    fhigConeAngle = angle;
}