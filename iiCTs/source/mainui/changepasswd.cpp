#include "changepasswd.h"
#include "userdb.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QDebug>


ChangePasswd::ChangePasswd(const QString &username, QWidget *parent): QWidget(parent) {
    QString loginUser = username;
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


    QWidget *container = new QWidget();
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

    connect(minBtn, &QPushButton::clicked, this, &ChangePasswd::showMinimized);
    //    connect(maxBtn, &QPushButton::clicked, this, [this]() {
    //        isMaximized() ? showNormal() : showMaximized();
    //    });
    connect(closeBtn, &QPushButton::clicked, this, &ChangePasswd::close);


    QString labelStyle = "background: transparent; min-height: 15px; color: rgb(255, 255, 255); font-size: 20px; font-weight: bold; font-family: 'Times New Roman'";
    QString editStyle = "min-height: 15px; color: rgb(255, 255, 255); font-size: 28px; font-family: 'Times New Roman'";
    QString editBoxStyle = "QLineEdit {"
                           "min-height: 15px;"
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
    QLabel *oldPwdLabel = new QLabel("Old Password:");
    QLineEdit *oldPwdEdit = new QLineEdit;
    QLabel *newPwdLabel = new QLabel("New Password:");
    QLineEdit *newPwdEdit = new QLineEdit;


    usernameEdit->setText(loginUser);
    oldPwdEdit->setText("");
    newPwdEdit->setText("");
    usernameLabel->setFixedWidth(150);
    oldPwdLabel->setFixedWidth(150);
    newPwdLabel->setFixedWidth(150);
    usernameEdit->setFixedWidth(300);
    oldPwdEdit->setFixedWidth(300);
    newPwdEdit->setFixedWidth(300);

    usernameLabel->setStyleSheet(labelStyle);
    usernameEdit->setStyleSheet(editStyle);
    usernameEdit->setStyleSheet(editBoxStyle);
    oldPwdLabel->setStyleSheet(labelStyle);
    oldPwdEdit->setStyleSheet(editStyle);
    oldPwdEdit->setStyleSheet(editBoxStyle);
    newPwdLabel->setStyleSheet(labelStyle);
    newPwdEdit->setStyleSheet(editStyle);
    newPwdEdit->setStyleSheet(editBoxStyle);
    usernameLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    oldPwdLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    newPwdLabel->setAlignment(Qt::AlignVCenter | Qt::AlignRight);

    QHBoxLayout *userLayout = new QHBoxLayout;
    userLayout->addStretch();
    userLayout->addWidget(usernameLabel);
    userLayout->addWidget(usernameEdit);
    userLayout->addStretch();

    QHBoxLayout *oldLayout = new QHBoxLayout;
    oldLayout->addStretch();
    oldLayout->addWidget(oldPwdLabel);
    oldLayout->addWidget(oldPwdEdit);
    oldLayout->addStretch();

    QHBoxLayout *newLayout = new QHBoxLayout;
    newLayout->addStretch();
    newLayout->addWidget(newPwdLabel);
    newLayout->addWidget(newPwdEdit);
    newLayout->addStretch();

    mainLayout->addLayout(userLayout);
    mainLayout->addLayout(oldLayout);
    mainLayout->addLayout(newLayout);

    oldPwdEdit->setEchoMode(QLineEdit::Password);
    newPwdEdit->setEchoMode(QLineEdit::Password);

    QPushButton *confirmBtn = new QPushButton("Confirm");
    QPushButton *cancelBtn = new QPushButton("Cancel");
    cancelBtn->setStyleSheet(R"(
                             QPushButton {
                             background-color: #B04248;
                             color: white;
                             font-family: "Times New Roman";
                             font-weight: bold;
                             font-size: 14pt;
                             border-radius: 8px;
                             padding: 6px 12px;
                             border: none;
                             }
                             QPushButton:hover {
                             background-color: #C85B5F;
                             }
                             )");
    confirmBtn->setStyleSheet(R"(
                              QPushButton {
                              background-color: #2F674C;
                              color: white;
                              font-family: "Times New Roman";
                              font-weight: bold;
                              font-size: 14pt;
                              border-radius: 8px;
                              padding: 6px 12px;
                              border: none;
                              }
                              QPushButton:hover {
                              background-color: #4FAB7F;
                              }
                              )");
    confirmBtn->setFixedHeight(36);
    cancelBtn->setFixedHeight(36);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(confirmBtn);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();

    mainLayout->addSpacing(8);
    mainLayout->addLayout(buttonLayout);

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(container);

    connect(confirmBtn, &QPushButton::clicked, this, [=]() {
        QString username = usernameEdit->text();
        QString oldPwd = oldPwdEdit->text();
        QString newPwd = newPwdEdit->text();

        QString valFlag = "true";
        QStringList exemptUsers = {"yiguo", "admin", "YiGuo", "GuoYi"};
        if (exemptUsers.contains(loginUser) && !exemptUsers.contains(username)) {
            valFlag = "false";
        }

        if (UserDB::updatePassword(username, oldPwd, newPwd, valFlag)) {
            showStyledWarning(this, "Success", "Password updated successfully.");
            this->close();
        } else {
            showStyledWarning(this, "Error", "Old password verification failed.");
        }
    });

    connect(cancelBtn, &QPushButton::clicked, this, [=]() {
        this->close();
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

void ChangePasswd::resizeEvent(QResizeEvent *event) {
    if (titleWidget)
        titleWidget->move(width() - titleWidget->width() - 15, 10);

    if (bottomInfoBox){
        bottomInfoBox->move(450, 100);
    }


    QWidget::resizeEvent(event);
}


void ChangePasswd::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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

