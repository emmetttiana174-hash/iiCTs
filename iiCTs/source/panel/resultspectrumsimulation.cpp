#include "resultspectrumsimulation.h"
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
#include <QtMath>
#include <QRegularExpressionValidator>
#include <QFile>
#include <QTextStream>
#include <QtCharts>
#include <QImage>
#include <QMouseEvent>
#include <cfloat>
#include <cstdlib>
#include <clocale>
#include <QFontMetrics>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QtGui/QPen>
#include <QtCharts/QLegendMarker>


#include "terminalwidget.h"
#include "mainwindow.h"

using namespace QtCharts;

ResultSpectrumSimulation::ResultSpectrumSimulation(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
    dimX_ = -1;
    imageLabel_->installEventFilter(this);
}

void ResultSpectrumSimulation::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10,10,10,10);

    auto *topLayout = new QHBoxLayout;


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
    densityButton_  = new QPushButton("Density");
    spectButton_ = new QPushButton("Spec");
    subSpectButton_ = new QPushButton("Sub Spec");
    for (auto btn: { densityButton_, spectButton_, subSpectButton_ }) {
        btn->setCheckable(true);
        btn->setStyleSheet(buttonGRStyle);
        btn->setFixedSize(110, 35);
        //        btn->setChecked(true);
    }
    densityButton_->setChecked(true);

    auto modeGroup = new QButtonGroup(this);
    modeGroup->setExclusive(true);
    enum Mode { ModeDensity = 0, ModeSpec = 1, ModeSubSpec = 2 };
    modeGroup->addButton(densityButton_,  ModeDensity);
    modeGroup->addButton(spectButton_,    ModeSpec);
    modeGroup->addButton(subSpectButton_, ModeSubSpec);

    connect(modeGroup,
            QOverload<int,bool>::of(&QButtonGroup::buttonToggled),
            this,
            &ResultSpectrumSimulation::onModeToggled);

    topLayout->setSpacing(10);
    topLayout->addSpacing(5);
    topLayout->addWidget(densityButton_);
    topLayout->addWidget(spectButton_);
    topLayout->addWidget(subSpectButton_);
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


    planeXY_ = new QRadioButton("XY");
    planeYZ_ = new QRadioButton("YZ");
    planeZX_ = new QRadioButton("ZX");
    planeXY_->setStyleSheet(radioStyle);
    planeYZ_->setStyleSheet(radioStyle);
    planeZX_->setStyleSheet(radioStyle);

    planeGroup_ = new QButtonGroup(this);
    planeGroup_->addButton(planeXY_, 0);
    planeGroup_->addButton(planeYZ_, 1);
    planeGroup_->addButton(planeZX_, 2);
    planeXY_->setChecked(true);

    QFrame* apFrame = new QFrame(this);
    apFrame->setFrameShape(QFrame::Box);
    apFrame->setStyleSheet(R"(
                           QFrame {
                           border: 2px solid #7697D8;
                           border-radius: 12px;
                           background-color: transparent;
                           padding: 2px;
                           }
                           )");

    QHBoxLayout* apLayout = new QHBoxLayout(apFrame);
    apLayout->addWidget(planeXY_);
    apLayout->addWidget(planeYZ_);
    apLayout->addWidget(planeZX_);
    apLayout->setSpacing(20);
    apLayout->setContentsMargins(10, 0, 10, 0);
    topLayout->addWidget(apFrame);
    mainLayout->addLayout(topLayout);


    imageContainer = new QWidget(this);
    imageContainer->setObjectName("imgContainer");
    imageContainer->setAttribute(Qt::WA_StyledBackground, true);
    imageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageContainer->setStyleSheet(R"(
                                  #imgContainer {
                                  background-color: #22489E;
                                  border: 2px solid #7697D8;
                                  border-radius: 12px;
                                  }
                                  )");

    imageLabel_ = new QLabel;
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setStyleSheet(R"(
                               background-color: transparent;
                               border: none;
                               )"
                               );
    imageLabel_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    originalPixmap_ = QPixmap(":/images/background/CTISS_logo.png");
    imageLabel_->setPixmap(originalPixmap_.scaled(imageLabel_->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));




    totalSpectChart_ = new QChartView;
    subSpectChart_ = new QChartView;
    totalSpectChart_->setAlignment(Qt::AlignCenter);
    totalSpectChart_->setStyleSheet(R"(
                                    background-color: transparent;
                                    border: none;
                                    )"
                                    );
    subSpectChart_->setAlignment(Qt::AlignCenter);
    subSpectChart_->setStyleSheet(R"(
                                  background-color: transparent;
                                  border: none;
                                  )"
                                  );
    totalSpectChart_->setRenderHint(QPainter::Antialiasing);
    subSpectChart_->setRenderHint(QPainter::Antialiasing);

    totalSpectChart_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    subSpectChart_  ->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto *pageCombined = new QWidget;
    auto *layComb = new QVBoxLayout(pageCombined);
    layComb->addWidget(imageLabel_);

    auto *pageTotalSpect = new QWidget;
    auto *layTotalSpect = new QVBoxLayout(pageTotalSpect);
    layTotalSpect->addWidget(totalSpectChart_, /*stretch=*/1);

    auto *pageSubSpect = new QWidget;
    auto *laySubSpect = new QVBoxLayout(pageSubSpect);
    laySubSpect->addWidget(subSpectChart_, /*stretch=*/1);


    stack = new QStackedLayout;
    stack->addWidget(pageCombined);   // index = 0
    stack->addWidget(pageTotalSpect);      // index = 1
    stack->addWidget(pageSubSpect);      // index = 1

    imageContainer->setLayout(stack);

    imageLabel_->show();
    totalSpectChart_->show();
    subSpectChart_->show();
    stack->setCurrentIndex(0);

    mainLayout->addWidget(imageContainer, /*stretch=*/1);



    QString wlStyle = R"(
                      color: white;
                      font-family: "Times New Roman";
                      font-size: 12pt;
                      background-color: transparent;
                      font-weight: bold;
                      border: none;
                      )";
    QString buttonBotLeftStyle = R"(
                                 QPushButton {
                                 color: white;
                                 font-family: "Times New Roman";
                                 font-size: 14pt;
                                 background-color: #3366CC;
                                 font-weight: bold;
                                 border-radius: 5px;
                                 border: 2px solid #2F528F;
                                 height:25;
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
    auto *botLayout = new QHBoxLayout;
    botLayout->setSpacing(8);
    botLayout->setContentsMargins(20, 0, 20, 0);

    auto *leftWidget = new QWidget(this);
    auto *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setSpacing(5);

    selectedContainer_ = new QWidget(leftWidget);
    auto *grid = new QGridLayout(selectedContainer_);
    grid->setHorizontalSpacing(20);
    grid->setVerticalSpacing(4);
    grid->setColumnMinimumWidth(0, 60);
    grid->setColumnMinimumWidth(1, 240);
    grid->setColumnStretch(0, 0);
    grid->setColumnStretch(1, 0);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    selectedContainer_->setSizePolicy(
                QSizePolicy::Fixed,
                QSizePolicy::Fixed
                );


    auto *lblGrid = new QLabel("Sub Spectrum Point", selectedContainer_);
    lblGrid->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lblGrid->setStyleSheet(R"(
                           color: white;
                           font-family: "Times New Roman";
                           font-size: 14pt;
                           background-color: transparent;
                           font-weight: bold;
                           border: none;
                           width:80;
                           )");
    grid->addWidget(lblGrid, 0, 0, 1, 2);

    auto *hLine = new QFrame(selectedContainer_);
    hLine->setFrameShape(QFrame::HLine);
    hLine->setFrameShadow(QFrame::Sunken);
    hLine->setLineWidth(1);
    hLine->setMaximumWidth(240);
    grid->addWidget(hLine, 1, 0, 1, 2);

    auto *lblRow = new QLabel("Row:", selectedContainer_);
    lblRow->setStyleSheet(wlStyle);
    lblRow->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid->addWidget(lblRow, 2, 0);

    auto *lblCol = new QLabel("Col:", selectedContainer_);
    lblCol->setStyleSheet(wlStyle);
    lblCol->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    grid->addWidget(lblCol, 3, 0);

    btnClear_       = new QPushButton("Clear", this);
    btnClear_->setStyleSheet(buttonBotLeftStyle);
    grid->addWidget(btnClear_, 4, 0);

    coordLayout_ = new QGridLayout;
    coordLayout_->setContentsMargins(0,0,0,0);
    coordLayout_->setSpacing(4);
    grid->addLayout(coordLayout_,         2, 1, 3, 1);


    leftLayout->addWidget(selectedContainer_);
    botLayout->addWidget(leftWidget, /*stretch=*/2);


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

    auto *midWidget = new QWidget(this);
    auto *midLayout = new QGridLayout(midWidget);
    midLayout->setHorizontalSpacing(4);
    midLayout->setVerticalSpacing(4);
    midLayout->setColumnMinimumWidth(0, 30);
    midLayout->setColumnMinimumWidth(1, 50);
    midLayout->setColumnMinimumWidth(2, 80);
    midLayout->setColumnMinimumWidth(3, 80);
    midLayout->setColumnStretch(0, 0);
    midLayout->setColumnStretch(1, 0);
    midLayout->setColumnStretch(2, 0);
    midLayout->setRowStretch(0, 1);

    auto *inputRowLabel = new QLabel("Row:");
    inputRowLabel->setStyleSheet(wlStyle);
    inputRowLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    midLayout->addWidget(inputRowLabel, 1, 0);
    editRow_ = new QLineEdit(midWidget);
    editRow_->setValidator(new QIntValidator(0, INT_MAX, this));
    editRow_->setStyleSheet(bottStyleEdit);
    midLayout->addWidget(editRow_, 1, 1);

    auto *inputColLabel = new QLabel("Col:");
    inputColLabel->setStyleSheet(wlStyle);
    inputColLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    midLayout->addWidget(inputColLabel, 2, 0);
    editCol_ = new QLineEdit(midWidget);
    editCol_->setValidator(new QIntValidator(0, INT_MAX, this));
    editCol_->setStyleSheet(bottStyleEdit);
    editRow_->setFixedWidth(50);
    editCol_->setFixedWidth(50);
    editRow_->setText("10");
    editCol_->setText("10");
    midLayout->addWidget(editCol_, 2, 1);
    midLayout->setRowStretch(3, 1);

    btnAdd_      = new QPushButton("Add", midWidget);
    btnDeleteSelected_  = new QPushButton("Del", midWidget);
    btnAdd_->setStyleSheet(buttonBotLeftStyle);
    btnDeleteSelected_->setStyleSheet(buttonBotLeftStyle);
    btnAdd_->setFixedWidth(60);
    btnDeleteSelected_->setFixedWidth(60);
    midLayout->addWidget(btnAdd_, 1, 2, Qt::AlignHCenter | Qt::AlignVCenter);
    midLayout->addWidget(btnDeleteSelected_, 2, 2, Qt::AlignHCenter | Qt::AlignVCenter);

    auto *rangeLabel = new QLabel("Range");
    rangeLabel->setStyleSheet(wlStyle);
    rangeLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    midLayout->addWidget(rangeLabel, 0, 3);

    editDspMin_ = new QLineEdit(midWidget);
    editDspMin_->setValidator(new QIntValidator(0, INT_MAX, this));
    editDspMin_->setStyleSheet(bottStyleEdit);
    editDspMin_->setFixedWidth(50);
    midLayout->addWidget(editDspMin_, 1, 3, Qt::AlignHCenter | Qt::AlignVCenter);

    editDspMax_ = new QLineEdit(midWidget);
    editDspMax_->setValidator(new QIntValidator(0, INT_MAX, this));
    editDspMax_->setStyleSheet(bottStyleEdit);
    editDspMax_->setFixedWidth(50);
    midLayout->addWidget(editDspMax_, 2, 3, Qt::AlignHCenter | Qt::AlignVCenter);

    editDspMin_->setText("0");
    editDspMax_->setText("80000");


    botLayout->addStretch();
    botLayout->addWidget(midWidget, /*stretch=*/1);


    QString buttonFrashStyle = R"(
                               QPushButton {
                               color: white;
                               font-family: "Times New Roman";
                               font-size: 16pt;
                               background-color: #3366CC;
                               font-weight: bold;
                               border-radius: 10px;
                               border: 2px solid #2F528F;
                               height:25;
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
    auto *rightLayout = new QHBoxLayout;

    btnRefreshFile_ = new QPushButton("Refresh", this);
    btnRefreshFile_->setStyleSheet(buttonFrashStyle);
    btnRefreshFile_->setFixedWidth(90);
    btnRefreshFile_->setFixedHeight(40);

    rightLayout->addWidget(btnRefreshFile_);

    leftLayout->setContentsMargins(0,0,0,0);
    grid->setContentsMargins(0,0,0,0);
    midLayout->setContentsMargins(0,0,0,0);
    midLayout->setContentsMargins(0,0,0,0);
    rightLayout->setContentsMargins(0,0,0,0);
    botLayout->addLayout(rightLayout, /*stretch=*/0);

    connect(btnAdd_, &QPushButton::clicked, this, &ResultSpectrumSimulation::onAddClicked);
    connect(btnDeleteSelected_, &QPushButton::clicked, this, &ResultSpectrumSimulation::onDeleteSelectedClicked);
    connect(btnClear_, &QPushButton::clicked, this, &ResultSpectrumSimulation::onClearClicked);
    connect(btnRefreshFile_, &QPushButton::clicked, this, &ResultSpectrumSimulation::onRefreshFileClicked);

    connect(editDspMin_, &QLineEdit::editingFinished, this, [this](){
        bool ok;
        double v = editDspMin_->text().toDouble(&ok);

        if (ok) {
            if (v > customMaxValue_) {
                v = customMaxValue_ - 1.0;
            }
            customMinValue_ = v;
            editDspMin_->setText(QString::number(v));
        }
        else {
            customMinValue_ = std::numeric_limits<double>::quiet_NaN();
        }

        if (actualMinValue_ == UNSET_VALUE) {
            return;
        }
        computeDensityGrid();
    });

    connect(editDspMax_, &QLineEdit::editingFinished, this, [this](){
        bool ok;
        double v = editDspMax_->text().toDouble(&ok);

        if (ok) {
            if (v < customMinValue_) {
                v = customMinValue_ + 1.0;
            }
            if (actualMaxValue_ != UNSET_VALUE) {
                if (v > actualMaxValue_) {
                    v = actualMaxValue_;
                }
            }
            customMaxValue_ = v;
            editDspMax_->setText(QString::number(v));
        }
        else {
            customMaxValue_ = std::numeric_limits<double>::quiet_NaN();
        }

        if (actualMaxValue_ == UNSET_VALUE) {
            return;
        }
        computeDensityGrid();
    });

    mainLayout->addLayout(botLayout);



    QVector<Point> defaults = {
        { 10, 10 },
        { 10, 15 },
        { 10, 5 },
        { 15, 10 },
        { 5, 10 }
    };
    initDefaultSelections(defaults);


    initTotalSpectrumChart();
    initSubSpectrumChart();

}

void ResultSpectrumSimulation::updateCoordinateLabels()
{
    for (int i = coordLayout_->count() - 1; i >= 0; --i) {
        QLayoutItem *item = coordLayout_->takeAt(i);
        if (auto w = item->widget()) {
            if (qobject_cast<QCheckBox*>(w) == nullptr) {
                delete w;
            }
        }
        delete item;
    }

    if (selectedPoints_.isEmpty()) {
        return;
    }

    int n = selectedPoints_.size();
    for (int i = 0; i < n; ++i) {
        const auto &pt = selectedPoints_[i];
        auto *cb = selectionBoxes_[i];
        auto *rLabel = new QLabel(QString::number(pt.row), selectedContainer_);
        auto *cLabel = new QLabel(QString::number(pt.col), selectedContainer_);

        int w = 30;
        rLabel->setFixedWidth(w);
        cLabel->setFixedWidth(w);
        rLabel->setAlignment(Qt::AlignCenter);
        cLabel->setAlignment(Qt::AlignCenter);

        coordLayout_->addWidget(rLabel, 0, i);
        coordLayout_->addWidget(cLabel, 1, i);
        coordLayout_->addWidget(cb,     2, i, Qt::AlignCenter);
    }
    coordLayout_->setColumnStretch(n, 1);
}

void ResultSpectrumSimulation::onAddClicked()
{
    bool ok1 = false, ok2 = false;
    int row = editRow_->text().toInt(&ok1);
    int col = editCol_->text().toInt(&ok2);
    if (!ok1 || !ok2) {
        showStyledWarning(this, "Input Error", "Row/Col Please enter an integer ≥0.");
        return;
    }
    if (selectedPoints_.size() >= 10) {
        showStyledWarning(this, "Input Error", "The quantity has reached the upper limit. \nYou can only add a maximum of 10 points.");
        return;
    }

    if (dimX_ != -1){
        if (row > dimX_){
            showStyledWarning(this, "Input Error", "The set coordinates exceed the number of grid cells.");
            editRow_->setText("5");
            editCol_->setText("5");
            return;
        }

        if (col > dimY_){
            showStyledWarning(this, "Input Error", "The set coordinates exceed the number of grid cells.");
            editRow_->setText("5");
            editCol_->setText("5");
            return;
        }

    }

    for (auto &pt : selectedPoints_) {
        if (pt.row == row && pt.col == col) {
            return;
        }
    }

    selectedPoints_.append({row, col});

    auto *cb = new QCheckBox(selectedContainer_);
    cb->setChecked(true);
    connect(cb, &QCheckBox::toggled,
            this, &ResultSpectrumSimulation::updatePlot);
    selectionBoxes_.append(cb);

    updateCoordinateLabels();
    updatePlot();
}


void ResultSpectrumSimulation::initDefaultSelections(QVector<Point> defaults)
{
    selectedPoints_.clear();
    for (auto *cb : selectionBoxes_) {
        cb->deleteLater();
    }
    selectionBoxes_.clear();

    for (int i = 0; i < defaults.size() && i < 10; ++i) {
        const Point &pt = defaults[i];
        selectedPoints_.append(pt);

        auto *cb = new QCheckBox(selectedContainer_);
        cb->setChecked(true);
        connect(cb, &QCheckBox::toggled,
                this, &ResultSpectrumSimulation::updatePlot);
        selectionBoxes_.append(cb);
    }

    updateCoordinateLabels();
}


void ResultSpectrumSimulation::onDeleteSelectedClicked()
{
    for (int i = selectionBoxes_.size() - 1; i >= 0; --i) {
        if (selectionBoxes_[i]->isChecked()) {
            selectedPoints_.removeAt(i);
            auto *cb = selectionBoxes_.takeAt(i);
            cb->deleteLater();
        }
    }
    updateCoordinateLabels();
    updatePlot();
}

void ResultSpectrumSimulation::onClearClicked()
{
    selectedPoints_.clear();
    for (auto *cb : qAsConst(selectionBoxes_)) {
        cb->deleteLater();
    }
    selectionBoxes_.clear();

    updateCoordinateLabels();
    updatePlot();
}

void ResultSpectrumSimulation::onRefreshFileClicked()
{
    bool successRead = readPhotonFile();
    if (!successRead){
        qDebug()<<">>> Failed to read the file, not updating the image.";
        return;
    }
    computeDensityGrid();
    computeGlobalSpectrum();
    updateTotalSpectrumChart();
    computeSubSpectra();
    updateCoordinateLabels();
    updatePlot();
}

void ResultSpectrumSimulation::updatePlot()
{
    QChart *chart = subSpectChart_->chart();
    if (!chart) return;

    chart->removeAllSeries();

    static const QVector<QColor> colorPool = {
        Qt::red, Qt::green, Qt::magenta, Qt::cyan,
        Qt::yellow, QColor(255,165,0), QColor(128,0,128),
        QColor(0,128,128), QColor(128,128,0), Qt::blue
    };

    qreal globalMaxY = 0;
    int seriesIdx = 0;
    for (int i = 0; i < selectedPoints_.size() && i < selectionBoxes_.size(); ++i) {
        if (!selectionBoxes_[i]->isChecked()) continue;

        const auto &pt = selectedPoints_[i];
        auto key = qMakePair(pt.col, pt.row);
        if (!subSpectra_.contains(key)) continue;

        const auto &spec = subSpectra_[key];
        auto *s = new QLineSeries(chart);

        QPen pen(colorPool[seriesIdx % colorPool.size()], 3, Qt::SolidLine);
        s->setPen(pen);
        s->setName(QString("(%1, %2)\n").arg(pt.col).arg(pt.row));

        for (int b = 0; b < spec.size(); ++b) {
            qreal x = (b + 0.5) * EnergyBinSize;
            qreal y = spec[b];
            s->append(x, y);
            globalMaxY = qMax(globalMaxY, y);
        }

        chart->addSeries(s);
        for (auto *axis : chart->axes())
            s->attachAxis(axis);

        ++seriesIdx;
    }

    {
        auto axesH = chart->axes(Qt::Horizontal);
        if (!axesH.isEmpty()) {
            auto *axisX = qobject_cast<QValueAxis*>(axesH.first());
            axisX->setRange(0, nEnergyBins_ * EnergyBinSize);
            axisX->setLabelsFont(QFont("Times New Roman", 16));
            axisX->setLabelsBrush(QBrush(Qt::white));
        }
    }

    {
        auto axesV = chart->axes(Qt::Vertical);
        if (!axesV.isEmpty()) {
            auto *axisY = qobject_cast<QValueAxis*>(axesV.first());
            axisY->setRange(0, globalMaxY * 1.1);
            axisY->setLabelsFont(QFont("Times New Roman", 16));
            axisY->setLabelsBrush(QBrush(Qt::white));
        }
    }

    chart->setContentsMargins(0, 0, 0, 0);
    chart->setMargins(QMargins(0, 0, 0, 0));

    QLegend *legend = chart->legend();
    legend->setVisible(true);
    legend->setBackgroundVisible(false);
    legend->setBorderColor(Qt::transparent);
    legend->setMarkerShape(QLegend::MarkerShapeFromSeries);


    QFont tnr12("Times New Roman", 15);
    legend->setFont(tnr12);
    QFontMetrics fm(tnr12);
    int longestText = 0;
    for (QLegendMarker *m : legend->markers()) {
        m->setFont(tnr12);
        m->setLabelBrush(QBrush(Qt::white));
        longestText = qMax(longestText, fm.horizontalAdvance(m->label()));
    }


    const int marginH   = 8;
    const int markerLen = 30;
    const int textGap   = 15;
    int legendW = marginH*2 + markerLen + textGap + longestText;

    const int marginV = 4;
    int rowH         = fm.height() + 14;
    int legendH      = marginV*2 + chart->legend()->markers().size() * rowH;

    legend = chart->legend();
    legend->setVisible(true);
    legend->detachFromChart();
    QRectF pa = chart->plotArea();
    qreal offR = 20, offT = 10;
    legend->setGeometry(QRectF(
                            pa.right() - legendW - offR,
                            pa.top()   + offT,
                            legendW,
                            legendH
                            ));

    subSpectChart_->update();
}

void ResultSpectrumSimulation::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
}

