#include "modelspectrumsimulation.h"
#include "mainwindow.h"
#include "terminalwidget.h"
#include "paraspectrumsimulation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTimer>
#include <QWindow>
#include <QDebug>
#include <unistd.h>
#include <QSocketNotifier>


ModelSpectrumSimulation::ModelSpectrumSimulation(QWidget *parent)
    : QWidget(parent),
      geant4Process(nullptr),
      embedContainer(nullptr),
      foreignWindow(nullptr),
      isEmbedded(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("ModelSpectrumSimulation");
    titleLabel->setStyleSheet("color: white; font-size: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    imageContainer = new QWidget;


    // 将背景改为全黑，并将边框颜色改为灰色
    imageContainer->setStyleSheet(R"(
        background-color: #000000;
        border: 2px dashed #666666;
        border-radius: 10px;
    )");


    mainLayout->addWidget(imageContainer, 1);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    btnRefresh = new QPushButton("Refresh");
    btnExport = new QPushButton("Export");
    btnSettings = new QPushButton("Settings");

    QSize btnSize(100, 40);
    btnRefresh->setFixedSize(btnSize);
    btnExport->setFixedSize(btnSize);
    btnSettings->setFixedSize(btnSize);

    QString btnStyle = R"(
        QPushButton {
            color: white;
            background-color: #3366CC;
            font-weight: bold;
            border-radius: 5px;
            border: 2px solid #2F528F;
        }
        QPushButton:hover {
            background-color: #61A7F5;
            border: 2px solid #41A1F5;
        }
    )";
    btnRefresh->setStyleSheet(btnStyle);
    btnExport->setStyleSheet(btnStyle);
    btnSettings->setStyleSheet(btnStyle);

    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addWidget(btnExport);
    buttonLayout->addWidget(btnSettings);


    mainLayout->addLayout(buttonLayout);

    // 安装事件过滤器以处理窗口大小调整
    imageContainer->installEventFilter(this);

    // 连接Refresh按钮到ModelRunFunction
    connect(btnRefresh, &QPushButton::clicked, this, [this]()
            { ModelRunFunction(projectInfo); });
}

void ModelSpectrumSimulation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void ModelSpectrumSimulation::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, QColor("#22489E"));
    gradient.setColorAt(1, QColor("#4875D0"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect(), 15, 15);
}

void ModelSpectrumSimulation::setMainWindow(MainWindow *main)
{
    mainWindow = main;
}

void ModelSpectrumSimulation::setProjectInfo(const ProjectInfo &info)
{
    projectInfo = info;
}

void ModelSpectrumSimulation::onProjectInfoChanged(const ProjectInfo &info)
{
    projectInfo = info;
}

void ModelSpectrumSimulation::ModelRunFunction(const ProjectInfo &info)
{
    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();

    QString cmdStart = "echo [ProgressSTART][Name:Terminal][MAX:100]";
    term->appendLine(cmdStart);
    term->appendLine("Start embedded Geant4 visualization...");

    // 启动嵌入式可视化
    startGeant4EmbeddedVisualization(info);
}

QString ModelSpectrumSimulation::detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term)
{
    QString projectPath = info.ProjectPath;
    QDir projectDir(projectPath);

    // 获取上一级目录
    if (projectDir.cdUp())
    {
        QString parentPath = projectDir.absolutePath();
        QString geant4ObjectPath = parentPath + "/Geant4Object";

        // 检查Geant4Object目录是否存在
        QDir geant4ObjectDir(geant4ObjectPath);
        if (geant4ObjectDir.exists())
        {
            // 查找SpectrumSimulation目录
            QString spectrumSimPath = geant4ObjectPath + "/SpectrumSimulation";
            QDir spectrumDir(spectrumSimPath);
            if (spectrumDir.exists())
            {
                QString buildPath = spectrumSimPath + "/build";
                QDir buildDir(buildPath);
                if (buildDir.exists())
                {
                    term->appendLine("Found Geant4 SpectrumSimulation project: " + spectrumSimPath);
                    return spectrumSimPath;
                }
                else
                {
                    term->appendLine("<font color='orange'>Warning: Build directory not found: " + buildPath + "</font>");
                    return spectrumSimPath; // 返回项目路径，即使build目录不存在
                }
            }
            else
            {
                term->appendLine("<font color='red'>Error: SpectrumSimulation directory not found in: " + geant4ObjectPath + "</font>");
            }
        }
        else
        {
            term->appendLine("<font color='red'>Error: Geant4Object directory not found in: " + parentPath + "</font>");
            term->appendLine("<font color='yellow'>Expected path: " + geant4ObjectPath + "</font>");
        }
    }
    else
    {
        term->appendLine("<font color='red'>Error: Cannot access parent directory of: " + projectPath + "</font>");
    }

    return QString(); // 返回空字符串表示失败
}

