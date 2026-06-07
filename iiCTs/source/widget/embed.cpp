#include "embed.h"
#include <QApplication>
#include <QTimer>
#include <QHBoxLayout>
#include <iostream>

EmbedWidget::EmbedWidget(QWidget *parent)
    : QWidget(parent)
    , m_geant4Process(nullptr)
    , m_embedContainer(nullptr)
    , m_foreignWindow(nullptr)
    , m_isEmbedded(false)
    , m_mainLayout(nullptr)
    , m_placeholderWidget(nullptr)
    , m_placeholderLabel(nullptr)
    , m_subtitleLabel(nullptr)
{
    setupPlaceholderUI();

    // 设置最小尺寸
    setMinimumSize(640, 480);

    // 安装事件过滤器来处理大小调整
    installEventFilter(this);
}

EmbedWidget::~EmbedWidget()
{
    // 清理Geant4进程
    stopGeant4Process();
}

void EmbedWidget::setupPlaceholderUI()
{
    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 占位符控件
    m_placeholderWidget = new QWidget(this);
    m_placeholderWidget->setStyleSheet(
        "QWidget {"
        "  background-color: #2c2c2c;"
        "  color: #a9a9a9;"
        "  border: 2px dashed #555555;"
        "  border-radius: 8px;"
        "  font-size: 18px;"
        "}"
    );

    // 占位符布局
    auto *placeholderLayout = new QVBoxLayout(m_placeholderWidget);

    // 主标签
    m_placeholderLabel = new QLabel("Geant4 可视化区域", m_placeholderWidget);
    m_placeholderLabel->setAlignment(Qt::AlignCenter);
    m_placeholderLabel->setStyleSheet("border: none; font-weight: bold;");

    // 子标题标签
    m_subtitleLabel = new QLabel("等待启动仿真模型...", m_placeholderWidget);
    m_subtitleLabel->setStyleSheet("font-size: 14px; color: #777; border: none;");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    placeholderLayout->addStretch();
    placeholderLayout->addWidget(m_placeholderLabel);
    placeholderLayout->addWidget(m_subtitleLabel);
    placeholderLayout->addStretch();

    m_mainLayout->addWidget(m_placeholderWidget);
}

bool EmbedWidget::startGeant4Visualization(const QString &executablePath, const QString &macroPath)
{
    // 如果已有进程在运行，先停止
    if (m_geant4Process && m_geant4Process->state() != QProcess::NotRunning) {
        stopGeant4Process();
    }

    // 检查可执行文件是否存在
    if (!QFileInfo::exists(executablePath)) {
        QString errorMsg = QString("Geant4可执行文件不存在: %1").arg(executablePath);
        emit errorOccurred(errorMsg);
        return false;
    }

    // 重置状态
    resetToPlaceholder();

    // 创建新的进程
    m_geant4Process = new QProcess(this);

    // 设置进程参数
    QStringList arguments;
    arguments << "--embed-mode";
    if (!macroPath.isEmpty()) {
        arguments << macroPath;
    }

    // 设置进程通道模式
    m_geant4Process->setProcessChannelMode(QProcess::SeparateChannels);

    // 连接信号
    connect(m_geant4Process, &QProcess::readyReadStandardOutput,
            this, &EmbedWidget::onGeant4StandardOutput);
    connect(m_geant4Process, &QProcess::readyReadStandardError,
            this, &EmbedWidget::onGeant4StandardError);
    connect(m_geant4Process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &EmbedWidget::onGeant4ProcessFinished);

    // 启动进程
    qDebug() << "[EmbedWidget] Starting Geant4 process:" << executablePath << arguments;
    m_geant4Process->start(executablePath, arguments);

    if (!m_geant4Process->waitForStarted(5000)) {
        QString errorMsg = QString("无法启动Geant4进程: %1").arg(m_geant4Process->errorString());
        emit errorOccurred(errorMsg);
        delete m_geant4Process;
        m_geant4Process = nullptr;
        return false;
    }

    // 更新占位符文本
    setSubtitleText("正在启动Geant4可视化...");

    emit geant4Started();
    return true;
}

void EmbedWidget::stopGeant4Process()
{
    if (m_geant4Process) {
        if (m_geant4Process->state() != QProcess::NotRunning) {
            qDebug() << "[EmbedWidget] Terminating Geant4 process...";
            m_geant4Process->kill();
            m_geant4Process->waitForFinished(3000);
        }

        m_geant4Process->deleteLater();
        m_geant4Process = nullptr;
    }

    // 重置嵌入状态
    resetToPlaceholder();
}

