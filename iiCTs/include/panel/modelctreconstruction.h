#ifndef MODELCTRECONSTRUCTION_H
#define MODELCTRECONSTRUCTION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ModelCTReconstruction : public QWidget {
    Q_OBJECT

public:
    explicit ModelCTReconstruction(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};

#endif // MODELCTRECONSTRUCTION_H
