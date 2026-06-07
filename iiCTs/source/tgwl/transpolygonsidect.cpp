#include "transpolygonsidect.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

TransPolygonSideCT::TransPolygonSideCT(QObject *parent)
    : QObject(parent)
{
}

TransPolygonSideCT::~TransPolygonSideCT()
{
}

bool TransPolygonSideCT::convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath)
{
    QFile gwlFile(gwlFilePath);
    if (!gwlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit conversionError("Failed to open GWL file: " + gwlFilePath);
        return false;
    }

    // 解析GWL文件
    PolygonSideCTGWLData gwlData;
    if (!parseGWLFile(&gwlFile, gwlData))
    {
        gwlFile.close();
        return false;
    }
    gwlFile.close();

    // 生成标准Mac文件
    if (!generateMacFile(macFilePath, gwlData))
    {
        return false;
    }

    // 生成可视化Mac文件
    QString visFilePath = generateVisMacFilePath(macFilePath);
    if (!generateVisMacFile(visFilePath, gwlData))
    {
        return false;
    }

    // 生成循环控制文件（如果有SourceOrder和SourceNumber参数）
    if (!gwlData.sourceOrder.isEmpty() && !gwlData.sourceNumber.isEmpty())
    {
        QFileInfo fileInfo(macFilePath);
        QString loopFilePath = fileInfo.absolutePath() + "/" + fileInfo.baseName() + "_loop.mac";
        if (!generateLoopControlFile(loopFilePath, gwlData))
        {
            return false;
        }
    }

    emit conversionCompleted("Conversion completed successfully: " + macFilePath + " and " + visFilePath);
    return true;
}

bool TransPolygonSideCT::parseGWLFile(QFile *gwlFile, PolygonSideCTGWLData &gwlData)
{
    QTextStream in(gwlFile);
    QString line;
    int lineNumber = 0;

    while (!in.atEnd())
    {
        line = in.readLine().trimmed();
        lineNumber++;

        // 跳过注释行和空行
        if (line.isEmpty() || line.startsWith("#"))
        {
            continue;
        }

        // 解析参数行格式: ParameterName Value # Comment
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 2)
        {
            continue; // 跳过格式不正确的行
        }

        QString paramName = parts[0];
        QString paramValue = parts[1];

        // 解析各种参数
        if (!parseParameter(paramName, paramValue, gwlData))
        {
            // 如果是未知参数，只输出警告但继续处理
            qDebug() << QString("警告：第%1行未知参数: %2，将被忽略").arg(lineNumber).arg(paramName);
        }
    }

    return true;
}

