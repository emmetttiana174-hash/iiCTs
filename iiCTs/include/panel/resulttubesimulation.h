#ifndef RESULTTUBESIMULATION_H
#define RESULTTUBESIMULATION_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLabel>
#include <QLineEdit>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QChartView>
#include <QBarSeries>
#include <QLineSeries>
#include <QStackedLayout>
#include <QCheckBox>
#include <QFutureWatcher>

#include <QtCharts/QScatterSeries>
#include <QtCharts/QValueAxis>

#include "projectinfo.h"
#include "resultctreconstruction.h"

using namespace QtCharts;

class MainWindow;
class ChatPanel;


class ResultTubeSimulation : public QWidget {
    Q_OBJECT

public:
    explicit ResultTubeSimulation(QWidget* parent = nullptr);
    void setMainWindow(MainWindow* main);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QVector<VariableEntry> parseVariableFile(const QString &filePath);
    QString getVariableValue(const QVector<VariableEntry> &variables, const QString &name);

private slots:
    void onRefreshFileClicked();
    void onChargeDataReady();

signals:
    void triggerUpToggled();

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);
    void scriptChanged(const QString &filePath);

private:
    struct Charge {
        float PositionX, PositionY, PositionZ;
        float MomentumX, MomentumY, MomentumZ;
        float Mass, ChargeMacro, Time, Current;
    };

    struct ChartData {
        QVector<QPointF> pts;
        qreal uMin, uMax, vMin, vMax;
    };

    QScatterSeries*        chargeSeries_  = nullptr;
    QFutureWatcher<ChartData> *chargeWatcher_ = nullptr;
    QValueAxis*            axisX_         = nullptr;
    QValueAxis*            axisY_         = nullptr;

    void setupUi();
    void initChargeChart();
    void updateChargeChart();

    MainWindow* mainWindow = nullptr;

    QRadioButton*     planeXY_;
    QRadioButton*     planeYZ_;
    QRadioButton*     planeZX_;
    QButtonGroup*     planeGroup_;

    QWidget*          imageContainer;
    QChartView*       chargeChart_;

    QPushButton*      btnRefreshFile_;

    float             AnodeRadius;
    QVector<Charge>       charges_;

    QString               scriptTube_;
    ProjectInfo            allInfo;
    QVector<VariableEntry> scriptTubes;

protected:
    bool readChargeFile();
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // RESULTTUBESIMULATION_H
