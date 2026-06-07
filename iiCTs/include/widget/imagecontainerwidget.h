#ifndef IMAGECONTAINERWIDGET_H
#define IMAGECONTAINERWIDGET_H

#include <QWidget>
#include <QSlider>
#include <QtGlobal>
#include <QWheelEvent>


class ImageContainerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ImageContainerWidget(QWidget *parent = nullptr, QSlider *slider = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    QSlider *slider_;
};

#endif // IMAGECONTAINERWIDGET_H
