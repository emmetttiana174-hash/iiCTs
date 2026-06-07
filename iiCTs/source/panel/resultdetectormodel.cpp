#include "resultdetectormodel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>

ResultDetectorModel::ResultDetectorModel(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(40);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setAlignment(Qt::AlignCenter);



    imageContainer = new QWidget;
    imageContainer->setStyleSheet(R"(
                                  background-color: transparent;
                                  border: 2px dashed none;
                                  )");
    imageContainer->setContentsMargins(0, 0, 0, 0);
    imageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    pixmapCT = QPixmap(":/images/background/CTISS_logo.png");
    imageLabel = new QLabel(imageContainer);
    imageLabel->setPixmap(pixmapCT.scaled(
                              QSize(500, 200),
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation
                              ));
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setStyleSheet(R"(
                              background-color: transparent;
                              border: none;
                              )");

    QHBoxLayout *imageLayout = new QHBoxLayout(imageContainer);
    imageLayout->addWidget(imageLabel);
    imageLayout->setAlignment(Qt::AlignCenter);

    mainLayout->addStretch();
    mainLayout->addWidget(imageContainer);

    QLabel *imageLeft = new QLabel;
    imageLeft->setPixmap(QPixmap(":/images/background/smu_logo.png").scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLeft->setScaledContents(true);
    imageLeft->setStyleSheet("background: transparent;");
    QLabel *imageRight = new QLabel;
    imageRight->setPixmap(QPixmap(":/images/background/xjtu_logo.png").scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageRight->setScaledContents(true);
    imageRight->setStyleSheet("background: transparent;");

    QHBoxLayout *schoolLayout = new QHBoxLayout;
    schoolLayout->setContentsMargins(0, 0, 0, 0);
    schoolLayout->setSpacing(50);
    schoolLayout->addStretch();
    schoolLayout->addWidget(imageLeft);
    schoolLayout->addWidget(imageRight);
    schoolLayout->addStretch();
    mainLayout->addLayout(schoolLayout);

    QLabel *topText = new QLabel("Southern Medical University\nXi’an Jiaotong University");
    topText->setStyleSheet("color: white; font-size: 36px; font-family: 'Times New Roman'; font-weight: 700; background: transparent;");
    topText->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(topText);


    QLabel *contactTex = new QLabel;
    contactTex->setText("If you encounter any problems, please contact us.\nEmail:**@gmial.com");
    contactTex->setStyleSheet(
                "background: transparent;"
                "border: 2px dashed white;"
                "color: white;"
                "font-size: 24px;"
                "font-style: italic;"
                "font-family: 'Times New Roman';"
                "border-radius: 10px;"
                );
    contactTex->setContentsMargins(0, 20, 0, 20);
    contactTex->setWordWrap(true);
    contactTex->setAlignment(Qt::AlignCenter);

    mainLayout->addStretch();
    mainLayout->addWidget(contactTex);

}


void ResultDetectorModel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}


void ResultDetectorModel::paintEvent(QPaintEvent *event)
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
