#ifndef TRANSSPECTRUM_H
#define TRANSSPECTRUM_H

#include <QObject>
#include <QString>
#include <QStringList>

class QFile;

// GWL数据结构
struct GWLData
{
    // 几何参数
    QString anodeAngle;
    QString tubeVoltage;
    QString anodeMaterial;
    QString targetMode;

    // 探测器参数
    QString detMaterial;
    QString detThickness;
    QString detSizeX;
    QString detSizeY;

    // 过滤层参数
    QString filter1Material;
    QString filter1Thickness;
    QString filter1Position;
    QString filter2Material;
    QString filter2Thickness;
    QString filter2Position;

    // 物理过程
    QString enableIonisation;
    QString enableBremsstrahlung;
    QString enableScatter;
    QString enableAnnihilation;

    // 仿真参数
    QString electronCount;
    QString gridSizeX;
    QString gridSizeY;
    QString energyBinSize;

    // 输入输出
    QString inputFilePath;
    QString outputFolder;
    QString outputFilename;
};

class TransSpectrum : public QObject
{
    Q_OBJECT

public:
    explicit TransSpectrum(QObject *parent = nullptr);
    ~TransSpectrum();

    // 主要转换方法
    bool convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath);

    // Qt接口方法
    bool convertGWLFile(const QString &gwlPath, const QString &outputDir);

    // 验证GWL文件
    bool validateGWLFile(const QString &gwlPath);

    // 获取支持的参数列表
    QStringList getSupportedParameters() const;

    // 获取最后的错误信息
    QString getLastError() const;

signals:
    // 转换完成信号
    void conversionCompleted(const QString &message);

    // 转换错误信号
    void conversionError(const QString &error);

    // 转换进度信号
    void conversionProgress(int percentage);

private:
    // 解析GWL文件
    bool parseGWLFile(QFile *gwlFile, GWLData &gwlData);

    // 解析单个参数
    bool parseParameter(const QString &paramName, const QString &paramValue, GWLData &gwlData);

    // 生成Mac文件
    bool generateMacFile(const QString &macFilePath, const GWLData &gwlData);

    // 生成可视化Mac文件路径
    QString generateVisMacFilePath(const QString &originalMacPath);

    // 生成可视化Mac文件
    bool generateVisMacFile(const QString &visMacFilePath, const GWLData &gwlData);

    // 错误信息
    QString lastError;
};

#endif // TRANSSPECTRUM_H