bool TransPolygonSideCT::parseParameter(const QString &paramName, const QString &paramValue, PolygonSideCTGWLData &gwlData)
{
    // 几何参数
    if (paramName == "PhantomScale")
    {
        gwlData.phantomScale = paramValue;
    }
    else if (paramName == "DetModuleNum")
    {
        gwlData.detModuleNum = paramValue;
    }
    else if (paramName == "DetectorModuleRadius")
    {
        gwlData.detectorModuleRadius = paramValue;
    }
    else if (paramName == "DetToSource")
    {
        gwlData.detToSource = paramValue;
    }
    else if (paramName == "TubeDistance")
    {
        gwlData.tubeDistance = paramValue;
    }
    else if (paramName == "DetDistance")
    {
        gwlData.detDistance = paramValue;
    }
    // 探测器参数
    else if (paramName == "DetectorMaterial")
    {
        gwlData.detectorMaterial = paramValue;
    }
    else if (paramName == "DetSizeX")
    {
        gwlData.detSizeX = paramValue;
    }
    else if (paramName == "DetSizeY")
    {
        gwlData.detSizeY = paramValue;
    }
    else if (paramName == "DetSizeZ")
    {
        gwlData.detSizeZ = paramValue;
    }
    else if (paramName == "DetOffsetX")
    {
        gwlData.detOffsetX = paramValue;
    }
    else if (paramName == "DetOffsetY")
    {
        gwlData.detOffsetY = paramValue;
    }
    else if (paramName == "DetCountX")
    {
        gwlData.detCountX = paramValue;
    }
    else if (paramName == "DetCountY")
    {
        gwlData.detCountY = paramValue;
    }
    else if (paramName == "DetUnitSizeX")
    {
        gwlData.detUnitSizeX = paramValue;
    }
    else if (paramName == "DetUnitSizeY")
    {
        gwlData.detUnitSizeY = paramValue;
    }
    else if (paramName == "DetUnitThickness")
    {
        gwlData.detUnitThickness = paramValue;
    }
    // 探测器过滤层参数
    else if (paramName == "DetFilter1")
    {
        gwlData.detFilter1 = paramValue;
    }
    else if (paramName == "DetFilter1SizeX")
    {
        gwlData.detFilter1SizeX = paramValue;
    }
    else if (paramName == "DetFilter1SizeY")
    {
        gwlData.detFilter1SizeY = paramValue;
    }
    else if (paramName == "DetFilter1Thick")
    {
        gwlData.detFilter1Thick = paramValue;
    }
    else if (paramName == "DetFilter1Pos")
    {
        gwlData.detFilter1Pos = paramValue;
    }
    else if (paramName == "DetFilter1OffsetX")
    {
        gwlData.detFilter1OffsetX = paramValue;
    }
    else if (paramName == "DetFilter1OffsetY")
    {
        gwlData.detFilter1OffsetY = paramValue;
    }
    else if (paramName == "DetFilter1Material")
    {
        gwlData.detFilter1Material = paramValue;
    }
    // X射线管过滤层参数
    else if (paramName == "TubeFilter1Material")
    {
        gwlData.tubeFilter1Material = paramValue;
    }
    else if (paramName == "TubeFilter1SizeX")
    {
        gwlData.tubeFilter1SizeX = paramValue;
    }
    else if (paramName == "TubeFilter1SizeY")
    {
        gwlData.tubeFilter1SizeY = paramValue;
    }
    else if (paramName == "TubeFilter1Thick")
    {
        gwlData.tubeFilter1Thick = paramValue;
    }
    else if (paramName == "TubeFilter1Pos")
    {
        gwlData.tubeFilter1Pos = paramValue;
    }
    else if (paramName == "TubeFilter1OffsetX")
    {
        gwlData.tubeFilter1OffsetX = paramValue;
    }
    else if (paramName == "TubeFilter1OffsetY")
    {
        gwlData.tubeFilter1OffsetY = paramValue;
    }
    // 检测器路径和偏移
    else if (paramName == "DetectorPath")
    {
        gwlData.detectorPath = paramValue;
    }
    else if (paramName == "PhantomOffsetX")
    {
        gwlData.phantomOffsetX = paramValue;
    }
    else if (paramName == "PhantomOffsetY")
    {
        gwlData.phantomOffsetY = paramValue;
    }
    else if (paramName == "PhantomOffsetZ")
    {
        gwlData.phantomOffsetZ = paramValue;
    }
    // 多边形特有参数
    else if (paramName == "PolygonCount")
    {
        gwlData.polygonCount = paramValue;
    }
    else if (paramName == "DetYaw1")
    {
        gwlData.detYaw1 = paramValue;
    }
    else if (paramName == "DetPitch1")
    {
        gwlData.detPitch1 = paramValue;
    }
    else if (paramName == "DetRoll1")
    {
        gwlData.detRoll1 = paramValue;
    }
    else if (paramName == "DetYaw2")
    {
        gwlData.detYaw2 = paramValue;
    }
    else if (paramName == "DetPitch2")
    {
        gwlData.detPitch2 = paramValue;
    }
    else if (paramName == "DetRoll2")
    {
        gwlData.detRoll2 = paramValue;
    }
    else if (paramName == "DetYaw3")
    {
        gwlData.detYaw3 = paramValue;
    }
    else if (paramName == "DetPitch3")
    {
        gwlData.detPitch3 = paramValue;
    }
    else if (paramName == "DetRoll3")
    {
        gwlData.detRoll3 = paramValue;
    }
    else if (paramName == "DetectorAngle")
    {
        gwlData.detectorAngle = paramValue;
    }
    // X射线管参数
    else if (paramName == "TubeXOffset")
    {
        gwlData.tubeXOffset = paramValue;
    }
    else if (paramName == "TubeYOffset")
    {
        gwlData.tubeYOffset = paramValue;
    }
    else if (paramName == "SourceOrder")
    {
        gwlData.sourceOrder = paramValue;
    }
    else if (paramName == "SourceNumber")
    {
        gwlData.sourceNumber = paramValue;
    }
    else if (paramName == "SourceLength")
    {
        gwlData.sourceLength = paramValue;
    }
    else if (paramName == "SourceOffset")
    {
        gwlData.sourceOffset = paramValue;
    }
    else if (paramName == "TubeYaw")
    {
        gwlData.tubeYaw = paramValue;
    }
    else if (paramName == "TubePitch")
    {
        gwlData.tubePitch = paramValue;
    }
    else if (paramName == "TubeRoll")
    {
        gwlData.tubeRoll = paramValue;
    }
    // 扇形角度参数
    else if (paramName == "LeftFanAngle")
    {
        gwlData.leftFanAngle = paramValue;
    }
    else if (paramName == "RightFanAngle")
    {
        gwlData.rightFanAngle = paramValue;
    }
    else if (paramName == "LowConeAngle")
    {
        gwlData.lowConeAngle = paramValue;
    }
    else if (paramName == "HighConeAngle")
    {
        gwlData.highConeAngle = paramValue;
    }
    // 床运动方向
    else if (paramName == "BedDirection")
    {
        gwlData.bedDirection = paramValue;
    }
    // 物理过程
    else if (paramName == "PhotoEffect")
    {
        gwlData.photoEffect = paramValue;
    }
    else if (paramName == "ComptonScatter")
    {
        gwlData.comptonScatter = paramValue;
    }
    else if (paramName == "RayleighScatter")
    {
        gwlData.rayleighScatter = paramValue;
    }
    else if (paramName == "Ionization")
    {
        gwlData.ionization = paramValue;
    }
    else if (paramName == "MultipleScattering")
    {
        gwlData.multipleScattering = paramValue;
    }
    // 输入输出路径
    else if (paramName == "G4dcmPath")
    {
        gwlData.g4dcmPath = paramValue;
    }
    else if (paramName == "SpectrumPosPath")
    {
        gwlData.spectrumPosPath = paramValue;
    }
    else if (paramName == "SpectrumPath")
    {
        gwlData.spectrumPath = paramValue;
    }
    else if (paramName == "OutputPath")
    {
        gwlData.outputPath = paramValue;
    }
    else if (paramName == "PhotonCount")
    {
        gwlData.photonCount = paramValue;
    }
    else
    {
        // 输出警告但不阻止转换
        qDebug() << "警告：未知参数 " << paramName << "，将被忽略";
        return true;
    }

    return true;
}