void ResultSpectrumSimulation::scriptChanged(const QString &filePath)
{
    scriptSpecs = parseVariableFile(filePath);
    DetSizeX      = getVariableValue(scriptSpecs, "DetSizeX").toFloat();
    DetSizeY      = getVariableValue(scriptSpecs, "DetSizeY").toFloat();
    GridSizeX     = getVariableValue(scriptSpecs, "GridSizeX").toFloat();
    GridSizeY     = getVariableValue(scriptSpecs, "GridSizeY").toFloat();
    EnergyBinSize = getVariableValue(scriptSpecs, "EnergyBinSize").toFloat();

    dimX_ = qCeil(DetSizeX / GridSizeX);
    dimY_ = qCeil(DetSizeY / GridSizeY);
    nEnergyBins_ = qCeil((getVariableValue(scriptSpecs, "TubeVoltage").toFloat()+5) / EnergyBinSize);

    int midX    = dimX_ / 2;
    int midY    = dimY_ / 2;
    int maxCol  = qMax(0, dimX_);
    int maxRow  = qMax(0, dimY_);

    QVector<Point> defaults = {
        { midY,       midX       },
        { midY + 3,   midX + 3   },
        { midY + 3,   midX - 3   },
        { midY - 3,   midX + 3   },
        { midY - 3,   midX - 3   }
    };
    for (auto &pt : defaults) {
        pt.row = qBound(0, pt.row, maxRow);
        pt.col = qBound(0, pt.col, maxCol);
    }
    initDefaultSelections(defaults);
}

