#include "imagecontainerwidget.h"
#include <QWidget>
#include <QPainter>

ImageContainerWidget::ImageContainerWidget(QWidget *parent, QSlider *slider)
    : QWidget(parent), slider_(slider) {

}


void ImageContainerWidget::wheelEvent(QWheelEvent *event) {
    if (!slider_) return;

    int delta = event->angleDelta().y();
    int step = delta > 0 ? -1 : 1;

    int newValue = qBound(slider_->minimum(), slider_->value() + step, slider_->maximum());
    slider_->setValue(newValue);
}
