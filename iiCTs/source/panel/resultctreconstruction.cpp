#include "resultctreconstruction.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QButtonGroup>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QTransform>
#include <QProcess>
#include <QDebug>
#include <QFormLayout>
#include <QtEndian>
#include <QRegularExpressionValidator>
#include <QTransform>

#include "verticalbutton.h"
#include "verticallabel.h"
#include "imagecontainer.h"
#include "imagecontainerwidget.h"
#include "terminalwidget.h"
#include "mainwindow.h"

ResultCTReconstruction::ResultCTReconstruction(QWidget *parent)
    : QWidget(parent), transform_()
{
    dimH_ = 0;
    dimW_ = 0;
    dimN_ = 0;
    currentLayer_ = 1;
    suffix_ = ".ct";
    currentPlane_ = "axial";
    setupUi();
}

void ResultCTReconstruction::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    auto *topLayout = new QHBoxLayout;
    topLayout->addStretch();

    QString radioStyle = R"(
                         QRadioButton {
                         font-family: "Times New Roman";
                         font-size: 16px;
                         color: white;
                         border: none;
                         }

                         QRadioButton::indicator {
                         width: 14px;
                         height: 14px;
                         border: 2px solid white;
                         border-radius: 8px;
                         background-color: transparent;
                         }

                         QRadioButton::indicator:checked {
                         background-color: white;
                         border: 2px solid white;
                         }

                         QRadioButton::indicator:hover {
                         border: 2px solid #41A1F5;
                         }
                         )";
    aiButton_ = new QRadioButton("AI");
    phButton_ = new QRadioButton("PH");
    aiButton_->setStyleSheet(radioStyle);
    phButton_->setStyleSheet(radioStyle);
    phButton_->setChecked(true);
    modeGroup_ = new QButtonGroup(this);
    modeGroup_->addButton(aiButton_, 0);
    modeGroup_->addButton(phButton_, 1);
    connect(modeGroup_, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &ResultCTReconstruction::onModeChanged);

    QFrame *apFrame = new QFrame(this);
    apFrame->setFrameShape(QFrame::Box);
    apFrame->setStyleSheet(R"(
                           QFrame {
                           border: 2px solid #7697D8;
                           border-radius: 12px;
                           background-color: transparent;
                           padding: 2px;
                           }
                           )");

    QHBoxLayout *apLayout = new QHBoxLayout(apFrame);
    apLayout->addWidget(aiButton_);
    apLayout->addWidget(phButton_);
    apLayout->setSpacing(20);
    apLayout->setContentsMargins(10, 0, 10, 0);

    QString buttonGRStyle = R"(
                            QPushButton {
                            color: white;
                            font-family: "Times New Roman";
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
    genButton_ = new QPushButton("Gen");
    refreshButton_ = new QPushButton("Refresh");
    genButton_->setStyleSheet(buttonGRStyle);
    refreshButton_->setStyleSheet(buttonGRStyle);
    genButton_->setFixedSize(60, 30);
    refreshButton_->setFixedSize(90, 30);
    connect(genButton_, &QPushButton::clicked, this, &ResultCTReconstruction::onGenClicked);
    connect(refreshButton_, &QPushButton::clicked, this, &ResultCTReconstruction::onRefreshClicked);

    topLayout->addWidget(apFrame);
    topLayout->addWidget(genButton_);
    topLayout->addWidget(refreshButton_);
    mainLayout->addLayout(topLayout);

    auto *middleLayout = new QHBoxLayout;
    middleLayout->setSpacing(5);

    QString leftBtnStyle = R"(
                           QPushButton {
                           color: white;
                           font-family: "Times New Roman";
                           font-size: 16pt;
                           background-color: #3366CC;
                           font-weight: bold;
                           border-radius: 1px;
                           border: 1px solid #2F528F;
                           width:100;
                           height:30;
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
    QString leftLabelStyle = R"(
                             QPushButton {
                             color: white;
                             font-family: "Times New Roman";
                             font-size: 16pt;
                             background-color: transparent;
                             font-weight: bold;
                             border: none;
                             width:70;
                             height:30;
                             }
                             )";
    QString leftEditStyle = R"(
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

    auto *planeLayout = new QVBoxLayout;
    planeLayout->setContentsMargins(0, 20, 0, 20);
    planeLayout->setSpacing(0);

    auto *axialBtn = new VerticalPushButton("Axial");
    auto *coronalBtn = new VerticalPushButton("Coronal");
    auto *sagittalBtn = new VerticalPushButton("Sagittal");
    auto *indexLabel = new VerticalPushButton("Index:");
    indexEdit_ = new QLineEdit;
    axialBtn->setStyleSheet(leftBtnStyle);
    coronalBtn->setStyleSheet(leftBtnStyle);
    sagittalBtn->setStyleSheet(leftBtnStyle);
    indexLabel->setStyleSheet(leftLabelStyle);
    indexEdit_->setStyleSheet(leftEditStyle);
    indexEdit_->setFixedWidth(30);
    indexEdit_->setText("1");

    axialBtn->setCheckable(true);
    coronalBtn->setCheckable(true);
    sagittalBtn->setCheckable(true);

    auto *leftGroup = new QButtonGroup(this);
    leftGroup->setExclusive(true);
    leftGroup->addButton(axialBtn, 0);
    leftGroup->addButton(coronalBtn, 1);
    leftGroup->addButton(sagittalBtn, 2);
    axialBtn->setChecked(true);

    connect(axialBtn, &QPushButton::clicked, this, &ResultCTReconstruction::onPlaneClicked);
    connect(coronalBtn, &QPushButton::clicked, this, &ResultCTReconstruction::onPlaneClicked);
    connect(sagittalBtn, &QPushButton::clicked, this, &ResultCTReconstruction::onPlaneClicked);
    connect(indexEdit_, &QLineEdit::editingFinished, this, &ResultCTReconstruction::indexEditChange);

    planeLayout->addWidget(axialBtn);
    planeLayout->addWidget(coronalBtn);
    planeLayout->addWidget(sagittalBtn);
    planeLayout->addSpacing(20);
    planeLayout->addWidget(indexLabel);
    planeLayout->addWidget(indexEdit_);
    planeLayout->addStretch();
    middleLayout->addLayout(planeLayout);

    layerSlider_ = new QSlider(Qt::Vertical);
    layerSlider_->setInvertedAppearance(true);
    layerSlider_->setInvertedControls(true);
    layerSlider_->setRange(1, 1);
    layerSlider_->setValue(1);
    layerSlider_->setStyleSheet(R"(
                                QSlider::groove:vertical {
                                border: 1px solid #21438F;
                                background: #1A3678;
                                width: 10px;
                                border-radius: 5px;
                                margin: 0px;
                                }

                                QSlider::handle:vertical {
                                background: #3961B4;
                                border: 1px solid #21438F;
                                height: 20px;
                                margin: 0 -4px; /* handle overlaps the groove */
                                border-radius: 5px;
                                }

                                QSlider::add-page:vertical, QSlider::sub-page:vertical {
                                background: #21438F;
                                border-radius: 5px;
                                }
                                )");

    connect(layerSlider_, &QSlider::valueChanged,
            this, &ResultCTReconstruction::onSliderValueChanged);

    imageLabel_ = new QLabel;
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setStyleSheet(R"(
                               background-color: black;
                               border: none;
                               )");
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    originalPixmap_ = QPixmap(":/images/background/CTISS_logo.png");
    imageLabel_->setPixmap(originalPixmap_.scaled(imageLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    imageContainer = new ImageContainerWidget(this, layerSlider_);
    imageContainer->setObjectName("imgContainer");
    imageContainer->setAttribute(Qt::WA_StyledBackground, true);
    imageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageContainer->setStyleSheet(R"(
                                  #imgContainer {
                                  background-color: #1A3678;
                                  border: 2px solid #1A3678;
                                  border-radius: 5px;
                                  }
                                  )");
    imageContainer->setContentsMargins(5, 5, 5, 5);
    auto *imgLay = new QVBoxLayout(imageContainer);
    imgLay->addWidget(imageLabel_);

    middleLayout->addWidget(imageContainer, /*stretch=*/1);
    middleLayout->addWidget(layerSlider_);

    mainLayout->addLayout(middleLayout);

    QString bottStyleEdit = R"(
                            QLineEdit {
                            font-family: "Times New Roman";
                            color: white;
                            font-size: 12pt;
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
    QString wlStyle = R"(
                      color: white;
                      font-family: "Times New Roman";
                      font-size: 12pt;
                      background-color: transparent;
                      font-weight: bold;
                      border: none;
                      width:80;
                      )";

    auto *bottomLayout = new QHBoxLayout;
    bottomLayout->addStretch();
    bottomLayout->setSpacing(10);

    auto regex = QRegularExpression("^-?\\d+(\\.\\d+)?$");
    auto *regValidator = new QRegularExpressionValidator(regex, this);

    auto *leftForm = new QFormLayout;
    leftForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    auto *widthLabel = new QLabel(tr("Window Width:"));
    auto *levelLabel = new QLabel(tr("Window Level:"));
    windowWidthEdit_ = new QLineEdit("400");
    windowLevelEdit_ = new QLineEdit("40");
    windowWidthEdit_->setValidator(regValidator);
    windowLevelEdit_->setValidator(regValidator);
    widthLabel->setStyleSheet(wlStyle);
    levelLabel->setStyleSheet(wlStyle);
    windowWidthEdit_->setFixedWidth(80);
    windowLevelEdit_->setFixedWidth(80);
    windowWidthEdit_->setStyleSheet(bottStyleEdit);
    windowLevelEdit_->setStyleSheet(bottStyleEdit);
    leftForm->addRow(levelLabel, windowLevelEdit_);
    leftForm->addRow(widthLabel, windowWidthEdit_);

    connect(windowWidthEdit_, &QLineEdit::editingFinished,
            this, &ResultCTReconstruction::widthWinChanged);
    connect(windowLevelEdit_, &QLineEdit::editingFinished,
            this, &ResultCTReconstruction::levelWinChanged);
    bottomLayout->addLayout(leftForm);

    auto *rightForm = new QFormLayout;
    rightForm->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    auto *maxLabel = new QLabel(tr("Window Max:"));
    auto *minLabel = new QLabel(tr("Window Min:"));
    dispMinEdit_ = new QLineEdit("-160");
    dispMaxEdit_ = new QLineEdit("240");
    dispMinEdit_->setValidator(regValidator);
    dispMaxEdit_->setValidator(regValidator);
    maxLabel->setStyleSheet(wlStyle);
    minLabel->setStyleSheet(wlStyle);
    dispMinEdit_->setStyleSheet(bottStyleEdit);
    dispMaxEdit_->setStyleSheet(bottStyleEdit);
    dispMinEdit_->setFixedWidth(80);
    dispMaxEdit_->setFixedWidth(80);
    rightForm->addRow(minLabel, dispMinEdit_);
    rightForm->addRow(maxLabel, dispMaxEdit_);
    connect(dispMinEdit_, &QLineEdit::editingFinished,
            this, &ResultCTReconstruction::minWinChanged);
    connect(dispMaxEdit_, &QLineEdit::editingFinished,
            this, &ResultCTReconstruction::maxWinChanged);
    bottomLayout->addLayout(rightForm);

    QString buttonBotStyle = R"(
                             QPushButton {
                             color: white;
                             font-family: "Times New Roman";
                             font-size: 14pt;
                             background-color: #3366CC;
                             font-weight: bold;
                             border-radius: 5px;
                             border: 1px solid #2F528F;
                             width:80;

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
    auto *grid = new QGridLayout;

    flipVertButton_ = new QPushButton("FV");
    flipHorzButton_ = new QPushButton("FH");

    rotateCWButton_ = new QPushButton("RCW");
    rotateCCWButton_ = new QPushButton("RCCW");

    flipVertButton_->setStyleSheet(buttonBotStyle);
    flipHorzButton_->setStyleSheet(buttonBotStyle);
    rotateCWButton_->setStyleSheet(buttonBotStyle);
    rotateCCWButton_->setStyleSheet(buttonBotStyle);

    connect(flipVertButton_, &QPushButton::clicked,
            this, &ResultCTReconstruction::onFlipVert);
    connect(flipHorzButton_, &QPushButton::clicked,
            this, &ResultCTReconstruction::onFlipHorz);
    connect(rotateCWButton_, &QPushButton::clicked,
            this, &ResultCTReconstruction::onRotateCW);
    connect(rotateCCWButton_, &QPushButton::clicked,
            this, &ResultCTReconstruction::onRotateCCW);

    grid->addWidget(flipVertButton_, 0, 0);
    grid->addWidget(flipHorzButton_, 1, 0);
    grid->addWidget(rotateCWButton_, 0, 1);
    grid->addWidget(rotateCCWButton_, 1, 1);
    bottomLayout->addSpacing(30);
    bottomLayout->addLayout(grid);

    auto *resetBtn = new QPushButton("Reset", this);
    connect(resetBtn, &QPushButton::clicked,
            this, &ResultCTReconstruction::onResetTransform);
    resetBtn->setStyleSheet(buttonBotStyle);

    auto *rangeBtn = new QPushButton("Range", this);
    connect(rangeBtn, &QPushButton::clicked,
            this, &ResultCTReconstruction::onRange);
    rangeBtn->setStyleSheet(buttonBotStyle);

    QVBoxLayout *btnRightLayout = new QVBoxLayout;
    btnRightLayout->addWidget(resetBtn);
    btnRightLayout->addWidget(rangeBtn);
    //    btnRightLayout->setSpacing(20);
    //    btnRightLayout->setContentsMargins(10, 0, 10, 0);

    bottomLayout->addLayout(btnRightLayout);
    bottomLayout->addStretch();

    mainLayout->addLayout(bottomLayout);
}

void ResultCTReconstruction::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
}

void ResultCTReconstruction::scriptCTChanged(const QString &filePath)
{
    scriptCT_ = filePath;
    scriptCTs = parseVariableFile(filePath);
}

void ResultCTReconstruction::scriptChanged(const QString &filePath)
{
    script_ = filePath;
    scripts = parseVariableFile(filePath);

    volumeFolder_ = getVariableValue(scripts, "OutputPath");
    volumeFileBase_ = getVariableValue(scripts, "OutputFilename");
    dimH_ = getVariableValue(scripts, "PixelCountX").toInt();
    dimW_ = getVariableValue(scripts, "PixelCountY").toInt();
}

bool ResultCTReconstruction::readVolumeFile()
{
    qDebug() << ">>> Loading data...";
    QString path = volumeFolder_ + QDir::separator() + volumeFileBase_ + suffix_;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        showStyledWarning(this,
                          "Load Error",
                          QString("Cannot open volume file:\n%1").arg(path));
        originalPixmap_ = QPixmap(":/images/CT/CT.PNG");
        imageLabel_->setPixmap(
            originalPixmap_
                .scaled(imageLabel_->size(),
                        Qt::KeepAspectRatio,
                        Qt::SmoothTransformation));
        return false;
    }

    qint64 fileSize = file.size();
    qint64 sliceBytes = qint64(dimH_) * dimW_ * sizeof(float);
    if (sliceBytes <= 0 || fileSize % sliceBytes != 0)
    {
        showStyledWarning(this,
                          "Load Error",
                          "Invalid volume dimensions or corrupted file.");
        file.close();
        return false;
    }

    dimN_ = fileSize / sliceBytes;
    qint64 voxelCount = qint64(dimH_) * dimW_ * dimN_;
    volume_.resize(voxelCount);

    char *dataPtr = reinterpret_cast<char *>(volume_.data());
    qint64 bytesToRead = voxelCount * qint64(sizeof(float));
    qint64 got = file.read(dataPtr, bytesToRead);
    if (got != bytesToRead)
    {
        showStyledWarning(this,
                          "Load Error",
                          "Failed reading volume data.");
        file.close();
        return false;
    }
    file.close();

    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        quint32 *buf = reinterpret_cast<quint32 *>(volume_.data());
        for (qint64 i = 0; i < voxelCount; ++i)
        {
            buf[i] = qFromLittleEndian<quint32>(buf[i]);
        }
    }

    updateSliderRange();
    updateImageDisplay();
    return true;
}

