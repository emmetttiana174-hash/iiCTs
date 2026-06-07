#include "parahome.h"

#include "mainwindow.h"
#include "projectinfo.h"
#include "chatpanel.h"
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
#include <QCompleter>
#include <QFile>
#include <QDir>
#include <QStyleFactory>
#include <QApplication>


ParaHome::ParaHome(QWidget* parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: transparent;");


    valueParams = {
        { "ProjectName", "sim_v1", "" },
        { "TubeType", "hot", "" },
        { "CTType", "helical", "" },
        { "TubeFolder", "tube", "" },
        { "SpectrumFolder", "spectrum", "" },
        { "DetectorFolder", "detector", "" },
        { "CTFolder", "ct", "" },
        { "ReconFolder", "recon", "" },
        { "MACFolder", "mac", "" }
    };

    QStringList TubeTypeList = QStringList{"hot", "cold"};
    QStringList CTTypeList = QStringList{"helical", "CBCT", "linear_static", "polygon_interleaved", "polygon_side"};


    fileParams  = {
        { "ProjectPath" , "/home/vgate/software_gy/share/iiCTs/user"},
        { "SystemScriptPath", "/home/vgate/software_gy/share/iiCTs/CTISS/scripts" },
        { "SystemMaterialPath", "/home/vgate/software_gy/share/iiCTs/CTISS/materials" },
        { "TubeDatabasePath", "/home/vgate/software_gy/share/iiCTs/database/tube" },
        { "SpectrumDatabasePath", "/home/vgate/software_gy/share/iiCTs/database/XraySpectrum" }
    };


    QVBoxLayout* m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 10, 15, 20);

    QHBoxLayout* buttonRow = new QHBoxLayout;
    buttonRow->setSpacing(50);
    buttonRow->setContentsMargins(10, 0, 10, 0);
    buttonRow->setAlignment(Qt::AlignCenter);

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


    QPushButton* buttonNew = new QPushButton("New");
    buttonNew->setFixedSize(btnMainSize);
    buttonNew->setStyleSheet(btnMainStyle);

    QPushButton* buttonOpen = new QPushButton("Open");
    buttonOpen->setFixedSize(btnMainSize);
    buttonOpen->setStyleSheet(btnMainStyle);

    QPushButton* buttonSim = new QPushButton("START");
    buttonSim->setFixedSize(btnMainSize);
    buttonSim->setStyleSheet(btnMainStyle);

    buttonRow->addStretch();
    buttonRow->addWidget(buttonNew);
    buttonRow->addWidget(buttonOpen);
    buttonRow->addWidget(buttonSim);
    buttonRow->addStretch();

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
    QPushButton* AIBtn = new QPushButton("AI");
    QFont toggleFont("Times New Roman", 12);
    QList<QPushButton*> toggleButtons = { UIBtn, AIBtn };
    for (auto btn : toggleButtons) {
        btn->setCheckable(true);
        btn->setFixedSize(40, 30);
        toggleGroup->addButton(btn);
        btn->setFont(toggleFont);
        topButtonsLayout->addWidget(btn);
    }
    UIBtn->setStyleSheet(UIBtnStyle);
    AIBtn->setStyleSheet(AIBtnStyle);
    topButtonsLayout->setSpacing(0);
    topButtonsLayout->setContentsMargins(0, 0, 20, 0);
    UIBtn->setChecked(true);
    topButtonsLayout->setAlignment(Qt::AlignRight);
    m_mainLayout->insertLayout(0, topButtonsLayout);

    QStackedWidget* centralView = new QStackedWidget;
    centralView->setStyleSheet("background-color: transparent; border-radius: 12px;");

    QWidget* uiEditPanel = new QWidget;
    centralView->addWidget(uiEditPanel);
    m_mainLayout->insertWidget(1, centralView);

    uiEditPanel->setAutoFillBackground(false);
    uiEditPanel->setAttribute(Qt::WA_StyledBackground);
    uiEditPanel->setStyleSheet(R"(
                               background-color: transparent;
                               border: 2px dashed #7697D8;
                               font-family: "Times New Roman";
                               font-weight: bold;
                               border-radius: 10px;
                               )");



    QWidget*  UIControlPlane     = new QWidget(uiEditPanel);
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
    //    grid->setHorizontalSpacing(20);
    //    grid->setVerticalSpacing(20);
    grid->setContentsMargins(10, 10, 10, 10);
    QString comboStyleSheet = R"(
                              QComboBox, QComboBox:on {
                              font-family: "Times New Roman";
                              font-size: 20px;
                              font-weight: bold;
                              color: white;
                              border: none;
                              border-radius: 0px;
                              padding: 0;
                              height:30;
                              background-color: #1A3678;
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
                              )";


    int row = 0, col = 0;
    for (const auto& vp : valueParams) {
        auto* hbox = new QHBoxLayout;
        hbox->setSpacing(0);

        QLabel* lblName = new QLabel(vp.name + ":", editContainer);
        lblName->setFixedWidth(160);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QWidget* widget = nullptr;
        if (vp.name == "TubeType" || vp.name == "CTType") {
            auto* combo = new QComboBox(editContainer);
            combo->setFixedWidth(150);
            combo->setStyleSheet(comboStyleSheet);
            combo->setFont(font);

            if (vp.name == "TubeType")
                combo->addItems(TubeTypeList);
            else
                combo->addItems(CTTypeList);

            combo->setCurrentText(vp.defaultValue);
            combo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            valueCombos.insert(vp.name, combo);
            widget = combo;

        }
        else {
            auto* edit = new QLineEdit(vp.defaultValue, editContainer);
            edit->setFixedWidth(150);
            edit->setStyleSheet(UIControlPlaneStyleEdit);
            edit->setFont(font);
            edit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

            valueEdits.insert(vp.name, edit);
            widget = edit;
        }

        QLabel* lblUnit = new QLabel(vp.unit, editContainer);
        lblUnit->setFixedWidth(1);
        lblUnit->setStyleSheet(UIControlPlaneStyleString);
        lblUnit->setFont(font);
        lblUnit->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lblUnit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        hbox->setSpacing(0);
        hbox->addStretch();
        hbox->addWidget(lblName);
        hbox->addWidget(widget);
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


    auto* vbox = new QVBoxLayout(filePathContainer);
    vbox->setContentsMargins(10, 10, 10, 10);


    for (const auto& fp : fileParams) {
        auto* hbox = new QHBoxLayout;
        hbox->setSpacing(6);

        QLabel* lblName = new QLabel(fp.name + ":", filePathContainer);
        lblName->setStyleSheet(UIControlPlaneStyleString);
        lblName->setFont(font);
        lblName->setFixedWidth(220);
        lblName->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        lblName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QLineEdit* pathEdit = new QLineEdit(fp.filePath, filePathContainer);
        pathEdit->setMinimumWidth(100);
        pathEdit->setStyleSheet(UIControlPlaneStyleEdit);
        pathEdit->setFont(font);
        pathEdit->setFrame(true);
        pathEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        fileEdits.insert(fp.name, pathEdit);

        QPushButton* btnSelect = new QPushButton("...", filePathContainer);
        btnSelect->setFixedSize(60, 40);
        btnSelect->setToolTip("Browse...");
        btnSelect->setStyleSheet(BrowseStyleString);
        connect(btnSelect, &QPushButton::clicked, this, [=](){
            QString dirPath = FileDialog::getExistingDirectory(
                        UIControlPlane,
                        "Select " + fp.name,
                        QDir::homePath(),
                        QFileDialog::ShowDirsOnly |
                        QFileDialog::DontResolveSymlinks
                        );
            if (!dirPath.isEmpty())
                pathEdit->setText(dirPath);
        });

        btnSelect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        QHBoxLayout* hboxControl = new QHBoxLayout;
        hboxControl->setSpacing(6);
        hboxControl->addWidget(lblName);
        hboxControl->addWidget(pathEdit);
        hboxControl->addWidget(btnSelect);
        vbox->addLayout(hboxControl);
    }

    const QString kSpectUILabelStyle = R"(
                                       QLabel {
                                       color: white;
                                       background-color: transparent;
                                       font-family: 'Times New Roman';
                                       border: none;
                                       font-size: 22pt;
                                       }
                                       )";


    QWidget* editTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout* editTitleLayout = new QVBoxLayout(editTitleContainer);
    editTitleLayout->setContentsMargins(20, 0, 0, 0);
    QLabel* editTitleLabel = new QLabel("Parameter Settings");
    editTitleLabel->setStyleSheet(kSpectUILabelStyle);
    editTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    editTitleLayout->addWidget(editTitleLabel);

    QWidget* pathTitleContainer = new QWidget(UIControlPlane);
    QVBoxLayout* pathTitleLayout = new QVBoxLayout(pathTitleContainer);
    pathTitleLayout->setContentsMargins(20, 0, 0, 0);
    QLabel* pathTitleLabel = new QLabel("Path Settings");
    pathTitleLabel->setStyleSheet(kSpectUILabelStyle);
    pathTitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    pathTitleLayout->addWidget(pathTitleLabel);


    UIControlPlane->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    editContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    filePathContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    UIControlLayout->addWidget(editTitleContainer);
    UIControlLayout->addWidget(editContainer);
    //    UIControlLayout->addStretch();
    UIControlLayout->addWidget(pathTitleContainer);
    UIControlLayout->addWidget(filePathContainer);
    UIControlLayout->addStretch();


    auto* uiEditLayout = new QVBoxLayout(uiEditPanel);
    uiEditLayout->setContentsMargins(10,10,10,10);
    uiEditLayout->addWidget(UIControlPlane);
    uiEditPanel->setLayout(uiEditLayout);

    connect(buttonSim, &QPushButton::clicked,
            this, &ParaHome::onStartButtonClicked);
    connect(buttonNew, &QPushButton::clicked,
            this, &ParaHome::onNewButtonClicked);
    connect(buttonOpen, &QPushButton::clicked,
            this, &ParaHome::onOpenButtonClicked);

    connect(UIBtn, &QPushButton::clicked, [=]() { centralView->setCurrentIndex(0); });
    connect(AIBtn, &QPushButton::clicked, this, [=]() { ChatPanel* chat = mainWindow->getChatPanel();

        if (centralView->indexOf(chat) < 0) {
            centralView->addWidget(chat);
        }
        centralView->setCurrentWidget(chat); });
}


