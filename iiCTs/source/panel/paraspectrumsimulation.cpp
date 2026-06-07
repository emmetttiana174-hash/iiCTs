#include "paraspectrumsimulation.h"

#include "mainwindow.h"
#include "projectinfo.h"
#include "chatpanel.h"
#include "paramfilemanager.h"
#include "parahome.h"
#include "transspectrum.h"
#include "terminalwidget.h"
#include "filedialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QResizeEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QDirIterator>
#include <QProxyStyle>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>

ParaSpectrumSimulation::ParaSpectrumSimulation(QWidget *parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: transparent;");

    valueParams = {
        {"AnodeAngle", -1, "°"},
        {"TubeVoltage", -1, "KeV"},
        {"ElectronCount", -1, ""},
        {"EnergyBinSize", -1, "KeV"},
        {"TargetMode", -1, ""},
        {"DetThickness", -1, "mm"},
        {"DetSizeX", -1, "mm"},
        {"GridSizeX", -1, "mm"},
        {"DetSizeY", -1, "mm"},
        {"GridSizeY", -1, "mm"},
        {"Filter1Thickness", -1, "mm"},
        {"Filter1Position", -1, "mm"},
        {"Filter2Thickness", -1, "mm"},
        {"Filter2Position", -1, "mm"},
    };

    fileParams = {
        {"OutputFilename", "-1"}, {"AnodeMaterial", "-1"}, {"DetMaterial", "-1"}, {"Filter1Material", "-1"}, {"InputFilePath", "/home/"}, {"OutputFolder", "/home/"}};

    const QStringList fileSelectionKeys = {"InputFilePath"};
    const QStringList floderSelectionKeys = {"OutputFolder"};
    const QStringList materialKeys = {"AnodeMaterial", "CathodeMaterial", "FocusMaterial"};

    userFileDir = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.SpectrumFolder;
    systemFileDir = allInfo.SystemScriptPath + "/spectrum";

    QString fileTitleName = "Template File:";

    TargetCombo = new QComboBox;
    TargetCombo->setStyleSheet(R"(
                               QComboBox, QComboBox:on {
                               font-family: "Times New Roman";
                               font-size: 18px;
                               font-weight: bold;
                               color: white;
                               border: 2px solid #315AB2;
                               border-radius: 2px;
                               padding: 0;
                               background-color: #3366CC;
                               }
                               QComboBox::drop-down {
                               width: 25px;
                               }

                               QComboBox QAbstractItemView {
                               color: white;
                               border: 2px solid #2F528F;
                               background-color: #588BF4;
                               selection-background-color: #CCE4F9;
                               selection-color: white;
                               border-radius: 0px;
                               outline: none;
                               }
                               )");

    TargetCombo->setMinimumHeight(40);
    TargetCombo->setMaximumWidth(300);
    TargetCombo->setMinimumWidth(250);
    TargetCombo->setEditable(true);
    TargetCombo->setInsertPolicy(QComboBox::NoInsert);
    TargetCombo->lineEdit()->setReadOnly(true);
    TargetCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    TargetCombo->lineEdit()->setPlaceholderText("Please select a script file");

    QLabel *promptLabel = new QLabel("Target Path:", this);
    const QString promptLabelStyle = R"(
                                     QLabel {
                                     color: #CEBAF6;
                                     font-weight: bold;
                                     background-color: transparent;
                                     font-family: 'Times New Roman';
                                     border: none;
                                     font-size: 18pt;
                                     }
                                     )";
    promptLabel->setStyleSheet(promptLabelStyle);

    populateTargetCombo();

    connect(TargetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int idx)
            {
        if (idx >= 0) {
            TargetPath = TargetCombo
                    ->itemData(idx, Qt::UserRole)
                    .toString();
            QString fn = QFileInfo(TargetPath).fileName();
            TargetCombo->lineEdit()->setText(fn);
            emit fileChanged(TargetPath);
            emit fileResultChanged(TargetPath);

            // 当选择Template File时，自动执行GWL转换 - 直接传递索引避免时序问题
            convertSelectedGWLFileWithIndex(idx);
        }
        else {
            TargetPath.clear();
            TargetCombo->lineEdit()->clear();
            TargetCombo->lineEdit()->setPlaceholderText("Please select a script file");
        } });

    QVBoxLayout *m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 10, 15, 20);

    QHBoxLayout *buttonRow = new QHBoxLayout;
    buttonRow->setSpacing(10);
    buttonRow->setContentsMargins(10, 0, 10, 0);
    buttonRow->setAlignment(Qt::AlignCenter);
    buttonRow->addStretch();
    buttonRow->addWidget(promptLabel);
    buttonRow->addWidget(TargetCombo);

    QSize btnMainSize(150, 50);
    QString btnMainStyle = "QPushButton {"
                           "  border: none;"
                           "  border-radius: 15px;"
                           "font-family: 'Times New Roman';"
                           "font-size: 16px;"
                           "font-weight: bold;"
                           "background: qlineargradient("
                           "x1: 0, y1: 0, x2: 1, y2: 0,"
                           "stop: 0 #CEBAF6,"
                           "stop: 1 #41A1F5"
                           ");"
                           "  color: white;"
                           "  font-size: 24px;"
                           "  font-family: 'Times New Roman';"
                           "  font-weight: bold;"
                           "  padding: 8px 16px;"
                           "}"
                           "QPushButton:hover {"
                           "background: qlineargradient("
                           "x1: 0, y1: 0, x2: 1, y2: 0,"
                           "stop: 0 #83C9CD,"
                           "stop: 1 #4D85F1"
                           ");"
                           "  border-color: #48ADB2;"
                           "  border: none;"
                           "}"
                           "QPushButton:pressed {"
                           "  background-color: #5588CC;"
                           "}";

    QPushButton *buttonSim = new QPushButton("Simulation");
    buttonSim->setFixedSize(btnMainSize);
    buttonSim->setStyleSheet(btnMainStyle);
    buttonRow->addSpacing(50);
    buttonRow->addWidget(buttonSim);
    buttonRow->addStretch();

    connect(buttonSim, &QPushButton::clicked, this, [this]()
            { PlaneRunFunction(TargetPath, allInfo); });

    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(buttonRow);

    QHBoxLayout *topButtonsLayout = new QHBoxLayout;
    QString UIBtnStyle = R"(
                         QPushButton {
                         color: white;
                         background-color: #3366CC;
                         font-weight: bold;
                         border-top-left-radius: 15px;
                         border-bottom-left-radius: 15px;
                         border-top-right-radius: 0px;
                         border-bottom-right-radius: 0px;
                         border: 1px solid #2F528F;

                         }
                         QPushButton:hover {
                         background-color: #3366CC;
                         border: 1px solid #41A1F5;
                         }
                         QPushButton:checked {
                         background-color: #61A7F5;
                         border: 1px solid #41A1F5;
                         }
                         )";
    QString detailedBtnStyle = R"(
                               QPushButton {
                               color: white;
                               background-color: #3366CC;
                               font-weight: bold;
                               border: 1px solid #2F528F;
                               border-radius: 0px;
                               }
                               QPushButton:hover {
                               background-color: #3366CC;
                               border: 1px solid #41A1F5;
                               }
                               QPushButton:checked {
                               background-color: #61A7F5;
                               border: 1px solid #41A1F5;
                               }
                               )";
    QString AIBtnStyle = R"(
                         QPushButton {
                         color: white;
                         background-color: #3366CC;
                         font-weight: bold;
                         border-top-left-radius: 0px;
                         border-bottom-left-radius: 0px;
                         border-top-right-radius: 15px;
                         border-bottom-right-radius: 15px;
                         border: 1px solid #2F528F;
                         }
                         QPushButton:hover {
                         background-color: #3366CC;
                         border: 1px solid #41A1F5;
                         }
                         QPushButton:checked {
                         background-color: #61A7F5;
                         border: 1px solid #41A1F5;
                         }
                         )";
    QButtonGroup *toggleGroup = new QButtonGroup(this);
    toggleGroup->setExclusive(true);
    QPushButton *UIBtn = new QPushButton("UI");
    QPushButton *detailedBtn = new QPushButton("Det");
    QPushButton *AIBtn = new QPushButton("AI");
    QFont toggleFont("Times New Roman", 12);
    QList<QPushButton *> toggleButtons = {UIBtn, detailedBtn, AIBtn};
    for (auto btn : toggleButtons)
    {
        btn->setCheckable(true);
        btn->setFixedSize(40, 30);
        toggleGroup->addButton(btn);
        btn->setFont(toggleFont);
        topButtonsLayout->addWidget(btn);
    }
    UIBtn->setStyleSheet(UIBtnStyle);
    detailedBtn->setStyleSheet(detailedBtnStyle);
    AIBtn->setStyleSheet(AIBtnStyle);
    topButtonsLayout->setSpacing(0);
    topButtonsLayout->setContentsMargins(0, 0, 20, 0);
    UIBtn->setChecked(true);
    topButtonsLayout->setAlignment(Qt::AlignRight);
    m_mainLayout->insertLayout(0, topButtonsLayout);

    QStackedWidget *centralView = new QStackedWidget;
    centralView->setStyleSheet("background-color: transparent; border-radius: 12px;");

    QWidget *uiEditPanel = new QWidget;
    QWidget *deEditPanel = new QWidget;
    centralView->addWidget(uiEditPanel);
    centralView->addWidget(deEditPanel);
    m_mainLayout->insertWidget(1, centralView);

    fileUICombo = new QComboBox(uiEditPanel);
    QPushButton *saveUIBtn = new QPushButton("Save", uiEditPanel);
    QPushButton *saveAsUIBtn = new QPushButton("Save As", uiEditPanel);
    QWidget *UIControlPlane = new QWidget(uiEditPanel);

    uiEditPanel->setAutoFillBackground(false);
    uiEditPanel->setAttribute(Qt::WA_StyledBackground);
    uiEditPanel->setStyleSheet(R"(
                               background-color: transparent;
                               border: 2px dashed #7697D8;
                               font-family: "Times New Roman";
                               font-weight: bold;
                               border-radius: 10px;
                               )");

    QLabel *spectUILabel = new QLabel(fileTitleName, uiEditPanel);
    spectUILabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    const QString kSpectUILabelStyle = R"(
                                       QLabel {
                                       color: white;
                                       background-color: transparent;
                                       font-family: 'Times New Roman';
                                       border: none;
                                       font-size: 20pt;
                                       }
                                       )";
    spectUILabel->setStyleSheet(kSpectUILabelStyle);
    spectUILabel->setFixedHeight(40);
    fileUICombo->setFixedHeight(40);
    saveUIBtn->setFixedSize(60, 40);
    saveAsUIBtn->setFixedSize(90, 40);
    QString buttonUIStyle = R"(
                            QPushButton {
                            color: white;
                            font-size: 16pt;
                            background-color: #3366CC;
                            font-weight: bold;
                            border-radius: 10px;
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
    saveUIBtn->setStyleSheet(buttonUIStyle);
    saveAsUIBtn->setStyleSheet(buttonUIStyle);

    UIControlPlane->setAttribute(Qt::WA_StyledBackground);
    UIControlPlane->setStyleSheet(R"(
                                  background-color: transparent;
                                  border: none;
                                  font-family: "Times New Roman";
                                  font-weight: bold;
                                  border-radius: 10px;
                                  )");

    auto *UIControlLayout = new QVBoxLayout(UIControlPlane);
    UIControlLayout->setContentsMargins(0, 0, 0, 0);
    UIControlLayout->setSpacing(5);

    QString UIControlPlaneStyleEdit = R"(
                                      QLineEdit {
                                      font-family: "Times New Roman";
                                      color: white;
                                      background-color: #1A3678;
                                      border: 1px solid #284FA5;
                                      border-radius: 0px;
                                      padding: 2px;
                                      }
                                      QLineEdit:hover {
                                      background-color: #3366CC;
                                      border-color: #7697D8;
                                      }
                                      )";
    QString UIControlPlaneStyleString = R"(

                                        QLabel {
                                        color: white;
                                        background-color: transparent;
                                        border: none;
                                        }
                                        )";
    QFont font("Times New Roman");
    font.setPointSize(16);

    QWidget *editContainer = new QWidget(UIControlPlane);
    editContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editContainer->setStyleSheet(R"(
                                 QWidget {
                                 background-color: #22489E;
                                 border: 1px solid #7697D8;
                                 border-radius: 12px;
                                 }
                                 )");

    QGridLayout *grid = new QGridLayout(editContainer);
    //    grid->setHorizontalSpacing(10);
    //    grid->setVerticalSpacing(12);
    grid->setContentsMargins(10, 10, 10, 10);

    int row = 0, col = 0;
    for (const auto &vp : valueParams)
    {
        auto *hbox = new QHBoxLayout;
        hbox->setSpacing(3);

        QLabel *lblName = new QLabel(vp.name + ":", editContainer);
        lblName->setFixedWidth(180);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLineEdit *edit = new QLineEdit(QString::number(vp.defaultValue), editContainer);
        edit->setFixedWidth(60);
        edit->setStyleSheet(UIControlPlaneStyleEdit);
        edit->setFont(font);
        edit->setAlignment(Qt::AlignCenter);
        valueEdits.insert(vp.name, edit);
        edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLabel *lblUnit = new QLabel(vp.unit, editContainer);
        lblUnit->setFixedWidth(40);
        lblUnit->setStyleSheet(UIControlPlaneStyleString);
        lblUnit->setFont(font);
        lblUnit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lblUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        hbox->addStretch();
        hbox->addWidget(lblName);
        hbox->addWidget(edit);
        hbox->addWidget(lblUnit);
        hbox->addStretch();

        grid->addLayout(hbox, row, col);

        col++;
        if (col == 2)
        {
            col = 0;
            row++;
        }
    }

    if (col == 1)
    {
        grid->addItem(new QSpacerItem(1, 1), row, 1);
        row++;
    }

    QWidget *filePathContainer = new QWidget(UIControlPlane);
    filePathContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filePathContainer->setStyleSheet(R"(
                                     QWidget {
                                     background-color: #22489E;
                                     border: 1px solid #7697D8;
                                     border-radius: 12px;
                                     }
                                     )");

    QString BrowseStyleString = R"(
                                QPushButton {
                                border: 1px solid #2F528F;
                                border-radius: 6px;
                                background-color: #3366CC;
                                color: white;
                                font-size: 22px;
                                font-weight: bold;
                                font-family: "Times New Roman";
                                }
                                QPushButton:hover {
                                background-color: #3366CC;
                                border: 1px solid #41A1F5;
                                }
                                QPushButton:pressed {
                                background-color: #61A7F5;
                                border: 1px solid #41A1F5;
                                }
                                )";

    QGridLayout *gridFile = new QGridLayout(filePathContainer);
    gridFile->setContentsMargins(10, 10, 10, 10);

    row = 0;
    col = 0;
    for (const auto &fp : fileParams)
    {
        QLabel *lblName = new QLabel(fp.name + ":", filePathContainer);
        lblName->setFixedWidth(180);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLineEdit *pathEdit = new QLineEdit(fp.filePath, filePathContainer);
        pathEdit->setStyleSheet(UIControlPlaneStyleEdit);
        pathEdit->setFont(font);
        pathEdit->setFrame(true);
        pathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        fileEdits.insert(fp.name, pathEdit);

        QWidget *selectorWidget = nullptr;
        if (fileSelectionKeys.contains(fp.name))
        {
            QPushButton *btn = new QPushButton("...", filePathContainer);
            btn->setFixedSize(40, 40);
            btn->setToolTip("Browse...");
            btn->setStyleSheet(BrowseStyleString);
            connect(btn, &QPushButton::clicked, this, [=]()
                    {
                QString filePath = FileDialog::getOpenFileName(
                            UIControlPlane,
                            "Select " + fp.name,
                            QDir::homePath(),
                            "All Files (*)"
                            );
                if (!filePath.isEmpty())
                    pathEdit->setText(filePath); });
            selectorWidget = btn;
            pathEdit->setMinimumWidth(200);
        }
        else if (floderSelectionKeys.contains(fp.name))
        {
            QPushButton *btn = new QPushButton("...", filePathContainer);
            btn->setFixedSize(40, 40);
            btn->setToolTip("Browse...");
            btn->setStyleSheet(BrowseStyleString);
            connect(btn, &QPushButton::clicked, this, [=]()
                    {
                QString dirPath = FileDialog::getExistingDirectory(
                            UIControlPlane,
                            "Select " + fp.name,
                            QDir::homePath(),
                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                            );
                if (!dirPath.isEmpty())
                    pathEdit->setText(dirPath); });
            selectorWidget = btn;
            pathEdit->setMinimumWidth(200);
        }
        else
        {
            QWidget *placeholder = new QWidget(filePathContainer);
            placeholder->setFixedSize(40, 40);
            placeholder->setStyleSheet("background: transparent; border: none;");
            selectorWidget = placeholder;
            pathEdit->setFixedWidth(103);
        }

        selectorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

        bool isFullRow = fileSelectionKeys.contains(fp.name) || floderSelectionKeys.contains(fp.name);

        if (isFullRow)
        {
            QHBoxLayout *hbox = new QHBoxLayout;
            hbox->setSpacing(3);
            hbox->addWidget(lblName);
            hbox->addWidget(pathEdit);
            hbox->addWidget(selectorWidget);
            hbox->addSpacing(20);
            if (col == 1)
            {
                gridFile->addItem(
                    new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum),
                    row, 1);
                row++;
                col = 0;
            }
            gridFile->addLayout(hbox, row, 0, 1, 2);
            row++;
            col = 0;
        }
        else
        {
            QHBoxLayout *hbox = new QHBoxLayout;
            hbox->addStretch();
            hbox->addWidget(lblName);
            hbox->addWidget(pathEdit);
            hbox->addStretch();
            gridFile->addLayout(hbox, row, col);
            col++;
            if (col == 2)
            {
                col = 0;
                row++;
            }
        }
    }

    if (col == 1)
    {
        gridFile->addItem(
            new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum),
            row, 1);
    }

    QWidget *editTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout *editTitleLayout = new QVBoxLayout(editTitleContainer);
    editTitleLayout->setContentsMargins(5, 0, 0, 0);
    QLabel *editTitleLabel = new QLabel("Parameter Settings");
    editTitleLabel->setStyleSheet(kSpectUILabelStyle);
    editTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    editTitleLayout->addWidget(editTitleLabel);

    QWidget *pathTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout *pathTitleLayout = new QVBoxLayout(pathTitleContainer);
    pathTitleLayout->setContentsMargins(5, 0, 0, 0);
    QLabel *pathTitleLabel = new QLabel("Path Settings");
    pathTitleLabel->setStyleSheet(kSpectUILabelStyle);
    pathTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pathTitleLayout->addWidget(pathTitleLabel);

    UIControlPlane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    editContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    filePathContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    //    UIControlLayout->addStretch();
    UIControlLayout->addSpacing(2);
    UIControlLayout->addWidget(editTitleContainer);
    UIControlLayout->addWidget(editContainer);
    //    UIControlLayout->addStretch();
    UIControlLayout->addSpacing(2);
    UIControlLayout->addWidget(pathTitleContainer);
    UIControlLayout->addWidget(filePathContainer);
    UIControlLayout->addStretch();

    fileUICombo->setStyleSheet(R"(
                               QComboBox, QComboBox:on {
                               font-family: "Times New Roman";
                               font-size: 18px;
                               color: white;
                               border: 2px solid #21438F;
                               border-radius: 2px;
                               padding: 0;
                               background-color: #588BF4;
                               }
                               QComboBox::drop-down {
                               width: 25px;
                               }

                               QComboBox QAbstractItemView {
                               color: white;
                               border: 2px solid #2F528F;
                               background-color: #588BF4;
                               selection-background-color: #CCE4F9;
                               selection-color: white;
                               border-radius: 0px;
                               outline: none;
                               }
                               )");
    fileUICombo->setMaximumWidth(300);
    fileUICombo->setMinimumWidth(250);

    auto *uiEditTitleLayout = new QHBoxLayout;
    uiEditTitleLayout->setSpacing(0);
    uiEditTitleLayout->setContentsMargins(5, 0, 5, 0);
    uiEditTitleLayout->addWidget(spectUILabel);
    uiEditTitleLayout->addWidget(fileUICombo);
    uiEditTitleLayout->addStretch();
    uiEditTitleLayout->addWidget(saveUIBtn);
    uiEditTitleLayout->addSpacing(10);
    uiEditTitleLayout->addWidget(saveAsUIBtn);

    auto *uiEditLayout = new QVBoxLayout(uiEditPanel);
    uiEditLayout->setContentsMargins(10, 10, 10, 10);
    uiEditLayout->setSpacing(10);
    uiEditLayout->addLayout(uiEditTitleLayout);
    uiEditLayout->addWidget(UIControlPlane);
    uiEditPanel->setLayout(uiEditLayout);

    fileUICombo->setEditable(true);
    fileUICombo->lineEdit()->setReadOnly(true);
    fileUICombo->lineEdit()->setPlaceholderText("Please select a template");

    populateUICombo();

    fileCombo = new QComboBox(deEditPanel);
    QPushButton *saveBtn = new QPushButton("Save", deEditPanel);
    QPushButton *saveAsBtn = new QPushButton("Save As", deEditPanel);
    QTextEdit *editor = new QTextEdit(deEditPanel);

    QLabel *spectLabel = new QLabel(fileTitleName, deEditPanel);
    spectLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    spectLabel->setStyleSheet(kSpectUILabelStyle);
    spectLabel->setFixedHeight(40);
    fileCombo->setFixedHeight(40);
    saveBtn->setFixedSize(60, 40);
    saveAsBtn->setFixedSize(90, 40);
    QString buttonStyle = R"(
                          QPushButton {
                          color: white;
                          font-size: 16pt;
                          background-color: #3366CC;
                          font-weight: bold;
                          border-radius: 10px;
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
    saveBtn->setStyleSheet(buttonStyle);
    saveAsBtn->setStyleSheet(buttonStyle);

    deEditPanel->setAutoFillBackground(false);
    deEditPanel->setAttribute(Qt::WA_StyledBackground);
    deEditPanel->setStyleSheet(R"(
                               background-color: transparent;
                               border: 2px dashed #7697D8;
                               font-family: "Times New Roman";
                               font-weight: bold;
                               border-radius: 10px;
                               )");

    editor->setStyleSheet(R"(
                          QTextEdit {
                          color: yellow;
                          background-color: #1A3678;
                          border: 5px solid #21438F;
                          font-size: 18px;
                          font-family: "Times New Roman";
                          border-radius: 10px;
                          padding: 8px;
                          }

                          QScrollBar:vertical {
                          background: #1A3678;
                          width: 20px;
                          margin: 0px;
                          border: 2px solid #3961B4;
                          border-radius: 5px;
                          }

                          QScrollBar::handle:vertical {
                          margin: 3px;
                          background: #3961B4;
                          border-radius: 5px;
                          min-height: 20px;
                          }

                          QScrollBar::add-line:vertical,
                          QScrollBar::sub-line:vertical {
                          background: none;
                          border: none;
                          }

                          QScrollBar::add-page:vertical,
                          QScrollBar::sub-page:vertical {
                          background: none;
                          }
                          )");

    editor->setPlaceholderText(
        QStringLiteral(
            "Welcome to Integrated Intelligent CT Simulation System.\n"
            "If you encounter any problems, please contact us.\n"
            "Now, please select a template file from the dropdown above and modify it."));

    fileCombo->setStyleSheet(R"(
                             QComboBox, QComboBox:on {
                             font-family: "Times New Roman";
                             font-size: 18px;
                             color: white;
                             border: 2px solid #21438F;
                             border-radius: 2px;
                             padding: 0;
                             background-color: #588BF4;
                             }
                             QComboBox::drop-down {
                             width: 25px;
                             }

                             QComboBox QAbstractItemView {
                             color: white;
                             border: 2px solid #2F528F;
                             background-color: #588BF4;
                             selection-background-color: #CCE4F9;
                             selection-color: white;
                             border-radius: 0px;
                             outline: none;
                             }
                             )");
    fileCombo->setMaximumWidth(300);
    fileCombo->setMinimumWidth(250);

    auto *deEditTitleLayout = new QHBoxLayout;
    deEditTitleLayout->setSpacing(0);
    deEditTitleLayout->setContentsMargins(5, 0, 5, 10);
    deEditTitleLayout->addWidget(spectLabel);
    deEditTitleLayout->addWidget(fileCombo);
    deEditTitleLayout->addStretch();
    deEditTitleLayout->addWidget(saveBtn);
    deEditTitleLayout->addSpacing(10);
    deEditTitleLayout->addWidget(saveAsBtn);

    auto *deEditLayout = new QVBoxLayout(deEditPanel);
    deEditLayout->setContentsMargins(10, 10, 10, 10);
    deEditLayout->setSpacing(10);
    deEditLayout->addLayout(deEditTitleLayout);
    deEditLayout->addWidget(editor);
    deEditPanel->setLayout(deEditLayout);
    fileCombo->setEditable(true);
    fileCombo->lineEdit()->setReadOnly(true);
    fileCombo->lineEdit()->setPlaceholderText("Please select a template");

    populateCombo();

    connect(fileUICombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int idx)
            {
        if (idx < 0) return;
        QString path   = fileUICombo->itemData(idx, Qt::UserRole).toString();
        QFile   file(path);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            //                    showStyledWarning(this, "Open File Error", "Cannot open file:"+path);
            return;
        }

        QTextStream in(&file);
        in.setAutoDetectUnicode(true);
        in.setCodec("UTF-8");
        fileContentList.clear();
        while (!in.atEnd()) {
            fileContentList << in.readLine();
        }
        file.close();
        emit fileChanged(path); });

    connect(saveUIBtn, &QPushButton::clicked, this, [=]()
            {
        int idx = fileUICombo->currentIndex();
        if (idx < 0) return;

        QString src = fileUICombo->itemData(idx, Qt::UserRole + 1).toString();
        QString origPath = fileUICombo->itemData(idx, Qt::UserRole).toString();
        QString targetPath = origPath;

        if (src == "system") {
            QString defaultName = QFileInfo(origPath).baseName() + "_edit";

            QDialog* dialog = new QDialog(deEditPanel, Qt::FramelessWindowHint);
            dialog->setFixedSize(400, 240);
            dialog->setStyleSheet(R"(
                                  QDialog {
                                  border: 1px solid #21438F;
                                  border-radius: 0px;
                                  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                  stop:0 #22489E, stop:1 #4875D0);
                                  }
                                  )");

            QVBoxLayout* uiEditDialogLayout = new QVBoxLayout(dialog);
            uiEditDialogLayout->setContentsMargins(0, 0, 0, 20);
            uiEditDialogLayout->setSpacing(0);

            QLabel* deEditDialogtitle = new QLabel("Save Edited File", dialog);
            deEditDialogtitle->setAlignment(Qt::AlignCenter);
            deEditDialogtitle->setStyleSheet(R"(
                                             color: white;
                                             background-color: #1A3678;
                                             height:300;
                                             border:none;
                                             border-radius: 0px;

                                             font-family: "Times New Roman";
                                             font-size: 28pt;
                                             font-weight: bold;
                                             )");
            deEditDialogtitle->setContentsMargins(0, 20, 0, 20);
            uiEditDialogLayout->addWidget(deEditDialogtitle);

            QWidget* deEditDialoglabelContainer = new QWidget(dialog);
            QHBoxLayout* deEditDialoglabelLayout = new QHBoxLayout(deEditDialoglabelContainer);
            deEditDialoglabelLayout->setContentsMargins(30, 0, 30, 0);
            deEditDialoglabelContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
            deEditDialoglabelLayout->setSpacing(0);

            QLabel* deEditDialoglabel = new QLabel("Enter filename to save:", deEditDialoglabelContainer);
            deEditDialoglabel->setStyleSheet(R"(color: white; font-size: 22pt; border:none; border-radius: none; background-color: none;)");
            deEditDialoglabelLayout->addWidget(deEditDialoglabel);

            uiEditDialogLayout->addStretch();
            uiEditDialogLayout->addWidget(deEditDialoglabelContainer);


            QWidget* deEditDialogEditContainer = new QWidget(dialog);
            QHBoxLayout* deEditDialogEditLayout = new QHBoxLayout(deEditDialogEditContainer);
            deEditDialogEditContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
            deEditDialogEditLayout->setSpacing(0);

            QLineEdit* deEditDialogInput = new QLineEdit(deEditDialogEditContainer);
            deEditDialogInput->setContentsMargins(20, 0, 20, 0);
            deEditDialogInput->setText(defaultName);
            QString editBoxStyle = "QLineEdit {"
                                   "min-height: 30px;"
                                   "  border: 2px solid #1A3678;"
                                   "  border-radius: 5px;"
                                   "  padding: 5px;"
                                   "  background-color: #F0F8FF;"
                                   "  color: #000000;"
                                   "  font-size: 18px;"
                                   "  font-weight: bold;"
                                   "}"
                                   "QLineEdit:hover {"
                                   "background-color: #FFF2C9;"
                                   "}";
            deEditDialogInput->setStyleSheet(editBoxStyle);
            deEditDialogEditLayout->addWidget(deEditDialogInput);

            uiEditDialogLayout->addWidget(deEditDialogEditContainer);

            QHBoxLayout* btnLayout = new QHBoxLayout;
            btnLayout->setSpacing(20);

            QPushButton* confirm = new QPushButton("Confirm", dialog);
            QPushButton* cancel = new QPushButton("Cancel", dialog);

            int buttonWidth = 120;
            int buttonHeight = 40;

            confirm->setFixedSize(buttonWidth, buttonHeight);
            cancel->setFixedSize(buttonWidth, buttonHeight);

            cancel->setStyleSheet(R"(
                                  QPushButton {
                                  background-color: #B04248;
                                  color: white;
                                  font-size: 12pt;
                                  font-weight: bold;
                                  font-family: "Times New Roman";
                                  border-radius: 8px;
                                  padding: 6px 12px;
                                  border: none;
                                  }
                                  QPushButton:hover {
                                  background-color: #C85B5F;
                                  }
                                  )");

            confirm->setStyleSheet(R"(
                                   QPushButton {
                                   background-color: #2F674C;
                                   color: white;
                                   font-size: 12pt;
                                   font-weight: bold;
                                   font-family: "Times New Roman";
                                   border-radius: 8px;
                                   padding: 6px 12px;
                                   border: none;
                                   }
                                   QPushButton:hover {
                                   background-color: #4FAB7F;
                                   }
                                   )");

            btnLayout->addWidget(confirm);
            btnLayout->addWidget(cancel);

            connect(confirm, &QPushButton::clicked, dialog, &QDialog::accept);
            connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

            uiEditDialogLayout->addStretch();
            uiEditDialogLayout->addLayout(btnLayout);

            if (dialog->exec() == QDialog::Accepted) {
                QString fileName = deEditDialogInput->text().trimmed();
                if (!fileName.isEmpty()) {
                    QString targetPath = QDir(userFileDir).filePath(fileName + ".gwl");

                    QFile file(targetPath);
                    QIODevice::OpenMode flags = QIODevice::WriteOnly
                            | QIODevice::Truncate
                            | QIODevice::Text;
                    if (!file.open(flags)) {
                        QString err = file.errorString();
                        showStyledWarning(this,
                                          "Open File Error",
                                          QString("Cannot open file:\n%1\n%2").arg(targetPath, err));
                        return;
                    }

                    QTextStream out(&file);
                    out.setCodec("UTF-8");
                    for (const QString &line : fileContentList) {
                        out << line << "\n";
                    }
                    file.close();

                    {
                        QSignalBlocker b(fileUICombo);
                        populateUICombo();
                    }

                    {
                        QSignalBlocker b2(fileCombo);
                        populateCombo();
                    }

                    {
                        QSignalBlocker b2(TargetCombo);
                        populateTargetCombo();
                    }

                    for (int i = 0; i < fileUICombo->count(); ++i) {
                        if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                            fileUICombo->setCurrentIndex(i);
                            break;
                        }
                    }
                    for (int i = 0; i < fileCombo->count(); ++i) {
                        if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                            fileCombo->setCurrentIndex(i);
                            break;
                        }
                    }
                    emit fileSaved(targetPath);
                }
            }
        } else {

            QFile file(targetPath);
            QIODevice::OpenMode flags = QIODevice::WriteOnly
                    | QIODevice::Truncate
                    | QIODevice::Text;
            if (!file.open(flags)) {
                QString err = file.errorString();
                showStyledWarning(this,
                                  "Open File Error",
                                  QString("Cannot open file:\n%1\n%2").arg(targetPath, err));
                return;
            }

            QTextStream out(&file);
            out.setCodec("UTF-8");
            for (const QString &line : fileContentList) {
                out << line << "\n";
            }
            file.close();


            {
                QSignalBlocker b(fileUICombo);
                populateUICombo();
            }

            {
                QSignalBlocker b2(fileCombo);
                populateCombo();
            }

            {
                QSignalBlocker b2(TargetCombo);
                populateTargetCombo();
            }

            for (int i = 0; i < fileUICombo->count(); ++i) {
                if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                    fileUICombo->setCurrentIndex(i);
                    break;
                }
            }
            for (int i = 0; i < fileCombo->count(); ++i) {
                if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                    fileCombo->setCurrentIndex(i);
                    break;
                }
            }
            emit fileSaved(targetPath);
        } });

    connect(saveAsUIBtn, &QPushButton::clicked, this, [=]()
            {
        int idx = fileUICombo->currentIndex();
        if (idx < 0) return;
        QString orig = fileUICombo->itemData(idx, Qt::UserRole).toString();
        QString defaultName = QFileInfo(orig).baseName() + "_copy";

        QDialog* dialog = new QDialog(deEditPanel, Qt::FramelessWindowHint);
        dialog->setFixedSize(400, 240);
        dialog->setStyleSheet(R"(
                              QDialog {
                              border: 1px solid #21438F;
                              border-radius: 0px;
                              background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                              stop:0 #22489E, stop:1 #4875D0);
                              }
                              )");

        QVBoxLayout* uiEditDialogLayout = new QVBoxLayout(dialog);
        uiEditDialogLayout->setContentsMargins(0, 0, 0, 20);
        uiEditDialogLayout->setSpacing(0);

        QLabel* deEditDialogtitle = new QLabel("Save As New File", dialog);
        deEditDialogtitle->setAlignment(Qt::AlignCenter);
        deEditDialogtitle->setStyleSheet(R"(
                                         color: white;
                                         background-color: #1A3678;
                                         height:300;
                                         border:none;
                                         border-radius: 0px;

                                         font-family: "Times New Roman";
                                         font-size: 28pt;
                                         font-weight: bold;
                                         )");
        deEditDialogtitle->setContentsMargins(0, 20, 0, 20);
        uiEditDialogLayout->addWidget(deEditDialogtitle);

        QWidget* deEditDialoglabelContainer = new QWidget(dialog);
        QHBoxLayout* deEditDialoglabelLayout = new QHBoxLayout(deEditDialoglabelContainer);
        deEditDialoglabelLayout->setContentsMargins(30, 0, 30, 0);
        deEditDialoglabelContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
        deEditDialoglabelLayout->setSpacing(0);

        QLabel* deEditDialoglabel = new QLabel("Enter filename to save:", deEditDialoglabelContainer);
        deEditDialoglabel->setStyleSheet(R"(color: white; font-size: 22pt; border:none; border-radius: none; background-color: none;)");
        deEditDialoglabelLayout->addWidget(deEditDialoglabel);

        uiEditDialogLayout->addStretch();
        uiEditDialogLayout->addWidget(deEditDialoglabelContainer);


        QWidget* deEditDialogEditContainer = new QWidget(dialog);
        QHBoxLayout* deEditDialogEditLayout = new QHBoxLayout(deEditDialogEditContainer);
        deEditDialogEditContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
        deEditDialogEditLayout->setSpacing(0);

        QLineEdit* deEditDialogInput = new QLineEdit(deEditDialogEditContainer);
        deEditDialogInput->setContentsMargins(20, 0, 20, 0);
        deEditDialogInput->setText(defaultName);
        QString editBoxStyle = "QLineEdit {"
                               "min-height: 30px;"
                               "  border: 2px solid #1A3678;"
                               "  border-radius: 5px;"
                               "  padding: 5px;"
                               "  background-color: #F0F8FF;"
                               "  color: #000000;"
                               "  font-size: 18px;"
                               "  font-weight: bold;"
                               "}"
                               "QLineEdit:hover {"
                               "background-color: #FFF2C9;"
                               "}";
        deEditDialogInput->setStyleSheet(editBoxStyle);
        deEditDialogEditLayout->addWidget(deEditDialogInput);

        uiEditDialogLayout->addWidget(deEditDialogEditContainer);

        QHBoxLayout* btnLayout = new QHBoxLayout;
        btnLayout->setSpacing(20);

        QPushButton* confirm = new QPushButton("Confirm", dialog);
        QPushButton* cancel = new QPushButton("Cancel", dialog);

        int buttonWidth = 120;
        int buttonHeight = 40;

        confirm->setFixedSize(buttonWidth, buttonHeight);
        cancel->setFixedSize(buttonWidth, buttonHeight);

        cancel->setStyleSheet(R"(
                              QPushButton {
                              background-color: #B04248;
                              color: white;
                              font-size: 12pt;
                              font-weight: bold;
                              font-family: "Times New Roman";
                              border-radius: 8px;
                              padding: 6px 12px;
                              border: none;
                              }
                              QPushButton:hover {
                              background-color: #C85B5F;
                              }
                              )");

        confirm->setStyleSheet(R"(
                               QPushButton {
                               background-color: #2F674C;
                               color: white;
                               font-size: 12pt;
                               font-weight: bold;
                               font-family: "Times New Roman";
                               border-radius: 8px;
                               padding: 6px 12px;
                               border: none;
                               }
                               QPushButton:hover {
                               background-color: #4FAB7F;
                               }
                               )");

        btnLayout->addWidget(confirm);
        btnLayout->addWidget(cancel);

        connect(confirm, &QPushButton::clicked, dialog, &QDialog::accept);
        connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

        uiEditDialogLayout->addStretch();
        uiEditDialogLayout->addLayout(btnLayout);

        if (dialog->exec() == QDialog::Accepted) {
            QString fileName = deEditDialogInput->text().trimmed();
            if (!fileName.isEmpty()) {
                QString targetPath = QDir(userFileDir).filePath(fileName + ".gwl");

                QFile file(targetPath);
                QIODevice::OpenMode flags = QIODevice::WriteOnly
                        | QIODevice::Truncate
                        | QIODevice::Text;
                if (!file.open(flags)) {
                    QString err = file.errorString();
                    showStyledWarning(this,
                                      "Open File Error",
                                      QString("Cannot open file:\n%1\n%2").arg(targetPath, err));
                    return;
                }

                QTextStream out(&file);
                out.setCodec("UTF-8");
                for (const QString &line : fileContentList) {
                    out << line << "\n";
                }
                out.flush();
                file.close();

                {
                    QSignalBlocker b(fileUICombo);
                    populateUICombo();
                }

                {
                    QSignalBlocker b2(fileCombo);
                    populateCombo();
                }

                {
                    QSignalBlocker b2(TargetCombo);
                    populateTargetCombo();
                }

                for (int i = 0; i < fileUICombo->count(); ++i) {
                    if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                        fileUICombo->setCurrentIndex(i);
                        break;
                    }
                }
                for (int i = 0; i < fileCombo->count(); ++i) {
                    if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                        fileCombo->setCurrentIndex(i);
                        break;
                    }
                }
                emit fileSaved(targetPath);
            }
        } });

    connect(fileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int idx)
            {
        if (idx < 0) return;
        QString path   = fileCombo->itemData(idx, Qt::UserRole).toString();
        QFile   file(path);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            editor->setPlainText(QString::fromUtf8(file.readAll()));
        }
        emit fileChanged(path); });

    connect(saveBtn, &QPushButton::clicked, this, [=]()
            {
        int idx = fileCombo->currentIndex();
        if (idx < 0) return;

        QString src = fileCombo->itemData(idx, Qt::UserRole + 1).toString();
        QString origPath = fileCombo->itemData(idx, Qt::UserRole).toString();
        QString targetPath = origPath;

        if (src == "system") {
            QString defaultName = QFileInfo(origPath).baseName() + "_edit";

            QDialog* dialog = new QDialog(deEditPanel, Qt::FramelessWindowHint);
            dialog->setFixedSize(400, 240);
            dialog->setStyleSheet(R"(
                                  QDialog {
                                  border: 1px solid #21438F;
                                  border-radius: 0px;
                                  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                  stop:0 #22489E, stop:1 #4875D0);
                                  }
                                  )");

            QVBoxLayout* deEditDialogLayout = new QVBoxLayout(dialog);
            deEditDialogLayout->setContentsMargins(0, 0, 0, 20);
            deEditDialogLayout->setSpacing(0);

            QLabel* deEditDialogtitle = new QLabel("Save Edited File", dialog);
            deEditDialogtitle->setAlignment(Qt::AlignCenter);
            deEditDialogtitle->setStyleSheet(R"(
                                             color: white;
                                             background-color: #1A3678;
                                             height:300;
                                             border:none;
                                             border-radius: 0px;

                                             font-family: "Times New Roman";
                                             font-size: 28pt;
                                             font-weight: bold;
                                             )");
            deEditDialogtitle->setContentsMargins(0, 20, 0, 20);
            deEditDialogLayout->addWidget(deEditDialogtitle);

            QWidget* deEditDialoglabelContainer = new QWidget(dialog);
            QHBoxLayout* deEditDialoglabelLayout = new QHBoxLayout(deEditDialoglabelContainer);
            deEditDialoglabelLayout->setContentsMargins(30, 0, 30, 0);
            deEditDialoglabelContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
            deEditDialoglabelLayout->setSpacing(0);

            QLabel* deEditDialoglabel = new QLabel("Enter filename to save:", deEditDialoglabelContainer);
            deEditDialoglabel->setStyleSheet(R"(color: white; font-size: 22pt; border:none; border-radius: none; background-color: none;)");
            deEditDialoglabelLayout->addWidget(deEditDialoglabel);

            deEditDialogLayout->addStretch();
            deEditDialogLayout->addWidget(deEditDialoglabelContainer);


            QWidget* deEditDialogEditContainer = new QWidget(dialog);
            QHBoxLayout* deEditDialogEditLayout = new QHBoxLayout(deEditDialogEditContainer);
            deEditDialogEditContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
            deEditDialogEditLayout->setSpacing(0);

            QLineEdit* deEditDialogInput = new QLineEdit(deEditDialogEditContainer);
            deEditDialogInput->setContentsMargins(20, 0, 20, 0);
            deEditDialogInput->setText(defaultName);
            QString editBoxStyle = "QLineEdit {"
                                   "min-height: 30px;"
                                   "  border: 2px solid #1A3678;"
                                   "  border-radius: 5px;"
                                   "  padding: 5px;"
                                   "  background-color: #F0F8FF;"
                                   "  color: #000000;"
                                   "  font-size: 18px;"
                                   "  font-weight: bold;"
                                   "}"
                                   "QLineEdit:hover {"
                                   "background-color: #FFF2C9;"
                                   "}";
            deEditDialogInput->setStyleSheet(editBoxStyle);
            deEditDialogEditLayout->addWidget(deEditDialogInput);

            deEditDialogLayout->addWidget(deEditDialogEditContainer);

            QHBoxLayout* btnLayout = new QHBoxLayout;
            btnLayout->setSpacing(20);

            QPushButton* confirm = new QPushButton("Confirm", dialog);
            QPushButton* cancel = new QPushButton("Cancel", dialog);

            int buttonWidth = 120;
            int buttonHeight = 40;

            confirm->setFixedSize(buttonWidth, buttonHeight);
            cancel->setFixedSize(buttonWidth, buttonHeight);

            cancel->setStyleSheet(R"(
                                  QPushButton {
                                  background-color: #B04248;
                                  color: white;
                                  font-size: 12pt;
                                  font-weight: bold;
                                  font-family: "Times New Roman";
                                  border-radius: 8px;
                                  padding: 6px 12px;
                                  border: none;
                                  }
                                  QPushButton:hover {
                                  background-color: #C85B5F;
                                  }
                                  )");

            confirm->setStyleSheet(R"(
                                   QPushButton {
                                   background-color: #2F674C;
                                   color: white;
                                   font-size: 12pt;
                                   font-weight: bold;
                                   font-family: "Times New Roman";
                                   border-radius: 8px;
                                   padding: 6px 12px;
                                   border: none;
                                   }
                                   QPushButton:hover {
                                   background-color: #4FAB7F;
                                   }
                                   )");

            btnLayout->addWidget(confirm);
            btnLayout->addWidget(cancel);

            connect(confirm, &QPushButton::clicked, dialog, &QDialog::accept);
            connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

            deEditDialogLayout->addStretch();
            deEditDialogLayout->addLayout(btnLayout);

            if (dialog->exec() == QDialog::Accepted) {
                QString fileName = deEditDialogInput->text().trimmed();
                if (!fileName.isEmpty()) {
                    QString targetPath = QDir(userFileDir).filePath(fileName + ".gwl");

                    QFile file(targetPath);
                    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                        QTextStream stream(&file);
                        stream << editor->toPlainText();
                    }

                    {
                        QSignalBlocker b(fileUICombo);
                        populateUICombo();
                    }

                    {
                        QSignalBlocker b2(fileCombo);
                        populateCombo();
                    }

                    {
                        QSignalBlocker b2(TargetCombo);
                        populateTargetCombo();
                    }

                    for (int i = 0; i < fileUICombo->count(); ++i) {
                        if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                            fileUICombo->setCurrentIndex(i);
                            break;
                        }
                    }
                    for (int i = 0; i < fileCombo->count(); ++i) {
                        if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                            fileCombo->setCurrentIndex(i);
                            break;
                        }
                    }
                    emit fileSaved(targetPath);
                }
            }
        } else {
            QFile file(targetPath);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream << editor->toPlainText();
            }

            {
                QSignalBlocker b(fileUICombo);
                populateUICombo();
            }

            {
                QSignalBlocker b2(fileCombo);
                populateCombo();
            }

            {
                QSignalBlocker b2(TargetCombo);
                populateTargetCombo();
            }

            for (int i = 0; i < fileUICombo->count(); ++i) {
                if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                    fileUICombo->setCurrentIndex(i);
                    break;
                }
            }
            for (int i = 0; i < fileCombo->count(); ++i) {
                if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                    fileCombo->setCurrentIndex(i);
                    break;
                }
            }
            emit fileSaved(targetPath);
        } });

    connect(saveAsBtn, &QPushButton::clicked, this, [=]()
            {
        int idx = fileCombo->currentIndex();
        if (idx < 0) return;
        QString orig = fileCombo->itemData(idx, Qt::UserRole).toString();
        QString defaultName = QFileInfo(orig).baseName() + "_copy";

        QDialog* dialog = new QDialog(deEditPanel, Qt::FramelessWindowHint);
        dialog->setFixedSize(400, 240);
        dialog->setStyleSheet(R"(
                              QDialog {
                              border: 1px solid #21438F;
                              border-radius: 0px;
                              background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                              stop:0 #22489E, stop:1 #4875D0);
                              }
                              )");

        QVBoxLayout* deEditDialogLayout = new QVBoxLayout(dialog);
        deEditDialogLayout->setContentsMargins(0, 0, 0, 20);
        deEditDialogLayout->setSpacing(0);

        QLabel* deEditDialogtitle = new QLabel("Save As New File", dialog);
        deEditDialogtitle->setAlignment(Qt::AlignCenter);
        deEditDialogtitle->setStyleSheet(R"(
                                         color: white;
                                         background-color: #1A3678;
                                         height:300;
                                         border:none;
                                         border-radius: 0px;

                                         font-family: "Times New Roman";
                                         font-size: 28pt;
                                         font-weight: bold;
                                         )");
        deEditDialogtitle->setContentsMargins(0, 20, 0, 20);
        deEditDialogLayout->addWidget(deEditDialogtitle);

        QWidget* deEditDialoglabelContainer = new QWidget(dialog);
        QHBoxLayout* deEditDialoglabelLayout = new QHBoxLayout(deEditDialoglabelContainer);
        deEditDialoglabelLayout->setContentsMargins(30, 0, 30, 0);
        deEditDialoglabelContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
        deEditDialoglabelLayout->setSpacing(0);

        QLabel* deEditDialoglabel = new QLabel("Enter filename to save:", deEditDialoglabelContainer);
        deEditDialoglabel->setStyleSheet(R"(color: white; font-size: 22pt; border:none; border-radius: none; background-color: none;)");
        deEditDialoglabelLayout->addWidget(deEditDialoglabel);

        deEditDialogLayout->addStretch();
        deEditDialogLayout->addWidget(deEditDialoglabelContainer);


        QWidget* deEditDialogEditContainer = new QWidget(dialog);
        QHBoxLayout* deEditDialogEditLayout = new QHBoxLayout(deEditDialogEditContainer);
        deEditDialogEditContainer->setStyleSheet(R"(border:none; border-radius: none; background-color: none;)");
        deEditDialogEditLayout->setSpacing(0);

        QLineEdit* deEditDialogInput = new QLineEdit(deEditDialogEditContainer);
        deEditDialogInput->setContentsMargins(20, 0, 20, 0);
        deEditDialogInput->setText(defaultName);
        QString editBoxStyle = "QLineEdit {"
                               "min-height: 30px;"
                               "  border: 2px solid #1A3678;"
                               "  border-radius: 5px;"
                               "  padding: 5px;"
                               "  background-color: #F0F8FF;"
                               "  color: #000000;"
                               "  font-size: 18px;"
                               "  font-weight: bold;"
                               "}"
                               "QLineEdit:hover {"
                               "background-color: #FFF2C9;"
                               "}";
        deEditDialogInput->setStyleSheet(editBoxStyle);
        deEditDialogEditLayout->addWidget(deEditDialogInput);

        deEditDialogLayout->addWidget(deEditDialogEditContainer);

        QHBoxLayout* btnLayout = new QHBoxLayout;
        btnLayout->setSpacing(20);

        QPushButton* confirm = new QPushButton("Confirm", dialog);
        QPushButton* cancel = new QPushButton("Cancel", dialog);

        int buttonWidth = 120;
        int buttonHeight = 40;

        confirm->setFixedSize(buttonWidth, buttonHeight);
        cancel->setFixedSize(buttonWidth, buttonHeight);

        cancel->setStyleSheet(R"(
                              QPushButton {
                              background-color: #B04248;
                              color: white;
                              font-size: 12pt;
                              font-weight: bold;
                              font-family: "Times New Roman";
                              border-radius: 8px;
                              padding: 6px 12px;
                              border: none;
                              }
                              QPushButton:hover {
                              background-color: #C85B5F;
                              }
                              )");

        confirm->setStyleSheet(R"(
                               QPushButton {
                               background-color: #2F674C;
                               color: white;
                               font-size: 12pt;
                               font-weight: bold;
                               font-family: "Times New Roman";
                               border-radius: 8px;
                               padding: 6px 12px;
                               border: none;
                               }
                               QPushButton:hover {
                               background-color: #4FAB7F;
                               }
                               )");

        btnLayout->addWidget(confirm);
        btnLayout->addWidget(cancel);

        connect(confirm, &QPushButton::clicked, dialog, &QDialog::accept);
        connect(cancel, &QPushButton::clicked, dialog, &QDialog::reject);

        deEditDialogLayout->addStretch();
        deEditDialogLayout->addLayout(btnLayout);

        if (dialog->exec() == QDialog::Accepted) {
            QString fileName = deEditDialogInput->text().trimmed();
            if (!fileName.isEmpty()) {
                QString targetPath = QDir(userFileDir).filePath(fileName + ".gwl");

                QFile f(targetPath);
                if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream ts(&f);
                    ts << editor->toPlainText();
                }

                {
                    QSignalBlocker b(fileUICombo);
                    populateUICombo();
                }

                {
                    QSignalBlocker b2(fileCombo);
                    populateCombo();
                }

                {
                    QSignalBlocker b2(TargetCombo);
                    populateTargetCombo();
                }

                for (int i = 0; i < fileCombo->count(); ++i) {
                    if (fileCombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                        fileCombo->setCurrentIndex(i);
                        break;
                    }
                }
                for (int i = 0; i < fileUICombo->count(); ++i) {
                    if (fileUICombo->itemData(i, Qt::UserRole).toString() == targetPath) {
                        fileUICombo->setCurrentIndex(i);
                        break;
                    }
                }
                emit fileSaved(targetPath);
            }
        } });

    paramMgr = new ParamFileManager(
        fileUICombo,
        valueParams,
        fileParams,
        valueEdits,
        fileEdits,
        this);

    connect(paramMgr, &ParamFileManager::fileContentListChanged,
            this, [this](const QStringList &newList)
            { fileContentList = newList; });

    connect(this, &ParaSpectrumSimulation::fileSaved,
            paramMgr, &ParamFileManager::reloadCurrentFile);

    connect(UIBtn, &QPushButton::clicked, [=]()
            { centralView->setCurrentIndex(0); });
    connect(detailedBtn, &QPushButton::clicked, [=]()
            { centralView->setCurrentIndex(1); });
    connect(AIBtn, &QPushButton::clicked, this, [=]()
            { ChatPanel* chat = mainWindow->getChatPanel();

        if (centralView->indexOf(chat) < 0) {
            centralView->addWidget(chat);
        }
        centralView->setCurrentWidget(chat); });

    // 初始化GWL转换器
    gwlConverter = new TransSpectrum(this);
    connect(gwlConverter, &TransSpectrum::conversionCompleted,
            this, [this](const QString &message)
            { qDebug() << "GWL Conversion completed:" << message; });
    connect(gwlConverter, &TransSpectrum::conversionError,
            this, [this](const QString &error)
            { qDebug() << "GWL Conversion error:" << error; });
}

