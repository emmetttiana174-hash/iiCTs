#ifndef MODELHOME_H
#define MODELHOME_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ModelHome : public QWidget {
    Q_OBJECT

public:
    explicit ModelHome(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};


#endif // MODELHOME_H
