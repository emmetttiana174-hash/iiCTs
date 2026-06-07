#ifndef TRANSPOLYGONSIDECT_H
#define TRANSPOLYGONSIDECT_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>

class QFile;

// 多边形侧面CT GWL数据结构
struct PolygonSideCTGWLData
{
    // 几何参数
    QString phantomScale;
    QString detModuleNum;
    QString detectorModuleRadius;
    QString detToSource;
    QString tubeDistance;
    QString detDistance;

    // 探测器参数
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

    // 物理过程
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

class TransPolygonSideCT : public QObject
{
    Q_OBJECT

public:
    explicit TransPolygonSideCT(QObject *parent = nullptr);
    ~TransPolygonSideCT();

    bool convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath);

signals:
    void conversionCompleted(const QString &message);
    void conversionError(const QString &error);

private:
    bool parseGWLFile(QFile *gwlFile, PolygonSideCTGWLData &gwlData);
    bool parseParameter(const QString &paramName, const QString &paramValue, PolygonSideCTGWLData &gwlData);
    bool generateMacFile(const QString &macFilePath, const PolygonSideCTGWLData &gwlData);
    bool generateVisMacFile(const QString &visFilePath, const PolygonSideCTGWLData &gwlData);
    bool generateLoopControlFile(const QString &loopFilePath, const PolygonSideCTGWLData &gwlData);
    QString generateVisMacFilePath(const QString &macFilePath);
    bool generatePolygonDetectorAngles(QTextStream &out, const PolygonSideCTGWLData &gwlData);
};

#endif // TRANSPOLYGONSIDECT_H