void ResultCTReconstruction::setMainWindow(MainWindow *main)
{
    mainWindow = main;
}

bool ResultCTReconstruction::generateVolume()
{
    QProcess proc;
    QStringList args;
    args << volumeFolder_ << volumeFileBase_ << (modeGroup_->checkedId() == 0 ? "ai" : "ph");
    proc.start(script_, args, QIODevice::ReadOnly);

    if (!proc.waitForFinished(-1) || proc.exitStatus() != QProcess::NormalExit)
    {
        if (!mainWindow)
        {
            return false;
        }

        QString cmdStart = "echo [ProgressSTART][Name:AiGen][MAX:20]";
        QString cmd = "for i in {1..20}; do echo CTISS:$i; sleep 0.5; done;";

        TerminalWidget *term = mainWindow->terminal();
        term->appendLine(script_);
        term->appendLine(scriptCT_);
        term->appendLine(cmdStart);
        term->runCommandExternally(cmd);

        showStyledWarning(this, "Gen Error", "Reconstruction script failed to run.");
        return false;
    }
    return readVolumeFile();
}

void ResultCTReconstruction::updateSliderRange()
{
    int maxLayer = 1;
    if (currentPlane_ == "axial")
    {
        maxLayer = dimN_;
        layerSlider_->setRange(1, qMax(1, maxLayer));

        currentLayer_ = 10;
        layerSlider_->setValue(currentLayer_);
        indexEdit_->setText(QString::number(currentLayer_));
    }
    else if (currentPlane_ == "coronal")
    {
        maxLayer = dimH_;
        layerSlider_->setRange(1, qMax(1, maxLayer));

        currentLayer_ = qRound(static_cast<double>(dimH_) / 2.0);
        layerSlider_->setValue(currentLayer_);
        indexEdit_->setText(QString::number(currentLayer_));
    }
    else if (currentPlane_ == "sagittal")
    {
        maxLayer = dimW_;
        layerSlider_->setRange(1, qMax(1, maxLayer));

        currentLayer_ = qRound(static_cast<double>(dimW_) / 2.0);
        layerSlider_->setValue(currentLayer_);
        indexEdit_->setText(QString::number(currentLayer_));
    }

    updateImageDisplay();
}

