#include "loginwindow.h"
#include "registerwindow.h"
#include "userdb.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QDebug>
#include <QGuiApplication>
#include <QScreen>
#include <QPainter>
#include <QStackedLayout>

const int BORDER_WIDTH = 8;

#ifndef SCALE_RATE
#define SCALE_RATE 0.6
#endif

LoginWindow::LoginWindow() {

    int winW = static_cast<int>(screenW * SCALE_RATE);
    int winH = winW * 9 / 16;
    if (winH > static_cast<int>(screenH * SCALE_RATE)) {
        winH = static_cast<int>(screenH * SCALE_RATE);
        winW = winH * 16 / 9;
    }
    resize(winW, winH);
    //    resize(1920, 980);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setMouseTracking(true);
    setAttribute(Qt::WA_TranslucentBackground);


    if (useDefaultGeometry) {
        int x = screenGeometry.center().x() - width() / 2;
        int y = screenGeometry.center().y() - height() / 2 + height() / 8;
        move(x, y);
    }

    QWidget *container = new QWidget(this);
    container->setStyleSheet("background-color: rgb(26, 54, 120); border-radius: 40px;");

    QLabel *cornerImage = new QLabel(this);
    QPixmap CTISS_logo(":/images/background/CTISS_logo.png");
    cornerImage->setPixmap(CTISS_logo.scaled(300, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    cornerImage->setFixedSize(300, 80);
    cornerImage->move(20, 20);
    cornerImage->setStyleSheet("background: transparent;");
    cornerImage->raise();


    topImage = new QLabel(this);
    QPixmap background_up(":/images/background/background_up.png");
    topImage->setPixmap(background_up.scaled(980, 129, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    topImage->setFixedSize(980, 129);
    topImage->setStyleSheet("background: transparent;");
    topImage->raise();
    int initialX = (this->width() - 700) / 2;
    topImage->move(initialX, 0);


    sysTopLabel = new QLabel("Integrated Intelligent CT Simulation System", this);
    sysTopLabel->setStyleSheet("color: white;");
    QFont font("Times New Roman", 32, QFont::Bold);
    sysTopLabel->setFont(font);
    sysTopLabel->setAlignment(Qt::AlignCenter);
    sysTopLabel->setFixedSize(980, 100);
    sysTopLabel->raise();
    sysTopLabel->move(initialX, 0);

    bottomInfoBox = new QWidget(this);
    bottomInfoBox->setAttribute(Qt::WA_TranslucentBackground);
    bottomInfoBox->setFixedSize(300, 160);
    bottomInfoBox->setStyleSheet("background: transparent;");


    QLabel *imageLeft = new QLabel(bottomInfoBox);
    QPixmap smuPixmap(":/images/background/smu_logo.png");
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setFixedSize(smuPixmap.size());
    imageLeft->setFixedSize(70, 70);
    imageLeft->setScaledContents(true);
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setStyleSheet("background: transparent;");
    QLabel *imageRight = new QLabel(bottomInfoBox);
    imageRight->setPixmap(QPixmap(":/images/background/xjtu_logo.png").scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageRight->setStyleSheet("background: transparent;");
    QHBoxLayout *imageLayout = new QHBoxLayout;
    imageLayout->setContentsMargins(0, 0, 0, 0);
    imageLayout->setSpacing(20);
    imageLayout->addStretch();
    imageLayout->addWidget(imageLeft);
    imageLayout->addWidget(imageRight);
    imageLayout->addStretch();


    QLabel *topText = new QLabel("Southern Medical University\nXi’an Jiaotong University", bottomInfoBox);
    topText->setStyleSheet("color: white; font-size: 18px; font-family: 'Times New Roman'; font-weight: 700; background: transparent;");
    topText->setAlignment(Qt::AlignCenter);
    QLabel *bottomText = new QLabel("Version 1.0 Beta", bottomInfoBox);
    bottomText->setStyleSheet("color: white; font-size: 18px; font-family: 'Times New Roman'; font-style: italic; background: transparent;");
    bottomText->setAlignment(Qt::AlignCenter);

    QVBoxLayout *mainInfoLayout = new QVBoxLayout(bottomInfoBox);
    mainInfoLayout->setContentsMargins(0, 0, 0, 0);
    mainInfoLayout->setSpacing(5);
    mainInfoLayout->addLayout(imageLayout);
    mainInfoLayout->addWidget(topText);
    mainInfoLayout->addWidget(bottomText);

    bottomInfoBox->setLayout(mainInfoLayout);
    bottomInfoBox->raise();


    mainLayout = new QVBoxLayout(container);
    QSpacerItem *topSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addSpacerItem(topSpacer);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(250, 0, 0, 0);

    titleWidget = new QWidget(container);
    titleWidget->setFixedSize(150, 40);
    titleWidget->move(this->width() - titleWidget->width() - 15, 10);
    titleWidget->setStyleSheet("background-color: transparent;");

    QHBoxLayout *titleBar = new QHBoxLayout(titleWidget);
    titleBar->setContentsMargins(0, 0, 0, 0);
    titleBar->setSpacing(2);

    QPushButton *minBtn = new QPushButton(this);
    QPushButton *maxBtn = new QPushButton(this);
    QPushButton *closeBtn = new QPushButton(this);

    QIcon minIcon(":/images/background/icon_min.png");
    QIcon maxIcon(":/images/background/icon_max.png");
    QIcon closeIcon(":/images/background/icon_close.png");
    minBtn->setIcon(minIcon);
    maxBtn->setIcon(maxIcon);
    closeBtn->setIcon(closeIcon);

    minBtn->setFixedSize(40, 40);
    maxBtn->setFixedSize(40, 40);
    closeBtn->setFixedSize(40, 40);
    minBtn->setContentsMargins(0, 0, 0, 0);
    maxBtn->setContentsMargins(0, 0, 0, 0);
    closeBtn->setContentsMargins(0, 0, 0, 0);

    QString hoverStyle =
            "QPushButton { background: transparent; border: none; }"
            "QPushButton:hover { background-color: #F3D8F1; border-radius: 10px;}";
    minBtn->setStyleSheet(hoverStyle);
    maxBtn->setStyleSheet(hoverStyle);
    closeBtn->setStyleSheet(hoverStyle);

    titleBar->addStretch();
    titleBar->addWidget(minBtn);
    titleBar->addWidget(maxBtn);
    titleBar->addWidget(closeBtn);

    connect(minBtn, &QPushButton::clicked, this, &LoginWindow::showMinimized);
    connect(maxBtn, &QPushButton::clicked, this, [=]() {
        if (isMaximized()) {
            showNormal();
        } else {
            QRect screenRect = QGuiApplication::primaryScreen()->availableGeometry();
            int screenW = screenRect.width();
            int screenH = screenRect.height();

            int targetW = screenW;
            int targetH = targetW * 9 / 16;
            if (targetH > screenH) {
                targetH = screenH;
                targetW = targetH * 16 / 9;
            }
            int x = screenRect.x() + (screenW - targetW) / 2;
            int y = screenRect.y() + (screenH - targetH) / 2;
            setGeometry(x, y, targetW, targetH);
        }
    });
    connect(closeBtn, &QPushButton::clicked, this, &LoginWindow::close);

    QString labelStyle = "min-height: 30px; color: rgb(255, 255, 255); font-size: 28px; font-weight: bold; font-family: 'Times New Roman'";
    QString editStyle = "min-height: 30px; color: rgb(255, 255, 255); font-size: 28px; font-family: 'Times New Roman'";
    QString editBoxStyle = "QLineEdit {"
                           "min-height: 30px;"
                           "  border: 2px solid #1A3678;"
                           "  border-radius: 5px;"
                           "  padding: 5px;"
                           "  background-color: #F0F8FF;"
                           "  color: #000000;"
                           "  font-size: 18px;"
                           "  font-weight: bold;"
                           "}"
                           "QLineEdit:hover {"
                           "background-color: #FFF2C9;"
                           "}";

    QLabel *usernameLabel = new QLabel("Username:");
    QLineEdit *usernameEdit = new QLineEdit;
    QLabel *passwordLabel = new QLabel("Password:");
    QLineEdit *passwordEdit = new QLineEdit;

    usernameEdit->setText("admin");
    passwordEdit->setText("123456");
    usernameLabel->setFixedWidth(150);
    usernameEdit->setFixedWidth(300);
    usernameLabel->setStyleSheet(labelStyle);
    usernameEdit ->setStyleSheet(editStyle);
    passwordLabel->setFixedWidth(150);
    passwordEdit->setFixedWidth(300);
    passwordLabel->setStyleSheet(labelStyle);
    passwordEdit ->setStyleSheet(editStyle);
    usernameEdit->setStyleSheet(editBoxStyle);
    passwordEdit->setStyleSheet(editBoxStyle);
    usernameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    passwordLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addStretch();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameEdit);
    userLayout->addStretch();
    mainLayout->addLayout(userLayout);

    passwordEdit->setEchoMode(QLineEdit::Password);
    QHBoxLayout *passLayout = new QHBoxLayout;
    passLayout->addStretch();
    passLayout->addWidget(passwordLabel);
    passLayout->addWidget(passwordEdit);
    passLayout->addStretch();
    mainLayout->addLayout(passLayout);


    QString btnStyle = "QPushButton {"
                       "min-width: 160px;"
                       "min-height: 40px;"
                       "  border: 3px solid #1A3678;"
                       "  border-radius: 25px;"
                       "background: qlineargradient("
                       "x1: 0, y1: 0, x2: 1, y2: 0,"
                       "stop: 0 #CEBAF6,"
                       "stop: 1 #41A1F5"
                       ");"
                       "  color: white;"
                       "  font-size: 24px;"
                       "  font-family: 'Times New Roman';"
                       "  font-weight: bold;"
                       "  padding: 8px 16px;"
                       "}"
                       "QPushButton:hover {"
                       "background: qlineargradient("
                       "x1: 0, y1: 0, x2: 1, y2: 0,"
                       "stop: 0 #83C9CD,"
                       "stop: 1 #4D85F1"
                       ");"
                       "  border-color: #48ADB2;"
                       "  border: none;"
                       "}"
                       "QPushButton:pressed {"
                       "  background-color: #5588CC;"
                       "}";

    QPushButton *loginBtn = new QPushButton("Login");
    QPushButton *registerBtn = new QPushButton("Register");
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginBtn);
    buttonLayout->addWidget(registerBtn);
    buttonLayout->addStretch();
    loginBtn->setStyleSheet(btnStyle);
    registerBtn->setStyleSheet(btnStyle);
    mainLayout->addLayout(buttonLayout);

    QSpacerItem *buttonSpacer = new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
    mainLayout->addSpacerItem(buttonSpacer);

    connect(loginBtn, &QPushButton::clicked, this, [=]() {
        if (UserDB::validate(usernameEdit->text(), passwordEdit->text())) {
            emit loginSuccess(usernameEdit->text(), this->geometry());
            this->close();
        } else {
            showStyledWarning(this, "Login Failed", "Invalid username or password.");
        }
    });

    connect(registerBtn, &QPushButton::clicked, this, &LoginWindow::openRegisterWindow);

    cTSystemContraryLabel = new QLabel(this);
    QPixmap CTSystemContrary(":/images/CT/CT.PNG");
    cTSystemContraryLabel->setPixmap(CTSystemContrary);
    cTSystemContraryLabel->setScaledContents(true);

    tubeLabel = new QLabel(this);
    QPixmap tube(":/images/CT/tube.PNG");
    tubeLabel->setPixmap(tube);
    tubeLabel->setScaledContents(true);

    detectorLabel = new QLabel(this);
    QPixmap detector(":/images/CT/detector.PNG");
    detectorLabel->setPixmap(detector);
    detectorLabel->setScaledContents(true);

    computerLabel = new QLabel(this);
    QPixmap computer(":/images/CT/computer.PNG");
    computerLabel->setPixmap(computer);
    computerLabel->setScaledContents(true);

    powerLabel = new QLabel(this);
    QPixmap power(":/images/CT/power.PNG");
    powerLabel->setPixmap(power);
    powerLabel->setScaledContents(true);

    line1Label = new QLabel(this);
    line2Label = new QLabel(this);
    line3Label = new QLabel(this);
    line4Label = new QLabel(this);
    line5Label = new QLabel(this);
    line1Label->setScaledContents(true);
    line2Label->setScaledContents(true);
    line3Label->setScaledContents(true);
    line4Label->setScaledContents(true);
    line5Label->setScaledContents(true);

    int w = width();
    int h = height();
    int x1 = w * 0.2;
    int x2 = w * 0.8;
    int y2 = h * 0.8;
    int y1 = h * 0.8;
    int lineWidth = 3;
    int width = qAbs(x2 - x1) + lineWidth/2;
    int height = qAbs(y2 - y1) + lineWidth/2;
    int x = qMin(x1, x2);
    int y = qMin(y1, y2);
    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QColor customColor(71, 116, 207, 255);
    QPen pen(customColor);
    pen.setWidth(lineWidth);
    pen.setStyle(Qt::DashDotLine);
    painter.setPen(pen);
    painter.drawLine(0, 0, width, height);
    painter.end();
    line5Label->setPixmap(pixmap);
    line5Label->setScaledContents(true);
    line5Label->setGeometry(x, y, width, height);

    updateImageGeometry();

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(container);

    setMinimumSize(480, 270);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


void LoginWindow::updateImageGeometry()
{
    QSize windowSize = this->size();

    if (cTSystemContraryLabel->pixmap()) {
        QSize pixmapSize = cTSystemContraryLabel->pixmap()->size();
        QSize targetSize = pixmapSize.scaled(windowSize, Qt::KeepAspectRatio);
        int x = (windowSize.width() - targetSize.width()) / 2;
        int y = (windowSize.height() - targetSize.height()) / 2;
        cTSystemContraryLabel->setGeometry(x+targetSize.width()*0.35, y+targetSize.height()*0.15, targetSize.width()*0.55, targetSize.height()*0.55);
    }

    if (computerLabel->pixmap()) {
        QSize pixmapSize = computerLabel->pixmap()->size();
        QSize targetSize = pixmapSize.scaled(windowSize, Qt::KeepAspectRatio);
        int x = (windowSize.width() - targetSize.width()) / 2;
        int y = (windowSize.height() - targetSize.height()) / 2;
        computerLabel->setGeometry(x+targetSize.width()*1.08, y+targetSize.height()*0.4, targetSize.width()*0.3, targetSize.height()*0.3);
    }

    if (tubeLabel->pixmap()) {
        QSize pixmapSize = tubeLabel->pixmap()->size();
        QSize targetSize = pixmapSize.scaled(windowSize, Qt::KeepAspectRatio);
        int x = (windowSize.width() - targetSize.width()) / 2;
        int y = (windowSize.height() - targetSize.height()) / 2;
        tubeLabel->setGeometry(x+targetSize.width()*0.05, y+targetSize.height()*0.17, targetSize.width()*0.25, targetSize.height()*0.25);
    }

    if (powerLabel->pixmap()) {
        QSize pixmapSize = powerLabel->pixmap()->size();
        QSize targetSize = pixmapSize.scaled(windowSize, Qt::KeepAspectRatio);
        int x = (windowSize.width() - targetSize.width()) / 2;
        int y = (windowSize.height() - targetSize.height()) / 2;
        powerLabel->setGeometry(x+targetSize.width()*0.13, y+targetSize.height()*0.42, targetSize.width()*0.25, targetSize.height()*0.25);
    }

    if (detectorLabel->pixmap()) {
        QSize pixmapSize = detectorLabel->pixmap()->size();
        QSize targetSize = pixmapSize.scaled(windowSize, Qt::KeepAspectRatio);
        int x = (windowSize.width() - targetSize.width()) / 2;
        int y = (windowSize.height() - targetSize.height()) / 2;
        detectorLabel->setGeometry(x+targetSize.width()*0.95, y+targetSize.height()*0.16, targetSize.width()*0.24, targetSize.height()*0.24);
    }

    if (line1Label) {
        QRect centerGeometry = cTSystemContraryLabel->geometry();
        QRect computerGeometry = computerLabel->geometry();
        QRect tubeGeometry = tubeLabel->geometry();
        QRect powerGeometry = powerLabel->geometry();
        QRect detectorGeometry = detectorLabel->geometry();

        int lineWidth = 2;
        int radius = 2;

        QRect boundingRect = centerGeometry.united(computerGeometry);
        boundingRect = boundingRect.adjusted(-radius, -radius, radius, radius);
        line1Label->setGeometry(boundingRect);
        line2Label->setGeometry(boundingRect);
        line3Label->setGeometry(boundingRect);
        line4Label->setGeometry(boundingRect);

        int x1 = centerGeometry.x() + centerGeometry.width()*0.75;
        int y1 = centerGeometry.y() + centerGeometry.height()*0.75;
        int x2 = computerGeometry.x() + computerGeometry.width()*0.15;
        int y2 = computerGeometry.y() + computerGeometry.height()*0.5;
        drawLineWithCircles(*line1Label, x1, y1, x2, y2, lineWidth, radius);

        x1 = centerGeometry.x() + centerGeometry.width()*0.25;
        y1 = centerGeometry.y() + centerGeometry.height()*0.5;
        x2 = tubeGeometry.x() + tubeGeometry.width()*0.85;
        y2 = tubeGeometry.y() + tubeGeometry.height()*0.75;
        drawLineWithCircles(*line2Label, x1, y1, x2, y2, lineWidth, radius);

        x1 = centerGeometry.x() + centerGeometry.width()*0.20;
        y1 = centerGeometry.y() + centerGeometry.height()*0.85;
        x2 = powerGeometry.x() + computerGeometry.width()*1.5;
        y2 = powerGeometry.y() + computerGeometry.height()*0.65;
        drawLineWithCircles(*line3Label, x1, y1, x2, y2, lineWidth, radius);

        x1 = centerGeometry.x() + centerGeometry.width()*0.75;
        y1 = centerGeometry.y() + centerGeometry.height()*0.4;
        x2 = detectorGeometry.x() + computerGeometry.width()*0.15;
        y2 = detectorGeometry.y() + computerGeometry.height()*0.6;
        drawLineWithCircles(*line4Label, x1, y1, x2, y2, lineWidth, radius);
    }
}


void LoginWindow::handleLogin() {
    if (UserDB::validate(usernameEdit->text(), passwordEdit->text())) {
        emit loginSuccess(usernameEdit->text(), this->geometry());
        this->close();
    } else {
        showStyledWarning(this, "Login Failed", "Invalid username or password.");

    }
}

void LoginWindow::openRegisterWindow() {
    RegisterWindow *reg = new RegisterWindow();
    reg->resize(960, 450);
    reg->setWindowFlags(Qt::FramelessWindowHint);
    reg->setAttribute(Qt::WA_TranslucentBackground);
    reg->show();
}

void LoginWindow::resizeEvent(QResizeEvent *event) {
    int w = width();
    int expectedH = w * 9 / 16;
    if (!isMaximized()) {
        if (height() != expectedH)
            resize(w, expectedH);
    }
    if (titleWidget)
        titleWidget->move(width() - titleWidget->width() - 15, 10);

    if (topImage) {
        int titleX = (width() - 700) / 2;
        topImage->move(titleX, 0);
    }

    if (sysTopLabel) {
        int titleX = (width() - 700) / 2;
        sysTopLabel->move(titleX, 0);
    }

    updateImageGeometry();

    if (line5Label){
        int w = width();
        int h = height();
        int x = w * 0.25;
        int y = h * 0.72;
        int newWidth = w * 0.63;
        int newHeight = 2;

        line5Label->setGeometry(x, y, newWidth, newHeight);
    }

    if (bottomInfoBox) {
        int marginLeft = 20;
        int marginBottom = 20;
        bottomInfoBox->move(marginLeft, height() - bottomInfoBox->height() - marginBottom);
    }

    QWidget::resizeEvent(event);
}

void LoginWindow::mousePressEvent(QMouseEvent *event) {
    QRect rect = this->rect();
    QPoint pos = event->pos();

    resizing = false;
    dragging = false;
    resizeRegion = None;

    if (rect.right() - pos.x() <= BORDER_WIDTH &&
            rect.bottom() - pos.y() <= BORDER_WIDTH) {
        resizeRegion = BottomRight;
        resizing = true;
    } else if (rect.right() - pos.x() <= BORDER_WIDTH) {
        resizeRegion = Right;
        resizing = true;
    } else if (rect.bottom() - pos.y() <= BORDER_WIDTH) {
        resizeRegion = Bottom;
        resizing = true;
    }

    if (!resizing && event->button() == Qt::LeftButton) {
        dragging = true;
        dragPos = event->globalPos() - frameGeometry().topLeft();
    }
}

void LoginWindow::mouseMoveEvent(QMouseEvent *event) {
    QPoint localPos = event->pos();
    QRect rect = this->rect();

    if (rect.right() - localPos.x() <= BORDER_WIDTH &&
            rect.bottom() - localPos.y() <= BORDER_WIDTH) {
        setCursor(Qt::SizeFDiagCursor);
    } else if (rect.right() - localPos.x() <= BORDER_WIDTH) {
        setCursor(Qt::SizeHorCursor);
    } else if (rect.bottom() - localPos.y() <= BORDER_WIDTH) {
        setCursor(Qt::SizeVerCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }

    if (resizing) {
        QRect geo = geometry();
        int winW = static_cast<int>(screenW * SCALE_RATE);
        int winH = winW * 9 / 16;
        if (winH > static_cast<int>(screenH * SCALE_RATE)) {
            winH = static_cast<int>(screenH * SCALE_RATE);
            winW = winH * 16 / 9;
        }

        if (resizeRegion == Right)
            geo.setWidth(qMax(qMax(minimumWidth(), localPos.x()), winW));
        else if (resizeRegion == Bottom)
            geo.setHeight(qMax(qMax(minimumHeight(), localPos.y()), winW));
        else if (resizeRegion == BottomRight) {
            geo.setWidth(qMax(qMax(minimumWidth(), localPos.x()), winW));
            geo.setHeight(qMax(qMax(minimumHeight(), localPos.y()), winW));
        }
        setGeometry(geo);
    }
    if (dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPos() - dragPos);
    }

}


void LoginWindow::mouseReleaseEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    dragging = false;
    resizing = false;
    resizeRegion = None;
    setCursor(Qt::ArrowCursor);
}

void LoginWindow::setupImage(QLabel *label, const QString &imagePath) {
    QPixmap pixmap(imagePath);
    if (!pixmap.isNull()) {
        label->setPixmap(pixmap.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background: transparent;");
    } else {
        label->setText("Image not found");
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background: transparent; color: red;");
    }
}

void LoginWindow::drawLineWithCircles(QLabel &lineLabel, int x1, int y1, int x2, int y2, int lineWidth, int radius)
{
    int width = qAbs(x2 - x1) + 2 * radius;
    int height = qAbs(y2 - y1) + 2 * radius;
    int x = qMin(x1, x2) - radius;
    int y = qMin(y1, y2) - radius;

    int offsetX = qMin(x1, x2) - radius;
    int offsetY = qMin(y1, y2) - radius;

    lineLabel.setGeometry(x, y, width, height);

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::white);
    pen.setWidth(lineWidth);
    //    pen.setStyle(Qt::DashLine);
    pen.setStyle(Qt::DotLine);
    //        pen.setStyle(Qt::DashDotLine);


    painter.setPen(pen);
    painter.setBrush(Qt::white);

    painter.drawLine(x1 - offsetX + radius, y1 - offsetY + radius, x2 - offsetX + radius, y2 - offsetY + radius);
    painter.drawEllipse(QPoint(x1 - offsetX + radius, y1 - offsetY + radius), radius, radius);
    painter.drawEllipse(QPoint(x2 - offsetX + radius, y2 - offsetY + radius), radius, radius);
    painter.end();

    lineLabel.setPixmap(pixmap);
    lineLabel.setScaledContents(true);

}

void LoginWindow::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
    QDialog *dialog = new QDialog(parent, Qt::FramelessWindowHint);
    dialog->setFixedSize(400, 220);

    dialog->setStyleSheet(R"(
                          QDialog {
                          background-color: #4774CF;
                          border: none;
                          }
                          )");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 10);
    layout->setSpacing(0);

    QLabel *titleLabel = new QLabel(titleText, dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(R"(
                              background-color: #1A3678;
                              color: white;
                              font-family: "Times New Roman";
                              font-size: 16pt;
                              font-weight: bold;
                              padding: 10px;
                              border: none;
                              )");

    QLabel *messageLabel = new QLabel(messageText, dialog);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet(R"(
                                background-color: rgba(0, 0, 0, 0);
                                color: white;
                                font-family: "Times New Roman";
                                font-size: 14pt;
                                padding: 14px;
                                border: none;
                                )");

    QPushButton *okButton = new QPushButton("OK", dialog);
    okButton->setStyleSheet(R"(
                            QPushButton {
                            background-color: #B04248;
                            color: white;
                            font-family: "Times New Roman";
                            font-weight: bold;
                            font-size: 12pt;
                            border-radius: 8px;
                            padding: 6px 12px;
                            border: none;
                            }
                            QPushButton:hover {
                            background-color: #C85B5F;
                            }
                            )");
    connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    dialog->exec();
}


