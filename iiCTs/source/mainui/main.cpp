#include <QApplication>
#include <QMessageBox>
#include "userdb.h"
#include "loginwindow.h"
#include "mainwindow.h"

#include <QObject>
#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QStyleFactory>

void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText)
{
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
    QObject::connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    dialog->exec();
}

bool showStyledConfirm(QWidget *parent, const QString &titleText, const QString &messageText)
{
    QDialog *dialog = new QDialog(parent, Qt::FramelessWindowHint);
    dialog->setFixedSize(400, 220);
    dialog->setStyleSheet("background-color: #4774CF; border: none;");

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(0, 0, 0, 20);

    QLabel *titleLabel = new QLabel(titleText, dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: white; font-size: 16pt; font-weight: bold; font-family: Times New Roman; background-color: #1A3678; padding: 10px;");

    QLabel *messageLabel = new QLabel(messageText, dialog);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet("color: white; font-size: 14pt; padding: 14px; font-family: Times New Roman; background-color: transparent;");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *yesBtn = new QPushButton("Yes", dialog);
    QPushButton *noBtn = new QPushButton("No", dialog);
    yesBtn->setStyleSheet(R"(
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
    noBtn->setStyleSheet(R"(
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
    yesBtn->setFixedSize(100, 40);
    noBtn->setFixedSize(100, 40);

    QObject::connect(yesBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    QObject::connect(noBtn, &QPushButton::clicked, dialog, &QDialog::reject);
    btnLayout->addWidget(yesBtn);
    btnLayout->addWidget(noBtn);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addLayout(btnLayout);

    return dialog->exec() == QDialog::Accepted;
}

int main(int argc, char *argv[])
{
    qputenv("QT_USE_NATIVE_DIALOGS", "0");
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QGuiApplication::setAttribute(Qt::AA_DontUseNativeDialogs);

    qDebug() << ">>> Welcome to the Integrated Intelligent CT Simulation System. The application has successfully started.";
    QApplication app(argc, argv);

    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette pal;
    pal.setColor(QPalette::Window, Qt::white);
    pal.setColor(QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::AlternateBase, Qt::white);
    pal.setColor(QPalette::ToolTipBase, Qt::white);
    pal.setColor(QPalette::ToolTipText, Qt::black);
    pal.setColor(QPalette::Text, Qt::black);
    pal.setColor(QPalette::Button, Qt::white);
    pal.setColor(QPalette::ButtonText, Qt::black);
    pal.setColor(QPalette::Highlight, QColor(76, 163, 224));
    pal.setColor(QPalette::HighlightedText, Qt::white);
    pal.setColor(QPalette::Link, QColor(0, 122, 204));
    pal.setColor(QPalette::LinkVisited, QColor(85, 85, 255));
    pal.setColor(QPalette::Light, QColor(240, 240, 240));
    pal.setColor(QPalette::Midlight, QColor(210, 210, 210));
    pal.setColor(QPalette::Mid, QColor(160, 160, 160));
    pal.setColor(QPalette::Dark, QColor(120, 120, 120));
    pal.setColor(QPalette::Shadow, QColor(60, 60, 60));
    app.setPalette(pal);

    app.setStyleSheet(R"(
                QFileDialog, QFileDialog QWidget, QFileDialog QTreeView, QFileDialog QListView {
                    background: white;
                    color: black;
                }
                QFileDialog QPushButton {
                    background: #f0f0f0;
                    color: black;
                }
                QFileDialog QLineEdit {
                    background: white;
                    color: black;
                }
            )");

    if (!UserDB::init())
    {
        qFatal("Failed to initialize database.");
        showStyledWarning(nullptr, "Initialization Failed",
                          "Unable to create user table.\nCheck file system access rights.");
        return -1;
    }

    qDebug() << ">>> Initialization completed successfully";
    LoginWindow *login = new LoginWindow();
    MainWindow *mainWin = nullptr;

    QObject::connect(login, &LoginWindow::loginSuccess, [&](const QString &username, const QRect &geometry)
                     {
        qDebug() << ">>> Redirecting to the login system.";
        qDebug() << ">>> Attempting to log in to the simulation system. Please wait...";
        mainWin = new MainWindow(username);
        mainWin->setGeometry(geometry);
        mainWin->show();
        qDebug() << ">>> Successfully logged into the simulation system.";
        login->hide();

        QObject::connect(mainWin, &MainWindow::logoutRequested, [&](const QRect &geometry) {
            if (showStyledConfirm(mainWin, "Logout", "Are you sure you want to log out?")) {
                qDebug() << ">>> Cleaning up the application and exiting the simulation system...";
                mainWin->cleanupProcess();
                mainWin->close();
                mainWin->deleteLater();
                mainWin = nullptr;
                login->useDefaultGeometry = false;
                login->applyGeometry(geometry);
                login->show();
                login->raise();
                login->activateWindow();
                qDebug() << ">>> Successfully exited the simulation system. Redirecting to the login system.";
            }
        }); });

    login->show();
    return app.exec();
}
