#ifndef EMBED_H
#define EMBED_H

#include <QWidget>
#include <QProcess>
#include <QWindow>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QDebug>
#include <QFileInfo>
#include <QMessageBox>

// 平台特定的包含文件
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <QX11Info>
#endif

/**
 * @brief Geant4嵌入控件类
 *
 * 该类负责将Geant4可视化界面嵌入到Qt应用程序中。
 * 基于Gate2测试软件的集成方法实现，支持启动外部Geant4进程并将其窗口嵌入到Qt控件中。
 */
class EmbedWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EmbedWidget(QWidget *parent = nullptr);
    ~EmbedWidget() override;

    /**
     * @brief 启动Geant4可视化进程
     * @param executablePath Geant4可执行文件路径
     * @param macroPath 可视化宏文件路径
     * @return 启动成功返回true，否则返回false
     */
    bool startGeant4Visualization(const QString &executablePath, const QString &macroPath = "");

    /**
     * @brief 停止当前的Geant4进程
     */
    void stopGeant4Process();

    /**
     * @brief 检查Geant4进程是否正在运行
     * @return 正在运行返回true，否则返回false
     */
    bool isGeant4Running() const;

    /**
     * @brief 设置占位符文本
     * @param text 占位符显示的文本
     */
    void setPlaceholderText(const QString &text);

    /**
     * @brief 设置子标题文本
     * @param text 子标题显示的文本
     */
    void setSubtitleText(const QString &text);

signals:
    /**
     * @brief Geant4进程启动成功信号
     */
    void geant4Started();

    /**
     * @brief Geant4进程结束信号
     * @param exitCode 进程退出码
     */
    void geant4Finished(int exitCode);

    /**
     * @brief Geant4窗口嵌入成功信号
     */
    void geant4Embedded();

    /**
     * @brief 错误信号
     * @param errorMessage 错误信息
     */
    void errorOccurred(const QString &errorMessage);

protected:
    /**
     * @brief 事件过滤器，处理窗口大小调整事件
     */
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    /**
     * @brief 处理Geant4进程的标准输出
     */
    void onGeant4StandardOutput();

    /**
     * @brief 处理Geant4进程的标准错误输出
     */
    void onGeant4StandardError();

    /**
     * @brief 处理Geant4进程结束事件
     */
    void onGeant4ProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    /**
     * @brief 尝试嵌入指定窗口ID的窗口
     * @param windowId 窗口ID
     * @param attempt 尝试次数
     */
    void tryEmbedWindow(qulonglong windowId, int attempt = 0);

    /**
     * @brief 重置控件到初始状态
     */
    void resetToPlaceholder();

    /**
     * @brief 设置占位符界面
     */
    void setupPlaceholderUI();

    // 成员变量
    QProcess *m_geant4Process;          // Geant4进程
    QWidget *m_embedContainer;          // 嵌入容器
    QWindow *m_foreignWindow;           // 外部窗口
    bool m_isEmbedded;                  // 是否已嵌入
    QString m_outputBuffer;             // 输出缓冲区

    // UI组件
    QVBoxLayout *m_mainLayout;          // 主布局
    QWidget *m_placeholderWidget;       // 占位符控件
    QLabel *m_placeholderLabel;         // 占位符标签
    QLabel *m_subtitleLabel;            // 子标题标签

    // 常量
    static const int MAX_EMBED_ATTEMPTS = 5;  // 最大嵌入尝试次数
    static const int EMBED_RETRY_DELAY = 100; // 嵌入重试延迟(ms)
};

#endif // EMBED_H
