#ifndef PARAUSERGUIDE_H
#define PARAUSERGUIDE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLineEdit>


#include "projectinfo.h"
#include "paramfilemanager.h"
#include "mainwindow.h"


class MainWindow;
class ChatPanel;

class ParaUserGuide : public QWidget
{
    Q_OBJECT
public:
    explicit ParaUserGuide(QWidget *parent = nullptr);

    void setMainWindow(MainWindow* main);
    void updateStatusLabel(const QString& text);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QString selectedTargetPath() const { return TargetPath; }
    const ProjectInfo& getAllInfo() const { return allInfo; }

signals:
    void fileSaved(const QString& filePath);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);

private:
    void PlaneRunFunction(const QString &targetPath, const ProjectInfo &info);

    MainWindow* mainWindow = nullptr;
    QLabel* statusLabel = nullptr;
    QVBoxLayout*  m_mainLayout = nullptr;
    QStackedWidget* centralView = nullptr;
    QStringList fileContentList;
    QMap<QString, QLineEdit*>         valueEdits;
    QMap<QString, QLineEdit*>         fileEdits;

    QVector<ValueParam>               valueParams;
    QVector<FileParam>                fileParams;
    ParamFileManager *paramMgr;

    QWidget* buttonPanelLeft = nullptr;
    QWidget* buttonPanelRight = nullptr;

    QComboBox* TargetCombo;
    QString    TargetPath;
    ProjectInfo allInfo;

};


#endif // PARAUSERGUIDE_H
