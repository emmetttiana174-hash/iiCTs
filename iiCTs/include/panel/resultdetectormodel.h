#ifndef RESULTDETECTORMODEL_H
#define RESULTDETECTORMODEL_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ResultDetectorModel : public QWidget {
    Q_OBJECT

public:
    explicit ResultDetectorModel(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};


#endif // RESULTDETECTORMODEL_H