void ResultCTReconstruction::updateImageDisplay()
{
    if (volume_.isEmpty())
        return;

    double dmin = dispMinEdit_->text().toDouble();
    double dmax = dispMaxEdit_->text().toDouble();
    if (dmax <= dmin)
        return;

    double scale = 255.0 / (dmax - dmin);

    // determine slice dimensions
    int w = dimW_, h = dimH_, d = dimN_;
    QVector<float> slice;
    slice.reserve(w * h);

    if (currentPlane_ == "axial")
    {
        int z = currentLayer_ - 1;
        for (int y = 0; y < h; ++y)
            for (int x = 0; x < w; ++x)
                slice.append(volume_[z * w * h + y * w + x]);
    }
    else if (currentPlane_ == "coronal")
    {
        int y0 = currentLayer_ - 1;
        for (int z = 0; z < d; ++z)
            for (int x = 0; x < w; ++x)
                slice.append(volume_[z * w * h + y0 * w + x]);
        h = d;
    }
    else
    { // sagittal
        int x0 = currentLayer_ - 1;
        for (int z = 0; z < d; ++z)
            for (int y = 0; y < h; ++y)
                slice.append(volume_[z * w * h + y * w + x0]);
        w = h;
        h = d;
    }

    QImage img(w, h, QImage::Format_Indexed8);
    QVector<QRgb> ct;
    for (int i = 0; i < 256; ++i)
        ct.append(qRgb(i, i, i));
    img.setColorTable(ct);

    const float *data = slice.constData();
    for (int i = 0, n = w * h; i < n; ++i)
    {
        double clamped = qBound(dmin, double(data[i]), dmax);
        double val = (clamped - dmin) * scale;
        int pv = qBound(0, int(val), 255);
        img.bits()[i] = static_cast<uchar>(pv);
    }

    QTransform trans(0, 1,
                     1, 0,
                     0, 0);
    QImage transposed = img.transformed(trans,
                                        Qt::FastTransformation);

    currentImage_ = transposed;
    originalPixmap_ = QPixmap::fromImage(currentImage_);
    QPixmap finalPixmap = originalPixmap_
                              .transformed(transform_, Qt::SmoothTransformation)
                              .scaled(imageLabel_->size(),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation);

    imageLabel_->setPixmap(finalPixmap);
}