bool TransPolygonSideCT::generatePolygonDetectorAngles(QTextStream &out, const PolygonSideCTGWLData &gwlData)
{
    if (gwlData.polygonCount.isEmpty())
    {
        return false;
    }

    bool ok;
    int polygonCount = gwlData.polygonCount.toInt(&ok);
    if (!ok)
    {
        return false;
    }

    if (polygonCount == 2)
    {
        // 生成两个探测器角度命令
        if (!gwlData.detYaw1.isEmpty() && !gwlData.detPitch1.isEmpty() && !gwlData.detRoll1.isEmpty())
        {
            out << "/detector/setDetectorRotationAngle1 " << gwlData.detYaw1 << " "
                << gwlData.detPitch1 << " " << gwlData.detRoll1 << "\n";
        }
        if (!gwlData.detYaw2.isEmpty() && !gwlData.detPitch2.isEmpty() && !gwlData.detRoll2.isEmpty())
        {
            out << "/detector/setDetectorRotationAngle2 " << gwlData.detYaw2 << " "
                << gwlData.detPitch2 << " " << gwlData.detRoll2 << "\n";
        }
    }
    else if (polygonCount == 3)
    {
        // 生成三个探测器角度命令
        if (!gwlData.detYaw1.isEmpty() && !gwlData.detPitch1.isEmpty() && !gwlData.detRoll1.isEmpty())
        {
            out << "/detector/setDetectorRotationAngle3 " << gwlData.detYaw1 << " "
                << gwlData.detPitch1 << " " << gwlData.detRoll1 << "\n";
        }
        if (!gwlData.detYaw2.isEmpty() && !gwlData.detPitch2.isEmpty() && !gwlData.detRoll2.isEmpty())
        {
            out << "/detector/setDetectorRotationAngle4 " << gwlData.detYaw2 << " "
                << gwlData.detPitch2 << " " << gwlData.detRoll2 << "\n";
        }
        if (!gwlData.detYaw3.isEmpty() && !gwlData.detPitch3.isEmpty() && !gwlData.detRoll3.isEmpty())
        {
            out << "/detector/setDetectorRotationAngle5 " << gwlData.detYaw3 << " "
                << gwlData.detPitch3 << " " << gwlData.detRoll3 << "\n";
        }
    }

    return true;
}

