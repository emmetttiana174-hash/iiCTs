#include "paradetectormodel.h"

#include "mainwindow.h"
#include "projectinfo.h"
#include "chatpanel.h"
#include "paramfilemanager.h"
#include "parahome.h"
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

ParaDetectorModel::ParaDetectorModel(QWidget* parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: transparent;");

    valueParams = {
        { "PCCT", -1, "" },                { "PCCTTh1", -1, "KeV" },
        { "DetCountX", -1, "" },           { "PCCTTh2", -1, "KeV" },
        { "DetCountY", -1, ""},            { "PCCTTh3", -1, "KeV" },
        { "DetUnitSizeX", -1, "mm" },      { "PCCTTh4", -1, "KeV" },
        { "DetUnitSizeY", -1, "mm" },      { "PCCTTh5", -1, "KeV" },
        { "DetUnitThickness", -1, "mm" },  { "PCCTTh6", -1, "KeV" },
        { "DetectorRadius", -1, "mm" },    { "PCCTTh7", -1, "KeV" },
    };

    fileParams  = {
        {"OutputFilename", "-1"},         {"DetectorMaterial", "-1"},
        { "OutputFolder", "/home/" }
    };

    const QStringList fileSelectionKeys = { "InputFilePath"};
    const QStringList floderSelectionKeys = { "OutputFolder"};
    const QStringList materialKeys = { "AnodeMaterial", "CathodeMaterial", "FocusMaterial"};


    userFileDir = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.DetectorFolder;
    systemFileDir = allInfo.SystemScriptPath + "/detector";

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

    QLabel* promptLabel = new QLabel("Target Path:", this);
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
            this, [this](int idx){
        if (idx >= 0) {
            TargetPath = TargetCombo
                    ->itemData(idx, Qt::UserRole)
                    .toString();
            QString fn = QFileInfo(TargetPath).fileName();
            TargetCombo->lineEdit()->setText(fn);
            emit fileChanged(TargetPath);
        }
        else {
            TargetPath.clear();
            TargetCombo->lineEdit()->clear();
            TargetCombo->lineEdit()->setPlaceholderText("Please select a script file");
        }
    });

    QVBoxLayout* m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 10, 15, 20);

    QHBoxLayout* buttonRow = new QHBoxLayout;
    buttonRow->setSpacing(10);
    buttonRow->setContentsMargins(10, 0, 10, 0);
    buttonRow->setAlignment(Qt::AlignCenter);
    buttonRow->addStretch();
    buttonRow->addWidget(promptLabel);
    buttonRow->addWidget(TargetCombo);

    QSize btnMainSize(150, 50);
    QString btnMainStyle  = "QPushButton {"
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


    QPushButton* buttonSim = new QPushButton("Build");
    buttonSim->setFixedSize(btnMainSize);
    buttonSim->setStyleSheet(btnMainStyle);
    buttonRow->addSpacing(50);
    buttonRow->addWidget(buttonSim);
    buttonRow->addStretch();

    connect(buttonSim, &QPushButton::clicked, this, [this]() {
        PlaneRunFunction(TargetPath, allInfo);
    });

    m_mainLayout->addSpacing(10);
    m_mainLayout->addLayout(buttonRow);


    QHBoxLayout* topButtonsLayout = new QHBoxLayout;
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
    QButtonGroup* toggleGroup = new QButtonGroup(this);
    toggleGroup->setExclusive(true);
    QPushButton* UIBtn = new QPushButton("UI");
    QPushButton* detailedBtn = new QPushButton("Det");
    QPushButton* AIBtn = new QPushButton("AI");
    QFont toggleFont("Times New Roman", 12);
    QList<QPushButton*> toggleButtons = { UIBtn, detailedBtn, AIBtn };
    for (auto btn : toggleButtons) {
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

    QStackedWidget* centralView = new QStackedWidget;
    centralView->setStyleSheet("background-color: transparent; border-radius: 12px;");

    QWidget* uiEditPanel = new QWidget;
    QWidget* deEditPanel = new QWidget;
    centralView->addWidget(uiEditPanel);
    centralView->addWidget(deEditPanel);
    m_mainLayout->insertWidget(1, centralView);

    fileUICombo = new QComboBox(uiEditPanel);
    QPushButton* saveUIBtn   = new QPushButton("Save",    uiEditPanel);
    QPushButton* saveAsUIBtn = new QPushButton("Save As", uiEditPanel);
    QWidget*  UIControlPlane     = new QWidget(uiEditPanel);

    uiEditPanel->setAutoFillBackground(false);
    uiEditPanel->setAttribute(Qt::WA_StyledBackground);
    uiEditPanel->setStyleSheet(R"(
                               background-color: transparent;
                               border: 2px dashed #7697D8;
                               font-family: "Times New Roman";
                               font-weight: bold;
                               border-radius: 10px;
                               )");

    QLabel* spectUILabel = new QLabel(fileTitleName, uiEditPanel);
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

    auto* UIControlLayout = new QVBoxLayout(UIControlPlane);
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

    QWidget* editContainer = new QWidget(UIControlPlane);
    editContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    editContainer->setStyleSheet(R"(
                                 QWidget {
                                 background-color: #22489E;
                                 border: 1px solid #7697D8;
                                 border-radius: 12px;
                                 }
                                 )");

    QGridLayout* grid = new QGridLayout(editContainer);
    //    grid->setHorizontalSpacing(10);
    //    grid->setVerticalSpacing(12);
    grid->setContentsMargins(10, 10, 10, 10);

    int row = 0, col = 0;
    for (const auto& vp : valueParams) {
        auto* hbox = new QHBoxLayout;
        hbox->setSpacing(3);

        QLabel* lblName = new QLabel(vp.name + ":", editContainer);
        lblName->setFixedWidth(180);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLineEdit* edit = new QLineEdit(QString::number(vp.defaultValue), editContainer);
        edit->setFixedWidth(60);
        edit->setStyleSheet(UIControlPlaneStyleEdit);
        edit->setFont(font);
        edit->setAlignment(Qt::AlignCenter);
        valueEdits.insert(vp.name, edit);
        edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLabel* lblUnit = new QLabel(vp.unit, editContainer);
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
        if (col == 2) {
            col = 0;
            row++;
        }
    }

    if (col == 1) {
        grid->addItem(new QSpacerItem(1,1), row, 1);
        row++;
    }

    QWidget* filePathContainer = new QWidget(UIControlPlane);
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

    QGridLayout* gridFile = new QGridLayout(filePathContainer);
    gridFile->setContentsMargins(10, 10, 10, 10);

    row = 0;
    col = 0;
    for (const auto& fp : fileParams) {
        QLabel* lblName = new QLabel(fp.name + ":", filePathContainer);
        lblName->setFixedWidth(180);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLineEdit* pathEdit = new QLineEdit(fp.filePath, filePathContainer);
        pathEdit->setStyleSheet(UIControlPlaneStyleEdit);
        pathEdit->setFont(font);
        pathEdit->setFrame(true);
        pathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        fileEdits.insert(fp.name, pathEdit);

        QWidget* selectorWidget = nullptr;
        if (fileSelectionKeys.contains(fp.name)) {
            QPushButton* btn = new QPushButton("...", filePathContainer);
            btn->setFixedSize(40,40);
            btn->setToolTip("Browse...");
            btn->setStyleSheet(BrowseStyleString);
            connect(btn, &QPushButton::clicked, this, [=](){
                QString filePath = FileDialog::getOpenFileName(
                            UIControlPlane,
                            "Select " + fp.name,
                            QDir::homePath(),
                            "All Files (*)"
                            );
                if (!filePath.isEmpty())
                    pathEdit->setText(filePath);
            });
            selectorWidget = btn;
            pathEdit->setMinimumWidth(200);
        }
        else if (floderSelectionKeys.contains(fp.name)) {
            QPushButton* btn = new QPushButton("...", filePathContainer);
            btn->setFixedSize(40,40);
            btn->setToolTip("Browse...");
            btn->setStyleSheet(BrowseStyleString);
            connect(btn, &QPushButton::clicked, this, [=](){
                QString dirPath = FileDialog::getExistingDirectory(
                            UIControlPlane,
                            "Select " + fp.name,
                            QDir::homePath(),
                            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                            );
                if (!dirPath.isEmpty())
                    pathEdit->setText(dirPath);
            });
            selectorWidget = btn;
            pathEdit->setMinimumWidth(200);
        }
        else {
            QWidget* placeholder = new QWidget(filePathContainer);
            placeholder->setFixedSize(40,40);
            placeholder->setStyleSheet("background: transparent; border: none;");
            selectorWidget = placeholder;
            pathEdit->setFixedWidth(103);
        }

        selectorWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


        bool isFullRow = fileSelectionKeys.contains(fp.name)
                || floderSelectionKeys.contains(fp.name);

        if (isFullRow) {
            QHBoxLayout* hbox = new QHBoxLayout;
            hbox->setSpacing(3);
            hbox->addWidget(lblName);
            hbox->addWidget(pathEdit);
            hbox->addWidget(selectorWidget);
            hbox->addSpacing(20);
            if (col == 1) {
                gridFile->addItem(
                            new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum),
                            row, 1
                            );
                row++;
                col = 0;
            }
            gridFile->addLayout(hbox, row, 0, 1, 2);
            row++;
            col = 0;
        }
        else {
            QHBoxLayout* hbox = new QHBoxLayout;
            hbox->addStretch();
            hbox->addWidget(lblName);
            hbox->addWidget(pathEdit);
            hbox->addStretch();
            gridFile->addLayout(hbox, row, col);
            col++;
            if (col == 2) {
                col = 0;
                row++;
            }
        }
    }

    if (col == 1) {
        gridFile->addItem(
                    new QSpacerItem(0,0,QSizePolicy::Expanding,QSizePolicy::Minimum),
                    row, 1
                    );
    }

    QWidget* editTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout* editTitleLayout = new QVBoxLayout(editTitleContainer);
    editTitleLayout->setContentsMargins(5, 0, 0, 0);
    QLabel* editTitleLabel = new QLabel("Parameter Settings");
    editTitleLabel->setStyleSheet(kSpectUILabelStyle);
    editTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    editTitleLayout->addWidget(editTitleLabel);

    QWidget* pathTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout* pathTitleLayout = new QVBoxLayout(pathTitleContainer);
    pathTitleLayout->setContentsMargins(5, 0, 0, 0);
    QLabel* pathTitleLabel = new QLabel("Path Settings");
    pathTitleLabel->setStyleSheet(kSpectUILabelStyle);
    pathTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pathTitleLayout->addWidget(pathTitleLabel);

    UIControlPlane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    editContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    filePathContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
