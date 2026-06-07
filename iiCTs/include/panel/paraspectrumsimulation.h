#ifndef PARASPECTRUMSIMULATION_H
#define PARASPECTRUMSIMULATION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPointer>

#include "projectinfo.h"
#include "paramfilemanager.h"
#include "mainwindow.h"

class MainWindow;
class ChatPanel;
class TerminalWidget;
class TransSpectrum;
class QProcess;

class ParaSpectrumSimulation : public QWidget
{
    Q_OBJECT
public:
    explicit ParaSpectrumSimulation(QWidget *parent = nullptr);

    void setMainWindow(MainWindow *main);
    void updateStatusLabel(const QString &text);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QString selectedTargetPath() const { return TargetPath; }
    const ProjectInfo &getAllInfo() const { return allInfo; }

signals:
    void fileSaved(const QString &filePath);
    void fileChanged(const QString &filePath);
    void fileResultChanged(const QString &filePath);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);

private:
    void PlaneRunFunction(const QString &targetPath, const ProjectInfo &info);
    void stopRunningSimulation();
    QString detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term);
    QString detectGeant4InstallPath(TerminalWidget *term);

    MainWindow *mainWindow = nullptr;
    QPointer<QProcess> activeGeant4Process;
    QLabel *statusLabel = nullptr;
    QVBoxLayout *m_mainLayout = nullptr;
    QStackedWidget *centralView = nullptr;
    QStringList fileContentList;
    QMap<QString, QLineEdit *> valueEdits;
    QMap<QString, QLineEdit *> fileEdits;

    QVector<ValueParam> valueParams;
    QVector<FileParam> fileParams;
    ParamFileManager *paramMgr;

    QWidget *buttonPanelLeft = nullptr;
    QWidget *buttonPanelRight = nullptr;

    QComboBox *TargetCombo;
    QComboBox *fileCombo;
    QComboBox *fileUICombo;
    QString TargetPath;
    ProjectInfo allInfo;
    QString userFileDir;
    QString systemFileDir;

    // GWL转换器
    TransSpectrum *gwlConverter;

    void populateUICombo();
    void populateCombo();
    void populateTargetCombo();

    // GWL转换相关方法
    void convertSelectedGWLFile();
    void convertSelectedGWLFileWithIndex(int idx);
    QString extractTemplateNameFromComboText(const QString &comboText);

    // 测试和调试方法
    void testGWLConversion();
};

#endif // PARASPECTRUMSIMULATION_H