bool TransPolygonSideCT::generateMacFile(const QString &macFilePath, const PolygonSideCTGWLData &gwlData)
{
    QFile macFile(macFilePath);
    if (!macFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create Mac file: " + macFilePath);
        return false;
    }

    QTextStream out(&macFile);

    // 按照原始宏命令转换规则生成Mac文件
    // 基础几何参数
    if (!gwlData.phantomScale.isEmpty())
        out << "/detector/setmultiple " << gwlData.phantomScale << "\n";
    if (!gwlData.detModuleNum.isEmpty())
        out << "/detector/setDetectorModuleNum " << gwlData.detModuleNum << "\n";
    if (!gwlData.detectorModuleRadius.isEmpty())
        out << "/detector/setDetectorRadius " << gwlData.detectorModuleRadius << "\n";
    if (!gwlData.detToSource.isEmpty())
        out << "/detector/setDSD " << gwlData.detToSource << "\n";
    if (!gwlData.tubeDistance.isEmpty())
        out << "/detector/setDSO " << gwlData.tubeDistance << "\n";
    if (!gwlData.detDistance.isEmpty())
        out << "/detector/setDDO " << gwlData.detDistance << "\n";
    if (!gwlData.detectorMaterial.isEmpty())
        out << "/detector/material " << gwlData.detectorMaterial << "\n";

    // 探测器尺寸参数
    if (!gwlData.detSizeX.isEmpty())
        out << "/detector/setDetectorXSize " << gwlData.detSizeX << "\n";
    if (!gwlData.detSizeY.isEmpty())
        out << "/detector/setDetectorYSize " << gwlData.detSizeY << "\n";
    if (!gwlData.detSizeZ.isEmpty())
        out << "/detector/setDetectorZSize " << gwlData.detSizeZ << "\n";
    if (!gwlData.detOffsetX.isEmpty())
        out << "/detector/setDetectorXBias " << gwlData.detOffsetX << "\n";
    if (!gwlData.detOffsetY.isEmpty())
        out << "/detector/setDetectorYBias " << gwlData.detOffsetY << "\n";

    // 像素参数
    if (!gwlData.detCountX.isEmpty())
        out << "/detector/setPixelXCount " << gwlData.detCountX << "\n";
    if (!gwlData.detCountY.isEmpty())
        out << "/detector/setPixelYCount " << gwlData.detCountY << "\n";
    if (!gwlData.detUnitSizeX.isEmpty())
        out << "/detector/setPixelXSize " << gwlData.detUnitSizeX << "\n";
    if (!gwlData.detUnitSizeY.isEmpty())
        out << "/detector/setPixelYSize " << gwlData.detUnitSizeY << "\n";
    if (!gwlData.detUnitThickness.isEmpty())
        out << "/detector/setPixelZSize " << gwlData.detUnitThickness << "\n";

    // 探测器过滤层参数
    if (!gwlData.detFilter1.isEmpty())
        out << "/detector/setFilter " << gwlData.detFilter1 << "\n";
    if (!gwlData.detFilter1SizeX.isEmpty())
        out << "/detector/setFilterXSize " << gwlData.detFilter1SizeX << "\n";
    if (!gwlData.detFilter1SizeY.isEmpty())
        out << "/detector/setFilterYSize " << gwlData.detFilter1SizeY << "\n";
    if (!gwlData.detFilter1Thick.isEmpty())
        out << "/detector/setFilterZSize " << gwlData.detFilter1Thick << "\n";
    if (!gwlData.detFilter1Pos.isEmpty())
        out << "/detector/setFilterPos " << gwlData.detFilter1Pos << "\n";
    if (!gwlData.detFilter1OffsetX.isEmpty())
        out << "/detector/setFilterXBias " << gwlData.detFilter1OffsetX << "\n";
    if (!gwlData.detFilter1OffsetY.isEmpty())
        out << "/detector/setFilterYBias " << gwlData.detFilter1OffsetY << "\n";
    if (!gwlData.detFilter1Material.isEmpty())
        out << "/detector/setFilterMaterial " << gwlData.detFilter1Material << "\n";

    // X射线管过滤层参数
    if (!gwlData.tubeFilter1Material.isEmpty())
        out << "/detector/setTubeFilterMaterial " << gwlData.tubeFilter1Material << "\n";
    if (!gwlData.tubeFilter1SizeX.isEmpty())
        out << "/detector/setTubeFilterXSize " << gwlData.tubeFilter1SizeX << "\n";
    if (!gwlData.tubeFilter1SizeY.isEmpty())
        out << "/detector/setTubeFilterYSize " << gwlData.tubeFilter1SizeY << "\n";
    if (!gwlData.tubeFilter1Thick.isEmpty())
        out << "/detector/setTubeFilterZSize " << gwlData.tubeFilter1Thick << "\n";
    if (!gwlData.tubeFilter1Pos.isEmpty())
        out << "/detector/setTubeFilterPos " << gwlData.tubeFilter1Pos << "\n";
    if (!gwlData.tubeFilter1OffsetX.isEmpty())
        out << "/detector/setTubeFilterXBias " << gwlData.tubeFilter1OffsetX << "\n";
    if (!gwlData.tubeFilter1OffsetY.isEmpty())
        out << "/detector/setTubeFilterYBias " << gwlData.tubeFilter1OffsetY << "\n";

    // 体模偏移参数
    if (!gwlData.phantomOffsetX.isEmpty())
        out << "/detector/setObjectXBias " << gwlData.phantomOffsetX << "\n";
    if (!gwlData.phantomOffsetY.isEmpty())
        out << "/detector/setObjectYBias " << gwlData.phantomOffsetY << "\n";
    if (!gwlData.phantomOffsetZ.isEmpty())
        out << "/detector/setObjectZBias " << gwlData.phantomOffsetZ << "\n";

    // 多边形计数
    if (!gwlData.polygonCount.isEmpty())
        out << "/detector/PolygonCount " << gwlData.polygonCount << "\n";

    // 多边形特有的探测器角度参数
    generatePolygonDetectorAngles(out, gwlData);

    // X射线管位置参数
    if (!gwlData.tubeXOffset.isEmpty())
        out << "/tube/setUOffset " << gwlData.tubeXOffset << "\n";
    if (!gwlData.tubeYOffset.isEmpty())
        out << "/tube/setVOffset " << gwlData.tubeYOffset << "\n";

    // 射线源参数
    if (!gwlData.sourceOrder.isEmpty())
        out << "/tube/setOrder {order}\n"; // 使用循环变量
    if (!gwlData.sourceNumber.isEmpty())
        out << "/tube/setTotalsource " << gwlData.sourceNumber << "\n";
    if (!gwlData.sourceLength.isEmpty())
        out << "/tube/setSourceLength " << gwlData.sourceLength << "\n";
    if (!gwlData.sourceOffset.isEmpty())
        out << "/tube/setOffcenter " << gwlData.sourceOffset << "\n";

    // X射线管角度参数
    if (!gwlData.tubeYaw.isEmpty())
        out << "/tube/setTubeXRotationAngle " << gwlData.tubeYaw << "\n";
    if (!gwlData.tubePitch.isEmpty())
        out << "/tube/setTubeYRotationAngle " << gwlData.tubePitch << "\n";
    if (!gwlData.tubeRoll.isEmpty())
        out << "/tube/setTubeZRotationAngle " << gwlData.tubeRoll << "\n";

    // 扇形角度参数
    if (!gwlData.leftFanAngle.isEmpty())
        out << "/tube/setLeftAngle " << gwlData.leftFanAngle << "\n";
    if (!gwlData.rightFanAngle.isEmpty())
        out << "/tube/setRightAngle " << gwlData.rightFanAngle << "\n";
    if (!gwlData.lowConeAngle.isEmpty())
        out << "/tube/setLowConeAngle " << gwlData.lowConeAngle << "\n";
    if (!gwlData.highConeAngle.isEmpty())
        out << "/tube/setHigConeAngle " << gwlData.highConeAngle << "\n";

    // 床运动方向
    if (!gwlData.bedDirection.isEmpty())
        out << "/detector/BedDirection " << gwlData.bedDirection << "\n";

    // 物理过程
    if (!gwlData.photoEffect.isEmpty())
        out << "/testem/PhotoEffect " << gwlData.photoEffect << "\n";
    if (!gwlData.comptonScatter.isEmpty())
        out << "/testem/ComptonScatter " << gwlData.comptonScatter << "\n";
    if (!gwlData.rayleighScatter.isEmpty())
        out << "/testem/RayleighScatter " << gwlData.rayleighScatter << "\n";
    if (!gwlData.ionization.isEmpty())
        out << "/testem/Ionization " << gwlData.ionization << "\n";
    if (!gwlData.multipleScattering.isEmpty())
        out << "/testem/MultipleScattering " << gwlData.multipleScattering << "\n";

    // 输入输出路径
    if (!gwlData.g4dcmPath.isEmpty())
        out << "/detector/G4dcm " << gwlData.g4dcmPath << "\n";
    if (!gwlData.spectrumPosPath.isEmpty())
        out << "/detector/SpectrumPos " << gwlData.spectrumPosPath << "\n";
    if (!gwlData.spectrumPath.isEmpty())
        out << "/detector/SpectrumEnergy " << gwlData.spectrumPath << "\n";
    if (!gwlData.outputPath.isEmpty())
        out << "/detector/Output " << gwlData.outputPath << "\n";

    // 运行初始化和射线数
    out << "/run/initialize\n";
    if (!gwlData.photonCount.isEmpty())
        out << "/run/beamOn " << gwlData.photonCount << "\n";

    macFile.close();
    return true;
}