void ParaSpectrumSimulation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void ParaSpectrumSimulation::setMainWindow(MainWindow *main)
{
    mainWindow = main;
}

void ParaSpectrumSimulation::paintEvent(QPaintEvent *event)
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

void ParaSpectrumSimulation::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText)
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

void ParaSpectrumSimulation::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
    userFileDir = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.SpectrumFolder;
    systemFileDir = allInfo.SystemScriptPath + "/spectrum";

    {
        QSignalBlocker b(fileUICombo);
        populateUICombo();
    }

    {
        QSignalBlocker b2(fileCombo);
        populateCombo();
    }

    {
        QSignalBlocker b2(TargetCombo);
        populateTargetCombo();
    }
}

void ParaSpectrumSimulation::populateUICombo()
{
    fileUICombo->clear();
    auto scanDir = [&](const QString &dirPath, const QString &tag)
    {
        QDirIterator it(dirPath,
                        QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString path = it.next();
            QString base = QFileInfo(path).baseName();
            fileUICombo->addItem(QString("• %1 (%2)").arg(base, tag));
            int idx = fileUICombo->count() - 1;
            fileUICombo->setItemData(idx, path, Qt::UserRole);
            fileUICombo->setItemData(idx, tag, Qt::UserRole + 1);
        }
    };

    scanDir(userFileDir, "user");
    scanDir(systemFileDir, "system");
    fileUICombo->setCurrentIndex(-1);
}

