#ifndef CHATPANEL_H
#define CHATPANEL_H

#include <QWidget>
#include <QVector>
#include <QPair>

#include "chatagent.h"

class QTextEdit;
class QLineEdit;
class QPushButton;
class QTextBrowser;

class ChatAgent;

class ChatPanel : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPanel(ChatAgent* agent, QWidget* parent = nullptr);

private slots:
    void sendMessage();
    void clearConversation();

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    ChatAgent*             agent;
    QTextBrowser*             chatView;
    QTextEdit*             inputEdit;
    QPushButton*           sendButton;
    QPushButton*           clearButton;
};

#endif // CHATPANEL_H
