#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QRect>
#include <QPoint>
#include <QLabel>
#include <QProcess>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QThread>
#include <atomic>
#include <QMainWindow>
#include <QProgressBar>
#include <QGuiApplication>
#include <QScreen>

#include "commandwriterthread.h"
#include "projectinfo.h"
#include "terminalwidget.h"

#include "parahome.h"
#include "paratubesimulation.h"
#include "paraspectrumsimulation.h"
#include "paradetectormodel.h"
#include "paractscansimulation.h"
#include "paractreconstruction.h"
#include "parauserguide.h"
#include "paraversioninformation.h"

#include "resulthome.h"
#include "resulttubesimulation.h"
#include "resultspectrumsimulation.h"
#include "resultdetectormodel.h"
#include "resultctscansimulation.h"
#include "resultctreconstruction.h"
#include "resultuserguide.h"
#include "resultversioninformation.h"

#include "modelhome.h"
#include "modeltubesimulation.h"
#include "modelspectrumsimulation.h"
#include "modeldetectormodel.h"
#include "modelctscansimulation.h"
#include "modelctreconstruction.h"
#include "modeluserguide.h"
#include "modelversioninformation.h"

class ParaHome;
class ParaTubeSimulation;
class ParaSpectrumSimulation;
class ParaDetectorModel;
class ParaCTScanSimulation;
class ParaCTReconstruction;
class ParaUserGuide;
class ParaVersionInformation;
class ModelSpectrumSimulation;
class ChatAgent;
class ChatPanel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(const QString &username);
    void showStyledWarning(QWidget *parent, const QString &title, const QString &text);
    void cleanupProcess();
    void updateRightContent(const QString &menuName, const QString &panelType);
    QPlainTextEdit *getTerminalOutput() const;
    ProjectInfo projectInfo;
    void showChatPanel();

    ChatPanel *getChatPanel() const { return chatPanelWidget; }
    TerminalWidget *terminal() const { return terminalWidget; }
    ModelSpectrumSimulation *getModelSpectrumSimulation() const { return modelSpecWidget; }
    ParaSpectrumSimulation *getParaSpectrumSimulation() const { return paraSpecWidget; }

private slots:
    void handleMenuClick(int index);

private:
    QListWidget *menu;
    QStackedWidget *centerPanel;
    QStackedWidget *rightPanel;
    QLabel *centerTitle;
    QStackedWidget *rightStackedContent;
    QMap<QString, QMap<QString, QWidget *>> rightContentMap;
    int currentMenuIndex = 0;
    int currentPanelTypeIndex = 0;

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int screenW = screenGeometry.width();
    int screenH = screenGeometry.height();

    QWidget *titleWidget;
    QWidget *container = nullptr;
    QWidget *bottomInfoBox = nullptr;
    QLabel *sideImage;
    QLabel *topImage;
    QLabel *sysTopLabel;
    QPoint dragPos;
    bool dragging;
    bool resizing;
    QLabel *progressBarTitle;
    QLabel *progressBarState;
    QProgressBar *progressBar;

    QString currentUser;

    TerminalWidget *terminalWidget = nullptr;
    QPlainTextEdit *terminalOutput;
    QLineEdit *inputLine;
    QProcess *process;
    std::atomic_bool commandRunning{false};
    CommandWriterThread *commandThread = nullptr;
    bool isCommandThreadActive = false;

    QPixmap scaledSidePixmap;

    enum ResizeRegion
    {
        None,
        Right,
        Bottom,
        BottomRight
    };
    ResizeRegion resizeRegion;

    ParaHome *paraHomeWidget;
    ParaTubeSimulation *paraTubeWidget;
    ParaSpectrumSimulation *paraSpecWidget;
    ParaDetectorModel *paraDeteWidget;
    ParaCTScanSimulation *paraCTScanWidget;
    ParaCTReconstruction *paraCTReconWidget;
    ParaUserGuide *paraUserGuideWidget;
    ParaVersionInformation *paraVersionWidget;

    ResultHome *resultHomeWidget;
    ResultTubeSimulation *resultTubeWidget;
    ResultSpectrumSimulation *resultSpecWidget;
    ResultDetectorModel *resultDeteWidget;
    ResultCTScanSimulation *resultCTScanWidget;
    ResultCTReconstruction *resultCTReconWidget;
    ResultUserGuide *resultUserGuideWidget;
    ResultVersionInformation *resultVersionWidget;

    ModelHome *modelHomeWidget;
    ModelTubeSimulation *modelTubeWidget;
    ModelSpectrumSimulation *modelSpecWidget;
    ModelDetectorModel *modelDeteWidget;
    ModelCTScanSimulation *modelCTScanWidget;
    ModelCTReconstruction *modelCTReconWidget;
    ModelUserGuide *modelUserGuideWidget;
    ModelVersionInformation *modelVersionWidget;

    ChatAgent *chatAgentWidget;
    ChatPanel *chatPanelWidget;

signals:
    void logoutRequested(const QRect &geometry);

private slots:
    void openChangePasswd(const QString &username);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

void runCommandInThread(QProcess *process, QPlainTextEdit *terminalOutput, const QString &command, QWidget *parent);

#endif
