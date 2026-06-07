#include "mainwindow.h"
#include "loginwindow.h"
#include "commandwriterthread.h"
#include "changepasswd.h"
#include "roundeditemdelegate.h"
#include "progressmonitor.h"
#include "chatagent.h"
#include "chatpanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGuiApplication>
#include <QScreen>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>
#include <QButtonGroup>
#include <QPlainTextEdit>
#include <QProcess>
#include <QTimer>
#include <atomic>
#include <exception>
#include <QMenu>
#include <QToolButton>
#include <QStyledItemDelegate>
#include <QProgressBar>

#ifndef SCALE_RATE
#define SCALE_RATE 0.6
#endif

const int BORDER_WIDTH = 8;
double scaleRate = SCALE_RATE;

MainWindow::MainWindow(const QString &username)
{
    currentUser = username;

    int winW = static_cast<int>(screenW * scaleRate);
    int winH = winW * 9 / 16;
    if (winH > static_cast<int>(screenH * scaleRate))
    {
        winH = static_cast<int>(screenH * scaleRate);
        winW = winH * 16 / 9;
    }
    resize(winW, winH);

    //    resize(1920, 980);
    //    resize(192, 98);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);

    int x = screenGeometry.center().x() - width() / 2;
    int y = screenGeometry.center().y() - height() / 2 + height() / 8;
    move(x, y);

    container = new QWidget(this);

    QPixmap sidePixmap(":/images/background/background_left.png");
    QRect sideRect(0, 0, 500, height());
    scaledSidePixmap = sidePixmap.scaled(sideRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    QLabel *cornerImage = new QLabel(this);
    QPixmap CTISS_logo(":/images/background/CTISS_logo.png");
    cornerImage->setPixmap(CTISS_logo.scaled(300, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    cornerImage->setFixedSize(300, 80);
    cornerImage->move(20, 20);
    cornerImage->setStyleSheet("background: transparent;");
    cornerImage->raise();

    topImage = new QLabel(this);
    QPixmap background_up(":/images/background/background_up.png");
    topImage->setPixmap(background_up.scaled(980, 129, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    topImage->setFixedSize(980, 129);
    topImage->setStyleSheet("background: transparent;");
    topImage->raise();
    int initialX = (this->width() - 700) / 2;
    topImage->move(initialX, 0);

    sysTopLabel = new QLabel("Integrated Intelligent CT Simulation System", this);
    sysTopLabel->setStyleSheet("color: white;");
    QFont font("Times New Roman", 32, QFont::Bold);
    sysTopLabel->setFont(font);
    sysTopLabel->setAlignment(Qt::AlignCenter);
    sysTopLabel->setFixedSize(980, 100);
    sysTopLabel->raise();
    sysTopLabel->move(initialX, 0);

    bottomInfoBox = new QWidget(this);
    bottomInfoBox->setAttribute(Qt::WA_TranslucentBackground);
    bottomInfoBox->setFixedSize(300, 160);
    bottomInfoBox->setStyleSheet("background: transparent;");

    QLabel *imageLeft = new QLabel(bottomInfoBox);
    QPixmap smuPixmap(":/images/background/smu_logo.png");
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setFixedSize(smuPixmap.size());
    imageLeft->setFixedSize(70, 70);
    imageLeft->setScaledContents(true);
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setStyleSheet("background: transparent;");
    QLabel *imageRight = new QLabel(bottomInfoBox);
    imageRight->setPixmap(QPixmap(":/images/background/xjtu_logo.png").scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageRight->setStyleSheet("background: transparent;");
    QHBoxLayout *imageLayout = new QHBoxLayout;
    imageLayout->setContentsMargins(0, 0, 0, 0);
    imageLayout->setSpacing(20);
    imageLayout->addStretch();
    imageLayout->addWidget(imageLeft);
    imageLayout->addWidget(imageRight);
    imageLayout->addStretch();

    QLabel *topText = new QLabel("Southern Medical University\nXi’an Jiaotong University", bottomInfoBox);
    topText->setStyleSheet("color: white; font-size: 18px; font-family: 'Times New Roman'; font-weight: 700; background: transparent;");
    topText->setAlignment(Qt::AlignCenter);
    QLabel *bottomText = new QLabel("Version 1.0 Beta", bottomInfoBox);
    bottomText->setStyleSheet("color: white; font-size: 18px; font-family: 'Times New Roman'; font-style: italic; background: transparent;");
    bottomText->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainInfoLayout = new QVBoxLayout(bottomInfoBox);
    mainInfoLayout->setContentsMargins(0, 0, 0, 0);
    mainInfoLayout->setSpacing(5);
    mainInfoLayout->addLayout(imageLayout);
    mainInfoLayout->addWidget(topText);
    mainInfoLayout->addWidget(bottomText);

    bottomInfoBox->setLayout(mainInfoLayout);
    bottomInfoBox->raise();

    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(12, 12, 12, 12);

    titleWidget = new QWidget(this);
    titleWidget->setFixedSize(300, 40);
    titleWidget->move(this->width() - titleWidget->width() - 15, 10);
    titleWidget->setStyleSheet("background-color: transparent;");

    QHBoxLayout *titleBar = new QHBoxLayout(titleWidget);
    titleBar->setContentsMargins(0, 0, 0, 0);
    titleBar->setSpacing(2);

    QPushButton *minBtn = new QPushButton(this);
    QPushButton *maxBtn = new QPushButton(this);
    QPushButton *closeBtn = new QPushButton(this);

    QToolButton *userBtnWidget = new QToolButton(this);
    userBtnWidget->setFixedSize(110, 40);
    userBtnWidget->setPopupMode(QToolButton::InstantPopup);

    QLabel *iconLabel = new QLabel(userBtnWidget);
    iconLabel->setFixedSize(45, 40);
    iconLabel->setPixmap(QPixmap(":/images/background/icon_yiguo.png").scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel *textLabel = new QLabel(currentUser.left(8) + "  ", userBtnWidget);
    textLabel->setAlignment(Qt::AlignCenter);

    QHBoxLayout *userBtnWidgetLayout = new QHBoxLayout(userBtnWidget);
    userBtnWidgetLayout->setContentsMargins(0, 0, 0, 0);
    userBtnWidgetLayout->setSpacing(0);
    userBtnWidgetLayout->addWidget(iconLabel);
    userBtnWidgetLayout->addWidget(textLabel);

    iconLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    textLabel->setAttribute(Qt::WA_TransparentForMouseEvents);

    userBtnWidget->setStyleSheet(R"(
                                 QToolButton {
                                 border: 2px solid #2F528F;
                                 border-radius: 10px;
                                 background: transparent;
                                 }

                                 QToolButton:hover {
                                 background-color: #F3D8F1;
                                 border-color: #F3D8F1;
                                 }

                                 QToolButton:pressed {
                                 background-color: #588BF4;
                                 border-color: none;
                                 }
                                 )");

    iconLabel->setStyleSheet("border: none;");
    textLabel->setStyleSheet("color: white; font-size: 14pt; font-weight: bold; font-family: 'Times New Roman'; border: none;");

    QMenu *userMenu = new QMenu(userBtnWidget);
    QAction *changePwdAction = userMenu->addAction(" •  Change Password");
    QAction *logoutAction = userMenu->addAction(" •  Log Out");

    userMenu->setAttribute(Qt::WA_TranslucentBackground);
    userMenu->setStyleSheet(R"(
                            QMenu {
                            background-color: transparent;
                            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                            stop:0 #22489E, stop:1 #4875D0);
                            border: 1px solid #2F528F;
                            border-radius: 16px;
                            min-height: 50px;
                            padding: 6px;
                            font-size: 14pt;
                            font-weight: bold;
                            font-family: 'Times New Roman';
                            color: white;
                            }

                            QMenu::item {
                            padding: 6px 20px;
                            background-color: transparent;
                            min-height: 20px;
                            min-width: 80px;
                            }

                            QMenu::item:hover {
                            background-color: #F3D8F1;
                            border-radius: 16px;
                            }

                            QMenu::item:selected {
                            background-color: #F3D8F1;
                            border-radius: 16px;
                            }
                            )");

    userBtnWidget->setMenu(userMenu);

    QIcon minIcon(":/images/background/icon_min.png");
    QIcon maxIcon(":/images/background/icon_max.png");
    QIcon closeIcon(":/images/background/icon_close.png");
    minBtn->setIcon(minIcon);
    maxBtn->setIcon(maxIcon);
    closeBtn->setIcon(closeIcon);

    minBtn->setFixedSize(40, 40);
    maxBtn->setFixedSize(40, 40);
    closeBtn->setFixedSize(40, 40);
    minBtn->setContentsMargins(0, 0, 0, 0);
    maxBtn->setContentsMargins(0, 0, 0, 0);
    closeBtn->setContentsMargins(0, 0, 0, 0);

    QString hoverStyle =
        "QPushButton { background: transparent; border: none; }"
        "QPushButton:hover { background-color: #F3D8F1; border-radius: 10px;}";
    minBtn->setStyleSheet(hoverStyle);
    maxBtn->setStyleSheet(hoverStyle);
    closeBtn->setStyleSheet(hoverStyle);

    titleBar->addStretch();
    titleBar->addWidget(userBtnWidget);
    titleBar->addWidget(minBtn);
    titleBar->addWidget(maxBtn);
    titleBar->addWidget(closeBtn);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    QListWidget *menu = new QListWidget;
    QStackedWidget *centerPanel = new QStackedWidget;

    QVBoxLayout *menuLayout = new QVBoxLayout;
    menuLayout->addSpacerItem(new QSpacerItem(0, 80, QSizePolicy::Minimum, QSizePolicy::Fixed));
    menuLayout->addWidget(menu);
    menuLayout->setContentsMargins(10, 80, 0, 80);

    menu->setFixedWidth(315);
    menu->setIconSize(QSize(40, 40));
    menu->setSpacing(0);
    struct MenuItem
    {
        QString iconPath;
        QString name;
    };
    QList<MenuItem> menuItems = {
        {":/images/background/icon_home.png", "Home"},
        {":/images/background/icon_tube.png", "Tube Simulation"},
        {":/images/background/icon_ray.png", "Spectrum Simulation"},
        {":/images/background/icon_detector.png", "Detector Model"},
        {":/images/background/icon_ct.png", "CT Scan Simulation"},
        {":/images/background/icon_recon.png", "CT Reconstruction"},
        {"", "User Guide"},
        {"", "Version Information"}};

    chatAgentWidget = new ChatAgent;
    chatPanelWidget = new ChatPanel(chatAgentWidget);

    QVector<QWidget *> centerWidgets;
    for (const auto &item : menuItems)
    {
        QListWidgetItem *listItem = new QListWidgetItem;

        QPixmap iconPixmap(item.iconPath);
        if (!iconPixmap.isNull())
        {
            QIcon icon(iconPixmap.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            listItem->setIcon(icon);
        }
        else
        {
            QPixmap placeholderPixmap(40, 40);
            placeholderPixmap.fill(Qt::transparent);
            QIcon placeholderIcon(placeholderPixmap);
            listItem->setIcon(placeholderIcon);
        }

        listItem->setText("  " + item.name);
        listItem->setFont(QFont("Times New Roman", 20, QFont::Bold));
        listItem->setForeground(QBrush(QColor("#FFFFFF")));

        menu->addItem(listItem);

        if (item.name == "Home")
        {
            paraHomeWidget = new ParaHome;
            paraHomeWidget->setMainWindow(this);
            centerPanel->addWidget(paraHomeWidget);
            centerWidgets.append(paraHomeWidget);
        }
        else if (item.name == "Tube Simulation")
        {
            paraTubeWidget = new ParaTubeSimulation;
            paraTubeWidget->setMainWindow(this);
            centerPanel->addWidget(paraTubeWidget);
            centerWidgets.append(paraTubeWidget);
        }
        else if (item.name == "Spectrum Simulation")
        {
            paraSpecWidget = new ParaSpectrumSimulation;
            paraSpecWidget->setMainWindow(this);
            centerPanel->addWidget(paraSpecWidget);
            centerWidgets.append(paraSpecWidget);
        }
        else if (item.name == "Detector Model")
        {
            paraDeteWidget = new ParaDetectorModel;
            paraDeteWidget->setMainWindow(this);
            centerPanel->addWidget(paraDeteWidget);
            centerWidgets.append(paraDeteWidget);
        }
        else if (item.name == "CT Scan Simulation")
        {
            paraCTScanWidget = new ParaCTScanSimulation;
            paraCTScanWidget->setMainWindow(this);
            centerPanel->addWidget(paraCTScanWidget);
            centerWidgets.append(paraCTScanWidget);
        }
        else if (item.name == "CT Reconstruction")
        {
            paraCTReconWidget = new ParaCTReconstruction;
            paraCTReconWidget->setMainWindow(this);
            centerPanel->addWidget(paraCTReconWidget);
            centerWidgets.append(paraCTReconWidget);
        }
        else if (item.name == "User Guide")
        {
            paraUserGuideWidget = new ParaUserGuide;
            paraUserGuideWidget->setMainWindow(this);
            centerPanel->addWidget(paraUserGuideWidget);
            centerWidgets.append(paraUserGuideWidget);
        }
        else if (item.name == "Version Information")
        {
            paraVersionWidget = new ParaVersionInformation;
            paraVersionWidget->setMainWindow(this);
            centerPanel->addWidget(paraVersionWidget);
            centerWidgets.append(paraVersionWidget);
        }
        else
        {
            QWidget *centerWidget = new QWidget;
            centerWidget->setStyleSheet(R"(
                                        background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #22489E, stop:1 #4875D0);
                                        border: none;
                                        border-radius: 15px;
                                        )");
            QVBoxLayout *centerLayout = new QVBoxLayout(centerWidget);
            centerLayout->setSpacing(10);
            centerLayout->addWidget(new QLabel(item.name + " - Center"));

            centerPanel->addWidget(centerWidget);
            centerWidgets.append(centerWidget);
        }
    }
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setStyleSheet(R"(
                        QListWidget {
                        background: transparent;
                        border: none;
                        }

                        QListWidget::item {
                        height: 60px;
                        color: white;
                        }
                        QListWidget::item:hover {
                        background-color: #3366CC;
                        border-radius: 5px;
                        }

                        QListWidget::item:selected {
                        background-color: #588BF4;
                        border-radius: 5px;
                        }
                        )");
    menu->setCurrentRow(0);

    centerTitle = new QLabel();
    centerTitle->setStyleSheet(R"(
                               QLabel {
                               color: white;
                               font-size: 42px;
                               font-weight: bold;
                               font-family: 'Times New Roman';
                               background-color: rgba(0, 0, 0, 0);
                               padding-left: 10px;
                               border: none;
                               }
                               )");
    centerTitle->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    centerTitle->setFont(font);
    centerTitle->setText(menuItems[0].name);

    QString btnAStyle = R"(
                        QPushButton {
                        color: white;
                        background-color: #3366CC;
                        font-weight: bold;
                        border-top-left-radius: 20px;
                        border-bottom-left-radius: 20px;
                        border-top-right-radius: 0px;
                        border-bottom-right-radius: 0px;
                        border: 2px solid #2F528F;
                        }
                        QPushButton:hover {
                        background-color: #3366CC;
                        border: 2px solid #41A1F5;
                        }
                        QPushButton:checked {
                        background-color: #61A7F5;
                        border: 2px solid #41A1F5;
                        }
                        )";

    QString btnBStyle = R"(
                        QPushButton {
                        color: white;
                        background-color: #3366CC;
                        font-weight: bold;
                        border: 2px solid #2F528F;
                        border-radius: 0px;
                        }
                        QPushButton:hover {
                        background-color: #3366CC;
                        border: 2px solid #41A1F5;
                        }
                        QPushButton:checked {
                        background-color: #61A7F5;
                        border: 2px solid #41A1F5;
                        }
                        )";

    QString btnCStyle = R"(
                        QPushButton {
                        color: white;
                        background-color: #3366CC;
                        font-weight: bold;
                        border-top-left-radius: 0px;
                        border-bottom-left-radius: 0px;
                        border-top-right-radius: 20px;
                        border-bottom-right-radius: 20px;
                        border: 2px solid #2F528F;
                        }
                        QPushButton:hover {
                        background-color: #3366CC;
                        border: 2px solid #41A1F5;
                        }
                        QPushButton:checked {
                        background-color: #61A7F5;
                        border: 2px solid #41A1F5;
                        }
                        )";

    QPushButton *btnA = new QPushButton("Model");
    QPushButton *btnB = new QPushButton("Results");
    QPushButton *btnC = new QPushButton("Terminal");
    btnA->setCheckable(true);
    btnB->setCheckable(true);
    btnC->setCheckable(true);
    QSize buttonSize(120, 40);
    btnA->setFixedSize(buttonSize);
    btnB->setFixedSize(buttonSize);
    btnC->setFixedSize(buttonSize);
    QFont buttonFont("Times New Roman", 18, QFont::Bold);
    btnA->setFont(buttonFont);
    btnB->setFont(buttonFont);
    btnC->setFont(buttonFont);

    btnA->setStyleSheet(btnAStyle);
    btnB->setStyleSheet(btnBStyle);
    btnC->setStyleSheet(btnCStyle);

    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->setExclusive(true);
    buttonGroup->addButton(btnA, 0);
    buttonGroup->addButton(btnB, 1);
    buttonGroup->addButton(btnC, 2);

    QHBoxLayout *buttonLayout = new QHBoxLayout;

    buttonLayout->addWidget(btnA);
    buttonLayout->addWidget(btnB);
    buttonLayout->addWidget(btnC);

    buttonLayout->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    buttonLayout->setSpacing(0);

    QWidget *buttonContainer = new QWidget;
    buttonContainer->setLayout(buttonLayout);
    buttonContainer->setStyleSheet(R"(
                                   QWidget {
                                   background-color: #1A3678;
                                   border: 2px solid #1A3678;
                                   border-radius: 25px;
                                   }
                                   )");
    buttonContainer->setContentsMargins(10, 0, 10, 0);

    terminalWidget = new TerminalWidget;
    QProcess *process = new QProcess(terminalWidget);
    process->start("bash");
    terminalWidget->setProcess(process);
    terminalWidget->appendLine("[Main] System initialized.");

    connect(terminalWidget, &TerminalWidget::processRecreated, this, [=](QProcess *newProcess)
            { this->process = newProcess; });

    QWidget *terminalPanel = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(terminalPanel);
    layout->setSpacing(10);
    layout->setContentsMargins(0, 0, 0, 0);
    terminalPanel->setStyleSheet(R"(
                                 background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                 stop:0 #22489E, stop:1 #4875D0);
                                 border: none;
                                 border-radius: 15px;
                                 )");
    layout->addWidget(terminalWidget);

    QStringList panelTypes = {"Model", "Results", "Terminal"};
    QStackedWidget *rightStackedContent = new QStackedWidget;

    for (const auto &menuItem : menuItems)
    {
        for (const QString &type : panelTypes)
        {
            if (type == "Terminal")
            {
                rightContentMap[menuItem.name][type] = terminalPanel;
                continue;
            }

            QWidget *panel = nullptr;
            if (menuItem.name == "Home" && type == "Results")
            {
                resultHomeWidget = new ResultHome;
                rightContentMap[menuItem.name][type] = resultHomeWidget;
                rightStackedContent->addWidget(resultHomeWidget);
            }
            else if (menuItem.name == "Home" && type == "Model")
            {
                modelHomeWidget = new ModelHome;
                rightContentMap[menuItem.name][type] = modelHomeWidget;
                rightStackedContent->addWidget(modelHomeWidget);
            }
            else if (menuItem.name == "Tube Simulation" && type == "Results")
            {
                resultTubeWidget = new ResultTubeSimulation;
                rightContentMap[menuItem.name][type] = resultTubeWidget;
                rightStackedContent->addWidget(resultTubeWidget);
            }
            else if (menuItem.name == "Tube Simulation" && type == "Model")
            {
                modelTubeWidget = new ModelTubeSimulation;
                rightContentMap[menuItem.name][type] = modelTubeWidget;
                rightStackedContent->addWidget(modelTubeWidget);
            }
            else if (menuItem.name == "Spectrum Simulation" && type == "Results")
            {
                resultSpecWidget = new ResultSpectrumSimulation;
                resultSpecWidget->setMainWindow(this);
                rightContentMap[menuItem.name][type] = resultSpecWidget;
                rightStackedContent->addWidget(resultSpecWidget);
            }
            else if (menuItem.name == "Spectrum Simulation" && type == "Model")
            {
                modelSpecWidget = new ModelSpectrumSimulation;
                modelSpecWidget->setMainWindow(this);
                rightContentMap[menuItem.name][type] = modelSpecWidget;
                rightStackedContent->addWidget(modelSpecWidget);
            }
            else if (menuItem.name == "Detector Model" && type == "Results")
            {
                resultDeteWidget = new ResultDetectorModel;
                rightContentMap[menuItem.name][type] = resultDeteWidget;
                rightStackedContent->addWidget(resultDeteWidget);
            }
            else if (menuItem.name == "Detector Model" && type == "Model")
            {
                modelDeteWidget = new ModelDetectorModel;
                rightContentMap[menuItem.name][type] = modelDeteWidget;
                rightStackedContent->addWidget(modelDeteWidget);
            }
            else if (menuItem.name == "CT Scan Simulation" && type == "Results")
            {
                resultCTScanWidget = new ResultCTScanSimulation;
                resultCTScanWidget->setMainWindow(this);
                rightContentMap[menuItem.name][type] = resultCTScanWidget;
                rightStackedContent->addWidget(resultCTScanWidget);
            }
            else if (menuItem.name == "CT Scan Simulation" && type == "Model")
            {
                modelCTScanWidget = new ModelCTScanSimulation;
                modelCTScanWidget->setMainWindow(this);
                // 设置与ParaCTScanSimulation的关联
                if (paraCTScanWidget)
                {
                    modelCTScanWidget->setParaCTScanSimulation(paraCTScanWidget);
                }
                rightContentMap[menuItem.name][type] = modelCTScanWidget;
                rightStackedContent->addWidget(modelCTScanWidget);
            }
            else if (menuItem.name == "CT Reconstruction" && type == "Results")
            {
                resultCTReconWidget = new ResultCTReconstruction;
                resultCTReconWidget->setMainWindow(this);
                rightContentMap[menuItem.name][type] = resultCTReconWidget;
                rightStackedContent->addWidget(resultCTReconWidget);
            }
            else if (menuItem.name == "CT Reconstruction" && type == "Model")
            {
                modelCTReconWidget = new ModelCTReconstruction;
                rightContentMap[menuItem.name][type] = modelCTReconWidget;
                rightStackedContent->addWidget(modelCTReconWidget);
            }
            else if (menuItem.name == "User Guide" && type == "Results")
            {
                resultUserGuideWidget = new ResultUserGuide;
                rightContentMap[menuItem.name][type] = resultUserGuideWidget;
                rightStackedContent->addWidget(resultUserGuideWidget);
            }
            else if (menuItem.name == "User Guide" && type == "Model")
            {
                modelUserGuideWidget = new ModelUserGuide;
                rightContentMap[menuItem.name][type] = modelUserGuideWidget;
                rightStackedContent->addWidget(modelUserGuideWidget);
            }
            else if (menuItem.name == "Version Information" && type == "Results")
            {
                resultVersionWidget = new ResultVersionInformation;
                rightContentMap[menuItem.name][type] = resultVersionWidget;
                rightStackedContent->addWidget(resultVersionWidget);
            }
            else if (menuItem.name == "Version Information" && type == "Model")
            {
                modelVersionWidget = new ModelVersionInformation;
                rightContentMap[menuItem.name][type] = modelVersionWidget;
                rightStackedContent->addWidget(modelVersionWidget);
            }
            else
            {
                panel = new QWidget;
                QVBoxLayout *layout = new QVBoxLayout(panel);
                layout->setSpacing(10);
                layout->setContentsMargins(20, 20, 20, 20);

                panel->setStyleSheet(R"(
                                     background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                     stop:0 #22489E, stop:1 #4875D0);
                                     border: none;
                                     border-radius: 15px;
                                     )");

                QLabel *titleLabel = new QLabel(menuItem.name + " - " + type);
                titleLabel->setStyleSheet("color: white; font-size: 20px;");
                titleLabel->setAlignment(Qt::AlignCenter);
                layout->addWidget(titleLabel);

                rightContentMap[menuItem.name][type] = panel;
                rightStackedContent->addWidget(panel);
            }
        }
    }

    rightStackedContent->addWidget(terminalPanel);
    int terminalPanelIndex = rightStackedContent->count() - 1;

    btnA->setChecked(true);
    rightStackedContent->setCurrentIndex(0);

    connect(menu, &QListWidget::currentRowChanged, this, [=](int index)
            {
        centerTitle->setText(menuItems[index].name);
        centerPanel->setCurrentIndex(index);
        currentMenuIndex = index;

        if (panelTypes[currentPanelTypeIndex] == "Terminal") {
            rightStackedContent->setCurrentIndex(terminalPanelIndex);
        } else {
            int computedIndex = currentMenuIndex * (panelTypes.size()-1) + currentPanelTypeIndex;
            rightStackedContent->setCurrentIndex(computedIndex);
        } });

    connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, [=](int id)
            {
        currentPanelTypeIndex = id;

        if (panelTypes[id] == "Terminal") {
            rightStackedContent->setCurrentIndex(terminalPanelIndex);
        } else {
            int computedIndex = currentMenuIndex * (panelTypes.size()-1) + currentPanelTypeIndex;
            rightStackedContent->setCurrentIndex(computedIndex);
        } });

    QGridLayout *panelLayout = new QGridLayout;
    panelLayout->addWidget(centerTitle, 0, 0);
    panelLayout->addWidget(centerPanel, 1, 0);
    panelLayout->addWidget(buttonContainer, 0, 1, Qt::AlignRight | Qt::AlignVCenter);
    panelLayout->addWidget(rightStackedContent, 1, 1);
    panelLayout->setColumnStretch(0, 1);
    panelLayout->setColumnStretch(1, 1);

    QWidget *progressContainer = new QWidget;
    QHBoxLayout *progressLayout = new QHBoxLayout(progressContainer);
    progressLayout->setContentsMargins(50, 20, 50, 0);
    progressLayout->setSpacing(10);

    progressBarTitle = new QLabel("Progress:");
    progressBarState = new QLabel;
    progressBar = new QProgressBar;

    progressBarTitle->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    progressBarTitle->setStyleSheet(R"(
                                    QLabel {
                                    color: #3366CC;
                                    font-size: 20px;
                                    font-family: 'Times New Roman';
                                    background-color: transparent;
                                    font-weight: bold;
                                    }
                                    )");
    progressBarState->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    progressBarState->setStyleSheet(R"(
                                    QLabel {
                                    color: #4571CC;
                                    font-size: 10px;
                                    font-family: 'Times New Roman';
                                    background-color: transparent;
                                    font-weight: bold;
                                    }
                                    )");
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setStyleSheet(R"(
                               QProgressBar {
                               height: 15px;
                               border: 2px solid #2F528F;
                               border-radius: 8px;
                               background-color: #112255;
                               color: #3366CC;
                               font-weight: bold;
                               font-size: 15px;
                               text-align: right;
                               }

                               QProgressBar::chunk {
                               background: qlineargradient(
                               x1:0, y1:0, x2:1, y2:0,
                               stop:0 #4FA3F5,
                               stop:1 #83C9CD
                               );
                               border-radius: 5px;
                               }
                               )");
    progressLayout->addWidget(progressBarTitle);
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(progressBarState);

    QPlainTextEdit *textEdit = terminalWidget->findChild<QPlainTextEdit *>();
    ProgressMonitor *monitor = new ProgressMonitor(textEdit, this);
    connect(monitor, &ProgressMonitor::progressTaskStarted, this, [=](const QString &name, int max)
            {
        progressBarTitle->setText(name);
        progressBarState->setText("Running...");
        progressBar->setRange(0, max);
        progressBar->setValue(0); });
    connect(monitor, &ProgressMonitor::progressUpdated, this, [=](int value)
            { progressBar->setValue(value); });
    connect(terminalWidget, &TerminalWidget::progressEnd, this, [=]()
            { progressBarState->setText("Complete!"); });

    panelLayout->addWidget(progressContainer, 2, 0, 1, 2);
    panelLayout->setHorizontalSpacing(30);
    panelLayout->setVerticalSpacing(10);
    panelLayout->setContentsMargins(40, 170, 20, 10);

    mainLayout->addLayout(menuLayout);
    mainLayout->addLayout(panelLayout);

    containerLayout->addLayout(mainLayout);
    container->setLayout(containerLayout);

    connect(minBtn, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(maxBtn, &QPushButton::clicked, this, [=]()
            {
        if (isMaximized()) {
            showNormal();
        } else {
            QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
            int screenW = screenRect.width();
            int screenH = screenRect.height();

            int targetW = screenW;
            int targetH = targetW * 9 / 16;
            if (targetH > screenH) {
                targetH = screenH;
                targetW = targetH * 16 / 9;
            }
            int x = screenRect.x() + (screenW - targetW) / 2;
            int y = screenRect.y() + (screenH - targetH) / 2;
            setGeometry(x, y, targetW, targetH);
        } });

    connect(closeBtn, &QPushButton::clicked, this, [this]()
            { this->close(); });
    connect(logoutAction, &QAction::triggered, this, [this]()
            {
        terminalWidget->stopAndResetTerminal();
        emit logoutRequested(this->geometry()); });

    connect(changePwdAction, &QAction::triggered, this, [=]()
            { this->openChangePasswd(username); });

    setCentralWidget(container);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            paraTubeWidget,
            &ParaTubeSimulation::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            paraSpecWidget,
            &ParaSpectrumSimulation::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            modelSpecWidget,
            &ModelSpectrumSimulation::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            modelCTScanWidget,
            &ModelCTScanSimulation::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            paraDeteWidget,
            &ParaDetectorModel::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            paraCTScanWidget,
            &ParaCTScanSimulation::onProjectInfoChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            paraCTReconWidget,
            &ParaCTReconstruction::onProjectInfoChanged);

    // *****************************************************
    connect(paraHomeWidget,
            &ParaHome::projectStart,
            this,
            [menu]()
            {
                menu->setCurrentRow(1);
            });

    // *****************************************************
    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            resultCTReconWidget,
            &ResultCTReconstruction::onProjectInfoChanged);

    connect(paraCTReconWidget,
            &ParaCTReconstruction::fileResultChanged,
            resultCTReconWidget,
            &ResultCTReconstruction::scriptChanged);

    connect(paraCTScanWidget,
            &ParaCTScanSimulation::fileResultChanged,
            resultCTReconWidget,
            &ResultCTReconstruction::scriptCTChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            resultCTScanWidget,
            &ResultCTScanSimulation::onProjectInfoChanged);

    connect(paraCTReconWidget,
            &ParaCTReconstruction::fileResultChanged,
            resultCTScanWidget,
            &ResultCTScanSimulation::scriptChanged);

    connect(paraCTScanWidget,
            &ParaCTScanSimulation::fileResultChanged,
            resultCTScanWidget,
            &ResultCTScanSimulation::scriptCTChanged);

    connect(paraHomeWidget,
            &ParaHome::projectInfoReady,
            resultSpecWidget,
            &ResultSpectrumSimulation::onProjectInfoChanged);

    connect(paraSpecWidget,
            &ParaSpectrumSimulation::fileResultChanged,
            resultSpecWidget,
            &ResultSpectrumSimulation::scriptChanged);

    connect(paraTubeWidget,
                &ParaTubeSimulation::fileResultChanged,
                resultTubeWidget,
                &ResultTubeSimulation::scriptChanged);
}

void MainWindow::handleMenuClick(int index)
{
    centerPanel->setCurrentIndex(index);
    rightPanel->setCurrentIndex(index);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    static bool inResize = false;
    if (inResize)
        return;

    int w = width();
    int expectedH = w * 9 / 16;

    if (!isMaximized())
    {
        if (height() != expectedH)
            resize(w, expectedH);
    }

    QPixmap sidePixmap(":/images/background/background_left.png");
    QRect sideRect(0, 0, 500, height());
    scaledSidePixmap = sidePixmap.scaled(sideRect.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if (titleWidget)
        titleWidget->move(width() - titleWidget->width() - 15, 10);

    if (topImage)
    {
        int titleX = (width() - 700) / 2;
        topImage->move(titleX, 0);
    }

    if (sysTopLabel)
    {
        int titleX = (width() - 700) / 2;
        sysTopLabel->move(titleX, 0);
    }

    if (bottomInfoBox)
    {
        int marginLeft = 20;
        int marginBottom = 20;
        bottomInfoBox->move(marginLeft, height() - bottomInfoBox->height() - marginBottom);
    }

    QWidget::resizeEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QRect rect = this->rect();
    QPoint pos = event->pos();

    resizing = false;
    dragging = false;
    resizeRegion = None;

    if (rect.right() - pos.x() <= BORDER_WIDTH &&
        rect.bottom() - pos.y() <= BORDER_WIDTH)
    {
        resizeRegion = BottomRight;
        resizing = true;
    }
    else if (rect.right() - pos.x() <= BORDER_WIDTH)
    {
        resizeRegion = Right;
        resizing = true;
    }
    else if (rect.bottom() - pos.y() <= BORDER_WIDTH)
    {
        resizeRegion = Bottom;
        resizing = true;
    }

    if (!resizing && event->button() == Qt::LeftButton)
    {
        dragging = true;
        dragPos = event->globalPos() - frameGeometry().topLeft();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    QPoint localPos = event->pos();
    QRect rect = this->rect();
    QRect geo = geometry();

    int winW = static_cast<int>(screenW * scaleRate);
    int winH = winW * 9 / 16;
    if (winH > static_cast<int>(screenH * scaleRate))
    {
        winH = static_cast<int>(screenH * scaleRate);
        winW = winH * 16 / 9;
    }

    if (rect.right() - localPos.x() <= BORDER_WIDTH &&
        rect.bottom() - localPos.y() <= BORDER_WIDTH)
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if (rect.right() - localPos.x() <= BORDER_WIDTH)
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if (rect.bottom() - localPos.y() <= BORDER_WIDTH)
    {
        setCursor(Qt::SizeVerCursor);
    }
    else
    {
        setCursor(Qt::ArrowCursor);
    }

    if (resizing)
    {
        int newW = geo.width();
        int newH = geo.height();

        if (resizeRegion == Right || resizeRegion == BottomRight)
            newW = qMax(qMax(winW, localPos.x()), winW);

        newH = newW * 9 / 16;

        geo.setWidth(newW);
        geo.setHeight(newH);
        setGeometry(geo);
    }

    if (dragging && (event->buttons() & Qt::LeftButton))
    {
        move(event->globalPos() - dragPos);
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    dragging = false;
    resizing = false;
    resizeRegion = None;
    setCursor(Qt::ArrowCursor);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPainterPath clipPath;
    clipPath.addRoundedRect(rect(), 40, 40);
    painter.setClipPath(clipPath);

    painter.fillRect(rect(), QColor(26, 54, 120));

    QRect sideRect(0, 0, 500, height());
    painter.drawPixmap(sideRect, scaledSidePixmap);

    QWidget::paintEvent(event);
}

void MainWindow::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText)
{
    QDialog *dialog = new QDialog(parent, Qt::FramelessWindowHint);
    dialog->setFixedSize(400, 220);

    dialog->setStyleSheet(R"(
                          QDialog {
                          background-color: #4774CF;
                          border: none;
                          }
                          )");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 10);
    layout->setSpacing(0);

    QLabel *titleLabel = new QLabel(titleText, dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
                              background-color: #1A3678;
                              color: white;
                              font-family: "Times New Roman";
                              font-size: 16pt;
                              font-weight: bold;
                              padding: 10px;
                              border: none;
                              )");

    QLabel *messageLabel = new QLabel(messageText, dialog);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet(R"(
                                background-color: rgba(0, 0, 0, 0);
                                color: white;
                                font-family: "Times New Roman";
                                font-size: 14pt;
                                padding: 14px;
                                border: none;
                                )");

    QPushButton *okButton = new QPushButton("OK", dialog);
    okButton->setStyleSheet(R"(
                            QPushButton {
                            background-color: #B04248;
                            color: white;
                            font-family: "Times New Roman";
                            font-weight: bold;
                            font-size: 12pt;
                            border-radius: 8px;
                            padding: 6px 12px;
                            border: none;
                            }
                            QPushButton:hover {
                            background-color: #C85B5F;
                            }
                            )");
    connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    dialog->exec();
}

void MainWindow::cleanupProcess()
{
    if (process)
    {
        if (process->state() == QProcess::Running)
        {
            process->terminate();
            if (!process->waitForFinished(3000))
                process->kill();
            process->waitForFinished();
        }
        delete process;
        process = nullptr;
    }
}

void MainWindow::openChangePasswd(const QString &username)
{
    ChangePasswd *chg = new ChangePasswd(username);
    chg->resize(960, 450);
    chg->setWindowFlags(Qt::FramelessWindowHint);
    chg->setAttribute(Qt::WA_TranslucentBackground);
    chg->show();
}

void MainWindow::updateRightContent(const QString &menuName, const QString &panelType)
{
    QWidget *widget = rightContentMap.value(menuName).value(panelType, nullptr);
    if (widget)
    {
        rightStackedContent->setCurrentWidget(widget);
    }
}

QPlainTextEdit *MainWindow::getTerminalOutput() const
{
    return terminalOutput;
}
