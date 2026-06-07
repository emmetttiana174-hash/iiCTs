#ifndef RESULTSPECTRUMSIMULATION_H
#define RESULTSPECTRUMSIMULATION_H

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

#include "projectinfo.h"
#include "resultctreconstruction.h"

using namespace QtCharts;

class MainWindow;
class ChatPanel;

class ResultSpectrumSimulation : public QWidget {
    Q_OBJECT

public:
    explicit ResultSpectrumSimulation(QWidget* parent = nullptr);
    void setMainWindow(MainWindow* main);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QVector<VariableEntry> parseVariableFile(const QString &filePath);
    QString getVariableValue(const QVector<VariableEntry> &variables, const QString &name);

private slots:
    void onModeToggled(int modeId);
    void onAddClicked();
    void onDeleteSelectedClicked();
    void onClearClicked();
    void onRefreshFileClicked();

signals:
    void triggerUpToggled();

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);
    void scriptChanged(const QString &filePath);

private:
    struct Photon {
        float energy;
        float u, v;
        int cellX, cellY;
    };

    struct Point {
        int row;
        int col;
    };

    void setupUi();
    void computeDensityGrid();
    void computeGlobalSpectrum();
    void computeSubSpectra();
    void initDefaultSelections(QVector<Point> defaults);
    void updatePlot();
    void updateCoordinateLabels();
    void refreshPixmap();
    void initTotalSpectrumChart();
    void initSubSpectrumChart();
    void updateTotalSpectrumChart();

    QVector<Point>        selectedPoints_;
    QVector<QCheckBox*>   selectionBoxes_;
    QLabel     *rowLabel_ = nullptr;
    QLabel     *colLabel_ = nullptr;
    QGridLayout *coordLayout_ = nullptr;

    MainWindow* mainWindow = nullptr;

    QPushButton*      densityButton_;
    QPushButton*      spectButton_;
    QPushButton*      subSpectButton_;
    QStackedLayout*   stack;

    QRadioButton*     planeXY_;
    QRadioButton*     planeYZ_;
    QRadioButton*     planeZX_;
    QButtonGroup*     planeGroup_;

    QLabel*           imageLabel_;
    QPixmap           originalPixmap_;
    QWidget*          imageContainer;
    QChartView*       totalSpectChart_;
    QChartView*       subSpectChart_;

    QWidget*          selectedContainer_;
    QLineEdit*        editRow_;
    QLineEdit*        editCol_;
    QLineEdit*        editDspMin_;
    QLineEdit*        editDspMax_;
    QPushButton*      btnAdd_;
    QPushButton*      btnDeleteSelected_;
    QPushButton*      btnClear_;
    QPushButton*      btnRefreshFile_;


    QVector<Photon>       photons_;
    QVector<int>          densityGrid_;
    QVector<int>          globalSpectrum_;
    QList<QPair<int,int>> selectedCells_;
    QMap<QPair<int,int>, QVector<int>> subSpectra_;

    QVector<float>        volume_;
    QString               scriptSpec_;

    float                 DetSizeX, DetSizeY;
    float                 GridSizeX, GridSizeY;
    float                 EnergyBinSize;
    int                   dimX_, dimY_, nEnergyBins_;
    float                 uMin_, uMax_, vMin_, vMax_;

    double customMinValue_{ std::numeric_limits<double>::quiet_NaN() };
    double customMaxValue_{ std::numeric_limits<double>::quiet_NaN() };
    static constexpr int  UNSET_VALUE = std::numeric_limits<int>::min();
    int actualMinValue_{ UNSET_VALUE };
    int actualMaxValue_{ UNSET_VALUE };

    ProjectInfo            allInfo;
    QVector<VariableEntry> scriptSpecs;


protected:
    bool readPhotonFile();
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // RESULTSPECTRUMSIMULATION_H