void ResultSpectrumSimulation::refreshPixmap() {
    if (originalPixmap_.isNull()) return;
    QSize target = imageLabel_->size();
    imageLabel_->setPixmap(
                originalPixmap_.scaled(
                    target,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );
    QResizeEvent *evt = new QResizeEvent(imageContainer->size(),imageContainer->size());
    QCoreApplication::postEvent(this, evt);
}


bool ResultSpectrumSimulation::readPhotonFile()
{
    photons_.clear();
    QString path = getVariableValue(scriptSpecs, "OutputFolder")
                 + QDir::separator()
                 + getVariableValue(scriptSpecs, "OutputFilename")
                 + ".txt";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
    {
        showStyledWarning(this,
                          "Load Error",
                          QString("Cannot open photon file:\n%1").arg(path));
        return false;
    }
    qDebug() << ">>> Reading: " << path;

    std::setlocale(LC_NUMERIC, "C");

    qint64 approx = qMax<qint64>(10, f.size() / 80);
    photons_.reserve(int(approx));

    QByteArray line;
    while (!f.atEnd()) {
        line = f.readLine();
        if (line.isEmpty()) continue;
        char c = line.trimmed().constData()[0];
        if ((c >= '0' && c <= '9') || c == '-' || c == '+')
            break;
    }

    struct Raw { float E, u, v; };
    std::vector<Raw> raw;
    raw.reserve(int(approx));

    uMin_ = vMin_ =  FLT_MAX;
    uMax_ = vMax_ = -FLT_MAX;
    int plane = planeGroup_->checkedId();

    auto parseFloat = [&](char *&p, float &out) {
        char *end = nullptr;
        out = strtof(p, &end);
        if (end == p) return false;
        p = end;
        return true;
    };

    do {
        if (line.startsWith('%') || line.trimmed().isEmpty())
            continue;

        char *p = line.data();
        float E, x, y, z, dummy;

        if (!parseFloat(p, E))   continue;
        if (!parseFloat(p, x))   continue;
        if (!parseFloat(p, y))   continue;
        if (!parseFloat(p, z))   continue;

        if (!parseFloat(p, dummy)) continue;
        if (!parseFloat(p, dummy)) continue;
        if (!parseFloat(p, dummy)) continue;

        float u = ((plane == 0 ? x
                   : plane == 1 ? y
                                : z) * 10.0f);
        float v = ((plane == 0 ? y
                   : plane == 1 ? z
                                : x) * 10.0f);

        uMin_ = qMin(uMin_, u);
        uMax_ = qMax(uMax_, u);
        vMin_ = qMin(vMin_, v);
        vMax_ = qMax(vMax_, v);

        raw.push_back({E, u, v});
    }
    while (!f.atEnd() && (line = f.readLine(), true));

    f.close();

    const float eps = 1e-6f;
    if (uMax_ <= uMin_) uMax_ = uMin_ + eps;
    if (vMax_ <= vMin_) vMax_ = vMin_ + eps;

    float gridSizeX = (uMax_ - uMin_) / float(dimX_);
    float gridSizeY = (vMax_ - vMin_) / float(dimY_);
    float invGX     = 1.0f / gridSizeX;
    float invGY     = 1.0f / gridSizeY;

    photons_.reserve(raw.size());
    for (auto &r : raw) {
        int cx = int((r.u - uMin_) * invGX);
        int cy = int((r.v - vMin_) * invGY);
        if (cx >= 0 && cx < dimX_ && cy >= 0 && cy < dimY_) {
            Photon p;
            p.energy = r.E;
            p.u      = r.u;
            p.v      = r.v;
            p.cellX  = cx;
            p.cellY  = cy;
            photons_.append(p);
        }
    }

    qDebug() << ">>> Reading Done, total photons =" << photons_.size();
    return true;
}


//bool ResultSpectrumSimulation::readPhotonFile()
//{
//    photons_.clear();
//    QString path = getVariableValue(scriptSpecs, "OutputFolder") + QDir::separator() + getVariableValue(scriptSpecs, "OutputFilename") + ".txt";

//    std::setlocale(LC_NUMERIC, "C");

//    QFile f(path);
//    if (!f.open(QIODevice::ReadOnly)) {
//        showStyledWarning(this,
//                          "Load Error",
//                          QString("Cannot open volume file:\n%1").arg(path));
//        originalPixmap_ = QPixmap(":/images/CT/CT.PNG");
//        refreshPixmap();
//        return false;
//    }
//    qDebug() << ">>> Reading: " << path;
//    QByteArray buffer = f.readAll();
//    const char *dataStart = buffer.constData();
//    const char *dataEnd   = dataStart + buffer.size();

//    const char *ptr = dataStart;
//    while (ptr < dataEnd && *ptr != '\n') ++ptr;
//    if (ptr < dataEnd && *ptr == '\n') ++ptr;

//    uMin_ = vMin_ =  FLT_MAX;
//    uMax_ = vMax_ = -FLT_MAX;
//    int plane = planeGroup_->checkedId();

//    auto hasSevenFloats = [&](const char *lineBegin, const char *lineEnd) {
//        int count = 0;
//        const char *p = lineBegin;
//        char *next;
//        while (p < lineEnd && count < 7) {
//            strtof(p, &next);
//            if (next == p)
//                break;
//            p = next;
//            ++count;
//        }
//        return count == 7;
//    };

//    {
//        const char *scan = ptr;
//        while (scan < dataEnd) {
//            const char *lineStart = scan;
//            const char *lineEnd = scan;
//            while (lineEnd < dataEnd && *lineEnd != '\n')
//                ++lineEnd;

//            if (!hasSevenFloats(lineStart, lineEnd)) {
//                scan = lineEnd;
//                if (scan < dataEnd && *scan == '\n') ++scan;
//                continue;
//            }

//            char *next;
//            strtof(scan, &next);
//            scan = next;

//            float x = strtof(scan, &next); scan = next;
//            float y = strtof(scan, &next); scan = next;
//            float z = strtof(scan, &next); scan = next;

//            for (int i = 0; i < 3; ++i) {
//                strtof(scan, &next);
//                scan = next;
//            }

//            float u = (plane == 0 ? x
//                                  : plane == 1 ? y
//                                               : z)*10;
//            float v = (plane == 0 ? y
//                                  : plane == 1 ? z
//                                               : x)*10;

//            uMin_ = qMin(uMin_, u);
//            uMax_ = qMax(uMax_, u);
//            vMin_ = qMin(vMin_, v);
//            vMax_ = qMax(vMax_, v);

//            scan = lineEnd;
//            if (scan < dataEnd && *scan == '\n') ++scan;
//        }
//    }

//    const float eps = 1e-6f;
//    if (uMax_ <= uMin_) uMax_ = uMin_ + eps;
//    if (vMax_ <= vMin_) vMax_ = vMin_ + eps;

//    float gridSizeX = (uMax_ - uMin_) / float(dimX_);
//    float gridSizeY = (vMax_ - vMin_) / float(dimY_);
//    float invGX     = 1.0f / gridSizeX;
//    float invGY     = 1.0f / gridSizeY;

//    photons_.clear();
//    photons_.reserve(buffer.size() / 50);

//    {
//        const char *scan = ptr;

//        while (scan < dataEnd) {
//            const char *lineStart = scan;
//            const char *lineEnd   = scan;
//            while (lineEnd < dataEnd && *lineEnd != '\n')
//                ++lineEnd;

//            if (!hasSevenFloats(lineStart, lineEnd)) {
//                scan = lineEnd;
//                if (scan < dataEnd && *scan == '\n') ++scan;
//                continue;
//            }

//            char *next;

//            float E = strtof(scan, &next);
//            scan = next;

//            float x = strtof(scan, &next); scan = next;
//            float y = strtof(scan, &next); scan = next;
//            float z = strtof(scan, &next); scan = next;

//            for (int i = 0; i < 3; ++i) {
//                strtof(scan, &next);
//                scan = next;
//            }

//            float u = (plane == 0 ? x
//                                  : plane == 1 ? y
//                                               : z)*10;
//            float v = (plane == 0 ? y
//                                  : plane == 1 ? z
//                                               : x)*10;

//            int cellX = int((u - uMin_) * invGX);
//            int cellY = int((v - vMin_) * invGY);

//            if (cellX >= 0 && cellX < dimX_ &&
//                    cellY >= 0 && cellY < dimY_) {
//                Photon p;
//                p.energy = E;
//                p.u      = u;
//                p.v      = v;
//                p.cellX  = cellX;
//                p.cellY  = cellY;
//                photons_.append(p);
//            }

//            scan = lineEnd;
//            if (scan < dataEnd && *scan == '\n') ++scan;
//        }
//    }
//    qDebug() << ">>> Reading Done";
//    return true;
//}

void ResultSpectrumSimulation::computeDensityGrid()
{
    densityGrid_.fill(0, dimX_*dimY_);
    for (auto &p : photons_) {
        int idx = p.cellY * dimX_ + p.cellX;
        densityGrid_[idx]++;
    }
    actualMaxValue_ = *std::max_element(densityGrid_.begin(),
                                        densityGrid_.end());
    actualMinValue_ = *std::min_element(densityGrid_.begin(),
                                        densityGrid_.end());


    if (qIsNaN(customMinValue_)) customMinValue_ = actualMinValue_;
    if (qIsNaN(customMaxValue_)) customMaxValue_ = actualMaxValue_;
    editDspMin_->setText(QString::number(customMinValue_));
    editDspMax_->setText(QString::number(customMaxValue_));

    double dispMin = customMinValue_;
    double dispMax = customMaxValue_;


    if (dispMax <= dispMin)
        dispMax = dispMin + 1.0;

    const int cellSize     = 20;
    const int marginLeft   = 60;
    const int marginTop    = 20;
    const int marginBottom = 40;
    const int marginRight  = 60;
    const int barWidth     = 20;
    const int barSpacing   = 20;

    int heatW = dimX_ * cellSize;
    int heatH = dimY_ * cellSize;
    int canvasW = marginLeft + heatW + barSpacing + barWidth + marginRight;
    int canvasH = marginTop  + heatH + marginBottom;


    QImage canvas(canvasW, canvasH, QImage::Format_ARGB32);
    canvas.fill(Qt::transparent);
    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);


    QFont font("Times New Roman", 12, QFont::Bold);
    painter.setFont(font);
    painter.setPen(Qt::white);
    QFontMetrics fm(font);

    QPointF origin(marginLeft, marginTop);
    for (int y = 0; y < dimY_; ++y) {
        for (int x = 0; x < dimX_; ++x) {
            int c = densityGrid_[y*dimX_ + x];
            double norm = (c - dispMin) / (dispMax - dispMin);
            norm = qBound(0.0, norm, 1.0);
            int r = qBound(0, int(255*(norm>0.5?2*(norm-0.5):0)), 255);
            int g = qBound(0, int(255*(norm<0.5?2*norm:2*(1-norm))), 255);
            int b = qBound(0, int(255*(norm<0.5?0:2*(1-norm))), 255);
            QRectF cellRect(
                        origin.x() +  x    * cellSize,
                        origin.y() + (dimY_-1-y) * cellSize,
                        cellSize, cellSize);
            painter.fillRect(cellRect, QColor(r,g,b));
        }
    }


    painter.setPen(QPen(Qt::white, 2.0));
    painter.drawRect(QRectF(origin.x(), origin.y(), heatW, heatH));

    painter.setPen(QPen(QColor(128,128,128,200), 1.0));
    for (int i = 0; i <= dimX_; ++i) {
        double x = origin.x() + i * cellSize;
        painter.drawLine(QPointF(x, origin.y()),
                         QPointF(x, origin.y() + heatH));
    }
    for (int j = 0; j <= dimY_; ++j) {
        double y = origin.y() + j * cellSize;
        painter.drawLine(QPointF(origin.x(), y),
                         QPointF(origin.x() + heatW, y));
    }

    painter.setPen(Qt::white);
    int ticks = 5;
    for (int t = 0; t <= ticks; ++t) {
        double frac = double(t)/ticks;
        double x = origin.x() + frac * heatW;
        double y0 = origin.y() + heatH;
        painter.drawLine(QPointF(x, y0), QPointF(x, y0+5));
        QString lbl = QString::number(int(frac*(dimX_-1)));
        int w = fm.horizontalAdvance(lbl);
        painter.drawText(QPointF(x - w/2,
                                 y0 + 5 + fm.ascent()),
                         lbl);
    }
    for (int t = 0; t <= ticks; ++t) {
        double frac = double(t)/ticks;
        double y = origin.y() + heatH - frac * heatH;
        double x0 = origin.x();
        painter.drawLine(QPointF(x0, y), QPointF(x0-5, y));
        QString lbl = QString::number(int(frac*(dimY_-1)));
        int w = fm.horizontalAdvance(lbl);
        painter.drawText(QPointF(x0 - 5 - w - 2,
                                 y + fm.ascent()/2),
                         lbl);
    }

    QString xlabel = "u (mm)";
    int xlabW = fm.horizontalAdvance(xlabel);
    painter.drawText(
                QPointF(origin.x() + heatW/2 - xlabW/2,
                        origin.y() + heatH + fm.height() + 5),
                xlabel);

    QString ylabel = "v (mm)";
    painter.save();
    painter.translate(origin.x() - fm.height() - 5,
                      origin.y() + heatH/2 +
                      fm.horizontalAdvance(ylabel)/2);
    painter.rotate(-90);
    painter.drawText(QPointF(0, 0), ylabel);
    painter.restore();

    painter.setPen(Qt::white);
    int barX = marginLeft + heatW + barSpacing;
    int barY = marginTop;
    QRect barRect(barX, barY, barWidth, heatH);

    QLinearGradient grad(barX, barY + heatH, barX, barY);
    grad.setColorAt(0.0, QColor(0,   0,   255));
    grad.setColorAt(0.5, QColor(0,   255, 0));
    grad.setColorAt(1.0, QColor(255, 0,   0));
    painter.fillRect(barRect, grad);
    painter.drawRect(barRect);

    for (int t = 0; t <= ticks; ++t) {
        double frac = double(t)/ticks;
        double y = barY + heatH - frac * heatH;
        painter.drawLine(QPointF(barX, y),
                         QPointF(barX - 5, y));
        double val = dispMin + frac * (dispMax - dispMin);
        QString lbl = QString::number(val, 'f', 0);
        painter.drawText(
                    QPointF(barX + barWidth + 5,
                            y + fm.ascent()/2),
                    lbl);
    }

    painter.end();

    originalPixmap_ = QPixmap::fromImage(canvas);
    refreshPixmap();
}

