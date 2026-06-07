#ifndef MODELCTSCANSIMULATION_H
#define MODELCTSCANSIMULATION_H

#include <QWidget>
#include <QPushButton>
#include "projectinfo.h"

// Forward declarations
class MainWindow;
class TerminalWidget;
class ParaCTScanSimulation;
class QProcess;
class QWindow;
class QRect; // Add QRect forward declaration for the function signature

class ModelCTScanSimulation : public QWidget
{
    Q_OBJECT

public:
    explicit ModelCTScanSimulation(QWidget *parent = nullptr);
    ~ModelCTScanSimulation();
    void setMainWindow(MainWindow *main);
    void setProjectInfo(const ProjectInfo &info);
    void setParaCTScanSimulation(ParaCTScanSimulation *paraCT);

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);

private slots:
    void onRefreshClicked();

private:
    void refreshFunction();
    QString detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term);
    QString detectGeant4InstallPath(TerminalWidget *term);
    QString getCTProjectFolder(const QString &ctType);

    // Geant4 可视化嵌入相关方法
    void startGeant4EmbeddedVisualization(const ProjectInfo &info);
    QString getCurrentSelectedVisMacFile(const ProjectInfo &info, TerminalWidget *term);
    QString getSelectedFileBasedVisMacFile(const ProjectInfo &info, TerminalWidget *term, const QString &buildPath);
    QString getLatestVisMacFile(const QString &buildPath, TerminalWidget *term);
    void cleanupEmbeddedVisualization();
    void tryEmbed(qulonglong windowId, int attempt = 0);

    // [FIXED] 更新了函数声明以匹配 .cpp 文件中的实现
    QRect calculateSquareGeometry(const QRect &containerRect);

    QWidget *imageContainer;
    QPushButton *btnRefresh;
    QPushButton *btnExport;
    QPushButton *btnSettings;

    MainWindow *mainWindow = nullptr;
    ProjectInfo projectInfo;
    ParaCTScanSimulation *paraCTScanSimulation = nullptr;

    // Geant4 可视化嵌入相关成员变量
    QProcess *geant4Process;
    QWidget *embedContainer;
    QWindow *foreignWindow;
    bool isEmbedded;
    QString outputBuffer;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // MODELCTSCANSIMULATION_H