void ParaHome::onStartButtonClicked()
{
    QString uiProjName = valueEdits["ProjectName"]->text().trimmed();
    if (uiProjName != allInfo.ProjectName) {
        onNewButtonClicked();
    }
    bool modified = false;

#define CHECK_EDIT(key, member)                       \
    {                                                  \
    QString v = valueEdits[key]->text().trimmed();\
    if (v != allInfo.member) {                    \
    allInfo.member = v;                       \
    modified = true;                          \
}                                              \
}

#define CHECK_COMBO(key, member)                             \
    {                                                        \
    QString v = valueCombos[key]->currentText().trimmed();\
    if (v != allInfo.member) {                            \
    allInfo.member = v;                               \
    modified = true;                                  \
}                                                      \
}

    CHECK_EDIT("TubeFolder",     TubeFolder);
    CHECK_EDIT("SpectrumFolder", SpectrumFolder);
    CHECK_EDIT("DetectorFolder", DetectorFolder);
    CHECK_EDIT("CTFolder",       CTFolder);
    CHECK_EDIT("ReconFolder",    ReconFolder);
    CHECK_EDIT("MACFolder",      MACFolder);
    CHECK_COMBO("TubeType", TubeType);
    CHECK_COMBO("CTType",   CTType);


#undef CHECK_EDIT
#define CHECK_EDIT(key, member)                          \
    {                                                     \
    QString v = fileEdits[key]->text().trimmed();     \
    if (v != allInfo.member) {                       \
    allInfo.member = v;                          \
    modified = true;                             \
}                                                 \
}

    CHECK_EDIT("ProjectPath",          ProjectPath);
    CHECK_EDIT("SystemScriptPath",     SystemScriptPath);
    CHECK_EDIT("SystemMaterialPath",   SystemMaterialPath);
    CHECK_EDIT("TubeDatabasePath",     TubeDatabasePath);
    CHECK_EDIT("SpectrumDatabasePath", SpectrumDatabasePath);

#undef CHECK_EDIT
#undef CHECK_COMBO

    if (modified) {
        writeProjectInfoFile();
        showStyledWarning(
                    this,
                    "Notice",
                    "Parameters updated and project file rewritten."
                    );

        QDir projectDir(allInfo.ProjectPath + '/' + allInfo.ProjectName);
        if (!projectDir.exists()) {
            if (!QDir().mkpath(allInfo.ProjectPath)) {
                showStyledWarning(
                            this,
                            "Error",
                            QString("Failed to create project root: %1")
                            .arg(allInfo.ProjectPath)
                            );
                return;
            }
        }

        const QStringList subFolders = {
            allInfo.TubeFolder,
            allInfo.SpectrumFolder,
            allInfo.DetectorFolder,
            allInfo.CTFolder,
            allInfo.ReconFolder,
            allInfo.MACFolder
        };

        for (const QString &sub : subFolders) {
            QString fullPath = projectDir.filePath(sub);
            if (!QDir(fullPath).exists()) {
                bool ok = QDir().mkpath(fullPath);
                if (ok) {
                    qDebug() << ">>> Created folder:" << fullPath;
                } else {
                    showStyledWarning(
                                this,
                                "Error",
                                QString("Failed to create folder: %1").arg(fullPath)
                                );
                }
            }
        }
    }

    emit projectInfoReady(allInfo);
    showStyledWarning(
                this,
                "Start...",
                "The project information has been updated and transferred to other modules. \n"
                "Please click the menu bar on the left to jump to each module for subsequent simulation."
                );
    qDebug() << ">>> The project information has been updated and transferred to other modules.";
    qDebug() << ">>> Start...";

    emit projectStart();

}


