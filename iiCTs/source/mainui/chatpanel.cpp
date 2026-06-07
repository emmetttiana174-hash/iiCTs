#include "chatpanel.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QScrollBar>
#include <QTextEdit>
#include <QPushButton>
#include <QTextBrowser>

ChatPanel::ChatPanel(ChatAgent* agent, QWidget* parent)
    : QWidget(parent)
    , agent(agent)
{

    this->setStyleSheet(R"(
                        background-color: #1A3678;
                        border: 5px solid #21438F;
                        font-size: 18px;
                        font-family: "Times New Roman";
                        border-top-left-radius: 10px;
                        border-bottom-left-radius: 0px;
                        border-top-right-radius: 10px;
                        border-bottom-right-radius: 0px;
                        border-bottom: 1px solid #21438F;
                        )");
    chatView = new QTextBrowser(this);
    chatView->setReadOnly(true);
    chatView->setFrameShape(QFrame::NoFrame);
    chatView->setFont(QFont("Times New Roman", 12));
    chatView->setContentsMargins(0, 0, 0, 0);
    chatView->document()->setDocumentMargin(0);
    chatView->setStyleSheet(R"(
                            QTextBrowser {
                            color: yellow;
                            background-color: #1A3678;
                            border: 5px solid #21438F;
                            font-size: 18px;
                            font-family: "Times New Roman";
                            border-top-left-radius: 10px;
                            border-bottom-left-radius: 0px;
                            border-top-right-radius: 10px;
                            border-bottom-right-radius: 0px;
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

    inputEdit = new QTextEdit(this);
    inputEdit->setFixedHeight(60);
    inputEdit->setFont(QFont("Times New Roman", 12));
    inputEdit->setPlaceholderText("Type your message (Shift+Enter for newline)...");
    inputEdit->setAcceptRichText(false);
    inputEdit->installEventFilter(this);
    inputEdit->setStyleSheet(R"(
                             QTextEdit {
                             color: white;
                             background-color: #1E3F8A;
                             border: 5px solid #21438F;
                             font-size: 20px;
                             font-family: "Times New Roman";
                             border-top-left-radius: 0px;
                             border-bottom-left-radius: 10px;
                             border-top-right-radius: 0px;
                             border-bottom-right-radius: 0px;
                             border-top: 1px solid #21438F;
                             padding-left: 8px;
                             }
                             )");

    sendButton  = new QPushButton("Send", this);
    clearButton = new QPushButton("Clear", this);
    QFont btnFont("Times New Roman", 12, QFont::Bold);
    sendButton->setFont(btnFont);
    clearButton->setFont(btnFont);
    sendButton->setFixedHeight(60);
    clearButton->setFixedHeight(60);
    QString clearButtonStyle = R"(
                               QPushButton {
                               color: white;
                               background-color: #22489E;
                               border: 5px solid #21438F;
                               font-size: 12px;
                               border-top-left-radius:    0px;
                               border-top-right-radius:   0px;
                               border-bottom-right-radius:10px;
                               border-bottom-left-radius: 0px;
                               }
                               QPushButton:hover {
                               background-color: #61A7F5;
                               }
                               )";
    QString sendButtonStyle = R"(
                              QPushButton {
                              color: white;
                              background-color: #22489E;
                              border: none;
                              border-bottom: 5px solid #21438F;
                              border-top: 5px solid #21438F;
                              font-size: 12px;
                              border-top-left-radius:    0px;
                              border-top-right-radius:   0px;
                              border-bottom-right-radius:0px;
                              border-bottom-left-radius: 0px;
                              }
                              QPushButton:hover {
                              background-color: #61A7F5;
                              }
                              )";

    clearButton->setStyleSheet(clearButtonStyle);
    sendButton->setStyleSheet(sendButtonStyle);

    auto bottomLayout = new QHBoxLayout;
    bottomLayout->addWidget(inputEdit, /*stretch=*/1);
    bottomLayout->addWidget(sendButton);
    bottomLayout->addWidget(clearButton);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(chatView, /*stretch=*/1);

    mainLayout->addLayout(bottomLayout);

    connect(sendButton,  &QPushButton::clicked,   this, &ChatPanel::sendMessage);
    connect(clearButton, &QPushButton::clicked,   this, &ChatPanel::clearConversation);
}

bool ChatPanel::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == inputEdit && ev->type() == QEvent::KeyPress) {
        QKeyEvent* ke = static_cast<QKeyEvent*>(ev);
        if (ke->key() == Qt::Key_Return && !(ke->modifiers() & Qt::ShiftModifier)) {
            sendMessage();
        }
    }
    return QWidget::eventFilter(obj, ev);
}

void ChatPanel::sendMessage()
{
    QString question = inputEdit->toPlainText().trimmed();
    if (question.isEmpty()) return;
    QString answer = agent->ask(question);

    QString userHtml = QString(R"(
                               <div style="margin:0px 0; background-color:transparent; color:white; font:bold 18pt 'Times New Roman';">
                               <img src=":/images/background/icon_yiguo.png"
                               width="24" height="24"
                               style="vertical-align:middle; background-color:transparent;"/>
                               <span style="vertical-align:middle; margin-left:6px;">User</span>
                               </div>
                               <div style="margin:0 0px 0px 0px; background-color:transparent; padding:8px; border-radius:6px; color:#CBB9F6; font:16pt 'Times New Roman';">
                               %1
                               </div>
                               )").arg(question.toHtmlEscaped());

    QString aiHtml = QString(R"(
                             <div style="margin:0px 0; background-color:transparent; color:white; font:bold 18pt 'Times New Roman';">
                             <img src=":/images/background/icon_ai.png"
                             width="24" height="24"
                             style="vertical-align:middle; background-color:transparent;"/>
                             <span style="vertical-align:middle; margin-left:6px;">Agent</span>
                             </div>
                             <div style="margin:0 0px 0px 0px; background-color:transparent; padding:8px; border-radius:6px; color:yellow; font:16pt 'Times New Roman';">
                             %1
                             </div>
                             )").arg(answer.toHtmlEscaped());

    chatView->append(userHtml);
    chatView->append(aiHtml);
    chatView->append("");
    chatView->append("");

    chatView->verticalScrollBar()->setValue(chatView->verticalScrollBar()->maximum());

    inputEdit->clear();
}

void ChatPanel::clearConversation()
{
    agent->clearHistory();
    chatView->clear();
}