bool EmbedWidget::isGeant4Running() const
{
    return m_geant4Process && m_geant4Process->state() == QProcess::Running;
}

void EmbedWidget::setPlaceholderText(const QString &text)
{
    if (m_placeholderLabel) {
        m_placeholderLabel->setText(text);
    }
}

void EmbedWidget::setSubtitleText(const QString &text)
{
    if (m_subtitleLabel) {
        m_subtitleLabel->setText(text);
    }
}

bool EmbedWidget::eventFilter(QObject *obj, QEvent *event)
{
    // 如果占位符控件大小改变，且已嵌入窗口，则调整嵌入窗口大小
    if (obj == m_placeholderWidget && m_isEmbedded && event->type() == QEvent::Resize && m_embedContainer) {
        m_embedContainer->setGeometry(m_placeholderWidget->rect());
    }
    return QWidget::eventFilter(obj, event);
}

void EmbedWidget::onGeant4StandardOutput()
{
    if (!m_geant4Process) return;

    // 读取输出并添加到缓冲区
    m_outputBuffer += QString::fromLocal8Bit(m_geant4Process->readAllStandardOutput());

    // 处理完整的行
    int newlineIndex;
    while ((newlineIndex = m_outputBuffer.indexOf('\n')) >= 0) {
        QString line = m_outputBuffer.left(newlineIndex);
        m_outputBuffer.remove(0, newlineIndex + 1);

        // 查找窗口ID标记
        const QString windowIdTag = "[WINIDGYYYY]";
        if (!m_isEmbedded && line.contains(windowIdTag)) {
            QString tail = line.mid(line.indexOf(windowIdTag) + windowIdTag.length()).trimmed();
            bool ok = false;
            qulonglong windowId = tail.toULongLong(&ok);
            if (ok && windowId > 0) {
                qDebug() << "[EmbedWidget] Found window ID:" << windowId;
                tryEmbedWindow(windowId, 0);
            }
        } else {
            // 输出其他Geant4信息
            std::cout << "[Geant4] " << line.toStdString() << std::endl;
        }
    }
}

void EmbedWidget::onGeant4StandardError()
{
    if (!m_geant4Process) return;

    auto errorData = m_geant4Process->readAllStandardError();
    QString errorOutput = QString::fromLocal8Bit(errorData);
    std::cerr << "[Geant4 Error] " << errorOutput.toStdString();
}

void EmbedWidget::onGeant4ProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "[EmbedWidget] Geant4 process finished. Exit code:" << exitCode << "Status:" << exitStatus;

    // 重置状态
    resetToPlaceholder();
    setSubtitleText("Geant4进程已结束");

    emit geant4Finished(exitCode);
}

void EmbedWidget::tryEmbedWindow(qulonglong windowId, int attempt)
{
    if (attempt > MAX_EMBED_ATTEMPTS) {
        qWarning() << "[EmbedWidget] Failed to embed window after" << MAX_EMBED_ATTEMPTS << "attempts";
        emit errorOccurred("无法嵌入Geant4窗口");
        return;
    }

    // 尝试获取外部窗口
    m_foreignWindow = QWindow::fromWinId((WId)windowId);
    if (!m_foreignWindow) {
        // 如果窗口未找到，延迟重试
        QTimer::singleShot(EMBED_RETRY_DELAY, this, [=]() {
            tryEmbedWindow(windowId, attempt + 1);
        });
        return;
    }

    // 创建窗口容器
    m_embedContainer = QWidget::createWindowContainer(m_foreignWindow, m_placeholderWidget);
    m_embedContainer->setGeometry(m_placeholderWidget->rect());
    m_embedContainer->show();

    m_isEmbedded = true;
    qDebug() << "[EmbedWidget] Window embedded successfully, ID:" << windowId;

    // 更新UI状态
    setSubtitleText("Geant4可视化已就绪");

    // 发送嵌入成功信号给Geant4进程
    QTimer::singleShot(50, this, [=]() {
        if (m_geant4Process) {
            m_geant4Process->write("EMBED_OK\n");
            m_geant4Process->waitForBytesWritten(100);
        }
    });

    emit geant4Embedded();
}

void EmbedWidget::resetToPlaceholder()
{
    // 重置嵌入状态
    m_isEmbedded = false;
    m_outputBuffer.clear();

    // 清理嵌入容器
    if (m_embedContainer) {
        delete m_embedContainer;
        m_embedContainer = nullptr;
    }

    m_foreignWindow = nullptr;

    // 重置占位符文本
    setSubtitleText("等待启动仿真模型...");
}