void ResultCTReconstruction::onModeChanged(int id)
{
    suffix_ = (id == 0 ? ".ai.ct" : ".ct");
}

void ResultCTReconstruction::onGenClicked()
{
    if (generateVolume())
        showStyledWarning(this, "Success", "Volume generated.");
}

void ResultCTReconstruction::onRefreshClicked()
{
    if (readVolumeFile())
        showStyledWarning(this, "Refreshed", "Volume reloaded.");
}

void ResultCTReconstruction::onPlaneClicked()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (!btn)
        return;

    QString text = btn->text();
    if (text.contains("Axial"))
        currentPlane_ = "axial";
    else if (text.contains("Coronal"))
        currentPlane_ = "coronal";
    else if (text.contains("Sagittal"))
        currentPlane_ = "sagittal";

    updateSliderRange();
    updateImageDisplay();
}

void ResultCTReconstruction::indexEditChange()
{
    bool ok;
    int val = indexEdit_->text().toInt(&ok);
    if (ok && val >= layerSlider_->minimum() && val <= layerSlider_->maximum())
    {
        layerSlider_->setValue(val);
    }
    else if (ok && val > layerSlider_->maximum())
    {
        layerSlider_->setValue(layerSlider_->maximum());
        indexEdit_->setText(QString::number(layerSlider_->maximum()));
    }
    else if (ok && val < layerSlider_->minimum())
    {
        layerSlider_->setValue(layerSlider_->minimum());
        indexEdit_->setText(QString::number(layerSlider_->minimum()));
    }
    updateImageDisplay();
}

