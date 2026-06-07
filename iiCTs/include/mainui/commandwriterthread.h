#ifndef COMMANDWRITERTHREAD_H
#define COMMANDWRITERTHREAD_H

#include <QThread>
#include <QString>

class CommandWriterThread : public QThread {
    Q_OBJECT
public:
    CommandWriterThread(const QString &cmd, QObject *parent = nullptr);
    ~CommandWriterThread();

signals:
    void requestWrite(const QString& commandText);

protected:
    void run() override;

private:
    QString command;
};

#endif // COMMANDWRITERTHREAD_H