QString ModelSpectrumSimulation::detectGeant4InstallPath(TerminalWidget *term)
{
    // 常见的Geant4安装路径
    QStringList possiblePaths = {
        "/home/day/Geant4/geant4-install",
        "/usr/local",
        "/opt/geant4",
        "/home/" + qgetenv("USER") + "/Geant4/geant4-install",
        "/usr/local/geant4"
    };

    for (const QString &path : possiblePaths)
    {
        QDir installDir(path);
        if (installDir.exists("lib") && installDir.exists("include/Geant4"))
        {
            term->appendLine("Auto-detected Geant4 installation: " + path);
            return path;
        }
    }

    term->appendLine("<font color='orange'>Warning: Could not auto-detect Geant4 installation path.</font>");
    term->appendLine("<font color='orange'>Using default path: /home/day/Geant4/geant4-install</font>");
    return "/home/day/Geant4/geant4-install";
}

QString ModelSpectrumSimulation::getCurrentSelectedVisMacFile(const ProjectInfo &info, TerminalWidget *term)
{
    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 project path!</font>");
        return QString();
    }

    QString buildPath = geant4ProjectPath + "/build";
    QDir buildDir(buildPath);
    if (!buildDir.exists())
    {
        term->appendLine("<font color='red'>Build directory not found: " + buildPath + "</font>");
        return QString();
    }

    // 方法1: 尝试从当前选中的GWL文件构造对应的vis.mac文件
    QString selectedVisMacFile = getSelectedFileBasedVisMacFile(info, term, buildPath);
    if (!selectedVisMacFile.isEmpty())
    {
        term->appendLine("Found selected file based visualization Mac file: " + selectedVisMacFile);
        return selectedVisMacFile;
    }

    // 方法2: 查找最新的vis.mac文件（备用方案）
    QStringList visFilters;
    visFilters << "*vis.mac";  // 查找格式如 test1vis.mac
    QFileInfoList visMacFiles = buildDir.entryInfoList(visFilters, QDir::Files, QDir::Time);

    if (!visMacFiles.isEmpty())
    {
        QString latestVisMacFile = visMacFiles.first().fileName();
        term->appendLine("Found latest visualization Mac file: " + latestVisMacFile);
        return latestVisMacFile;
    }

    // 方法3: 查找带下划线的vis.mac文件（兼容性）
    QStringList altFilters;
    altFilters << "*_vis.mac";
    QFileInfoList altVisMacFiles = buildDir.entryInfoList(altFilters, QDir::Files, QDir::Time);

    if (!altVisMacFiles.isEmpty())
    {
        QString latestVisMacFile = altVisMacFiles.first().fileName();
        term->appendLine("Found alternative visualization Mac file: " + latestVisMacFile);
        return latestVisMacFile;
    }

    // 方法4: 使用默认的vis.mac文件
    QString defaultVisFile = "vis.mac";
    QString defaultVisPath = buildPath + "/" + defaultVisFile;
    if (QFile::exists(defaultVisPath))
    {
        term->appendLine("Using default visualization Mac file: " + defaultVisFile);
        return defaultVisFile;
    }

    term->appendLine("<font color='red'>No visualization Mac file found!</font>");
    term->appendLine("<font color='yellow'>Please ensure you have selected a template file in Spectrum Simulation and it has been converted.</font>");
    return QString();
}