void ParaHome::writeProjectInfoFile()
{
    QString infoFile = allInfo.ProjectPath
            + "/" + allInfo.ProjectName
            + "/" + allInfo.ProjectName
            + ".jlab";
    QFile file(infoFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showStyledWarning(this,
                          "Error",
                          "Unable to write configuration file: "
                          + infoFile);
        return;
    }

    QTextStream out(&file);
    out << "ProjectName "          << allInfo.ProjectName          << "\n";
    out << "TubeType "             << allInfo.TubeType             << "\n";
    out << "CTType "               << allInfo.CTType               << "\n";
    out << "TubeFolder "           << allInfo.TubeFolder           << "\n";
    out << "SpectrumFolder "       << allInfo.SpectrumFolder       << "\n";
    out << "DetectorFolder "       << allInfo.DetectorFolder       << "\n";
    out << "CTFolder "             << allInfo.CTFolder             << "\n";
    out << "ReconFolder "          << allInfo.ReconFolder          << "\n";
    out << "MACFolder "            << allInfo.MACFolder            << "\n";
    out << "ProjectPath "          << allInfo.ProjectPath          << "\n";
    out << "SystemScriptPath "     << allInfo.SystemScriptPath     << "\n";
    out << "SystemMaterialPath "   << allInfo.SystemMaterialPath   << "\n";
    out << "TubeDatabasePath "     << allInfo.TubeDatabasePath     << "\n";
    out << "SpectrumDatabasePath " << allInfo.SpectrumDatabasePath << "\n";
    file.close();
}