void ResultSpectrumSimulation::computeGlobalSpectrum()
{
    if (nEnergyBins_ <= 0) {
        qWarning() << "computeGlobalSpectrum: nEnergyBins_ <= 0, skip.";
        return;
    }

    globalSpectrum_.resize(nEnergyBins_);
    std::fill(globalSpectrum_.begin(), globalSpectrum_.end(), 0);

    for (auto &p : photons_) {
        if (!qIsFinite(p.energy) || p.energy < 0.0)
            continue;

        int ebin = int(p.energy / EnergyBinSize);
        if (ebin < 0)
            ebin = 0;
        else if (ebin >= nEnergyBins_)
            ebin = nEnergyBins_ - 1;

        globalSpectrum_[ebin]++;
    }
}

void ResultSpectrumSimulation::initTotalSpectrumChart()
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setBackgroundVisible(false);

    chart->setTitle("Global Spectrum");
    chart->setTitleFont(QFont("Times New Roman", 20, QFont::Bold));
    chart->setTitleBrush(QBrush(Qt::white));

    chart->setContentsMargins(0, 0, 0, 0);
    chart->setMargins(QMargins(0, 0, 0, 0));

    auto *axisX = new QValueAxis();
    axisX->setRange(0, 120);
    axisX->setTickCount(13);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Energy");
    axisX->setTitleFont(QFont("Times New Roman", 18));
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsFont(QFont("Times New Roman", 16));
    axisX->setLabelsBrush(QBrush(Qt::white));
    axisX->setGridLineVisible(true);
    axisX->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    //axisX->setGridLineVisible(false);
    //axisX->setMinorTickCount(axisX->tickCount() - 1);
    //axisX->setMinorGridLineVisible(true);
    //axisX->setMinorGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    //axisX->setMinorTickCount(1);
    //axisX->setMinorGridLineVisible(true);
    //axisX->setMinorGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    axisX->setLinePen(QPen(Qt::white, 2));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 10000);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("Photon Count");
    axisY->setTitleFont(QFont("Times New Roman", 18));
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsFont(QFont("Times New Roman", 16));
    axisY->setLabelsBrush(QBrush(Qt::white));
    axisY->setGridLineVisible(true);
    axisY->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    //    axisY->setGridLineVisible(false);
    //    axisY->setMinorTickCount(axisY->tickCount() - 1);
    //    axisY->setMinorGridLineVisible(true);
    //    axisY->setMinorGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    //    axisY->setMinorTickCount(1);
    //    axisY->setMinorGridLineVisible(true);
    //    axisY->setMinorGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    axisY->setLinePen(QPen(Qt::white, 2));
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(Qt::transparent);
    chart->setPlotAreaBackgroundPen(QPen(Qt::white, 2));

    totalSpectChart_->setChart(chart);
    totalSpectChart_->setRenderHint(QPainter::Antialiasing);
    totalSpectChart_->setStyleSheet("background: transparent; border: none;");
    totalSpectChart_->setAttribute(Qt::WA_TranslucentBackground);
    totalSpectChart_->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    totalSpectChart_->setContentsMargins(0, 0, 0, 0);
    totalSpectChart_->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
}

