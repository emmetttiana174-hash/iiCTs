#include "chatagent.h"

ChatAgent::ChatAgent(QObject* parent)
    : QObject(parent)
{
}

ChatAgent::~ChatAgent() = default;

QString ChatAgent::ask(const QString& question)
{
    QString answer = "The AI model has not been connected yet, "
                     "and related functions are under development, so stay tuned.";

    m_history.append({ question, answer });
    return answer;
}

void ChatAgent::clearHistory()
{
    m_history.clear();
}

const QVector<QPair<QString, QString>>& ChatAgent::history() const
{
    return m_history;
}
