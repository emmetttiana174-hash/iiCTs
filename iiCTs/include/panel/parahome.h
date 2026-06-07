#ifndef PARAHOME_H
#define PARAHOME_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>
#include <QComboBox>


#include "projectinfo.h"
#include "mainwindow.h"

class MainWindow;
class ChatPanel;

struct ValueHomeParam { QString name; QString defaultValue; QString unit; };
struct FileHomeParam  { QString name; QString filePath; };

class ParaHome : public QWidget
{
    Q_OBJECT
public:
    explicit ParaHome(QWidget *parent = nullptr);

    void setMainWindow(MainWindow* main);
    void updateStatusLabel(const QString& text);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QString selectedTargetPath() const { return TargetPath; }
    const ProjectInfo& getAllInfo() const { return allInfo; }

signals:
    void projectInfoReady(const ProjectInfo &info);
    void projectStart();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);
    void onNewButtonClicked();
    void onOpenButtonClicked();
    void onStartButtonClicked();

private:
    void PlaneRunFunction(const QString &targetPath, const ProjectInfo &info);
    void writeProjectInfoFile();

    MainWindow* mainWindow = nullptr;
    QLabel* statusLabel = nullptr;
    QVBoxLayout*  m_mainLayout = nullptr;
    QStackedWidget* centralView = nullptr;
    QStringList fileContentList;
    QMap<QString, QLineEdit*>         valueEdits;
    QMap<QString, QComboBox*>   valueCombos;
    QMap<QString, QLineEdit*>         fileEdits;

    QVector<ValueHomeParam>               valueParams;
    QVector<FileHomeParam>                fileParams;

    QWidget* buttonPanelLeft = nullptr;
    QWidget* buttonPanelRight = nullptr;

    QString    TargetPath;
    ProjectInfo allInfo;

};

#endif // PARAHOME_H