void ParaSpectrumSimulation::populateCombo()
{
    fileCombo->clear();
    auto scanDir = [&](const QString &dirPath, const QString &tag)
    {
        QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString path = it.next();
            QString base = QFileInfo(path).baseName();
            fileCombo->addItem(QString("\u2022 %1 (%2)").arg(base, tag));
            int idx = fileCombo->count() - 1;
            fileCombo->setItemData(idx, path, Qt::UserRole);
            fileCombo->setItemData(idx, tag, Qt::UserRole + 1);
        }
    };

    scanDir(userFileDir, "user");
    scanDir(systemFileDir, "system");
    fileCombo->setCurrentIndex(-1);
}

void ParaSpectrumSimulation::populateTargetCombo()
{
    TargetCombo->clear();
    auto scanDir = [&](const QString &dirPath, const QString &tag)
    {
        QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext())
        {
            QString fullPath = it.next();
            QString baseName = QFileInfo(fullPath).baseName();
            TargetCombo->addItem(QString("• %1 (%2)").arg(baseName, tag));
            int idx = TargetCombo->count() - 1;
            TargetCombo->setItemData(idx, fullPath, Qt::UserRole);
        }
    };
    scanDir(userFileDir, "user");
    scanDir(systemFileDir, "system");
    TargetCombo->setCurrentIndex(-1);
}

