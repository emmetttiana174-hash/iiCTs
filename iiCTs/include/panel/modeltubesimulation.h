#ifndef MODELTUBESIMULATION_H
#define MODELTUBESIMULATION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ModelTubeSimulation : public QWidget {
    Q_OBJECT

public:
    explicit ModelTubeSimulation(QWidget* parent = nullptr);

private:
    void refreshPixmap();

    QWidget* imageContainer;
    QPushButton* btnRefresh;
    QPushButton* btnExport;
    QPushButton* btnSettings;

    QLabel*           imageLabel_;
    QPixmap           originalPixmap_;
    QLabel*           imageLabel1_;
    QPixmap           originalPixmap1_;
    QLabel*           imageMove_;
    QMovie*           gifMovie_;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};


#endif // MODELTUBESIMULATION_H
