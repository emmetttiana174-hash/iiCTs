#include "modeltubesimulation.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QMovie>

ModelTubeSimulation::ModelTubeSimulation(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);



    imageContainer = new QWidget(this);
    imageContainer->setObjectName("imgContainer");
    imageContainer->setAttribute(Qt::WA_StyledBackground, true);
    imageContainer->setSizePolicy(QSizePolicy::Expanding,
                                  QSizePolicy::Expanding);
    imageContainer->setStyleSheet(R"(
                                  #imgContainer {
                                  background-color: #22489E;
                                  border: 2px solid #7697D8;
                                  border-radius: 12px;
                                  }
                                  )");
    imageContainer->setContentsMargins(10, 10, 10, 10);


    imageLabel_ = new QLabel(imageContainer);
    originalPixmap_ = QPixmap(":/images/eduCT/eduCTCathode.png");
    imageLabel_->setAlignment(Qt::AlignCenter);
    imageLabel_->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Expanding);
    imageLabel_->setScaledContents(false);

    imageLabel1_ = new QLabel(imageContainer);
    originalPixmap1_ = QPixmap(":/images/eduCT/eduCTAnode.png");
    imageLabel1_->setAlignment(Qt::AlignCenter);
    imageLabel1_->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
    imageLabel1_->setScaledContents(false);

    imageMove_ = new QLabel(imageContainer);
    gifMovie_ = new QMovie(":/images/eduCT/eduCTTubeRun.gif", QByteArray(), this);
    imageMove_->setAlignment(Qt::AlignCenter);
    imageMove_->setSizePolicy(QSizePolicy::Expanding,
                              QSizePolicy::Expanding);
    imageMove_->setScaledContents(false);
    imageMove_->setMovie(gifMovie_);
    gifMovie_->setScaledSize( imageMove_->size() );
    gifMovie_->start();

    auto *layUp = new QHBoxLayout;
    layUp->setContentsMargins(0,0,0,0);
    layUp->addWidget(imageLabel_);
    layUp->addWidget(imageLabel1_);

    auto *lay = new QVBoxLayout(imageContainer);
    lay->setContentsMargins(0,0,0,0);
    lay->addLayout(layUp);
    lay->addWidget(imageMove_);
    mainLayout->addWidget(imageContainer, /*stretch=*/1);

    refreshPixmap();

    QLabel* titleLabel = new QLabel("Please use the commercial CST Studio Suite software to perform the simulation.");
    titleLabel->setStyleSheet(R"(
                              color: white;
                              font-family: 'Times New Roman';
                              font-size: 20px;
                              font-weight: bold;
                              font-style: italic;
                              )");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
}


void ModelTubeSimulation::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    refreshPixmap();

}


void ModelTubeSimulation::refreshPixmap() {
    if (originalPixmap_.isNull()) return;
    QSize target = imageLabel_->size();
    imageLabel_->setPixmap(
                originalPixmap_.scaled(
                    target,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );

    if (originalPixmap1_.isNull()) return;
    target = imageLabel1_->size();
    imageLabel1_->setPixmap(
                originalPixmap1_.scaled(
                    target,
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation
                    )
                );

    if (gifMovie_) {
        QSize frameSize = gifMovie_->currentImage().size();
        if (frameSize.isEmpty())
            frameSize = gifMovie_->frameRect().size();

        if (!frameSize.isEmpty()) {
            QSize labelSize = imageMove_->size();

            double sx = double(labelSize.width())  / frameSize.width();
            double sy = double(labelSize.height()) / frameSize.height();
            double scale = qMin(sx, sy);

            QSize scaledSize(
                        int(frameSize.width()  * scale),
                        int(frameSize.height() * scale)
                        );

            gifMovie_->setScaledSize(scaledSize);
        }
    }

    QResizeEvent *evt = new QResizeEvent(imageContainer->size(),imageContainer->size());
    QCoreApplication::postEvent(this, evt);
}


void ModelTubeSimulation::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, QColor("#22489E"));
    gradient.setColorAt(1, QColor("#4875D0"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect(), 15, 15);
}