void ParaSpectrumSimulation::PlaneRunFunction(const QString &targetPath,
                                              const ProjectInfo &info)
{
    Q_UNUSED(targetPath)

    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();

    QString cmdStart = "echo [ProgressSTART][Name:Terminal][MAX:100]";
    term->appendLine(cmdStart);
    term->appendLine("Start simulation...");

    // 1. 检查是否选择了Template File
    int currentIndex = TargetCombo->currentIndex();
    if (currentIndex == -1)
    {
        term->appendLine("<font color='red'>Error: No template file selected!</font>");
        term->appendLine("<font color='yellow'>Please select a template file from the dropdown.</font>");
        return;
    }

    // 2. 获取Template文件名（去掉括号部分）
    QString selectedText = TargetCombo->currentText();
    QString templateName = extractTemplateNameFromComboText(selectedText);
    if (templateName.isEmpty())
    {
        term->appendLine("<font color='red'>Error: Failed to parse template file name!</font>");
        return;
    }

    // 确保移除任何扩展名，获取基础名称
    QString baseName = QFileInfo(templateName).baseName();
    term->appendLine("Debug: templateName = '" + templateName + "', baseName = '" + baseName + "'");

    // 3. 检索Geant4项目路径
    QString geant4ProjectPath = detectGeant4ProjectPath(info, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 SpectrumSimulation project!</font>");
        return;
    }

    QString buildPath = geant4ProjectPath + "/build";

    // 4. 检查转换后的Mac文件是否存在 - 使用基础名称构造文件名
    QString macFileName = baseName + ".mac";
    QString macFilePath = buildPath + "/" + macFileName;

    QFile macFile(macFilePath);
    if (!macFile.exists())
    {
        term->appendLine("<font color='red'>Error: Mac file not found: " + macFilePath + "</font>");
        term->appendLine("<font color='yellow'>Please ensure the template file has been properly converted.</font>");
        return;
    }

    term->appendLine("<font color='green'>Using Mac file: " + macFilePath + "</font>");

    // 5. 执行Geant4仿真
    QString command = "./GammaTherapy " + macFileName;

    // 检查build目录是否存在
    QDir buildDir(buildPath);
    if (!buildDir.exists())
    {
        term->appendLine("<font color='red'>Error: Build directory not found: " + buildPath + "</font>");
        term->appendLine("<font color='yellow'>Please make sure the Geant4 project has been compiled.</font>");
        return;
    }

    term->appendLine("Build directory: " + buildPath);

    QProcess *geant4Process = new QProcess(this);
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

    term->appendLine("LD_LIBRARY_PATH = " + env.value("LD_LIBRARY_PATH"));
    term->appendLine("G4INSTALL = " + env.value("G4INSTALL"));

    connect(geant4Process, &QProcess::readyReadStandardOutput, [=]()
            {
        QString output = geant4Process->readAllStandardOutput();
        term->appendLine(output); });

    connect(geant4Process, &QProcess::readyReadStandardError, [=]()
            {
        QString error = geant4Process->readAllStandardError();
        term->appendLine("<font color='red'>" + error + "</font>"); });

    connect(geant4Process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus exitStatus)
            {
                if (exitStatus == QProcess::NormalExit)
                {
                    if (exitCode == 0)
                    {
                        term->appendLine("<font color='green'>Simulation completed successfully！</font>");
                    }
                    else
                    {
                        term->appendLine("<font color='orange'>Simulation finished, exit code: " + QString::number(exitCode) + "</font>");
                    }
                }
                else
                {
                    term->appendLine("<font color='red'>Abnormal termination！</font>");
                }

                geant4Process->deleteLater();
            });

    term->appendLine("Working directory: " + buildPath);
    term->appendLine("Execute command: " + command);

    geant4Process->start(command);

    if (!geant4Process->waitForStarted(5000))
    {
        term->appendLine("<font color='red'>Failed to launch the Geant4 project! Please verify that the paths and commands are correct.</font>");
        term->appendLine("<font color='yellow'>Tip: Please confirm that the following paths exist and are accessible:</font>");
        term->appendLine("- Geant4 installation directory: " + geant4InstallPath);
        term->appendLine("- Project build directory: " + buildPath);
        term->appendLine("- Executable file: " + buildPath + "/" + command.split(" ").first());
        term->appendLine("- Project path used: " + info.ProjectPath);
        term->appendLine("- Detected Geant4 project path: " + geant4ProjectPath);
        geant4Process->deleteLater();
        return;
    }

    term->appendLine("<font color='green'>Geant4 project launched; simulation is now running...</font>");
}

