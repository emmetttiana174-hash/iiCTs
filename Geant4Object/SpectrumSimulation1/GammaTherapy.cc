//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "G4Types.hh"
// [MODIFICATION] 使用 G4RunManagerFactory 来创建 RunManager
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4AnalysisManager.hh"

// 项目特定头文件
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"


#include "EmbeddingHelper.hh"

// [MODIFICATION] 确保包含必要的标准库头文件
#include <chrono>
#include <iostream>
#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

int main(int argc, char** argv)
{
    // ========================================================================
    // 1. 统一启动逻辑：首先检查是否为嵌入模式 (最高优先级)
    //    该函数会检查 "--embed-mode" 标志。
    // ========================================================================
    int embed_return_code = runInEmbeddedMode(argc, argv);
    if (embed_return_code != -1) {
        // 如果是嵌入模式，runInEmbeddedMode 会处理所有逻辑并返回退出码。
        // main 函数直接返回该值，程序结束。
        return embed_return_code;
    }

    // ========================================================================
    // 2. 独立模式逻辑 
    // ========================================================================
    G4cout << "=== Program Start (Standalone Mode) ===" << G4endl;

    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);

    // 判断是进入独立可视化模式，还是批处理模式
    
    bool isVisMode = false;
    if (argc == 1) {
        isVisMode = true; // 没有参数 → 默认可视化模式
    } else if (argc >= 2) {
        // 检查第一个参数是否为 "--embed-mode"，避免与嵌入模式逻辑冲突
        std::string firstArg = argv[1];
        if (firstArg != "--embed-mode") {
            if (firstArg.find("vis") != std::string::npos) { // 包含 "vis"
                if (firstArg.size() >= 4 && firstArg.rfind(".mac") == firstArg.size() - 4) {
                    isVisMode = true; // 任意名字包含 vis 且以 .mac 结尾 → 可视化模式
                }
            }
        }
    }

    G4UIExecutive* ui = nullptr;
    if (isVisMode) {
        ui = new G4UIExecutive(argc, argv);
    }

    // [MODIFICATION] 使用工厂模式创建单线程 RunManager
    auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);

    DetectorConstruction* detector = new DetectorConstruction();
    runManager->SetUserInitialization(detector);

    runManager->SetUserInitialization(new PhysicsList());

    // [MODIFICATION] 修正：ActionInitialization 包含了所有用户行为，设置一次即可
    runManager->SetUserInitialization(new ActionInitialization(detector));
    G4cout << "User initializations (Detector, Physics, Actions) are set." << G4endl;

    
    runManager->Initialize();

    // 初始化可视化，仅在独立可视化模式下
    G4VisManager* visManager = nullptr;
    if (isVisMode) {
        visManager = new G4VisExecutive;
        visManager->Initialize();
    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();

    auto startTime = std::chrono::steady_clock::now();
    G4cout << "=== Simulation timer started ===" << G4endl;

    if (isVisMode) {
        // 独立可视化模式
        G4String macroFile = (argc == 2) ? argv[1] : "vis.mac";
        G4cout << "=== Entering visualization mode, executing macro: " << macroFile << " ===" << G4endl;
        UImanager->ApplyCommand("/control/execute " + macroFile);
        ui->SessionStart();
        delete ui;
    } else {
        // 批处理模式
        G4String fileName = argv[1];
        G4cout << "=== Entering batch mode, executing macro: " << fileName << " ===" << G4endl;
        UImanager->ApplyCommand("/control/execute " + fileName);
    }

    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    G4cout << "=== Simulation timer stopped ===" << G4endl;
    G4cout << "Total duration: " << duration << " ms" << G4endl;

    // 任务终止
    if (visManager) delete visManager;
    delete runManager;
    
    return 0;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......