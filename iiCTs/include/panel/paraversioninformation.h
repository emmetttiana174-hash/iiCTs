#ifndef PARAVERSIONINFORMATION_H
#define PARAVERSIONINFORMATION_H

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

class ParaVersionInformation : public QWidget
{
    Q_OBJECT
public:
    explicit ParaVersionInformation(QWidget *parent = nullptr);

    void setMainWindow(MainWindow* main);
    void updateStatusLabel(const QString& text);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    const ProjectInfo& getAllInfo() const { return allInfo; }

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);

private:
    MainWindow* mainWindow = nullptr;
    QLabel* statusLabel = nullptr;
    QVBoxLayout*  m_mainLayout = nullptr;
    QStackedWidget* centralView = nullptr;

    QWidget* buttonPanelLeft = nullptr;
    QWidget* buttonPanelRight = nullptr;

    ProjectInfo allInfo;

};

#endif // PARAVERSIONINFORMATION_H