void ResultCTReconstruction::onSliderValueChanged(int v)
{
    currentLayer_ = v;
    indexEdit_->setText(QString::number(v));
    updateImageDisplay();
}

void ResultCTReconstruction::maxWinChanged()
{

    QString maxStr = dispMaxEdit_->text().trimmed();
    QString minStr = dispMinEdit_->text().trimmed();
    bool okDispMax = false;
    bool okDispMin = false;
    double dispMax = maxStr.toDouble(&okDispMax);
    double dispMin = minStr.toDouble(&okDispMin);
    if (!(okDispMax && okDispMin))
    {
        return;
    }

    if (dispMax <= dispMin)
    {
        QString levelStr = windowLevelEdit_->text().trimmed();
        QString widthStr = windowWidthEdit_->text().trimmed();
        bool okLevel = false;
        bool okWidth = false;
        double windowLevel = levelStr.toDouble(&okLevel);
        double windowWidth = widthStr.toDouble(&okWidth);

        double maxWIn = windowLevel + windowWidth / 2;
        dispMaxEdit_->setText(QString::number(qRound(maxWIn)));
        return;
    }

    double level = (dispMax + dispMin) / 2;
    double width = dispMax - dispMin;

    windowLevelEdit_->setText(QString::number(qRound(level)));
    windowWidthEdit_->setText(QString::number(qRound(width)));

    updateImageDisplay();
}

