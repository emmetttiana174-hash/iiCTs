// EmbeddingHelper.cc

#include "EmbeddingHelper.hh"

// Geant4核心头文件
#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"

// 项目特定头文件
#include "DetectorConstruction.hh"
#include "PhysicsList.hh"
#include "ActionInitialization.hh"
#include "RunAction.hh"

// 标准库与计时器
#include <chrono>
#include <iostream>
#include <string>

// 嵌入模式所需的Qt头文件
#ifdef G4VIS_USE_QT
#include <QApplication>
#include <QGuiApplication>
#include <QMainWindow>
#include <QWindow>
#include <QWidget>
#include <QTimer>
#include <QSocketNotifier>
#include <unistd.h> // For fileno, read
#include <QByteArray>
#include <QObject>
#include <QVBoxLayout>
#endif

// 通用辅助函数
static bool ApplyCmd(const std::string &cmd)
{
    auto *ui = G4UImanager::GetUIpointer();
    G4int rc = ui->ApplyCommand(cmd);
    if (rc != 0) {
        G4cerr << "[G4UI] Command failed(" << rc << "): " << cmd << G4endl;
        return false;
    }
    return true;
}

// runInEmbeddedMode 函数的实现
int runInEmbeddedMode(int argc, char* argv[])
{
#ifndef G4VIS_USE_QT
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--embed-mode") {
            G4cerr << "ERROR: This executable was not compiled with Qt support (G4VIS_USE_QT is not defined)." << G4endl;
            G4cerr << "Cannot run in --embed-mode." << G4endl;
            return 1;
        }
    }
    return -1;
#else
    bool isEmbedMode = false;
    std::string macroFile = "";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--embed-mode") {
            isEmbedMode = true;
            if (i + 1 < argc) {
                macroFile = argv[i + 1];
            }
            break;
        }
    }

    if (!isEmbedMode) {
        return -1;
    }

    G4cout << "[INFO] GammaTherapy starting in EMBED mode." << G4endl;

    // Force xcb platform to ensure real X11 window IDs under Linux/Wayland.
    // Must be set BEFORE QApplication is constructed.
    qputenv("QT_QPA_PLATFORM", QByteArray("xcb"));

    int sim_argc = 0;
    char *sim_argv[] = {nullptr};
    QApplication simApp(sim_argc, sim_argv);

    // Diagnostic: print platform name actually in use
    G4cout << "[INFO] Qt platform: " << QGuiApplication::platformName().toStdString() << G4endl;

    QMainWindow *winSub = new QMainWindow;
    winSub->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    
    // 设置窗口背景为黑色
    winSub->setStyleSheet("QMainWindow { background-color: black; }");
    
    QWidget *central = new QWidget(winSub);
    // 设置中央widget背景为黑色
    central->setStyleSheet("QWidget { background-color: black; }");
    
    winSub->setCentralWidget(central);
    
    winSub->move(-10000, -10000);
    winSub->show();

    QTimer::singleShot(50, [=]() {
        // Always print a pure decimal integer for the native window handle.
        // Use the top-level windowHandle()->winId() for a robust native ID.
        const WId wid = winSub->windowHandle() ? winSub->windowHandle()->winId() : winSub->winId();
        const auto widNumeric = static_cast<unsigned long long>(quintptr(wid));
        std::cout << "[WINIDGYYYY] " << widNumeric << std::endl;
        std::cout.flush();
    });

    QSocketNotifier *notifier = new QSocketNotifier(fileno(stdin), QSocketNotifier::Read, &simApp);
    static QByteArray inbuf;

    QObject::connect(notifier, &QSocketNotifier::activated, &simApp, [=](int) mutable {
        char buf[256];
        const ssize_t n = ::read(fileno(stdin), buf, sizeof(buf));
        if (n <= 0) return;
        inbuf.append(buf, int(n));
        
        if (!inbuf.contains("EMBED_OK")) return;
        notifier->setEnabled(false);

        auto* layout = new QVBoxLayout(central);
        layout->setContentsMargins(0,0,0,0);
        central->setLayout(layout);

        auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Serial);
        
        DetectorConstruction *detector = new DetectorConstruction();
        runManager->SetUserInitialization(detector);
        runManager->SetUserInitialization(new PhysicsList());
        runManager->SetUserInitialization(new ActionInitialization(detector));

        auto* visManager = new G4VisExecutive(central, "warnings");
        visManager->Initialize();

        auto startTime = std::chrono::steady_clock::now();
        RunAction::SetGlobalStartTime(startTime);
        RunAction::SetBatchMode(true);

        if (!macroFile.empty()) {
            ApplyCmd("/control/execute " + macroFile);
        }
        ApplyCmd("/vis/drawVolume");
        ApplyCmd("/vis/viewer/flush");
    });

    return simApp.exec();
#endif
}