//    UIControlLayout->addStretch();
    UIControlLayout->addSpacing(30);
    UIControlLayout->addWidget(editTitleContainer);
    UIControlLayout->addWidget(editContainer);
//    UIControlLayout->addStretch();
    UIControlLayout->addSpacing(30);
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

    auto* uiEditTitleLayout = new QHBoxLayout;
    uiEditTitleLayout->setSpacing(0);
    uiEditTitleLayout->setContentsMargins(5,0,5,0);
    uiEditTitleLayout->addWidget(spectUILabel);
    uiEditTitleLayout->addWidget(fileUICombo);
    uiEditTitleLayout->addStretch();
    uiEditTitleLayout->addWidget(saveUIBtn);
    uiEditTitleLayout->addSpacing(10);
    uiEditTitleLayout->addWidget(saveAsUIBtn);


    auto* uiEditLayout = new QVBoxLayout(uiEditPanel);
    uiEditLayout->setContentsMargins(10,10,10,10);
    uiEditLayout->setSpacing(10);
    uiEditLayout->addLayout(uiEditTitleLayout);
    uiEditLayout->addWidget(UIControlPlane);
    uiEditPanel->setLayout(uiEditLayout);

    fileUICombo->setEditable(true);
    fileUICombo->lineEdit()->setReadOnly(true);
    fileUICombo->lineEdit()->setPlaceholderText("Please select a template");

    populateUICombo();

    fileCombo = new QComboBox(deEditPanel);
    QPushButton* saveBtn   = new QPushButton("Save",    deEditPanel);
    QPushButton* saveAsBtn = new QPushButton("Save As", deEditPanel);
    QTextEdit*  editor     = new QTextEdit(deEditPanel);

    QLabel* spectLabel = new QLabel(fileTitleName, deEditPanel);
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
                    "Now, please select a template file from the dropdown above and modify it."
                    )
                );


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

    auto* deEditTitleLayout = new QHBoxLayout;
    deEditTitleLayout->setSpacing(0);
    deEditTitleLayout->setContentsMargins(5,0,5,10);
    deEditTitleLayout->addWidget(spectLabel);
    deEditTitleLayout->addWidget(fileCombo);
    deEditTitleLayout->addStretch();
    deEditTitleLayout->addWidget(saveBtn);
    deEditTitleLayout->addSpacing(10);
    deEditTitleLayout->addWidget(saveAsBtn);


    auto* deEditLayout = new QVBoxLayout(deEditPanel);
    deEditLayout->setContentsMargins(10,10,10,10);
    deEditLayout->setSpacing(10);
    deEditLayout->addLayout(deEditTitleLayout);
    deEditLayout->addWidget(editor);
    deEditPanel->setLayout(deEditLayout);
    fileCombo->setEditable(true);
    fileCombo->lineEdit()->setReadOnly(true);
    fileCombo->lineEdit()->setPlaceholderText("Please select a template");

    populateCombo();

    connect(fileUICombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int idx){
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
        emit fileChanged(path);
        emit fileResultChanged(TargetPath);
    });

    connect(saveUIBtn, &QPushButton::clicked, this, [=]() {
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
        }
    });


    connect(saveAsUIBtn, &QPushButton::clicked, this, [=]() {
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
        }
    });

    connect(fileCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int idx){
        if (idx < 0) return;
        QString path   = fileCombo->itemData(idx, Qt::UserRole).toString();
        QFile   file(path);
        if (file.open(QIODevice::ReadOnly|QIODevice::Text)) {
            editor->setPlainText(QString::fromUtf8(file.readAll()));
        }
        emit fileChanged(path);
    });

    connect(saveBtn, &QPushButton::clicked, this, [=]() {
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
        }
    });


    connect(saveAsBtn, &QPushButton::clicked, this, [=]() {
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
        }
    });

    paramMgr = new ParamFileManager(
                fileUICombo,
                valueParams,
                fileParams,
                valueEdits,
                fileEdits,
                this
                );

    connect(paramMgr, &ParamFileManager::fileContentListChanged,
            this, [this](const QStringList &newList){
        fileContentList = newList;
    });

    connect(this, &ParaDetectorModel::fileSaved,
            paramMgr, &ParamFileManager::reloadCurrentFile);

    connect(UIBtn, &QPushButton::clicked, [=]() { centralView->setCurrentIndex(0); });
    connect(detailedBtn, &QPushButton::clicked, [=]() { centralView->setCurrentIndex(1); });
    connect(AIBtn, &QPushButton::clicked, this, [=]() { ChatPanel* chat = mainWindow->getChatPanel();

        if (centralView->indexOf(chat) < 0) {
            centralView->addWidget(chat);
        }
        centralView->setCurrentWidget(chat); });
}

