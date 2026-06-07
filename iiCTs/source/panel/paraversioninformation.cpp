#include "paraversioninformation.h"


#include "mainwindow.h"
#include "projectinfo.h"
#include "chatpanel.h"
#include "paramfilemanager.h"
#include "filedialog.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QResizeEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QButtonGroup>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QTextStream>
#include <QDirIterator>
#include <QProxyStyle>
#include <QStandardItemModel>
#include <QTextDocument>
#include <QTextEdit> // 确保包含了 QTextEdit 的头文件


ParaVersionInformation::ParaVersionInformation(QWidget* parent)
    : QWidget(parent)
{
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground);
    setStyleSheet("background-color: transparent;");

    QString fileTitleName = "Integrated Intelligent CT Simulation System";


    QVBoxLayout* m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 10, 15, 20);

    QHBoxLayout* topButtonsLayout = new QHBoxLayout;
    QString DeBtnStyle = R"(
                                QPushButton {
                                color: white;
                                background-color: #3366CC;
                                font-weight: bold;
                                border-top-left-radius: 15px;
                                border-bottom-left-radius: 15px;
                                border-top-right-radius: 0px;
                                border-bottom-right-radius: 0px;
                                border: 1px solid #2F528F;

                                }
                                QPushButton:hover {
                                background-color: #3366CC;
                                border: 1px solid #41A1F5;
                                }
                                QPushButton:checked {
                                background-color: #61A7F5;
                                border: 1px solid #41A1F5;
                                }
                                )";
    QString AIBtnStyle = R"(
                                QPushButton {
                                color: white;
                                background-color: #3366CC;
                                font-weight: bold;
                                border-top-left-radius: 0px;
                                border-bottom-left-radius: 0px;
                                border-top-right-radius: 15px;
                                border-bottom-right-radius: 15px;
                                border: 1px solid #2F528F;
                                }
                                QPushButton:hover {
                                background-color: #3366CC;
                                border: 1px solid #41A1F5;
                                }
                                QPushButton:checked {
                                background-color: #61A7F5;
                                border: 1px solid #41A1F5;
                                }
                                )";
    QButtonGroup* toggleGroup = new QButtonGroup(this);
    toggleGroup->setExclusive(true);
    QPushButton* detailedBtn = new QPushButton("Det");
    QPushButton* AIBtn = new QPushButton("AI");
    QFont toggleFont("Times New Roman", 12);
    QList<QPushButton*> toggleButtons = {detailedBtn, AIBtn };
    for (auto btn : toggleButtons) {
        btn->setCheckable(true);
        btn->setFixedSize(40, 30);
        toggleGroup->addButton(btn);
        btn->setFont(toggleFont);
        topButtonsLayout->addWidget(btn);
    }
    detailedBtn->setStyleSheet(DeBtnStyle);
    AIBtn->setStyleSheet(AIBtnStyle);
    topButtonsLayout->setSpacing(0);
    topButtonsLayout->setContentsMargins(0, 0, 20, 0);
    detailedBtn->setChecked(true);
    topButtonsLayout->setAlignment(Qt::AlignRight);
    m_mainLayout->insertLayout(0, topButtonsLayout);

    QStackedWidget* centralView = new QStackedWidget;
    centralView->setStyleSheet("background-color: transparent; border-radius: 12px;");


    QWidget* deEditPanel = new QWidget;
    centralView->addWidget(deEditPanel);
    m_mainLayout->insertWidget(1, centralView);

    const QString kSpectUILabelStyle = R"(
                                            QLabel {
                                            color: white;
                                            background-color: transparent;
                                            font-family: 'Times New Roman';
                                            font-weight: bold;
                                            border: none;
                                            font-size: 24pt;
                                            }
                                            )";




    QTextEdit* editor       = new QTextEdit(deEditPanel);
    QLabel* spectLabel = new QLabel(fileTitleName, deEditPanel);
    spectLabel->setAlignment(Qt::AlignCenter);
    spectLabel->setStyleSheet(kSpectUILabelStyle);
    spectLabel->setFixedHeight(40);

    deEditPanel->setAutoFillBackground(false);
    deEditPanel->setAttribute(Qt::WA_StyledBackground);
    deEditPanel->setStyleSheet(R"(
                                        background-color: transparent;
                                        border: 2px dashed #7697D8;
                                        font-family: "Times New Roman";
                                        font-weight: bold;
                                        border-radius: 10px;
                                        )");

    editor->setReadOnly(true);
    //    editor->setTextInteractionFlags(Qt::NoTextInteraction);
    editor->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    editor->setStyleSheet(R"(
                                 QTextEdit {
                                 color: white;
                                 background-color: transparent;
                                 border: none;
                                 font-size: 18px;
                                 font-family: "Times New Roman";
                                 border-radius: 10px;
                                 padding: 8px;
                                 }

                                 QScrollBar:vertical {
                                 background: #22489E;
                                 width: 20px;
                                 margin: 0px;
                                 border: 2px solid #3961B4;
                                 border-radius: 5px;
                                 }

                                 QScrollBar::handle:vertical {
                                 margin: 3px;
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


    QFile mdFile(":/version/version.md");
    if (mdFile.open(QIODevice::ReadOnly | QIODevice::Text)) {

        QByteArray data = mdFile.readAll();
        mdFile.close();

        // 关键修改：将 setMarkdown 替换为 setHtml
        editor->setMarkdown(QString::fromUtf8(data));

    }

    auto* deEditTitleLayout = new QHBoxLayout;
    deEditTitleLayout->setSpacing(0);
    deEditTitleLayout->setContentsMargins(10,10,10,10);
    deEditTitleLayout->addWidget(spectLabel);

    auto* deEditLayout = new QVBoxLayout(deEditPanel);
    deEditLayout->setContentsMargins(10,10,10,10);
    deEditLayout->setSpacing(10);
    deEditLayout->addLayout(deEditTitleLayout);
    deEditLayout->addWidget(editor);
    deEditPanel->setLayout(deEditLayout);


    connect(detailedBtn, &QPushButton::clicked, [=]() { centralView->setCurrentIndex(0); });
    connect(AIBtn, &QPushButton::clicked, this, [=]() { ChatPanel* chat = mainWindow->getChatPanel();

        if (centralView->indexOf(chat) < 0) {
            centralView->addWidget(chat);
        }
        centralView->setCurrentWidget(chat); });
}


void ParaVersionInformation::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

}

void ParaVersionInformation::setMainWindow(MainWindow* main) {
    mainWindow = main;
}

void ParaVersionInformation::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
    gradient.setColorAt(0, QColor("#22489E"));
    gradient.setColorAt(1, QColor("#4875D0"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(gradient);
    painter.drawRoundedRect(rect(), 15, 15);
}

void ParaVersionInformation::showStyledWarning(QWidget *parent, const QString &titleText, const QString &messageText) {
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
    connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);

    layout->addWidget(titleLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(okButton, 0, Qt::AlignCenter);

    dialog->exec();
}

void ParaVersionInformation::onProjectInfoChanged(const ProjectInfo &info)
{
    allInfo = info;
}