void ResultSpectrumSimulation::initSubSpectrumChart()
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setBackgroundVisible(false);

    chart->setTitle("Local Spectrum");
    chart->setTitleFont(QFont("Times New Roman", 20, QFont::Bold));
    chart->setTitleBrush(QBrush(Qt::white));

    chart->setContentsMargins(0, 0, 0, 0);
    chart->setMargins(QMargins(0, 0, 0, 0));

    auto *axisX = new QValueAxis();
    axisX->setRange(0, 120);
    axisX->setTickCount(13);
    axisX->setLabelFormat("%d");
    axisX->setTitleText("Energy");
    axisX->setTitleFont(QFont("Times New Roman", 18));
    axisX->setTitleBrush(QBrush(Qt::white));
    axisX->setLabelsFont(QFont("Times New Roman", 16));
    axisX->setLabelsBrush(QBrush(Qt::white));
    axisX->setLinePen(QPen(Qt::white, 2));
    axisX->setGridLineVisible(true);
    axisX->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    chart->addAxis(axisX, Qt::AlignBottom);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, 10000);
    axisY->setTickCount(11);
    axisY->setLabelFormat("%d");
    axisY->setTitleText("Photon Count");
    axisY->setTitleFont(QFont("Times New Roman", 18));
    axisY->setTitleBrush(QBrush(Qt::white));
    axisY->setLabelsFont(QFont("Times New Roman", 16));
    axisY->setLabelsBrush(QBrush(Qt::white));
    axisY->setLinePen(QPen(Qt::white, 2));
    axisY->setGridLineVisible(true);
    axisY->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    chart->addAxis(axisY, Qt::AlignLeft);

    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(Qt::transparent);
    chart->setPlotAreaBackgroundPen(QPen(Qt::white, 2));

    subSpectChart_->setChart(chart);
    subSpectChart_->setRenderHint(QPainter::Antialiasing);
    subSpectChart_->setStyleSheet("background: transparent; border: none;");
    subSpectChart_->setAttribute(Qt::WA_TranslucentBackground);
    subSpectChart_->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    subSpectChart_->setContentsMargins(0, 0, 0, 0);
    subSpectChart_->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
}

