#ifndef IMAGECONTAINER_H
#define IMAGECONTAINER_H

// ImageContainer.h
#pragma once
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>
#include <QResizeEvent>

class ImageContainer : public QWidget {
  Q_OBJECT

public:
  explicit ImageContainer(QWidget *parent = nullptr);

  void setPixmap(const QPixmap &pix);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QLabel *imageLabel_;
  QPixmap original_;
};


#endif // IMAGECONTAINER_H
