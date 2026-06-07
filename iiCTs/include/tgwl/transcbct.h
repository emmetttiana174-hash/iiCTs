#ifndef TRANSCBCT_H
#define TRANSCBCT_H

#include <QObject>
#include <QString>
#include <QStringList>

class QFile;

// CBCT GWL数据结构
struct CBCTGWLData
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

    // 探测器角度参数
    QString detYaw;
    QString detPitch;
    QString detRoll;
    QString detectorAngle;
    QString objectAngle1;

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

    // 螺旋CT专用参数
    QString helicalPitch;

    // 循环
    QString totalViews;
    QString circleViews;
    QString startIndex;
};

class TransCBCT : public QObject
{
    Q_OBJECT

public:
    explicit TransCBCT(QObject *parent = nullptr);
    ~TransCBCT();

    bool convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath);

signals:
    void conversionCompleted(const QString &message);
    void conversionError(const QString &error);

private:
    bool parseGWLFile(QFile *gwlFile, CBCTGWLData &gwlData);
    bool parseParameter(const QString &paramName, const QString &paramValue, CBCTGWLData &gwlData);
    bool generateMacFile(const QString &macFilePath, const CBCTGWLData &gwlData);
    bool generateVisMacFile(const QString &visFilePath, const CBCTGWLData &gwlData);
    bool generateLoopControlFile(const QString &loopFilePath, const CBCTGWLData &gwlData);
    QString generateVisMacFilePath(const QString &macFilePath);
};

#endif // TRANSCBCT_H