void ResultSpectrumSimulation::updateTotalSpectrumChart()
{
    QChart *chart = totalSpectChart_->chart();
    if (!chart)
        return;

    chart->removeAllSeries();
    auto *series = new QLineSeries();
    QPen redPen(Qt::red);
    redPen.setWidth(2);
    redPen.setStyle(Qt::SolidLine);
    series->setPen(redPen);
    series->setName("Global Spectrum");

    for (int i = 0; i < nEnergyBins_; ++i) {
        qreal x = (i + 0.5) * EnergyBinSize;
        qreal y = globalSpectrum_.at(i);
        series->append(x, y);
    }
    chart->addSeries(series);

    for (QAbstractAxis *axis : chart->axes()) {
        series->attachAxis(axis);
    }

    const qreal xMax = nEnergyBins_ * EnergyBinSize;
    const auto hAxes = chart->axes(Qt::Horizontal);
    if (!hAxes.isEmpty()) {
        if (auto *axisX = qobject_cast<QValueAxis*>(hAxes.first()))
            axisX->setRange(0, xMax);
    }

    int maxCount = globalSpectrum_.isEmpty()
            ? 0
            : *std::max_element(globalSpectrum_.begin(),
                                globalSpectrum_.end());
    const qreal yMax = maxCount * 1.1;
    const auto vAxes = chart->axes(Qt::Vertical);
    if (!vAxes.isEmpty()) {
        if (auto *axisY = qobject_cast<QValueAxis*>(vAxes.first()))
            axisY->setRange(0, yMax);
    }
    totalSpectChart_->update();
}


