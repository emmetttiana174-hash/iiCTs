#include "modeldetectormodel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QLabel>


ModelDetectorModel::ModelDetectorModel(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* titleLabel = new QLabel("ModelDetectorModel");
    titleLabel->setStyleSheet("color: white; font-size: 20px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    imageContainer = new QWidget;



    // 将背景改为全黑，并将边框颜色改为灰色
    imageContainer->setStyleSheet(R"(
        background-color: #000000;
        border: 2px dashed #666666;
        border-radius: 10px;
    )");

    mainLayout->addWidget(imageContainer, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    btnRefresh = new QPushButton("Refresh");
    btnExport  = new QPushButton("Export");
    btnSettings= new QPushButton("Settings");

    QSize btnSize(100, 40);
    btnRefresh->setFixedSize(btnSize);
    btnExport->setFixedSize(btnSize);
    btnSettings->setFixedSize(btnSize);

    QString btnStyle = R"(
        QPushButton {
            color: white;
            background-color: #3366CC;
            font-weight: bold;
            border-radius: 5px;
            border: 2px solid #2F528F;
        }
        QPushButton:hover {
            background-color: #61A7F5;
            border: 2px solid #41A1F5;
        }
    )";
    btnRefresh->setStyleSheet(btnStyle);
    btnExport->setStyleSheet(btnStyle);
    btnSettings->setStyleSheet(btnStyle);

    buttonLayout->addWidget(btnRefresh);
    buttonLayout->addWidget(btnExport);
    buttonLayout->addWidget(btnSettings);



    mainLayout->addLayout(buttonLayout);
}


void ModelDetectorModel::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

}


void ModelDetectorModel::paintEvent(QPaintEvent *event)
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
