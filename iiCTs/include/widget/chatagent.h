#ifndef CHATAGENT_H
#define CHATAGENT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QPair>

class ChatAgent : public QObject
{
    Q_OBJECT

public:
    explicit ChatAgent(QObject* parent = nullptr);
    ~ChatAgent();

    QString ask(const QString& question);
    void clearHistory();

    const QVector<QPair<QString, QString>>& history() const;

private:
    QVector<QPair<QString, QString>> m_history;
};

#endif // CHATAGENT_H
