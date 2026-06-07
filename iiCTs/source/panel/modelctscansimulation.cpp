#include "modelctscansimulation.h"
#include "mainwindow.h"
#include "terminalwidget.h"
#include "paractscansimulation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QFileInfoList>
#include <QTimer>
#include <QWindow>
#include <QDateTime>
#include <unistd.h>
#include <QSocketNotifier>

ModelCTScanSimulation::ModelCTScanSimulation(QWidget *parent)
    : QWidget(parent),
      geant4Process(nullptr),
      embedContainer(nullptr),
      foreignWindow(nullptr),
      isEmbedded(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel *titleLabel = new QLabel("ModelCTScanSimulation");
    titleLabel->setStyleSheet("color: white; font-size: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    imageContainer = new QWidget;
    imageContainer->setMinimumHeight(200);

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

    // [FIXED] 移除了 addStretch()，以允许 imageContainer 垂直扩展
    mainLayout->addLayout(buttonLayout);

    imageContainer->installEventFilter(this);
    connect(btnRefresh, &QPushButton::clicked, this, &ModelCTScanSimulation::onRefreshClicked);
}

void ModelCTScanSimulation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

bool ModelCTScanSimulation::eventFilter(QObject *o, QEvent *e)
{
    if (o == imageContainer && isEmbedded && e->type() == QEvent::Resize && embedContainer)
    {
        // 使用与头文件匹配的函数
        embedContainer->setGeometry(calculateSquareGeometry(imageContainer->rect()));
    }
    return QWidget::eventFilter(o, e);
}

ModelCTScanSimulation::~ModelCTScanSimulation()
{
    cleanupEmbeddedVisualization();
}

void ModelCTScanSimulation::paintEvent(QPaintEvent *event)
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

void ModelCTScanSimulation::setMainWindow(MainWindow *main)
{
    mainWindow = main;
}

void ModelCTScanSimulation::setProjectInfo(const ProjectInfo &info)
{
    projectInfo = info;
}

void ModelCTScanSimulation::setParaCTScanSimulation(ParaCTScanSimulation *paraCT)
{
    paraCTScanSimulation = paraCT;
}

void ModelCTScanSimulation::onProjectInfoChanged(const ProjectInfo &info)
{
    projectInfo = info;
}

void ModelCTScanSimulation::onRefreshClicked()
{
    refreshFunction();
}

void ModelCTScanSimulation::refreshFunction()
{
    if (!mainWindow) return;
    TerminalWidget *term = mainWindow->terminal();
    term->appendLine("echo [ProgressSTART][Name:Terminal][MAX:100]");
    term->appendLine("Start embedded Geant4 CT visualization...");
    startGeant4EmbeddedVisualization(projectInfo);
}

QString ModelCTScanSimulation::detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term)
{
    QString projectPath = info.ProjectPath;
    QDir projectDir(projectPath);

    if (projectDir.cdUp())
    {
        QString parentPath = projectDir.absolutePath();
        QString geant4ObjectPath = parentPath + "/Geant4Object";
        QDir geant4ObjectDir(geant4ObjectPath);
        if (geant4ObjectDir.exists())
        {
            QString ctProjectFolder = getCTProjectFolder(info.CTType);
            QString ctProjectPath = geant4ObjectPath + "/" + ctProjectFolder;
            if (QDir(ctProjectPath).exists())
            {
                term->appendLine("Found Geant4 " + info.CTType + " project: " + ctProjectPath);
                return ctProjectPath;
            }
            else
            {
                term->appendLine("<font color='red'>Error: " + ctProjectFolder + " directory not found in: " + geant4ObjectPath + "</font>");
            }
        }
        else
        {
            term->appendLine("<font color='red'>Error: Geant4Object directory not found in: " + parentPath + "</font>");
        }
    }
    else
    {
        term->appendLine("<font color='red'>Error: Cannot access parent directory of: " + projectPath + "</font>");
    }
    return QString();
}

QString ModelCTScanSimulation::detectGeant4InstallPath(TerminalWidget *term)
{
    QStringList possiblePaths = {
        "/home/day/Geant4/geant4-install",
        "/usr/local",
        "/opt/geant4",
        "/home/" + qgetenv("USER") + "/Geant4/geant4-install",
        "/usr/local/geant4"};

    for (const QString &path : possiblePaths)
    {
        if (QDir(path).exists("lib") && QDir(path).exists("include/Geant4"))
        {
            term->appendLine("Auto-detected Geant4 installation: " + path);
            return path;
        }
    }
    term->appendLine("<font color='orange'>Warning: Could not auto-detect Geant4 installation path. Using default: /home/day/Geant4/geant4-install</font>");
    return "/home/day/Geant4/geant4-install";
}

