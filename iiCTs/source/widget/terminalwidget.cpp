#include "terminalwidget.h"
#include "commandwriterthread.h"


#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QTimer>
#include <QThread>
#include <QProcess>
#include <QMessageBox>
#include <QRegularExpression>

TerminalWidget::TerminalWidget(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    terminalOutput = new QPlainTextEdit;
    terminalOutput->setReadOnly(true);
    terminalOutput->setStyleSheet(R"(
                                  QPlainTextEdit {
                                  color: yellow;
                                  background-color: #1A3678;
                                  border: 5px solid #21438F;
                                  font-size: 18px;
                                  font-family: "Times New Roman";
                                  border-top-left-radius: 10px;
                                  border-bottom-left-radius: 0px;
                                  border-top-right-radius: 10px;
                                  border-bottom-right-radius: 0px;
                                  border-bottom: 1px solid #21438F;
                                  padding-left: 8px;
                                  }

                                  QScrollBar:vertical {
                                  background: #1A3678;
                                  width: 20px;
                                  margin: 3px;
                                  border: 2px solid #1A3678;
                                  border-radius: 5px;
                                  }

                                  QScrollBar::handle:vertical {
                                  background: #3961B4;
                                  border-radius: 5px;
                                  min-height: 20px;
                                  }

                                  QScrollBar::add-line:vertical,
                                  QScrollBar::sub-line:vertical {
                                  background: none;
                                  border: none;
                                  }

                                  QScrollBar::add-page:vertical,
                                  QScrollBar::sub-page:vertical {
                                  background: none;
                                  }
                                  )");

    mainLayout->addWidget(terminalOutput);

    inputLine = new QLineEdit;
    inputLine->setPlaceholderText("Type the command and press Enter...");
    inputLine->setStyleSheet(R"(
                             QLineEdit {
                             color: white;
                             background-color: #1E3F8A;
                             border: 5px solid #21438F;
                             font-size: 20px;
                             font-family: "Times New Roman";
                             border-top-left-radius: 0px;
                             border-bottom-left-radius: 10px;
                             border-top-right-radius: 0px;
                             border-bottom-right-radius: 10px;
                             border-top: 1px solid #21438F;
                             height: 30px;
                             padding-left: 8px;
                             }
                             )");
    mainLayout->addWidget(inputLine);

    QLabel* warningLabel = new QLabel("The operation will modify local files and is irreversible, so please be careful!");
    warningLabel->setFont(QFont("Times New Roman", 14));
    warningLabel->setStyleSheet(R"(
                                QLabel {
                                background-color: rgba(0, 0, 0, 0);
                                color: #AA0000;
                                }
                                )");
    warningLabel->setAlignment(Qt::AlignCenter);

    killButton = new QPushButton("Stop");
    killButton->setFont(QFont("Times New Roman", 14, QFont::Bold));
    killButton->setFixedSize(60, 40);
    killButton->setStyleSheet(R"(
                              QPushButton {
                              background-color: #6B1F23;
                              color: white;
                              border-radius: 10px;
                              border: 2px solid #441416;
                              }
                              QPushButton:hover {
                              background-color: #AF3339;
                              border: 2px solid #6B1F23;
                              }
                              )");

    QHBoxLayout* killLayout = new QHBoxLayout;
    killLayout->addWidget(warningLabel);
    killLayout->addStretch();
    killLayout->addWidget(killButton);
    killLayout->setSpacing(10);
    killLayout->setContentsMargins(20, 0, 20, 0);

    mainLayout->addSpacing(10);
    mainLayout->addLayout(killLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    terminalOutput->appendPlainText("Welcome to Integrated Intelligent CT Simulation System Terminal!");
    terminalOutput->appendPlainText("Type your command below or use preset actions.");
    terminalOutput->appendPlainText("Waiting for process to start...\n");

}

void TerminalWidget::setProcess(QProcess* p)
{
    this->process = p;

    connect(inputLine, &QLineEdit::returnPressed, this, [=]() {
        if (!process || isCommandThreadActive) {
            terminalOutput->appendPlainText("[Warning] A command is already running.");
            return;
        }

        QString command = inputLine->text();
        if (!command.isEmpty()) {
            terminalOutput->appendPlainText("[Command] $ " + command);
            process->write(command.toUtf8() + "\n");

            QString text = command.trimmed();
            static const QRegularExpression re(R"(\[\s*Name:\s*([^\]\s]+)\s*\]\s*\[\s*MAX:\s*(\d+)\s*\])");
            QRegularExpressionMatch match = re.match(text);
            if (match.hasMatch()) {
                process->write("echo Defines jobs that can be monitored by a progress bar.\n");
            }
            else{
                process->write("echo __yidoneguo__\n");
                commandRunning.storeRelaxed(true);
                isCommandThreadActive = true;
            }
            inputLine->clear();
        }
    });

    connect(killButton, &QPushButton::clicked, this, [=]() {
        emit stopRequested();
        terminalOutput->appendPlainText("[Stopping terminal and related tasks...]");

        if (commandThread && commandThread->isRunning()) {
            commandThread->requestInterruption();
            commandThread->quit();
            commandThread->wait();
            commandThread->deleteLater();
            commandThread = nullptr;
            terminalOutput->appendPlainText("[Subthread terminated.]");
        }

        if (process->state() != QProcess::NotRunning) {
            process->kill();
            process->waitForFinished(3000);
            terminalOutput->appendPlainText("[Process killed.]");
        }

        disconnect(process, nullptr, this, nullptr);
        process->deleteLater();
        process = new QProcess(this);
        process->start("bash");
        emit processRecreated(process);

        commandRunning.storeRelaxed(false);
        isCommandThreadActive = false;
        terminalOutput->appendPlainText("[Terminal has been fully reset.]");

        connectProcessIO();
    });

    connectProcessIO();
}

void TerminalWidget::connectProcessIO()
{
    connect(process, &QProcess::readyReadStandardOutput, this, [=]() {
        QString output = process->readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString& line : lines) {
            QString trimmed = line.trimmed();
            if (trimmed == "__yidoneguo__") {
                terminalOutput->appendPlainText("[Command execution completed.]");
                emit progressEnd();
                commandRunning.storeRelaxed(false);
                isCommandThreadActive = false;

            } else if (!trimmed.isEmpty()) {
                terminalOutput->appendPlainText(">>> " + trimmed);
            }
        }
    });

    connect(process, &QProcess::readyReadStandardError, this, [=]() {
        QString error = process->readAllStandardError().trimmed();
        if (!error.isEmpty()) {
            terminalOutput->appendPlainText("[Error] " + error);
        }
    });
}

