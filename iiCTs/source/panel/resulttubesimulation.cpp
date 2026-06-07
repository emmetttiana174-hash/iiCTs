#include "resulttubesimulation.h"
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
#include <QtConcurrent>

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QLegend>
#include <QtGui/QPen>
#include <QtCharts/QLegendMarker>


#include "terminalwidget.h"
#include "mainwindow.h"

using namespace QtCharts;

ResultTubeSimulation::ResultTubeSimulation(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

void ResultTubeSimulation::setupUi()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10,10,10,10);

    auto *topLayout = new QHBoxLayout;

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
    topLayout->addStretch();
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


    chargeChart_ = new QChartView;
    chargeChart_->setAlignment(Qt::AlignCenter);
    chargeChart_->setStyleSheet(R"(
                                    background-color: transparent;
                                    border: none;
                                    )"
                                    );
    chargeChart_->setRenderHint(QPainter::Antialiasing);
    chargeChart_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto *layImageContainer = new QVBoxLayout(imageContainer);
    layImageContainer->setContentsMargins(0, 0, 0, 0);
    layImageContainer->addWidget(chargeChart_);
    imageContainer->setLayout(layImageContainer);

    mainLayout->addWidget(imageContainer, /*stretch=*/1);


    auto *botLayout = new QHBoxLayout;
    botLayout->setSpacing(8);
    botLayout->setContentsMargins(20, 0, 20, 0);

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
    rightLayout->setContentsMargins(0,0,0,0);
    botLayout->addLayout(rightLayout, /*stretch=*/0);

    connect(btnRefreshFile_, &QPushButton::clicked, this, &ResultTubeSimulation::onRefreshFileClicked);


    mainLayout->addLayout(botLayout);

    initChargeChart();
}



void ResultTubeSimulation::onRefreshFileClicked()
{
    bool successRead = readChargeFile();
    if (!successRead){
        qDebug()<<">>> Failed to read the file, not updating the image.";
        return;
    }
    updateChargeChart();
}

void ResultTubeSimulation::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
}

void ResultTubeSimulation::scriptChanged(const QString &filePath)
{
    scriptTubes = parseVariableFile(filePath);
    AnodeRadius      = getVariableValue(scriptTubes, "AnodeRadius").toFloat();
}


bool ResultTubeSimulation::readChargeFile()
{
    charges_.clear();
    QString path = getVariableValue(scriptTubes, "OutputPath")
                 + QDir::separator()
                 + getVariableValue(scriptTubes, "OutputFilename")
                 + ".txt";
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        showStyledWarning(this,
                          "Load Error",
                          QString("Cannot open charge file:\n%1").arg(path));
        return false;
    }
    qDebug() << ">>> Reading: " << path;
    std::setlocale(LC_NUMERIC, "C");

    charges_.reserve(int(f.size() / 80));

    for (int i = 0; i < 3 && !f.atEnd(); ++i)
        f.readLine();

    QByteArray rawLine;
    while (!f.atEnd()) {
        rawLine = f.readLine();
        if (rawLine.isEmpty() || rawLine.startsWith('%'))
            continue;

        char *p     = rawLine.data();
        char *endp  = nullptr;

        auto parseFloat = [&](float &out) {
            out = strtof(p, &endp);
            if (endp == p) return false;
            p = endp;
            return true;
        };
        auto parseInt = [&](int &out) {
            out = int(strtol(p, &endp, 10));
            if (endp == p) return false;
            p = endp;
            return true;
        };

        Charge c;
        if (!parseFloat(c.PositionX))   continue;
        if (!parseFloat(c.PositionY))   continue;
        if (!parseFloat(c.PositionZ))   continue;
        if (!parseFloat(c.MomentumX))   continue;
        if (!parseFloat(c.MomentumY))   continue;
        if (!parseFloat(c.MomentumZ))   continue;
        if (!parseFloat(c.Mass))        continue;
        if (!parseFloat(c.ChargeMacro)) continue;
        if (!parseFloat(c.Time))        continue;

        int tmp;
        if (!parseInt(tmp))             continue; // ParticleID
        if (!parseInt(tmp))             continue; // EmissionID

        if (!parseFloat(c.Current))     continue;
        parseInt(tmp);

        c.PositionX = c.PositionX*1000;
        c.PositionY = c.PositionY*1000;
        c.PositionZ = c.PositionZ*1000;

        charges_.append(c);
    }

    f.close();

    qDebug() << ">>> Read Done";
    return true;
}