QString ModelSpectrumSimulation::getSelectedFileBasedVisMacFile(const ProjectInfo &info, TerminalWidget *term, const QString &buildPath)
{
    // 方法1: 尝试从MainWindow获取ParaSpectrumSimulation的选中文件
    QString selectedGWLFileName = getSelectedFileFromMainWindow(term);

    if (!selectedGWLFileName.isEmpty())
    {
        // 根据选中的GWL文件构造vis.mac文件名
        // 例如: test1.gwl -> test1vis.mac
        QFileInfo fileInfo(selectedGWLFileName);
        QString baseName = fileInfo.baseName();  // 获取不带扩展名的文件名
        QString visMacFileName = baseName + "vis.mac";

        QString fullVisMacPath = buildPath + "/" + visMacFileName;
        if (QFile::exists(fullVisMacPath))
        {
            term->appendLine("Found selected-based visualization Mac file: " + visMacFileName);
            return visMacFileName;
        }
        else
        {
            term->appendLine("<font color='orange'>Selected-based visualization Mac file not found: " + visMacFileName + "</font>");
        }
    }

    // 方法2: 尝试从项目信息中获取最近转换的文件
    QString projectBasedVisMacFile = getProjectBasedVisMacFile(info, term, buildPath);
    if (!projectBasedVisMacFile.isEmpty())
    {
        return projectBasedVisMacFile;
    }

    return QString();  // 没有找到
}

QString ModelSpectrumSimulation::getProjectBasedVisMacFile(const ProjectInfo &info, TerminalWidget *term, const QString &buildPath)
{
    // 在用户项目的MAC目录中查找最近转换的文件
    QString userMacRoot = info.ProjectPath + "/" + info.ProjectName + "/" + info.MACFolder;
    QString spectrumMacDir = userMacRoot + "/spectrum";

    QDir spectrumDir(spectrumMacDir);
    if (spectrumDir.exists())
    {
        // 查找spectrum目录中最新的.mac文件
        QStringList macFilters;
        macFilters << "*.mac";
        QFileInfoList macFiles = spectrumDir.entryInfoList(macFilters, QDir::Files, QDir::Time);

        if (!macFiles.isEmpty())
        {
            // 获取最新的.mac文件名
            QString latestMacFileName = macFiles.first().fileName();
            QFileInfo macFileInfo(latestMacFileName);
            QString baseName = macFileInfo.baseName();

            // 构造对应的vis.mac文件名
            QString visMacFileName = baseName + "vis.mac";
            QString fullVisMacPath = buildPath + "/" + visMacFileName;

            if (QFile::exists(fullVisMacPath))
            {
                term->appendLine("Found project-based visualization Mac file: " + visMacFileName);
                return visMacFileName;
            }
            else
            {
                term->appendLine("<font color='orange'>Project-based visualization Mac file not found: " + visMacFileName + "</font>");
            }
        }
    }

    return QString();
}

QString ModelSpectrumSimulation::findLatestVisMacFile(const ProjectInfo &info, TerminalWidget *term)
{
    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty())
    {
        return QString();
    }

    QString buildPath = geant4ProjectPath + "/build";
    QDir buildDir(buildPath);
    if (!buildDir.exists())
    {
        term->appendLine("<font color='red'>Build directory not found: " + buildPath + "</font>");
        return QString();
    }

    // 首先查找格式为 *vis.mac 的文件（如 test1vis.mac）
    QStringList nameFilters;
    nameFilters << "*vis.mac";
    QFileInfoList visMacFiles = buildDir.entryInfoList(nameFilters, QDir::Files, QDir::Time);

    if (!visMacFiles.isEmpty())
    {
        QString latestVisMacFile = visMacFiles.first().fileName();
        term->appendLine("Found latest visualization Mac file: " + latestVisMacFile);
        return latestVisMacFile;
    }

    // 备用方案：查找格式为 *_vis.mac 的文件（兼容性）
    QStringList altFilters;
    altFilters << "*_vis.mac";
    QFileInfoList altVisMacFiles = buildDir.entryInfoList(altFilters, QDir::Files, QDir::Time);

    if (!altVisMacFiles.isEmpty())
    {
        QString latestVisMacFile = altVisMacFiles.first().fileName();
        term->appendLine("Found alternative latest visualization Mac file: " + latestVisMacFile);
        return latestVisMacFile;
    }

    term->appendLine("<font color='orange'>No *vis.mac files found in: " + buildPath + "</font>");
    return QString();
}

ModelSpectrumSimulation::~ModelSpectrumSimulation()
{
    cleanupEmbeddedProcess();
}

bool ModelSpectrumSimulation::eventFilter(QObject *o, QEvent *e)
{
    // 如果imageContainer被调整大小，并且我们有嵌入的窗口，调整嵌入窗口的大小以匹配
    if (o == imageContainer && isEmbedded && e->type() == QEvent::Resize && embedContainer)
    {
        QRect squareRect = calculateSquareGeometry(imageContainer->rect());
        embedContainer->setGeometry(squareRect);
    }
    return QWidget::eventFilter(o, e);
}

