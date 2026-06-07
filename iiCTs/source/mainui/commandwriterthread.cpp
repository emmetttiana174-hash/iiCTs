#include "commandwriterthread.h"
#include <QDebug>

CommandWriterThread::CommandWriterThread(const QString &cmd, QObject *parent)
    : QThread(parent), command(cmd) {}

CommandWriterThread::~CommandWriterThread() {
//    qDebug() << "[CommandWriterThread destroyed]";
}

void CommandWriterThread::run() {
    QThread::msleep(300);
    emit requestWrite(command);
}
