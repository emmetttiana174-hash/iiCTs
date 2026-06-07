#ifndef TRANSPOLYGONINTERVALCT_H
#define TRANSPOLYGONINTERVALCT_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>

// GWL数据结构，用于存储从GWL文件解析的参数
struct PolygonIntervalCTGWLData
{
    // 基础几何参数
    QString phantomScale;
    QString detModuleNum;
    QString detectorModuleRadius;
    QString detToSource;
    QString tubeDistance;
    QString detDistance;

    // 探测器基本参数
    QString detectorMaterial;
    QString detSizeX;
    QString detSizeY;
    QString detSizeZ;
    QString detOffsetX;
    QString detOffsetY;
    QString detCountX;
    QString detCountY;
    QString detUnitSizeX;
    QString detUnitSizeY;
    QString detUnitThickness;

    // 探测器过滤层参数
    QString detFilter1;
    QString detFilter1SizeX;
    QString detFilter1SizeY;
    QString detFilter1Thick;
    QString detFilter1Pos;
    QString detFilter1OffsetX;
    QString detFilter1OffsetY;
    QString detFilter1Material;

    // X射线管过滤层参数
    QString tubeFilter1Material;
    QString tubeFilter1SizeX;
    QString tubeFilter1SizeY;
    QString tubeFilter1Thick;
    QString tubeFilter1Pos;
    QString tubeFilter1OffsetX;
    QString tubeFilter1OffsetY;

    // 检测器路径和偏移
    QString detectorPath;
    QString phantomOffsetX;
    QString phantomOffsetY;
    QString phantomOffsetZ;

    // 多边形特有参数
    QString polygonCount;
    QString detYaw1;
    QString detPitch1;
    QString detRoll1;
    QString detYaw2;
    QString detPitch2;
    QString detRoll2;
    QString detYaw3;
    QString detPitch3;
    QString detRoll3;
    QString detectorAngle;

    // 间隔CT特有参数
    QString rotationAngleStart;
    QString rotationAngleEnd;
    QString rotationAngleStep;
    QString projectionInterval;

    // X射线管参数
    QString tubeXOffset;
    QString tubeYOffset;
    QString sourceOrder;
    QString sourceNumber;
    QString sourceLength;
    QString sourceOffset;
    QString tubeYaw;
    QString tubePitch;
    QString tubeRoll;

    // 扇形角度参数
    QString leftFanAngle;
    QString rightFanAngle;
    QString lowConeAngle;
    QString highConeAngle;

    // 床运动方向
    QString bedDirection;

    // 物理过程参数
    QString photoEffect;
    QString comptonScatter;
    QString rayleighScatter;
    QString ionization;
    QString multipleScattering;

    // 输入输出路径
    QString g4dcmPath;
    QString spectrumPosPath;
    QString spectrumPath;
    QString outputPath;
    QString photonCount;
};

class PolygonIntervalCT : public QObject
{
    Q_OBJECT

public:
    explicit PolygonIntervalCT(QObject *parent = nullptr);
    ~PolygonIntervalCT();

    // 主要转换函数
    bool convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath);

private:
    // 解析GWL文件
    bool parseGWLFile(QFile *gwlFile, PolygonIntervalCTGWLData &gwlData);

    // 解析单个参数
    bool parseParameter(const QString &paramName, const QString &paramValue, PolygonIntervalCTGWLData &gwlData);

    // 生成Mac文件
    bool generateMacFile(const QString &macFilePath, const PolygonIntervalCTGWLData &gwlData);

    // 生成可视化Mac文件
    bool generateVisMacFile(const QString &visFilePath, const PolygonIntervalCTGWLData &gwlData);

    // 生成循环控制文件
    bool generateLoopControlFile(const QString &loopFilePath, const PolygonIntervalCTGWLData &gwlData);

    // 生成间隔CT的投影角度控制
    bool generateIntervalProjectionAngles(QTextStream &out, const PolygonIntervalCTGWLData &gwlData);

    // 生成多边形探测器角度
    bool generatePolygonDetectorAngles(QTextStream &out, const PolygonIntervalCTGWLData &gwlData);

    // 生成可视化Mac文件路径
    QString generateVisMacFilePath(const QString &macFilePath);

signals:
    void conversionCompleted(const QString &message);
    void conversionError(const QString &error);
};

#endif // POLYGONINTERVALCT_H