void ParaHome::onNewButtonClicked()
{
    allInfo.ProjectName          = valueEdits["ProjectName"]->text();
    allInfo.TubeType             = valueCombos["TubeType"]->currentText();
    allInfo.CTType               = valueCombos["CTType"]->currentText();
    allInfo.TubeFolder           = valueEdits["TubeFolder"]->text();
    allInfo.SpectrumFolder       = valueEdits["SpectrumFolder"]->text();
    allInfo.DetectorFolder       = valueEdits["DetectorFolder"]->text();
    allInfo.CTFolder             = valueEdits["CTFolder"]->text();
    allInfo.ReconFolder          = valueEdits["ReconFolder"]->text();
    allInfo.MACFolder            = valueEdits["MACFolder"]->text();

    allInfo.ProjectPath          = fileEdits["ProjectPath"]->text();
    allInfo.SystemScriptPath     = fileEdits["SystemScriptPath"]->text();
    allInfo.SystemMaterialPath   = fileEdits["SystemMaterialPath"]->text();
    allInfo.TubeDatabasePath     = fileEdits["TubeDatabasePath"]->text();
    allInfo.SpectrumDatabasePath = fileEdits["SpectrumDatabasePath"]->text();

    QDir baseDir(allInfo.ProjectPath);
    if (!baseDir.exists()) {
        if (!baseDir.mkpath(".")) {
            showStyledWarning(this, "Error", "Unable to create project root directory:" + allInfo.ProjectPath);
            return;
        }
    }

    QString projDirPath = allInfo.ProjectPath + "/" + allInfo.ProjectName;

    if (QDir(projDirPath).exists()) {
        showStyledWarning(
                    this,
                    "Warning",
                    "Project already exists. Configuration file may be overwritten.\n"
                    "Please change the project name to avoid overwriting the existing project."
                    );
        //        return;
    }

    QDir projDir;
    if (!projDir.mkpath(projDirPath)) {
        showStyledWarning(this, "Error", "Unable to create project directory:" + projDirPath);
        return;
    }

    const QStringList subFolders = {
        allInfo.TubeFolder,
        allInfo.SpectrumFolder,
        allInfo.DetectorFolder,
        allInfo.CTFolder,
        allInfo.ReconFolder,
        allInfo.MACFolder
    };
    for (const QString &sub : subFolders) {
        projDir.mkpath(projDirPath + "/" + sub);
    }


    QString infoFile = projDirPath + "/" + allInfo.ProjectName + ".jlab";
    QFile file(infoFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        showStyledWarning(this, "Error", "Unable to write configuration file:" + infoFile);
        return;
    }

    QTextStream out(&file);
    out << "ProjectName "          << allInfo.ProjectName          << "\n";
    out << "TubeType "             << allInfo.TubeType             << "\n";
    out << "CTType "               << allInfo.CTType               << "\n";
    out << "TubeFolder "           << allInfo.TubeFolder           << "\n";
    out << "SpectrumFolder "       << allInfo.SpectrumFolder       << "\n";
    out << "DetectorFolder "       << allInfo.DetectorFolder       << "\n";
    out << "CTFolder "             << allInfo.CTFolder             << "\n";
    out << "ReconFolder "          << allInfo.ReconFolder          << "\n";
    out << "MACFolder "            << allInfo.MACFolder            << "\n";
    out << "ProjectPath "          << allInfo.ProjectPath          << "\n";
    out << "SystemScriptPath "     << allInfo.SystemScriptPath     << "\n";
    out << "SystemMaterialPath "   << allInfo.SystemMaterialPath   << "\n";
    out << "TubeDatabasePath "     << allInfo.TubeDatabasePath     << "\n";
    out << "SpectrumDatabasePath " << allInfo.SpectrumDatabasePath << "\n";
    file.close();

    showStyledWarning(this,
                      "Success",
                      "Successfully created project:" + infoFile);
}



