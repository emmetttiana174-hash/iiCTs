#ifndef RESULTCTSCANSIMULATION_H
#define RESULTCTSCANSIMULATION_H

#include <QWidget>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QTransform>

#include "projectinfo.h"
#include "resultctreconstruction.h"

class MainWindow;
class ChatPanel;

class ResultCTScanSimulation : public QWidget {
    Q_OBJECT

public:
    explicit ResultCTScanSimulation(QWidget* parent = nullptr);
    void setMainWindow(MainWindow* main);
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);
    QVector<VariableEntry> parseVariableFile(const QString &filePath);
    QString getVariableValue(const QVector<VariableEntry> &variables, const QString &name);

private slots:
    void onModeChanged(int id);
    void onGenClicked();
    void onRefreshClicked();

    void onPlaneClicked();
    void indexEditChange();
    void onSliderValueChanged(int v);

    void maxWinChanged();
    void minWinChanged();
    void levelWinChanged();
    void widthWinChanged();

    void onFlipVert();
    void onFlipHorz();
    void onRotateCW();
    void onRotateCCW();
    void onResetTransform();
    void onRange();

public slots:
    void onProjectInfoChanged(const ProjectInfo &info);
    void scriptChanged(const QString &filePath);
    void scriptCTChanged(const QString &filePath);

private:
    void setupUi();
    bool generateVolume();
    bool readVolumeFile();
    void updateSliderRange();
    void updateImageDisplay();

    MainWindow* mainWindow = nullptr;

    QRadioButton*    aiButton_;
    QRadioButton*    phButton_;
    QButtonGroup*    modeGroup_;
    QPushButton*     genButton_;
    QPushButton*     refreshButton_;

    QLineEdit*       indexEdit_;
    QSlider*         layerSlider_;
    QLabel*          imageLabel_;
    QPixmap          originalPixmap_;
    QWidget*         imageContainer;

    QLineEdit*       windowLevelEdit_;
    QLineEdit*       windowWidthEdit_;
    QLineEdit*       dispMaxEdit_;
    QLineEdit*       dispMinEdit_;
    QPushButton*     flipVertButton_;
    QPushButton*     flipHorzButton_;
    QPushButton*     rotateCWButton_;
    QPushButton*     rotateCCWButton_;
    QTransform transform_;

    QVector<float>   volume_;
    int              dimH_, dimW_, dimN_;
    QString          suffix_;
    QString          currentPlane_;
    int              currentLayer_;
    QString          script_;
    QString          scriptCT_;
    QString          volumeFolder_;
    QString          volumeFileBase_;
    QImage           currentImage_;

    ProjectInfo allInfo;
    QVector<VariableEntry> scripts;
    QVector<VariableEntry> scriptCTs;
    QVector<VariableEntry> detectorScripts;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
};

#endif // RESULTCTSCANSIMULATION_H
