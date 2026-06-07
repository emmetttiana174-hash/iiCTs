#include "modeluserguide.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>

ModelUserGuide::ModelUserGuide(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    imageContainer = new QWidget;
//    imageContainer->setStyleSheet(R"(
//                                  background-color: #22489E;
//                                  border: 2px dashed #4875D0;
//                                  border-radius: 10px;
//                                  )");
    imageContainer->setStyleSheet(R"(
                                  background-color: transparent;
                                  border: 2px dashed none;
                                  )");
    imageContainer->setContentsMargins(0, 0, 0, 0);
    imageContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    pixmapCT = QPixmap(":/images/CT/CT.PNG");
    imageLabel = new QLabel(imageContainer);
    imageLabel->setPixmap(pixmapCT.scaled(
                              QSize(550, 550),
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


    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    QLabel* welcomeLabel = new QLabel("Welcome to Integrated Intelligent CT Simulation System");
    welcomeLabel->setFont(QFont("Times New Roman", 20, QFont::Bold));
    welcomeLabel->setStyleSheet("color: white; background: transparent;");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    buttonLayout->addWidget(welcomeLabel);

    mainLayout->addSpacing(20);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addSpacing(20);
    mainLayout->addStretch();
}


void ModelUserGuide::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
}


void ModelUserGuide::paintEvent(QPaintEvent *event)
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
