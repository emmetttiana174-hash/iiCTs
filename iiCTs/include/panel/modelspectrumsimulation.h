#ifndef MODELSPECTRUMSIMULATION_H
#define MODELSPECTRUMSIMULATION_H

#include <QWidget>
#include <QPushButton>
#include <QProcess>
#include <QTimer>
#include <QWindow>
#include "projectinfo.h"

class MainWindow;
class TerminalWidget;

class ModelSpectrumSimulation : public QWidget
{
    Q_OBJECT

public:
    explicit ModelSpectrumSimulation(QWidget *parent = nullptr);
    ~ModelSpectrumSimulation();
    void setMainWindow(MainWindow *main);
    void setProjectInfo(const ProjectInfo &info);

    // 新增方法：供外部触发可视化嵌入
    void triggerVisualizationAfterGWLConversion(const ProjectInfo &info);

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);

private:
    void ModelRunFunction(const ProjectInfo &info);
    QString detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term);
    QString detectGeant4InstallPath(TerminalWidget *term);
    QString getCurrentSelectedVisMacFile(const ProjectInfo &info, TerminalWidget *term);
    QString getSelectedFileFromMainWindow(TerminalWidget *term);
    QString getSelectedFileBasedVisMacFile(const ProjectInfo &info, TerminalWidget *term, const QString &buildPath);
    QString getProjectBasedVisMacFile(const ProjectInfo &info, TerminalWidget *term, const QString &buildPath);
    QString findLatestVisMacFile(const ProjectInfo &info, TerminalWidget *term);

    // Geant4可视化嵌入相关方法
    void startGeant4EmbeddedVisualization(const ProjectInfo &info);
    void tryEmbed(qulonglong windowId, int attempt = 0);
    void cleanupEmbeddedProcess();
    QRect calculateSquareGeometry(const QRect &containerRect);

    QWidget *imageContainer;
    QPushButton *btnRefresh;
    QPushButton *btnExport;
    QPushButton *btnSettings;

    MainWindow *mainWindow = nullptr;
    ProjectInfo projectInfo;

    // Geant4嵌入相关成员变量
    QProcess *geant4Process = nullptr;
    QWidget *embedContainer = nullptr;
    QWindow *foreignWindow = nullptr;
    bool isEmbedded = false;
    QString outputBuffer;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *o, QEvent *e) override;
};

#endif // MODELSPECTRUMSIMULATION_H