void ResultCTReconstruction::minWinChanged()
{
    QString maxStr = dispMaxEdit_->text().trimmed();
    QString minStr = dispMinEdit_->text().trimmed();
    bool okDispMax = false;
    bool okDispMin = false;
    double dispMax = maxStr.toDouble(&okDispMax);
    double dispMin = minStr.toDouble(&okDispMin);
    if (!(okDispMax && okDispMin))
    {
        return;
    }

    if (dispMax <= dispMin)
    {
        QString levelStr = windowLevelEdit_->text().trimmed();
        QString widthStr = windowWidthEdit_->text().trimmed();
        bool okLevel = false;
        bool okWidth = false;
        double windowLevel = levelStr.toDouble(&okLevel);
        double windowWidth = widthStr.toDouble(&okWidth);

        double minWIn = windowLevel - windowWidth / 2;
        dispMinEdit_->setText(QString::number(qRound(minWIn)));
        return;
    }

    double level = (dispMax + dispMin) / 2;
    double width = dispMax - dispMin;

    windowLevelEdit_->setText(QString::number(qRound(level)));
    windowWidthEdit_->setText(QString::number(qRound(width)));

    updateImageDisplay();
}

void ResultCTReconstruction::levelWinChanged()
{
    QString levelStr = windowLevelEdit_->text().trimmed();
    QString widthStr = windowWidthEdit_->text().trimmed();
    bool okLevel = false;
    bool okWidth = false;
    double windowLevel = levelStr.toDouble(&okLevel);
    double windowWidth = widthStr.toDouble(&okWidth);
    if (!(okLevel && okWidth))
    {
        return;
    }

    double maxWIn = windowLevel + windowWidth / 2;
    double minWIn = windowLevel - windowWidth / 2;

    dispMaxEdit_->setText(QString::number(qRound(maxWIn)));
    dispMinEdit_->setText(QString::number(qRound(minWIn)));

    updateImageDisplay();
}

