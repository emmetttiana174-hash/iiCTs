#include "transcbct.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QFileInfo> // 包含QFileInfo头文件

TransCBCT::TransCBCT(QObject *parent)
    : QObject(parent)
{
}

TransCBCT::~TransCBCT()
{
}

bool TransCBCT::convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath)
{
    QFile gwlFile(gwlFilePath);
    if (!gwlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit conversionError("Failed to open GWL file: " + gwlFilePath);
        return false;
    }

    // 解析GWL文件
    CBCTGWLData gwlData;
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

    // 生成循环控制文件（如果有TotalViews、CircleViews和StartIndex参数）
    if (!gwlData.totalViews.isEmpty() && !gwlData.circleViews.isEmpty() && !gwlData.startIndex.isEmpty())
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

bool TransCBCT::parseGWLFile(QFile *gwlFile, CBCTGWLData &gwlData)
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
            // 对于未知参数，发出警告但继续处理
            qDebug() << QString("Warning: Unknown parameter at line %1: %2").arg(lineNumber).arg(paramName);
            // 不返回false，继续处理其他参数
        }
    }

    return true;
}

bool TransCBCT::parseParameter(const QString &paramName, const QString &paramValue, CBCTGWLData &gwlData)
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
    // 探测器角度参数
    else if (paramName == "DetYaw")
    {
        gwlData.detYaw = paramValue;
    }
    else if (paramName == "DetPitch")
    {
        gwlData.detPitch = paramValue;
    }
    else if (paramName == "DetRoll")
    {
        gwlData.detRoll = paramValue;
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
    // 螺旋CT专用参数
    else if (paramName == "HelicalPitch")
    {
        gwlData.helicalPitch = paramValue;
    }
    // 新增参数
    else if (paramName == "TotalViews")
    {
        gwlData.totalViews = paramValue;
    }
    else if (paramName == "CircleViews")
    {
        gwlData.circleViews = paramValue;
    }
    else if (paramName == "StartIndex")
    {
        gwlData.startIndex = paramValue;
    }
    else
    {
        return false; // 未知参数
    }

    return true;
}

bool TransCBCT::generateMacFile(const QString &macFilePath, const CBCTGWLData &gwlData)
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

    // 探测器角度参数（组合输出）
    // 优先使用已解析的 DetectorAngle 字段（如果存在），否则当 DetYaw/DetPitch/DetRoll 全部存在时组合输出
    if (!gwlData.detectorAngle.isEmpty())
    {
        out << "/detector/setDetectorRotationAngle " << gwlData.detectorAngle << "\n";
    }
    else if (!gwlData.detYaw.isEmpty() && !gwlData.detPitch.isEmpty() && !gwlData.detRoll.isEmpty())
    {
        out << "/detector/setDetectorRotationAngle " << gwlData.detYaw << " " << gwlData.detPitch << " " << gwlData.detRoll << "\n";
    }

    // X射线管位置参数
    if (!gwlData.tubeXOffset.isEmpty())
        out << "/tube/setUOffset " << gwlData.tubeXOffset << "\n";
    if (!gwlData.tubeYOffset.isEmpty())
        out << "/tube/setVOffset " << gwlData.tubeYOffset << "\n";
    // 螺旋CT pitch 参数
    if (!gwlData.helicalPitch.isEmpty())
        out << "/detector/setPitch " << gwlData.helicalPitch << "\n";
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

    // 如果存在 TotalViews/CircleViews/StartIndex，输出用于循环的 ObjectAngle1 占位命令
    if (!gwlData.totalViews.isEmpty() && !gwlData.circleViews.isEmpty() && !gwlData.startIndex.isEmpty())
    {
        // 使用与 generateLoopControlFile 中一致的循环变量名 "angle"
        out << "/detector/setObjectAngle1 0 0 {angle}\n";
    }
    // 运行初始化和射线数
    out << "/run/initialize\n";
    if (!gwlData.photonCount.isEmpty())
        out << "/run/beamOn " << gwlData.photonCount << "\n";

    macFile.close();
    return true;
}

