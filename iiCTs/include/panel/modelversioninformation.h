#ifndef MODELVERSIONINFORMATION_H
#define MODELVERSIONINFORMATION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ModelVersionInformation : public QWidget {
    Q_OBJECT

public:
    explicit ModelVersionInformation(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};
#endif // MODELVERSIONINFORMATION_H