bool TransPolygonSideCT::generateVisMacFile(const QString &visFilePath, const PolygonSideCTGWLData &gwlData)
{
    QFile visFile(visFilePath);
    if (!visFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create visualization Mac file: " + visFilePath);
        return false;
    }

    QTextStream out(&visFile);


    // 使用与标准Mac文件相同的命令（复制相同逻辑）
    // 基础几何参数
    if (!gwlData.phantomScale.isEmpty())
        out << "/detector/setmultiple " << gwlData.phantomScale << "\n";
    if (!gwlData.detModuleNum.isEmpty())
        out << "/detector/setDetectorModuleNum " << gwlData.detModuleNum << "\n";
    if (!gwlData.detectorModuleRadius.isEmpty())
        out << "/detector/setDetectorRadius " << gwlData.detectorModuleRadius << "\n";
    if (!gwlData.detToSource.isEmpty())
        out << "/detector/setDSD " << gwlData.detToSource << "\n";
    if (!gwlData.tubeDistance.isEmpty())
        out << "/detector/setDSO " << gwlData.tubeDistance << "\n";
    if (!gwlData.detDistance.isEmpty())
        out << "/detector/setDDO " << gwlData.detDistance << "\n";
    if (!gwlData.detectorMaterial.isEmpty())
        out << "/detector/material " << gwlData.detectorMaterial << "\n";

    // 探测器尺寸参数
    if (!gwlData.detSizeX.isEmpty())
        out << "/detector/setDetectorXSize " << gwlData.detSizeX << "\n";
    if (!gwlData.detSizeY.isEmpty())
        out << "/detector/setDetectorYSize " << gwlData.detSizeY << "\n";
    if (!gwlData.detSizeZ.isEmpty())
        out << "/detector/setDetectorZSize " << gwlData.detSizeZ << "\n";
    if (!gwlData.detOffsetX.isEmpty())
        out << "/detector/setDetectorXBias " << gwlData.detOffsetX << "\n";
    if (!gwlData.detOffsetY.isEmpty())
        out << "/detector/setDetectorYBias " << gwlData.detOffsetY << "\n";

    // 像素参数
    if (!gwlData.detCountX.isEmpty())
        out << "/detector/setPixelXCount " << gwlData.detCountX << "\n";
    if (!gwlData.detCountY.isEmpty())
        out << "/detector/setPixelYCount " << gwlData.detCountY << "\n";
    if (!gwlData.detUnitSizeX.isEmpty())
        out << "/detector/setPixelXSize " << gwlData.detUnitSizeX << "\n";
    if (!gwlData.detUnitSizeY.isEmpty())
        out << "/detector/setPixelYSize " << gwlData.detUnitSizeY << "\n";
    if (!gwlData.detUnitThickness.isEmpty())
        out << "/detector/setPixelZSize " << gwlData.detUnitThickness << "\n";

    // 探测器过滤层参数
    if (!gwlData.detFilter1.isEmpty())
        out << "/detector/setFilter " << gwlData.detFilter1 << "\n";
    if (!gwlData.detFilter1SizeX.isEmpty())
        out << "/detector/setFilterXSize " << gwlData.detFilter1SizeX << "\n";
    if (!gwlData.detFilter1SizeY.isEmpty())
        out << "/detector/setFilterYSize " << gwlData.detFilter1SizeY << "\n";
    if (!gwlData.detFilter1Thick.isEmpty())
        out << "/detector/setFilterZSize " << gwlData.detFilter1Thick << "\n";
    if (!gwlData.detFilter1Pos.isEmpty())
        out << "/detector/setFilterPos " << gwlData.detFilter1Pos << "\n";
    if (!gwlData.detFilter1OffsetX.isEmpty())
        out << "/detector/setFilterXBias " << gwlData.detFilter1OffsetX << "\n";
    if (!gwlData.detFilter1OffsetY.isEmpty())
        out << "/detector/setFilterYBias " << gwlData.detFilter1OffsetY << "\n";
    if (!gwlData.detFilter1Material.isEmpty())
        out << "/detector/setFilterMaterial " << gwlData.detFilter1Material << "\n";

    // X射线管过滤层参数
    if (!gwlData.tubeFilter1Material.isEmpty())
        out << "/detector/setTubeFilterMaterial " << gwlData.tubeFilter1Material << "\n";
    if (!gwlData.tubeFilter1SizeX.isEmpty())
        out << "/detector/setTubeFilterXSize " << gwlData.tubeFilter1SizeX << "\n";
    if (!gwlData.tubeFilter1SizeY.isEmpty())
        out << "/detector/setTubeFilterYSize " << gwlData.tubeFilter1SizeY << "\n";
    if (!gwlData.tubeFilter1Thick.isEmpty())
        out << "/detector/setTubeFilterZSize " << gwlData.tubeFilter1Thick << "\n";
    if (!gwlData.tubeFilter1Pos.isEmpty())
        out << "/detector/setTubeFilterPos " << gwlData.tubeFilter1Pos << "\n";
    if (!gwlData.tubeFilter1OffsetX.isEmpty())
        out << "/detector/setTubeFilterXBias " << gwlData.tubeFilter1OffsetX << "\n";
    if (!gwlData.tubeFilter1OffsetY.isEmpty())
        out << "/detector/setTubeFilterYBias " << gwlData.tubeFilter1OffsetY << "\n";

    // 体模偏移参数
    if (!gwlData.phantomOffsetX.isEmpty())
        out << "/detector/setObjectXBias " << gwlData.phantomOffsetX << "\n";
    if (!gwlData.phantomOffsetY.isEmpty())
        out << "/detector/setObjectYBias " << gwlData.phantomOffsetY << "\n";
    if (!gwlData.phantomOffsetZ.isEmpty())
        out << "/detector/setObjectZBias " << gwlData.phantomOffsetZ << "\n";

    // 多边形计数
    if (!gwlData.polygonCount.isEmpty())
        out << "/detector/PolygonCount " << gwlData.polygonCount << "\n";

    // 多边形特有的探测器角度参数
    generatePolygonDetectorAngles(out, gwlData);

    // X射线管位置参数
    if (!gwlData.tubeXOffset.isEmpty())
        out << "/tube/setUOffset " << gwlData.tubeXOffset << "\n";
    if (!gwlData.tubeYOffset.isEmpty())
        out << "/tube/setVOffset " << gwlData.tubeYOffset << "\n";

    // 射线源参数
    if (!gwlData.sourceOrder.isEmpty())
        out << "/tube/setOrder 4\n"; // 使用循环变量
    if (!gwlData.sourceNumber.isEmpty())
        out << "/tube/setTotalsource " << gwlData.sourceNumber << "\n";
    if (!gwlData.sourceLength.isEmpty())
        out << "/tube/setSourceLength " << gwlData.sourceLength << "\n";
    if (!gwlData.sourceOffset.isEmpty())
        out << "/tube/setOffcenter " << gwlData.sourceOffset << "\n";

    // X射线管角度参数
    if (!gwlData.tubeYaw.isEmpty())
        out << "/tube/setTubeXRotationAngle " << gwlData.tubeYaw << "\n";
    if (!gwlData.tubePitch.isEmpty())
        out << "/tube/setTubeYRotationAngle " << gwlData.tubePitch << "\n";
    if (!gwlData.tubeRoll.isEmpty())
        out << "/tube/setTubeZRotationAngle " << gwlData.tubeRoll << "\n";

    // 扇形角度参数
    if (!gwlData.leftFanAngle.isEmpty())
        out << "/tube/setLeftAngle " << gwlData.leftFanAngle << "\n";
    if (!gwlData.rightFanAngle.isEmpty())
        out << "/tube/setRightAngle " << gwlData.rightFanAngle << "\n";
    if (!gwlData.lowConeAngle.isEmpty())
        out << "/tube/setLowConeAngle " << gwlData.lowConeAngle << "\n";
    if (!gwlData.highConeAngle.isEmpty())
        out << "/tube/setHigConeAngle " << gwlData.highConeAngle << "\n";

    // 床运动方向
    if (!gwlData.bedDirection.isEmpty())
        out << "/detector/BedDirection " << gwlData.bedDirection << "\n";

    // 物理过程
    if (!gwlData.photoEffect.isEmpty())
        out << "/testem/PhotoEffect " << gwlData.photoEffect << "\n";
    if (!gwlData.comptonScatter.isEmpty())
        out << "/testem/ComptonScatter " << gwlData.comptonScatter << "\n";
    if (!gwlData.rayleighScatter.isEmpty())
        out << "/testem/RayleighScatter " << gwlData.rayleighScatter << "\n";
    if (!gwlData.ionization.isEmpty())
        out << "/testem/Ionization " << gwlData.ionization << "\n";
    if (!gwlData.multipleScattering.isEmpty())
        out << "/testem/MultipleScattering " << gwlData.multipleScattering << "\n";

    // 输入输出路径
    if (!gwlData.g4dcmPath.isEmpty())
        out << "/detector/G4dcm " << gwlData.g4dcmPath << "\n";
    if (!gwlData.spectrumPosPath.isEmpty())
        out << "/detector/SpectrumPos " << gwlData.spectrumPosPath << "\n";
    if (!gwlData.spectrumPath.isEmpty())
        out << "/detector/SpectrumEnergy " << gwlData.spectrumPath << "\n";
    if (!gwlData.outputPath.isEmpty())
        out << "/detector/Output " << gwlData.outputPath << "\n";

    out << "/run/initialize\n";
    // 添加可视化命令
    out << "/vis/open OGL 600x600-0+0\n";
    out << "/vis/drawVolume\n";
    out << "/vis/viewer/set/autoRefresh false\n";
    out << "/vis/verbose errors\n";
    out << "/vis/scene/create\n";
    out << "/vis/scene/add/volume\n";
    out << "/vis/scene/add/trajectories smooth\n";
    out << "/vis/modeling/trajectories/create/drawByCharge\n";
    out << "/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true\n";
    out << "/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2\n";
    out << "/vis/sceneHandler/attach\n";
    out << "/vis/viewer/set/viewpointVector -1 0 0\n";
    out << "/vis/viewer/set/lightsVector -1 0 0\n";
    out << "/vis/viewer/set/upVector 0 0 1\n";
    out << "/vis/viewer/set/projection orthogonal\n";
    out << "/vis/viewer/zoomTo 1.4\n";
    out << "/vis/viewer/set/style surface\n";
    out << "/vis/viewer/set/hiddenMarker true\n";
    out << "/vis/viewer/set/hiddenEdge true\n";



    visFile.close();
    return true;
}

bool TransPolygonSideCT::generateLoopControlFile(const QString &loopFilePath, const PolygonSideCTGWLData &gwlData)
{
    QFile loopFile(loopFilePath);
    if (!loopFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create loop control file: " + loopFilePath);
        return false;
    }

    QTextStream out(&loopFile);

    // 生成循环控制命令，根据原始代码逻辑
    if (!gwlData.sourceOrder.isEmpty() && !gwlData.sourceNumber.isEmpty())
    {
        QFileInfo mainFileInfo(loopFilePath);
        QString mainMacFile = mainFileInfo.baseName().replace("_loop", "") + ".mac";
        out << "/control/loop " << mainMacFile << " order " << gwlData.sourceOrder << " " << gwlData.sourceNumber << " 1\n";
    }

    loopFile.close();
    return true;
}

QString TransPolygonSideCT::generateVisMacFilePath(const QString &macFilePath)
{
    QFileInfo fileInfo(macFilePath);
    QString baseName = fileInfo.baseName();
    QString dirPath = fileInfo.absolutePath();
    return dirPath + "/" + baseName + "_vis.mac";
}
