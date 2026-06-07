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
        G4double detLeng = fDetector->GetDetectorLength();
        G4double polygoncount = fDetector->GetfPolygonCount();
        fPolygonCount = polygoncount;
        fdetLength = detLeng;
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
    G4double upperConeAngle = fhigConeAngle; // 上锥角
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
    if (fPolygonCount == 2){
        if (fOrder <= fTotalsource/2.0){
            // 计算方向向量
            G4ThreeVector direction(x, y, z);
            direction = direction.unit();  // 归一化
            G4RotationMatrix rot;
            rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
            rot.rotateY(fTubeYRotationAngle); // 绕 Y 轴旋转
            rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转
            direction = rot * direction;
            G4ThreeVector centre(sourceXPos+fUOffset-2*(fOrder-1)*fdetLength/(fTotalsource-2)+fdetLength/2,sourceYPos+fVOffset, -fdetLength/2);
            G4ThreeVector position = centre;
            G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
            // 根据编号读取对应的能量文件
            std::ostringstream oss;
            oss << (sourceIndex + 1) << ".dat";
            std::string energyFilePath = fSpectrumEnergy + oss.str();
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
        }else{
            // 计算方向向量
            G4ThreeVector direction(x, y, z);
            direction = direction.unit();  // 归一化
            G4RotationMatrix rot;
            rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
            rot.rotateY(fTubeYRotationAngle+90 * CLHEP::pi / 180.0); // 绕 Y 轴旋转
            rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转
            direction = rot * direction;
            G4ThreeVector centre(-fdetLength/2, sourceYPos+fVOffset, -sourceXPos-fUOffset+2*(fOrder-fTotalsource/2.0-1)*fdetLength/(fTotalsource-2)-fdetLength/2);
            G4ThreeVector position = centre;
            G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
            // 根据编号读取对应的能量文件
            std::ostringstream oss;
            oss << (sourceIndex + 1) << ".dat"; // 编号从 1 开始
            std::string energyFilePath = fSpectrumEnergy + oss.str();
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
    }else if (fPolygonCount == 3){
        if (fOrder <= fTotalsource/3.0){
            // 计算方向向量
            G4ThreeVector direction(x, y, z);
            direction = direction.unit();  // 归一化
            G4RotationMatrix rot;
            rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
            rot.rotateY(fTubeYRotationAngle-60 * CLHEP::pi / 180.0); // 绕 Y 轴旋转
            rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转
            direction = rot * direction;
            G4ThreeVector centre(sourceXPos+fUOffset-(3*(fOrder-1)*fdetLength)/(2*(fTotalsource-3))+fdetLength,sourceYPos+fVOffset, -(std::sin(60*deg)*3*(fOrder-1)*fdetLength)/(fTotalsource-3));
            G4ThreeVector position = centre;
            G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
            // 根据编号读取对应的能量文件
            std::ostringstream oss;
            oss << (sourceIndex + 1) << ".dat";
            std::string energyFilePath = fSpectrumEnergy + oss.str();
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
        }else if (fOrder > fTotalsource/3.0 && fOrder <= 2*fTotalsource/3.0){
            // 计算方向向量
            G4ThreeVector direction(x, y, z);
            direction = direction.unit();  // 归一化
            G4RotationMatrix rot;
            rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
            rot.rotateY(fTubeYRotationAngle); // 绕 Y 轴旋转
            rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转
            direction = rot * direction;
            G4ThreeVector centre(sourceXPos+fUOffset-(3*(fOrder-fTotalsource/3-1)*fdetLength)/(fTotalsource-3)+fdetLength/2,sourceYPos+fVOffset, -(std::sin(60*deg)*fdetLength));
            G4ThreeVector position = centre;
            G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
            // 根据编号读取对应的能量文件
            std::ostringstream oss;
            oss << (sourceIndex + 1) << ".dat";
            std::string energyFilePath = fSpectrumEnergy + oss.str();
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
        }else {
            // 计算方向向量
            G4ThreeVector direction(x, y, z);
            direction = direction.unit();  // 归一化
            G4RotationMatrix rot;
            rot.rotateX(fTubeXRotationAngle); // 绕 X 轴旋转
            rot.rotateY(fTubeYRotationAngle+60 * CLHEP::pi / 180.0); // 绕 Y 轴旋转
            rot.rotateZ(fTubeZRotationAngle); // 绕 Z 轴旋转
            direction = rot * direction;
            G4ThreeVector centre(sourceXPos+fUOffset-(3*(fOrder-2*fTotalsource/3-1)*fdetLength)/(2*(fTotalsource-3))-fdetLength/2,sourceYPos+fVOffset, -(std::sin(60*deg)*3*(fTotalsource-fOrder)*fdetLength)/(fTotalsource-3));
            G4ThreeVector position = centre;
            G4PrimaryVertex* vertex = new G4PrimaryVertex(position, 0);
            // 根据编号读取对应的能量文件
            std::ostringstream oss;
            oss << (sourceIndex + 1) << ".dat";
            std::string energyFilePath = fSpectrumEnergy + oss.str();
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
    }
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
    frightAngle = angle;
}
void PrimaryGeneratorAction::SetLowTubeConeAngle(G4double angle) {
    flowConeAngle = angle;
}
void PrimaryGeneratorAction::SetHigTubeConeAngle(G4double angle) {
    fhigConeAngle = angle;
}
void PrimaryGeneratorAction::SetOrder(G4double Order)
{
    fOrder = Order;
}
void PrimaryGeneratorAction::SetTotalsource(G4double Totalsource)
{
    fTotalsource = Totalsource;
}