void ParaDetectorModel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

}

void ParaDetectorModel::setMainWindow(MainWindow* main) {
    mainWindow = main;
}

void ParaDetectorModel::paintEvent(QPaintEvent *event)
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

void ParaDetectorModel::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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

void ParaDetectorModel::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
    userFileDir = allInfo.ProjectPath + "/" + allInfo.ProjectName + "/" + allInfo.DetectorFolder;
    systemFileDir = allInfo.SystemScriptPath + "/detector";

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

void ParaDetectorModel::populateUICombo()
{
    fileUICombo->clear();
    auto scanDir = [&](const QString& dirPath, const QString& tag){
        QDirIterator it(dirPath,
                        QDir::Files,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString path = it.next();
            QString base = QFileInfo(path).baseName();
            fileUICombo->addItem(QString("• %1 (%2)").arg(base, tag));
            int idx = fileUICombo->count() - 1;
            fileUICombo->setItemData(idx, path,      Qt::UserRole);
            fileUICombo->setItemData(idx, tag,       Qt::UserRole + 1);
        }
    };

    scanDir(userFileDir,   "user");
    scanDir(systemFileDir, "system");
    fileUICombo->setCurrentIndex(-1);
}

void ParaDetectorModel::populateCombo()
{
    fileCombo->clear();
    auto scanDir = [&](const QString& dirPath, const QString& tag){
        QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString path = it.next();
            QString base = QFileInfo(path).baseName();
            fileCombo->addItem(QString("\u2022 %1 (%2)").arg(base, tag));
            int idx = fileCombo->count() - 1;
            fileCombo->setItemData(idx, path,    Qt::UserRole);
            fileCombo->setItemData(idx, tag,     Qt::UserRole + 1);
        }
    };

    scanDir(userFileDir,   "user");
    scanDir(systemFileDir, "system");
    fileCombo->setCurrentIndex(-1);
}