QString ParaSpectrumSimulation::detectGeant4ProjectPath(const ProjectInfo &info, TerminalWidget *term)
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

QString ParaSpectrumSimulation::detectGeant4InstallPath(TerminalWidget *term)
{
    // 常见的Geant4安装路径
    QStringList possiblePaths = {
        "/home/day/Geant4/geant4-install",
        "/usr/local",
        "/opt/geant4",
        "/home/" + qgetenv("USER") + "/Geant4/geant4-install",
        "/usr/local/geant4"};

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

void ParaSpectrumSimulation::testGWLConversion()
{
    qDebug() << "=== GWL Conversion Test ===";
    qDebug() << "Current project info:";
    qDebug() << "ProjectPath:" << allInfo.ProjectPath;
    qDebug() << "ProjectName:" << allInfo.ProjectName;

    if (TargetCombo->currentIndex() == -1)
    {
        qDebug() << "No template file selected in TargetCombo";
        return;
    }

    QString selectedText = TargetCombo->currentText();
    qDebug() << "Selected template text:" << selectedText;

    QString templateName = extractTemplateNameFromComboText(selectedText);
    if (!templateName.isEmpty())
    {
        qDebug() << "Parsed template name:" << templateName;

        QString gwlFilePath = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/spectrum/" + templateName + ".gwl";
        qDebug() << "Expected GWL file path:" << gwlFilePath;
        qDebug() << "GWL file exists:" << QFile::exists(gwlFilePath);
    }
    else
    {
        qDebug() << "Failed to parse template name from:" << selectedText;
    }
}

void ParaSpectrumSimulation::convertSelectedGWLFile()
{
    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();

    // 1. 检查是否选择了Template File
    int idx = TargetCombo->currentIndex();
    term->appendLine("Debug: TargetCombo->currentIndex() = " + QString::number(idx));

    if (idx == -1)
    {
        term->appendLine("<font color='red'>Error: No template file selected!</font>");
        term->appendLine("Debug: TargetCombo count = " + QString::number(TargetCombo->count()));
        term->appendLine("Debug: TargetCombo currentText = '" + TargetCombo->currentText() + "'");
        return;
    }

    // 2. 获取选中文件的完整路径
    QString fullPath = TargetCombo->itemData(idx, Qt::UserRole).toString();
    if (fullPath.isEmpty())
    {
        term->appendLine("<font color='red'>Error: Failed to get template file path!</font>");
        return;
    }

    // 3. 获取文件名并构建对应的GWL文件路径
    QString fileName = QFileInfo(fullPath).fileName();
    QString baseName = QFileInfo(fileName).baseName(); // 去掉扩展名
    QString gwlFileName = baseName + ".gwl";
    QString gwlFilePath = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/spectrum/" + gwlFileName;

    // 4. 检查GWL文件是否存在
    QFile gwlFile(gwlFilePath);
    if (!gwlFile.exists())
    {
        term->appendLine("<font color='red'>Error: GWL file not found: " + gwlFilePath + "</font>");
        return;
    }

    // 5. 获取Geant4项目路径
    QString geant4ProjectPath = detectGeant4ProjectPath(allInfo, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 SpectrumSimulation project for conversion!</font>");
        return;
    }

    QString buildPath = geant4ProjectPath + "/build";

    // 6. 执行GWL转Mac转换
    term->appendLine("Auto-converting GWL to Mac file: " + baseName);
    term->appendLine("Selected template file: " + fileName);
    term->appendLine("GWL source file: " + gwlFilePath);
    QString macFileName = baseName + ".mac";
    QString macFilePath = buildPath + "/" + macFileName;
    term->appendLine("Mac target file: " + macFilePath);

    if (!gwlConverter->convertGWLToMac(gwlFilePath, macFilePath))
    {
        term->appendLine("<font color='red'>Error: Failed to convert GWL to Mac file!</font>");
        term->appendLine("<font color='red'>Error details: " + gwlConverter->getLastError() + "</font>");
        return;
    }

    term->appendLine("<font color='green'>GWL conversion completed: " + macFileName + "</font>");
    // --- 同步复制 mac/vis 到用户项目的 MACFolder 下的 spectrum 分类目录 ---
    QString userMacRoot = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.MACFolder;
    QDir userMacDir(userMacRoot);
    if (!userMacDir.exists())
    {
        if (!QDir().mkpath(userMacRoot))
        {
            term->appendLine("<font color='orange'>Warning: Failed to create user MAC folder: " + userMacRoot + "</font>");
        }
    }

    QString targetSubFolder = "spectrum";
    QString destDirPath = userMacRoot + "/" + targetSubFolder;
    if (!QDir(destDirPath).exists())
    {
        if (QDir().mkpath(destDirPath))
        {
            term->appendLine("Debug: Created category folder: " + destDirPath);
        }
        else
        {
            term->appendLine("<font color='orange'>Warning: Failed to create category folder: " + destDirPath + "</font>");
        }
    }

    QString destMacPath = destDirPath + "/" + macFileName;
    if (QFile::exists(destMacPath))
        QFile::remove(destMacPath);
    if (QFile::copy(macFilePath, destMacPath))
    {
        term->appendLine("<font color='green'>Copied Mac to project folder: " + destMacPath + "</font>");
    }
    else
    {
        term->appendLine("<font color='orange'>Warning: Failed to copy Mac to project folder: " + destMacPath + "</font>");
    }

    QString visMacFilePath = buildPath + "/" + baseName + "_vis.mac";
    QFile visMacFile(visMacFilePath);
    if (visMacFile.exists())
    {
        QString destVisPath = destDirPath + "/" + baseName + "_vis.mac";
        if (QFile::exists(destVisPath))
            QFile::remove(destVisPath);
        if (QFile::copy(visMacFilePath, destVisPath))
        {
            term->appendLine("<font color='green'>Copied Vis Mac to project folder: " + destVisPath + "</font>");
        }
        else
        {
            term->appendLine("<font color='orange'>Warning: Failed to copy Vis Mac to project folder: " + destVisPath + "</font>");
        }
    }
}

void ParaSpectrumSimulation::convertSelectedGWLFileWithIndex(int idx)
{
    if (!mainWindow)
        return;

    TerminalWidget *term = mainWindow->terminal();

    term->appendLine("Debug: convertSelectedGWLFileWithIndex called with idx = " + QString::number(idx));
    term->appendLine("Debug: allInfo.ProjectPath = '" + allInfo.ProjectPath + "'");
    term->appendLine("Debug: allInfo.ProjectName = '" + allInfo.ProjectName + "'");

    // 1. 检查是否选择了Template File
    if (idx == -1)
    {
        term->appendLine("<font color='red'>Error: No template file selected (invalid index)!</font>");
        return;
    }

    // 2. 获取选中文件的完整路径
    QString fullPath = TargetCombo->itemData(idx, Qt::UserRole).toString();
    term->appendLine("Debug: fullPath = '" + fullPath + "'");

    if (fullPath.isEmpty())
    {
        term->appendLine("<font color='red'>Error: Failed to get template file path!</font>");
        return;
    }

    // 3. 获取文件名并构建对应的GWL文件路径
    QString fileName = QFileInfo(fullPath).fileName();
    QString baseName = QFileInfo(fileName).baseName(); // 去掉扩展名
    term->appendLine("Debug: fileName = '" + fileName + "', baseName = '" + baseName + "'");

    QString gwlFileName = baseName + ".gwl";
    QString gwlFilePath = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/spectrum/" + gwlFileName;
    term->appendLine("Debug: Expected GWL path = '" + gwlFilePath + "'");

    // 4. 检查GWL文件是否存在
    QFile gwlFile(gwlFilePath);
    if (!gwlFile.exists())
    {
        term->appendLine("<font color='red'>Error: GWL file not found: " + gwlFilePath + "</font>");
        return;
    }

    // 5. 获取Geant4项目路径
    QString geant4ProjectPath = detectGeant4ProjectPath(allInfo, term);
    if (geant4ProjectPath.isEmpty())
    {
        term->appendLine("<font color='red'>Failed to locate Geant4 SpectrumSimulation project for conversion!</font>");
        return;
    }

    QString buildPath = geant4ProjectPath + "/build";

    // 6. 执行GWL转Mac转换
    term->appendLine("Auto-converting GWL to Mac file: " + baseName);
    term->appendLine("Selected template file: " + fileName);
    term->appendLine("GWL source file: " + gwlFilePath);
    QString macFileName = baseName + ".mac";
    QString macFilePath = buildPath + "/" + macFileName;
    term->appendLine("Mac target file: " + macFilePath);

    if (!gwlConverter->convertGWLToMac(gwlFilePath, macFilePath))
    {
        term->appendLine("<font color='red'>Error: Failed to convert GWL to Mac file!</font>");
        term->appendLine("<font color='red'>Error details: " + gwlConverter->getLastError() + "</font>");
        return;
    }

    term->appendLine("<font color='green'>GWL conversion completed: " + macFileName + "</font>");

    // --- 同步复制 mac/vis 到用户项目的 MACFolder 下的 spectrum 分类目录 ---
    QString userMacRoot = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.MACFolder;
    QDir userMacDir(userMacRoot);
    if (!userMacDir.exists())
    {
        if (!QDir().mkpath(userMacRoot))
        {
            term->appendLine("<font color='orange'>Warning: Failed to create user MAC folder: " + userMacRoot + "</font>");
        }
    }

    QString targetSubFolder = "spectrum";
    QString destDirPath = userMacRoot + "/" + targetSubFolder;
    if (!QDir(destDirPath).exists())
    {
        if (QDir().mkpath(destDirPath))
        {
            term->appendLine("Debug: Created category folder: " + destDirPath);
        }
        else
        {
            term->appendLine("<font color='orange'>Warning: Failed to create category folder: " + destDirPath + "</font>");
        }
    }

    QString destMacPath = destDirPath + "/" + macFileName;
    if (QFile::exists(destMacPath))
        QFile::remove(destMacPath);
    if (QFile::copy(macFilePath, destMacPath))
    {
        term->appendLine("<font color='green'>Copied Mac to project folder: " + destMacPath + "</font>");
    }
    else
    {
        term->appendLine("<font color='orange'>Warning: Failed to copy Mac to project folder: " + destMacPath + "</font>");
    }

    QString visMacFilePath = buildPath + "/" + baseName + "_vis.mac";
    QFile visMacFile(visMacFilePath);
    if (visMacFile.exists())
    {
        QString destVisPath = destDirPath + "/" + baseName + "_vis.mac";
        if (QFile::exists(destVisPath))
            QFile::remove(destVisPath);
        if (QFile::copy(visMacFilePath, destVisPath))
        {
            term->appendLine("<font color='green'>Copied Vis Mac to project folder: " + destVisPath + "</font>");
        }
        else
        {
            term->appendLine("<font color='orange'>Warning: Failed to copy Vis Mac to project folder: " + destVisPath + "</font>");
        }
    }
}

QString ParaSpectrumSimulation::extractTemplateNameFromComboText(const QString &comboText)
{
    // 处理格式如: "• spectrum_hot_default_test1 (user)" 或 "• spectrum_hot_default_test1 (system)"
    // 提取中间的文件名部分，去掉 "• " 前缀和 " (xxx)" 后缀

    QRegularExpression re("• ([^\\s\\(]+)");
    QRegularExpressionMatch match = re.match(comboText);
    if (match.hasMatch())
    {
        return match.captured(1);
    }

    // 如果正则表达式不匹配，尝试简单的字符串处理
    QString result = comboText.trimmed();
    if (result.startsWith("• "))
    {
        result = result.mid(2); // 去掉 "• "
    }

    int parenIndex = result.indexOf(" (");
    if (parenIndex != -1)
    {
        result = result.left(parenIndex); // 去掉 " (xxx)" 部分
    }

    return result.trimmed();
}