void ParaHome::onOpenButtonClicked()
{
    QString jlabFilter = "JLab Files (*.jlab);;All Files (*)";

    QString fileName = FileDialog::getOpenFileName(
                this,
                "Open Project",
                fileEdits["ProjectPath"]->text(),
            jlabFilter
            );

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        showStyledWarning(this,
                          "Error",
                          "Unable to open project file: " + fileName);
        return;
    }

    QMap<QString, QString> kv;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;
        int idx = line.indexOf(' ');
        if (idx < 0)
            continue;

        QString key   = line.left(idx);
        QString value = line.mid(idx + 1).trimmed();
        kv[key] = value;
    }
    file.close();

    allInfo.ProjectName          = kv.value("ProjectName");
    allInfo.TubeType             = kv.value("TubeType");
    allInfo.CTType               = kv.value("CTType");
    allInfo.TubeFolder           = kv.value("TubeFolder");
    allInfo.SpectrumFolder       = kv.value("SpectrumFolder");
    allInfo.DetectorFolder       = kv.value("DetectorFolder");
    allInfo.CTFolder             = kv.value("CTFolder");
    allInfo.ReconFolder          = kv.value("ReconFolder");
    allInfo.MACFolder            = kv.value("MACFolder");

    allInfo.ProjectPath          = kv.value("ProjectPath");
    allInfo.SystemScriptPath     = kv.value("SystemScriptPath");
    allInfo.SystemMaterialPath   = kv.value("SystemMaterialPath");
    allInfo.TubeDatabasePath     = kv.value("TubeDatabasePath");
    allInfo.SpectrumDatabasePath = kv.value("SpectrumDatabasePath");

    valueEdits["ProjectName"]->setText(allInfo.ProjectName);
    valueCombos["TubeType"]->setCurrentText(allInfo.TubeType);
    valueCombos["CTType"]->setCurrentText(allInfo.CTType);
    valueEdits["TubeFolder"]->setText(allInfo.TubeFolder);
    valueEdits["SpectrumFolder"]->setText(allInfo.SpectrumFolder);
    valueEdits["DetectorFolder"]->setText(allInfo.DetectorFolder);
    valueEdits["CTFolder"]->setText(allInfo.CTFolder);
    valueEdits["ReconFolder"]->setText(allInfo.ReconFolder);
    valueEdits["MACFolder"]->setText(allInfo.MACFolder);

    fileEdits["ProjectPath"]->setText(allInfo.ProjectPath);
    fileEdits["SystemScriptPath"]->setText(allInfo.SystemScriptPath);
    fileEdits["SystemMaterialPath"]->setText(allInfo.SystemMaterialPath);
    fileEdits["TubeDatabasePath"]->setText(allInfo.TubeDatabasePath);
    fileEdits["SpectrumDatabasePath"]->setText(allInfo.SpectrumDatabasePath);

    showStyledWarning(this,
                      "Success",
                      "Successfully opened project:" + fileName);
}







void ParaHome::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

}

void ParaHome::setMainWindow(MainWindow* main) {
    mainWindow = main;
}

void ParaHome::paintEvent(QPaintEvent *event)
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

void ParaHome::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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

void ParaHome::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
}

void ParaHome::PlaneRunFunction(const QString &targetPath,
                                const ProjectInfo &info)
{
    //    qDebug()<<targetPath;
    Q_UNUSED(targetPath)
    Q_UNUSED(info)

    if (!mainWindow)
        return;

    QString cmdStart = "echo [ProgressSTART][Name:Terminal][MAX:2000]";
    QString cmd      = "for i in {1..2000}; do echo iiCTs:$i; sleep 0.5; done;";

    TerminalWidget *term = mainWindow->terminal();
    term->appendLine(cmdStart);
    term->runCommandExternally(cmd);
}