bool TransCBCT::generateVisMacFile(const QString &visFilePath, const CBCTGWLData &gwlData)
{
    QFile visFile(visFilePath);
    if (!visFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create visualization Mac file: " + visFilePath);
        return false;
    }

    QTextStream out(&visFile);

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

    // 探测器角度参数（组合输出）
    if (!gwlData.detYaw.isEmpty() && !gwlData.detPitch.isEmpty() && !gwlData.detRoll.isEmpty())
    {
        out << "/detector/setDetectorRotationAngle " << gwlData.detYaw << " " << gwlData.detPitch << " " << gwlData.detRoll << "\n";
    }

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
    // 螺旋CT pitch 参数（可视化）
    if (!gwlData.helicalPitch.isEmpty())
        out << "/detector/setPitch " << gwlData.helicalPitch << "\n";

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

    // 为可视化宏文件设置一个固定的物体旋转角度
    out << "/detector/setObjectAngle1 0 0 90\n";
    // *************************

    // 可视化命令
    out << "/run/initialize\n";
    out << "\n";
    out << "# Visualization commands\n";
    out << "/vis/open OGL 600x600-0+0\n";
    out << "/vis/viewer/set/autoRefresh false\n";
    out << "/vis/verbose errors\n";
    out << "/vis/drawVolume\n";
    out << "/vis/viewer/set/viewpointThetaPhi -90. 0.\n";
    out << "/vis/viewer/zoom 30.0\n";
    out << "/vis/scene/add/axes 0 0 0 5 cm\n";
    out << "/vis/scene/add/trajectories\n";
    out << "/vis/scene/add/trajectories smooth\n";
    out << "/vis/modeling/trajectories/create/drawByCharge\n";
    out << "/vis/modeling/trajectories/drawByCharge-0/default/setDrawStepPts true\n";
    out << "/vis/modeling/trajectories/drawByCharge-0/default/setStepPtsSize 2\n";
    out << "/vis/scene/endOfEventAction accumulate\n";
    out << "/vis/viewer/set/autoRefresh true\n";
    out << "/vis/verbose warnings\n";

    visFile.close();
    return true;
}

QString TransCBCT::generateVisMacFilePath(const QString &macFilePath)
{
    QFileInfo fileInfo(macFilePath);
    QString baseName = fileInfo.baseName();
    QString dirPath = fileInfo.absolutePath();
    return dirPath + "/" + baseName + "_vis.mac";
}

bool TransCBCT::generateLoopControlFile(const QString &loopFilePath, const CBCTGWLData &gwlData)
{
    QFile loopFile(loopFilePath);
    if (!loopFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create loop control file: " + loopFilePath);
        return false;
    }

    QTextStream out(&loopFile);

    // 检查 TotalViews、CircleViews 和 StartIndex 是否都存在
    if (!gwlData.totalViews.isEmpty() && !gwlData.circleViews.isEmpty() && !gwlData.startIndex.isEmpty())
    {
        try
        {
            // 转换为双精度浮点数
            double totalViews = gwlData.totalViews.toDouble();
            double circleViews = gwlData.circleViews.toDouble();
            double startIndex = gwlData.startIndex.toDouble();

            if (circleViews == 0)
            {
                emit conversionError("Error: CircleViews cannot be 0!");
                loopFile.close();
                return false;
            }

            // 计算角度步长、起始角度和结束角度
            double step = 360.0 / circleViews;
            double start = startIndex * 360.0 / circleViews;
            double end = startIndex * 360.0 / circleViews + totalViews * 360.0 / circleViews;

            // 获取主 mac 文件名
            QFileInfo mainFileInfo(loopFilePath);
            QString mainMacFile = mainFileInfo.baseName().replace("_loop", "") + ".mac";

            // 写入循环控制命令
            out << "/control/loop " << mainMacFile << " angle " << start << " " << end << " " << step << "\n";

            // 记录生成的命令（用于调试）
            qDebug() << "Generated loop command: /control/loop" << mainMacFile << "angle" << start << end << step;
        }
        catch (const std::exception &e)
        {
            emit conversionError("Error: Failed to convert TotalViews, CircleViews, or StartIndex to numbers: " + QString(e.what()));
            loopFile.close();
            return false;
        }
    }
    else
    {
        // 缺少必要参数，生成空文件
        emit conversionError("Warning: Missing TotalViews, CircleViews, or StartIndex. Loop control file will be empty.");
    }

    loopFile.close();
    return true;
}