void ParaDetectorModel::populateTargetCombo()
{
    TargetCombo->clear();
    auto scanDir = [&](const QString& dirPath, const QString& tag) {
        QDirIterator it(dirPath, QDir::Files, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString fullPath = it.next();
            QString baseName = QFileInfo(fullPath).baseName();
            TargetCombo->addItem(QString("• %1 (%2)").arg(baseName, tag));
            int idx = TargetCombo->count() - 1;
            TargetCombo->setItemData(idx, fullPath, Qt::UserRole);
        }
    };
    scanDir(userFileDir,   "user");
    scanDir(systemFileDir, "system");
    TargetCombo->setCurrentIndex(-1);
}


void ParaDetectorModel::PlaneRunFunction(const QString &targetPath,
                                              const ProjectInfo &info)
{

    //    Q_UNUSED(targetPath)
    qDebug() << "Simulating...";
    Q_UNUSED(info)

    if (!mainWindow)
        return;

    QString cmdStart = "echo [ProgressSTART][Name:Terminal][MAX:10]";
    QString cmd      = "for i in {1..10}; do echo iiCTs:$i; sleep 0.5; done;";

    TerminalWidget *term = mainWindow->terminal();
    term->appendLine(targetPath);
    term->appendLine(cmdStart);
    term->runCommandExternally(cmd);
}
