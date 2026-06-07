#include "registerwindow.h"
#include "userdb.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>

RegisterWindow::RegisterWindow(QWidget *parent) : QWidget(parent) {
    setMinimumSize(1100, 600);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    int x = screenGeometry.center().x() - width() / 2;
    int y = screenGeometry.center().y() - height() / 2 + height() / 6;
    move(x, y);

    sysTopLabel = new QLabel("Integrated Intelligent CT Simulation System", this);
    sysTopLabel->setStyleSheet("color: white; background: transparent;");
    QFont font("Times New Roman", 32, QFont::Bold);
    sysTopLabel->setFont(font);
    sysTopLabel->setAlignment(Qt::AlignCenter);
    sysTopLabel->setFixedSize(1100, 80);
    sysTopLabel->move((width() - sysTopLabel->width()) / 2, 0);
    sysTopLabel->raise();


    QWidget *container = new QWidget(this);
    container->setStyleSheet(R"(
                             background: qlineargradient(
                             x1: 0, y1: 0,
                             x2: 0, y2: 1,
                             stop: 0 #22489E,
                             stop: 1 #4875D0
                             );
                             border-radius: 24px;
                             )");

    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(10, 0, 10, 10);
    mainLayout->setSpacing(5);
    mainLayout->addStretch();
    mainLayout->insertWidget(0, sysTopLabel);

    QFrame *titleUnderline = new QFrame(container);
    titleUnderline->setFixedSize(800, 2);
    titleUnderline->setStyleSheet("background-color: white;");
    titleUnderline->setFrameShape(QFrame::HLine);
    titleUnderline->setFrameShadow(QFrame::Plain);
    mainLayout->insertWidget(1, titleUnderline, 0, Qt::AlignHCenter);

    titleWidget = new QWidget(container);
    titleWidget->setFixedSize(150, 40);
    titleWidget->move(this->width() - titleWidget->width() - 15, 10);
    titleWidget->setStyleSheet("background-color: transparent;");

    QHBoxLayout *titleBar = new QHBoxLayout(titleWidget);
    titleBar->setContentsMargins(0, 0, 0, 0);
    titleBar->setSpacing(2);

    QPushButton *minBtn = new QPushButton(this);
//    QPushButton *maxBtn = new QPushButton(this);
    QPushButton *closeBtn = new QPushButton(this);

    QIcon minIcon(":/images/background/icon_min.png");
    QIcon maxIcon(":/images/background/icon_max.png");
    QIcon closeIcon(":/images/background/icon_close.png");
    minBtn->setIcon(minIcon);
//    maxBtn->setIcon(maxIcon);
    closeBtn->setIcon(closeIcon);

    minBtn->setFixedSize(40, 40);
//    maxBtn->setFixedSize(40, 40);
    closeBtn->setFixedSize(40, 40);
    minBtn->setContentsMargins(0, 0, 0, 0);
//    maxBtn->setContentsMargins(0, 0, 0, 0);
    closeBtn->setContentsMargins(0, 0, 0, 0);

    QString hoverStyle =
            "QPushButton { background: transparent; border: none; }"
            "QPushButton:hover { background-color: #F3D8F1; border-radius: 10px;}";
    minBtn->setStyleSheet(hoverStyle);
//    maxBtn->setStyleSheet(hoverStyle);
    closeBtn->setStyleSheet(hoverStyle);

    titleBar->addStretch();
    titleBar->addWidget(minBtn);
//    titleBar->addWidget(maxBtn);
    titleBar->addWidget(closeBtn);

    connect(minBtn, &QPushButton::clicked, this, &RegisterWindow::showMinimized);
//    connect(maxBtn, &QPushButton::clicked, this, [this]() {
//        isMaximized() ? showNormal() : showMaximized();
//    });
    connect(closeBtn, &QPushButton::clicked, this, &RegisterWindow::close);


    QString labelStyle = "background: transparent; min-height: 20px; color: rgb(255, 255, 255); font-size: 28px; font-weight: bold; font-family: 'Times New Roman'";
    QString editStyle = "min-height: 20px; color: rgb(255, 255, 255); font-size: 28px; font-family: 'Times New Roman'";
    QString editBoxStyle = "QLineEdit {"
                           "min-height: 20px;"
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

    usernameEdit->setText("");
    passwordEdit->setText("");
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
                       "min-height: 30px;"
                       "  border: 3px solid #335DB5;"
                       "  border-radius: 12px;"
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
                       "}"
                       "QPushButton:pressed {"
                       "  background-color: #5588CC;"
                       "}";
    QPushButton *registerBtn = new QPushButton("Register");
    registerBtn->setStyleSheet(btnStyle);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(registerBtn);
    buttonLayout->addStretch();

    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(container);

    connect(registerBtn, &QPushButton::clicked, this, [=]() {
        if (UserDB::registerUser(usernameEdit->text(), passwordEdit->text())) {
            showStyledWarning(this, "Success", "Registration completed.");
            this->close();
        } else {
            showStyledWarning(this, "Error", "Username already exists. Or username is not in the allowed registration list!");
        }
    });

    bottomInfoBox = new QWidget(this);
    bottomInfoBox->setAttribute(Qt::WA_TranslucentBackground);
    bottomInfoBox->setFixedSize(600, 320);
    bottomInfoBox->setStyleSheet("background: transparent;");

    QLabel *imageLeft = new QLabel(bottomInfoBox);
    QPixmap smuPixmap(":/images/background/smu_logo.png");
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setFixedSize(smuPixmap.size());
    imageLeft->setFixedSize(80, 80);
    imageLeft->setScaledContents(true);
    imageLeft->setPixmap(smuPixmap);
    imageLeft->setStyleSheet("background: transparent;");
    QLabel *imageRight = new QLabel(bottomInfoBox);
    imageRight->setPixmap(QPixmap(":/images/background/xjtu_logo.png").scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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

    QVBoxLayout *mainInfoLayout = new QVBoxLayout(bottomInfoBox);
    mainInfoLayout->setSpacing(0);
    mainInfoLayout->addStretch();
    mainInfoLayout->addLayout(imageLayout);
    mainInfoLayout->addWidget(topText);
    mainInfoLayout->addStretch();

    bottomInfoBox->setLayout(mainInfoLayout);
    bottomInfoBox->raise();
    bottomInfoBox->show();

    QLabel *machineImage = new QLabel(this);
    QPixmap CTSystemContrary(":/images/CT/CT.PNG");
    machineImage->setPixmap(CTSystemContrary.scaled(320, 320, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    machineImage->setFixedSize(320, 320);
    machineImage->move(200, 100);
    machineImage->setStyleSheet("background: transparent;");
    machineImage->raise();

    QLabel *cornerImage = new QLabel(this);
    QPixmap CTISS_logo(":/images/background/CTISS_logo.png");
    cornerImage->setPixmap(CTISS_logo.scaled(300, 240, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    cornerImage->setFixedSize(300, 80);
    cornerImage->move(600, 100);
    cornerImage->setStyleSheet("background: transparent;");
    cornerImage->raise();

    QLabel *contactTex = new QLabel(this);
    contactTex->setText("If you encounter any problems, please contact us.\nEmail:**@gmial.com");
    contactTex->setStyleSheet(
                "background: transparent;"
                "border: 1px dashed white;"
                "color: white;"
                "font-size: 18px;"
                //        "font-weight: bold;"
                "font-style: italic;"
                "font-family: 'Times New Roman';"
                "border-radius: 10px;"
                );
    contactTex->setWordWrap(true);
    contactTex->setAlignment(Qt::AlignCenter);
    contactTex->setFixedSize(400, 60);
    contactTex->move(550, 350);
    contactTex->raise();

}

void RegisterWindow::resizeEvent(QResizeEvent *event) {
    if (titleWidget)
        titleWidget->move(width() - titleWidget->width() - 15, 10);

    if (bottomInfoBox){
        bottomInfoBox->move(450, 100);
    }


    QWidget::resizeEvent(event);
}


void RegisterWindow::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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
