#ifndef TERMINALWIDGET_H
#define TERMINALWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QProcess>
#include <QThread>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAtomicInt>
#include <QQueue>

#include <atomic>

#include "commandwriterthread.h"

class TerminalWidget : public QWidget {
    Q_OBJECT

public:
    explicit TerminalWidget(QWidget* parent = nullptr);
    void setProcess(QProcess* p);
    void appendLine(const QString& text);
    void runCommandExternally(const QString& cmd);


    QPlainTextEdit* terminalOutput;

private:
    QLineEdit* inputLine;
    QPushButton* killButton;
    QProcess* process = nullptr;

    QAtomicInt commandRunning = false;
    bool isCommandThreadActive = false;
    QThread* commandThread = nullptr;
    QQueue<QString> cmdQueue;

    void resetTerminal();
    void connectProcessIO();
    void showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText);

signals:
    void processRecreated(QProcess* newProcess);
    void progressEnd();

public slots:
    void stopAndResetTerminal();

};

#endif // TERMINALWIDGET_H