void ResultTubeSimulation::initChargeChart()
{
    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->setBackgroundVisible(false);

//    chart->setTitle("Global Spectrum");
//    chart->setTitleFont(QFont("Times New Roman", 20, QFont::Bold));
//    chart->setTitleBrush(QBrush(Qt::white));

    chart->setContentsMargins(0, 0, 0, 0);
    chart->setMargins(QMargins(0, 0, 0, 0));

    axisX_ = new QValueAxis();
    axisX_->setRange(-10, 10);
    axisX_->setTickCount(13);
    axisX_->setLabelFormat("%d");
    axisX_->setTitleText("U (mm)");
    axisX_->setTitleFont(QFont("Times New Roman", 18));
    axisX_->setTitleBrush(QBrush(Qt::white));
    axisX_->setLabelsFont(QFont("Times New Roman", 16));
    axisX_->setLabelsBrush(QBrush(Qt::white));
    axisX_->setGridLineVisible(true);
    axisX_->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    axisX_->setLinePen(QPen(Qt::white, 2));
    chart->addAxis(axisX_, Qt::AlignBottom);

    axisY_ = new QValueAxis();
    axisY_->setRange(-10, 10);
    axisY_->setTickCount(11);
    axisY_->setLabelFormat("%d");
    axisY_->setTitleText("V (mm)");
    axisY_->setTitleFont(QFont("Times New Roman", 18));
    axisY_->setTitleBrush(QBrush(Qt::white));
    axisY_->setLabelsFont(QFont("Times New Roman", 16));
    axisY_->setLabelsBrush(QBrush(Qt::white));
    axisY_->setGridLineVisible(true);
    axisY_->setGridLinePen(QPen(QColor(192,192,192), 1, Qt::DashLine));
    axisY_->setLinePen(QPen(Qt::white, 2));
    chart->addAxis(axisY_, Qt::AlignLeft);

    chart->setPlotAreaBackgroundVisible(true);
    chart->setPlotAreaBackgroundBrush(Qt::transparent);
    chart->setPlotAreaBackgroundPen(QPen(Qt::white, 2));

    chargeSeries_ = new QScatterSeries(chart);
    chargeSeries_->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    chargeSeries_->setMarkerSize(5.0);
    chargeSeries_->setBrush(Qt::red);
    chargeSeries_->setBorderColor(Qt::transparent);
    chart->addSeries(chargeSeries_);
    chargeSeries_->attachAxis(axisX_);
    chargeSeries_->attachAxis(axisY_);

    chargeWatcher_ = new QFutureWatcher<ChartData>(this);
    connect(chargeWatcher_, &QFutureWatcher<ChartData>::finished,
            this, &ResultTubeSimulation::onChargeDataReady);

    chargeChart_->setChart(chart);
    chargeChart_->setRenderHint(QPainter::Antialiasing);
    chargeChart_->setStyleSheet("background: transparent; border: none;");
    chargeChart_->setAttribute(Qt::WA_TranslucentBackground);
    chargeChart_->viewport()->setAttribute(Qt::WA_TranslucentBackground);
    chargeChart_->setContentsMargins(0, 0, 0, 0);
    chargeChart_->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
}


void ResultTubeSimulation::onChargeDataReady()
{
    if (chargeWatcher_->isCanceled())
        return;

    ChartData d = chargeWatcher_->result();
    chargeSeries_->replace(d.pts);

    float uRange = d.uMax - d.uMin;
    float vRange = d.vMax - d.vMin;
    float uMin   = d.uMin - uRange/2;
    float uMax   = d.uMax + uRange/2;
    float vMin   = d.vMin - vRange/2;
    float vMax   = d.vMax + vRange/2;

    axisX_->setRange(uMin, uMax);
    axisY_->setRange(vMin, vMax);

    QRectF pa = chargeChart_->chart()->plotArea();
    qreal w = pa.width(), h = pa.height();
    qreal unitPxX = (uMax - uMin) / w;
    qreal unitPxY = (vMax - vMin) / h;

    if (unitPxX < unitPxY) {
        qreal newVR = unitPxX * h;
        qreal mid   = (vMin + vMax) / 2;
        axisY_->setRange(mid - newVR/2, mid + newVR/2);
    } else {
        qreal newUR = unitPxY * w;
        qreal mid   = (uMin + uMax) / 2;
        axisX_->setRange(mid - newUR/2, mid + newUR/2);
    }

    const int ticksX = 11;
    const int ticksY = 11;
    axisX_->setTickCount(ticksX);
    axisY_->setTickCount(ticksY);

    axisX_->setLabelFormat("%.1f");
    axisY_->setLabelFormat("%.1f");

    chargeChart_->update();
}


void ResultTubeSimulation::updateChargeChart()
{
    if (chargeWatcher_->isRunning()) {
        chargeWatcher_->cancel();
        chargeWatcher_->waitForFinished();
    }
    int plane = planeGroup_->checkedId();

    auto future = QtConcurrent::run([this, plane]() -> ChartData {
        ChartData d;
        d.uMin = d.vMin =  std::numeric_limits<qreal>::max();
        d.uMax = d.vMax = -std::numeric_limits<qreal>::max();
        d.pts.reserve(charges_.size());

        const qreal eps = 1e-3;
        for (const Charge &c : charges_) {
            qreal u, v;
            switch (plane) {
                case 1: u = c.PositionY; v = c.PositionZ; break;
                case 2: u = c.PositionZ; v = c.PositionX; break;
                default: u = c.PositionX; v = c.PositionY; break;
            }
            d.pts.append({u, v});
            d.uMin = qMin(d.uMin, u);
            d.uMax = qMax(d.uMax, u);
            d.vMin = qMin(d.vMin, v);
            d.vMax = qMax(d.vMax, v);
        }

        if (d.uMax <= d.uMin) d.uMax = d.uMin + eps;
        if (d.vMax <= d.vMin) d.vMax = d.vMin + eps;
        return d;
    });

    chargeWatcher_->setFuture(future);
}


void ResultTubeSimulation::setMainWindow(MainWindow* main) {
    mainWindow = main;
}

void ResultTubeSimulation::resizeEvent(QResizeEvent* event)
{

    QWidget::resizeEvent(event);
}

void ResultTubeSimulation::paintEvent(QPaintEvent *event)
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

QVector<VariableEntry> ResultTubeSimulation::parseVariableFile(const QString &filePath) {
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

QString ResultTubeSimulation::getVariableValue(const QVector<VariableEntry> &variables, const QString &name) {
    for (const auto &entry : variables) {
        if (entry.name == name) {
            return entry.value;
        }
    }
    return QString();
}

void ResultTubeSimulation::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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