QString ModelCTScanSimulation::getCTProjectFolder(const QString &ctType)
{
    if (ctType == "helical") return "HelicalCT";
    if (ctType == "CBCT") return "CBCT";
    if (ctType == "linear_static") return "LinearCT";
    if (ctType == "polygon_interleaved") return "StaticCT_interval";
    return "StaticCT_Side";
}

void ModelCTScanSimulation::startGeant4EmbeddedVisualization(const ProjectInfo &info)
{
    if (!mainWindow) return;
    TerminalWidget *term = mainWindow->terminal();
    cleanupEmbeddedVisualization();

    QString visMacFileName = getCurrentSelectedVisMacFile(info, term);
    if (visMacFileName.isEmpty())
    {
        term->appendLine("<font color='red'>Error: No visualization Mac file found! Please select a template file and ensure it has been converted.</font>");
        return;
    }

    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 CT project!</font>");
        return;
    }

    QString buildPath = geant4ProjectPath + "/build";
    QString executable = buildPath + "/GammaTherapy";

    if (!QFileInfo::exists(executable))
    {
        term->appendLine("<font color='red'>Error: Geant4 executable not found: " + executable + "</font>");
        return;
    }

    term->appendLine("Starting embedded Geant4 CT visualization...");
    term->appendLine("Executable: " + executable);
    term->appendLine("Visualization Mac file: " + visMacFileName);

    geant4Process = new QProcess(this);
    geant4Process->setWorkingDirectory(buildPath);

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString geant4InstallPath = detectGeant4InstallPath(term);
    QString geant4LibPath = geant4InstallPath + "/lib";
    QString currentLdPath = env.value("LD_LIBRARY_PATH");
    env.insert("LD_LIBRARY_PATH", geant4LibPath + (currentLdPath.isEmpty() ? "" : ":" + currentLdPath));
    env.insert("G4INSTALL", geant4InstallPath);
    env.insert("G4SYSTEM", "Linux-g++");
    geant4Process->setProcessEnvironment(env);

    QStringList args;
    args << "--embed-mode" << visMacFileName;
    geant4Process->setProcessChannelMode(QProcess::SeparateChannels);

    connect(geant4Process, &QProcess::readyReadStandardOutput, this, [this, term]() {
        outputBuffer += QString::fromLocal8Bit(geant4Process->readAllStandardOutput());
        int newlinePos;
        while ((newlinePos = outputBuffer.indexOf('\n')) >= 0) {
            QString line = outputBuffer.left(newlinePos);
            outputBuffer.remove(0, newlinePos + 1);
            const QString tag = "[WINIDGYYYY]";
            if (!isEmbedded && line.contains(tag)) {
                QString windowIdStr = line.mid(line.indexOf(tag) + tag.length()).trimmed();
                bool ok;
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

    connect(geant4Process, &QProcess::readyReadStandardError, this, [this, term]() {
        QString error = QString::fromLocal8Bit(geant4Process->readAllStandardError());
        term->appendLine("<font color='red'>[Geant4 ERROR] " + error + "</font>");
    });

    connect(geant4Process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this, term](int exitCode, QProcess::ExitStatus exitStatus) {
        if (exitStatus == QProcess::NormalExit) {
            term->appendLine("<font color='" + QString(exitCode == 0 ? "green" : "orange") + "'>Geant4 CT visualization finished with exit code: " + QString::number(exitCode) + "</font>");
        } else {
            term->appendLine("<font color='red'>Geant4 CT visualization terminated abnormally!</font>");
        }
        cleanupEmbeddedVisualization();
    });

    geant4Process->start(executable, args);
    if (!geant4Process->waitForStarted(5000)) {
        term->appendLine("<font color='red'>Failed to start Geant4 embedded CT visualization!</font>");
        cleanupEmbeddedVisualization();
    } else {
        term->appendLine("<font color='green'>Geant4 embedded CT visualization started successfully!</font>");
    }
}

void ModelCTScanSimulation::tryEmbed(qulonglong windowId, int attempt)
{
    if (!mainWindow) return;
    TerminalWidget *term = mainWindow->terminal();

    if (attempt > 5) {
        term->appendLine("<font color='red'>Failed to embed window after multiple attempts.</font>");
        return;
    }

    foreignWindow = QWindow::fromWinId((WId)windowId);
    if (!foreignWindow) {
        QTimer::singleShot(100, this, [=]() { tryEmbed(windowId, attempt + 1); });
        return;
    }

    if (embedContainer) delete embedContainer;

    embedContainer = QWidget::createWindowContainer(foreignWindow, imageContainer);
    embedContainer->setGeometry(calculateSquareGeometry(imageContainer->rect()));
    embedContainer->show();

    isEmbedded = true;
    term->appendLine("<font color='green'>CT Window embedded successfully, ID=" + QString::number(windowId) + "</font>");

    QTimer::singleShot(50, this, [this]() {
        if (geant4Process && geant4Process->state() == QProcess::Running) {
            geant4Process->write("EMBED_OK\n");
            geant4Process->waitForBytesWritten(100);
        }
    });
}

QString ModelCTScanSimulation::getCurrentSelectedVisMacFile(const ProjectInfo &info, TerminalWidget *term)
{
    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty()) return QString();

    QString buildPath = geant4ProjectPath + "/build";
    if (!QDir(buildPath).exists()) {
        term->appendLine("<font color='red'>Build directory not found: " + buildPath + "</font>");
        return QString();
    }

    QString selectedVisMacFile = getSelectedFileBasedVisMacFile(info, term, buildPath);
    if (!selectedVisMacFile.isEmpty()) {
        term->appendLine("Found selected file based visualization Mac file: " + selectedVisMacFile);
        return selectedVisMacFile;
    }

    QString latestVisMacFile = getLatestVisMacFile(buildPath, term);
    if (!latestVisMacFile.isEmpty()) {
        term->appendLine("Found latest visualization Mac file: " + latestVisMacFile);
        return latestVisMacFile;
    }

    term->appendLine("<font color='orange'>No visualization Mac file found in build directory!</font>");
    return QString();
}

QString ModelCTScanSimulation::getSelectedFileBasedVisMacFile(const ProjectInfo &, TerminalWidget *term, const QString &buildPath)
{
    if (!paraCTScanSimulation) return QString();
    QString selectedTargetPath = paraCTScanSimulation->selectedTargetPath();
    if (selectedTargetPath.isEmpty()) return QString();

    QFileInfo selectedFileInfo(selectedTargetPath);
    if (selectedFileInfo.suffix().toLower() != "gwl") return QString();

    QString baseName = selectedFileInfo.baseName();
    QStringList possibleVisMacNames = {
        baseName + "_vis.mac",
        baseName + "vis.mac",
        baseName + "_visualization.mac"
    };

    for (const QString &visMacName : possibleVisMacNames) {
        if (QFile::exists(buildPath + "/" + visMacName)) {
            term->appendLine("Found corresponding visualization file: " + visMacName);
            return visMacName;
        }
    }
    return QString();
}

QString ModelCTScanSimulation::getLatestVisMacFile(const QString &buildPath, TerminalWidget *term)
{
    QDir buildDir(buildPath);
    QStringList visFilters;
    visFilters << "*vis.mac" << "*_vis.mac" << "*visualization.mac";

    QFileInfoList visMacFiles = buildDir.entryInfoList(visFilters, QDir::Files, QDir::Time);
    if (visMacFiles.isEmpty()) {
        term->appendLine("<font color='orange'>No visualization Mac files found in build directory.</font>");
        return QString();
    }
    return visMacFiles.first().fileName();
}

// [FIXED] 实现了与头文件中声明匹配的新函数
QRect ModelCTScanSimulation::calculateSquareGeometry(const QRect &containerRect)
{
    int squareSize = containerRect.height();
    int x = (containerRect.width() - squareSize) / 2;
    int y = 0; // 顶部对齐
    if (x < 0) x = 0;
    return QRect(x, y, squareSize, squareSize);
}

void ModelCTScanSimulation::cleanupEmbeddedVisualization()
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
            // [FIXED] 修复了拼写错误
            geant4Process->waitForFinished(3000);
        }
        geant4Process->deleteLater();
        geant4Process = nullptr;
    }
}