void ModelSpectrumSimulation::startGeant4EmbeddedVisualization(const ProjectInfo &info)
{
    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();

    // 如果已有进程正在运行，先清理
    cleanupEmbeddedProcess();

    // 获取当前选中的可视化Mac文件
    QString visMacFileName = getCurrentSelectedVisMacFile(info, term);
    if (visMacFileName.isEmpty())
    {
        term->appendLine("<font color='red'>Error: No visualization Mac file found! Please select a template file in Spectrum Simulation and ensure it has been converted.</font>");
        return;
    }

    // 自动检索Geant4项目路径
    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 SpectrumSimulation project!</font>");
        return;
    }

    QString buildPath = geant4ProjectPath + "/build";
    QString executable = buildPath + "/GammaTherapy";

    // 检查可执行文件是否存在
    QFileInfo execInfo(executable);
    if (!execInfo.exists())
    {
        term->appendLine("<font color='red'>Error: Geant4 executable not found: " + executable + "</font>");
        term->appendLine("<font color='yellow'>Please make sure the Geant4 project has been compiled.</font>");
        return;
    }

    term->appendLine("Starting embedded Geant4 visualization...");
    term->appendLine("Executable: " + executable);
    term->appendLine("Visualization Mac file: " + visMacFileName);

    // 创建新的进程
    geant4Process = new QProcess(this);
    geant4Process->setWorkingDirectory(buildPath);

    // 设置环境变量
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString geant4InstallPath = detectGeant4InstallPath(term);
    QString geant4LibPath = geant4InstallPath + "/lib";

    QString currentLdPath = env.value("LD_LIBRARY_PATH");
    if (currentLdPath.isEmpty())
    {
        env.insert("LD_LIBRARY_PATH", geant4LibPath);
    }
    else
    {
        env.insert("LD_LIBRARY_PATH", geant4LibPath + ":" + currentLdPath);
    }

    env.insert("G4INSTALL", geant4InstallPath);
    env.insert("G4SYSTEM", "Linux-g++");
    geant4Process->setProcessEnvironment(env);

    // 准备命令行参数：--embed-mode + visMacFile
    QStringList args;
    args << "--embed-mode" << visMacFileName;

    geant4Process->setProcessChannelMode(QProcess::SeparateChannels);

    // 连接标准输出以检测窗口ID
    connect(geant4Process, &QProcess::readyReadStandardOutput, this, [this, term]() {
        outputBuffer += QString::fromLocal8Bit(geant4Process->readAllStandardOutput());
        int newlinePos;
        while ((newlinePos = outputBuffer.indexOf('\n')) >= 0) {
            QString line = outputBuffer.left(newlinePos);
            outputBuffer.remove(0, newlinePos + 1);

            const QString tag = "[WINIDGYYYY]";
            if (!isEmbedded && line.contains(tag)) {
                QString windowIdStr = line.mid(line.indexOf(tag) + tag.length()).trimmed();
                bool ok = false;
                qulonglong windowId = windowIdStr.toULongLong(&ok);
                if (ok && windowId > 0) {
                    term->appendLine("Received window ID: " + QString::number(windowId));
                    tryEmbed(windowId, 0);
                }
            } else {
                term->appendLine("[Geant4] " + line);
            }
        }
    });

    // 连接标准错误
    connect(geant4Process, &QProcess::readyReadStandardError, this, [this, term]() {
        QString error = QString::fromLocal8Bit(geant4Process->readAllStandardError());
        term->appendLine("<font color='red'>[Geant4 ERROR] " + error + "</font>");
    });

    // 连接进程结束信号
    connect(geant4Process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, term](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit) {
            if (exitCode == 0) {
                term->appendLine("<font color='green'>Geant4 visualization completed successfully!</font>");
            } else {
                term->appendLine("<font color='orange'>Geant4 visualization finished with exit code: " + QString::number(exitCode) + "</font>");
            }
        } else {
            term->appendLine("<font color='red'>Geant4 visualization terminated abnormally!</font>");
        }
        cleanupEmbeddedProcess();
    });

    // 启动进程
    geant4Process->start(executable, args);
    if (!geant4Process->waitForStarted(5000)) {
        term->appendLine("<font color='red'>Failed to start Geant4 embedded visualization!</font>");
        cleanupEmbeddedProcess();
        return;
    }

    term->appendLine("<font color='green'>Geant4 embedded visualization started successfully!</font>");
}

