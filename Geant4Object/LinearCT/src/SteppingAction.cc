#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"
#include "TrackInformation.hh"

SteppingAction::SteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    G4Track* track = step->GetTrack();

    // 为所有轨迹设置 TrackInformation（如果尚未设置）
    TrackInformation* info = dynamic_cast<TrackInformation*>(track->GetUserInformation());
    if (!info) {
        info = new TrackInformation();
        track->SetUserInformation(info);
    }

    if (track->GetParticleDefinition()->GetParticleName() == "gamma") {
        G4String volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
        if (volumeName == "Object") { // 确保 "Object" 是你的物体名称
            info->SetPassedObject(true);
            const G4VProcess* process = step->GetPostStepPoint()->GetProcessDefinedStep();
            if (process != nullptr) {
                G4String processName = process->GetProcessName();
                if (processName == "compt" || processName == "Rayl") {
                    info->SetScattered(true);
                }
            }
        }
        // 标记主信号：穿过物体且未散射
        if (info->HasPassedObject() && !info->IsScattered()) {
            info->SetPrimary(true);
        } else {
            info->SetPrimary(false);
        }
    }
    // 传递散射标志给次级粒子
    const G4TrackVector* secondaries = step->GetSecondary();
    G4String volumeName = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    // 在物体中，只为次级光子（gamma）传递散射标志
    if (volumeName == "Object") {
        for (size_t i = 0; i < secondaries->size(); ++i) {
            G4Track* secondary = (*secondaries)[i];
            TrackInformation* secInfo = dynamic_cast<TrackInformation*>(secondary->GetUserInformation());
            if (!secInfo) {
                secInfo = new TrackInformation();
                secondary->SetUserInformation(secInfo);
            }
            // 传递穿过物体标志
            secInfo->SetPassedObject(true);
            // 只为次级光子（gamma）传递散射标志
            if (secondary->GetParticleDefinition()->GetParticleName() == "gamma" && info->IsScattered()) {
                secInfo->SetScattered(true);
            }
            // 次级粒子不继承 primary 标志
            secInfo->SetPrimary(false);
        }
    }
    // 在探测器中，为所有次级粒子传递散射标志
    if (volumeName == "cell") { // 仅在探测器中传递散射标志
        for (size_t i = 0; i < secondaries->size(); ++i) {
            G4Track* secondary = (*secondaries)[i];
            TrackInformation* secInfo = dynamic_cast<TrackInformation*>(secondary->GetUserInformation());
            if (!secInfo) {
                secInfo = new TrackInformation();
                secondary->SetUserInformation(secInfo);
            }
            // 如果当前轨迹是散射光子，则次级粒子继承散射标志
            if (info->IsScattered()) {
                secInfo->SetScattered(true);
            }
            //传递主信号标志
            if (info->IsPrimary()) {
                secInfo->SetPrimary(true);
            }
            //secInfo->SetPrimary(false);
        }
    }
}