void TerminalWidget::appendLine(const QString& text)
{
    if (terminalOutput) {
        terminalOutput->appendPlainText(text);
    }
}


void TerminalWidget::runCommandExternally(const QString& cmd)
{
    if (commandRunning.loadRelaxed()) {
        showStyledWarning(this, "Command in Progress",
                             "A command is currently running.\n"
                             "Please wait or stop the running process.\n"
                             "The current instruction was not executed.");
        return;
    }

    commandThread = new CommandWriterThread(cmd, this);


    connect(static_cast<CommandWriterThread*>(commandThread), &CommandWriterThread::requestWrite, this,
            [this](const QString& commandText) {
        if (process && process->state() == QProcess::Running) {
            commandRunning.storeRelaxed(true);
            isCommandThreadActive = true;

            process->write(commandText.toUtf8() + "\n");
            process->write("echo __yidoneguo__\n");

            terminalOutput->appendPlainText("[External Command] $ " + commandText);
        } else {
            terminalOutput->appendPlainText("[Process not running]");
        }
    });

    connect(commandThread, &QThread::finished, this, [this]() {
        commandThread->deleteLater();
        commandThread = nullptr;
    });


    connect(process, &QProcess::readyReadStandardOutput, this, [this]() {
        QStringList lines = QString(process->readAllStandardOutput()).split('\n');
        for (const QString& line : lines) {
            const QString t = line.trimmed();
            if (t == "__yidoneguo__") {
                terminalOutput->appendPlainText("[Command execution completed.]");
                commandRunning.storeRelaxed(false);
                isCommandThreadActive = false;
            }
            else if (!t.isEmpty()) {
                terminalOutput->appendPlainText(">>> " + t);
            }
        }
    });

    QTimer::singleShot(50, [=]() { commandThread->start(); });
}


void TerminalWidget::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText)
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
                              border-radius:none;
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


void TerminalWidget::stopAndResetTerminal()
{
    terminalOutput->appendPlainText("[Stopping terminal and related tasks...]");
    if (commandThread && commandThread->isRunning()) {
        commandThread->requestInterruption();
        commandThread->quit();
        commandThread->wait();
        commandThread->deleteLater();
        commandThread = nullptr;
        terminalOutput->appendPlainText("[Subthread terminated.]");
    }

    if (process->state() != QProcess::NotRunning) {
        process->kill();
        process->waitForFinished(3000);
        terminalOutput->appendPlainText("[Process killed.]");
    }

    disconnect(process, nullptr, this, nullptr);
    process->deleteLater();
    process = new QProcess(this);
    process->start("bash");
    emit processRecreated(process);

    commandRunning.storeRelaxed(false);
    isCommandThreadActive = false;

    terminalOutput->appendPlainText("[Terminal has been fully reset.]");
    connectProcessIO();
}