void ModelSpectrumSimulation::tryEmbed(qulonglong windowId, int attempt)
{
    if (attempt > 5) {
        if (mainWindow) {
            TerminalWidget *term = mainWindow->terminal();
            term->appendLine("<font color='red'>Failed to embed window after multiple attempts.</font>");
        }
        return;
    }

    foreignWindow = QWindow::fromWinId((WId)windowId);
    if (!foreignWindow) {
        // 如果窗口未找到，等待一下后重试
        QTimer::singleShot(100, this, [=]() { tryEmbed(windowId, attempt + 1); });
        return;
    }

    // 清理之前的嵌入容器
    if (embedContainer) {
        delete embedContainer;
        embedContainer = nullptr;
    }

    // 创建窗口容器并嵌入到imageContainer中
    embedContainer = QWidget::createWindowContainer(foreignWindow, imageContainer);

    // 设置正方形几何
    QRect squareRect = calculateSquareGeometry(imageContainer->rect());
    embedContainer->setGeometry(squareRect);
    embedContainer->show();

    isEmbedded = true;

    if (mainWindow) {
        TerminalWidget *term = mainWindow->terminal();
        term->appendLine("<font color='green'>Window embedded successfully, ID=" + QString::number(windowId) + "</font>");
    }

    // 发送"EMBED_OK"确认信号给Geant4进程
    QTimer::singleShot(50, this, [this]() {
        if (geant4Process && geant4Process->state() == QProcess::Running) {
            geant4Process->write("EMBED_OK\n");
            geant4Process->waitForBytesWritten(100);
        }
    });
}

void ModelSpectrumSimulation::cleanupEmbeddedProcess()
{
    isEmbedded = false;
    outputBuffer.clear();

    if (embedContainer) {
        delete embedContainer;
        embedContainer = nullptr;
    }

    foreignWindow = nullptr;

    if (geant4Process) {
        if (geant4Process->state() != QProcess::NotRunning) {
            geant4Process->kill();
            geant4Process->waitForFinished(3000);
        }
        geant4Process->deleteLater();
        geant4Process = nullptr;
    }
}

void ModelSpectrumSimulation::triggerVisualizationAfterGWLConversion(const ProjectInfo &info)
{
    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();
    term->appendLine("<font color='blue'>GWL conversion detected, starting embedded visualization...</font>");

    // 延迟一秒后启动可视化，确保文件完全写入
    QTimer::singleShot(1000, this, [this, info]() {
        startGeant4EmbeddedVisualization(info);
    });
}

QString ModelSpectrumSimulation::getSelectedFileFromMainWindow(TerminalWidget *term)
{
    if (!mainWindow)
    {
        term->appendLine("<font color='orange'>MainWindow not available for getting selected file</font>");
        return QString();
    }

    // 获取ParaSpectrumSimulation实例
    ParaSpectrumSimulation *paraSpecWidget = mainWindow->getParaSpectrumSimulation();
    if (!paraSpecWidget)
    {
        term->appendLine("<font color='orange'>ParaSpectrumSimulation widget not available</font>");
        return QString();
    }

    // 获取当前选中的目标路径
    QString selectedPath = paraSpecWidget->selectedTargetPath();
    if (selectedPath.isEmpty())
    {
        term->appendLine("<font color='orange'>No file selected in ParaSpectrumSimulation</font>");
        return QString();
    }

    // 提取文件名
    QFileInfo fileInfo(selectedPath);
    QString fileName = fileInfo.fileName();

    term->appendLine("Debug: Selected file from ParaSpectrumSimulation: " + fileName);

    return fileName;
}

QRect ModelSpectrumSimulation::calculateSquareGeometry(const QRect &containerRect)
{
    // 计算正方形窗口的大小，边长由高度确定
    int squareSize = containerRect.height();

    // 计算居中位置
    int x = (containerRect.width() - squareSize) / 2;
    int y = 0;  // 顶部对齐

    // 确保x不为负数
    if (x < 0) x = 0;

    return QRect(x, y, squareSize, squareSize);
}
