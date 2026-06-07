#include "imagecontainer.h"

ImageContainer::ImageContainer(QWidget *parent)
  : QWidget(parent),
    imageLabel_(new QLabel(this))
{
  imageLabel_->setAlignment(Qt::AlignCenter);
  auto *lay = new QVBoxLayout(this);
  lay->setContentsMargins(0,0,0,0);
  lay->addWidget(imageLabel_);
}

void ImageContainer::setPixmap(const QPixmap &pix) {
  original_ = pix;
  imageLabel_->setPixmap(pix);
}

void ImageContainer::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);

  if (!original_.isNull()) {
    auto scaled = original_.scaled(
      event->size(),
      Qt::KeepAspectRatio,
      Qt::SmoothTransformation
    );
    imageLabel_->setPixmap(scaled);
  }
}
