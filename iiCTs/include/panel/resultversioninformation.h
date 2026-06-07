#ifndef RESULTVERSIONINFORMATION_H
#define RESULTVERSIONINFORMATION_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class ResultVersionInformation : public QWidget {
    Q_OBJECT

public:
    explicit ResultVersionInformation(QWidget* parent = nullptr);

private:
    QWidget* imageContainer;
    QLabel* imageLabel;
    QPixmap pixmapCT;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

};

#endif // RESULTVERSIONINFORMATION_H
