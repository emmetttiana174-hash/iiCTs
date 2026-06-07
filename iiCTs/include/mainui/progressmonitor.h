#ifndef PROGRESSMONITOR_H
#define PROGRESSMONITOR_H

#include <QObject>

class QPlainTextEdit;

class ProgressMonitor : public QObject {
    Q_OBJECT

public:
    explicit ProgressMonitor(QPlainTextEdit* editor, QObject* parent = nullptr);
    ~ProgressMonitor() override;

signals:
    void progressUpdated(int value);
    void progressTaskStarted(const QString& name, int maxValue);
    void progressEnd();

private slots:
    void onContentsChanged();

private:
    QPlainTextEdit* m_editor;
    int             m_lastPos;
};

#endif // PROGRESSMONITOR_H