void ResultSpectrumSimulation::computeSubSpectra()
{
    subSpectra_.clear();
    for (auto &p : photons_) {
        auto key = qMakePair(p.cellX, p.cellY);
        if (!subSpectra_.contains(key))
            subSpectra_[key].fill(0, nEnergyBins_);
        int ebin = qFloor(p.energy / EnergyBinSize);
        if (ebin >= 0 && ebin < nEnergyBins_)
            subSpectra_[key][ebin]++;
    }
}

void ResultSpectrumSimulation::onModeToggled(int modeId)
{
    stack->setCurrentIndex(modeId);
}

void ResultSpectrumSimulation::setMainWindow(MainWindow* main) {
    mainWindow = main;
}

void ResultSpectrumSimulation::resizeEvent(QResizeEvent* event)
{

    QWidget::resizeEvent(event);
    refreshPixmap();
}

void ResultSpectrumSimulation::paintEvent(QPaintEvent *event)
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

QVector<VariableEntry> ResultSpectrumSimulation::parseVariableFile(const QString &filePath) {
    QVector<VariableEntry> variables;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        showStyledWarning(this, "Warning", "An error occurred while trying to read the file: " + filePath);
        return variables;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        QString comment;
        int commentIndex = line.indexOf('#');
        if (commentIndex != -1) {
            comment = line.mid(commentIndex + 1).trimmed();
            line = line.left(commentIndex).trimmed();
        }

        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
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

QString ResultSpectrumSimulation::getVariableValue(const QVector<VariableEntry> &variables, const QString &name) {
    for (const auto &entry : variables) {
        if (entry.name == name) {
            return entry.value;
        }
    }
    return QString();
}

void ResultSpectrumSimulation::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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