void ResultCTReconstruction::widthWinChanged()
{
    QString levelStr = windowLevelEdit_->text().trimmed();
    QString widthStr = windowWidthEdit_->text().trimmed();
    bool okLevel = false;
    bool okWidth = false;
    double windowLevel = levelStr.toDouble(&okLevel);
    double windowWidth = widthStr.toDouble(&okWidth);
    if (!(okLevel && okWidth))
    {
        return;
    }
    if (windowWidth <= 0)
    {
        QString maxStr = dispMaxEdit_->text().trimmed();
        QString minStr = dispMinEdit_->text().trimmed();
        bool okDispMax = false;
        bool okDispMin = false;
        double dispMax = maxStr.toDouble(&okDispMax);
        double dispMin = minStr.toDouble(&okDispMin);

        double width = dispMax - dispMin;
        windowWidthEdit_->setText(QString::number(qRound(width)));
        return;
    }

    double maxWIn = windowLevel + windowWidth / 2;
    double minWIn = windowLevel - windowWidth / 2;

    dispMaxEdit_->setText(QString::number(qRound(maxWIn)));
    dispMinEdit_->setText(QString::number(qRound(minWIn)));

    updateImageDisplay();
}

void ResultCTReconstruction::onRange()
{
    if (volume_.isEmpty())
        return;

    auto mm = std::minmax_element(volume_.constBegin(), volume_.constEnd());
    float minVal = *mm.first;
    float maxVal = *mm.second;

    double level = (minVal + maxVal) / 2;
    double width = maxVal - minVal;

    dispMaxEdit_->setText(QString::number(qRound(maxVal)));
    dispMinEdit_->setText(QString::number(qRound(minVal)));
    windowLevelEdit_->setText(QString::number(qRound(level)));
    windowWidthEdit_->setText(QString::number(qRound(width)));

    updateImageDisplay();
}

void ResultCTReconstruction::onFlipVert()
{
    transform_.scale(1, -1);
    updateImageDisplay();
}

void ResultCTReconstruction::onFlipHorz()
{
    transform_.scale(-1, 1);
    updateImageDisplay();
}

void ResultCTReconstruction::onRotateCW()
{
    transform_.rotate(90);
    updateImageDisplay();
}

void ResultCTReconstruction::onRotateCCW()
{
    transform_.rotate(-90);
    updateImageDisplay();
}

void ResultCTReconstruction::onResetTransform()
{
    transform_.reset();
    updateImageDisplay();
}

void ResultCTReconstruction::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);
    if (originalPixmap_.isNull())
        return;
    QSize target = imageLabel_->size();
    imageLabel_->setPixmap(
        originalPixmap_.scaled(
            target,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
}

void ResultCTReconstruction::paintEvent(QPaintEvent *event)
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

QVector<VariableEntry> ResultCTReconstruction::parseVariableFile(const QString &filePath)
{
    QVector<VariableEntry> variables;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        showStyledWarning(this, "Warning", "An error occurred while trying to read the file: " + filePath);
        return variables;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
        {
            continue;
        }

        QString comment;
        int commentIndex = line.indexOf('#');
        if (commentIndex != -1)
        {
            comment = line.mid(commentIndex + 1).trimmed();
            line = line.left(commentIndex).trimmed();
        }

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2)
        {
            VariableEntry entry;
            entry.name = parts[0];
            entry.value = parts[1];
            entry.comment = comment;
            variables.append(entry);
        }
    }

    file.close();
    return variables;
}

QString ResultCTReconstruction::getVariableValue(const QVector<VariableEntry> &variables, const QString &name)
{
    for (const auto &entry : variables)
    {
        if (entry.name == name)
        {
            return entry.value;
        }
    }
    return QString();
}

void ResultCTReconstruction::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText)
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
