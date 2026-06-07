#include "transspectrum.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>

TransSpectrum::TransSpectrum(QObject *parent)
    : QObject(parent)
{
}

TransSpectrum::~TransSpectrum()
{
}

bool TransSpectrum::convertGWLToMac(const QString &gwlFilePath, const QString &macFilePath)
{
    QFile gwlFile(gwlFilePath);
    if (!gwlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        emit conversionError("Failed to open GWL file: " + gwlFilePath);
        return false;
    }

    // 解析GWL文件
    GWLData gwlData;
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

    emit conversionCompleted("Conversion completed successfully: " + macFilePath + " and " + visFilePath);
    return true;
}

bool TransSpectrum::parseGWLFile(QFile *gwlFile, GWLData &gwlData)
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
            emit conversionError(QString("Unknown parameter at line %1: %2").arg(lineNumber).arg(paramName));
            return false;
        }
    }

    return true;
}

bool TransSpectrum::parseParameter(const QString &paramName, const QString &paramValue, GWLData &gwlData)
{
    // 几何参数
    if (paramName == "AnodeAngle")
    {
        gwlData.anodeAngle = paramValue;
    }
    else if (paramName == "TubeVoltage")
    {
        gwlData.tubeVoltage = paramValue;
    }
    else if (paramName == "AnodeMaterial")
    {
        gwlData.anodeMaterial = paramValue;
    }
    else if (paramName == "TargetMode")
    {
        gwlData.targetMode = paramValue;
    }
    // 探测器参数
    else if (paramName == "DetMaterial")
    {
        gwlData.detMaterial = paramValue;
    }
    else if (paramName == "DetThickness")
    {
        gwlData.detThickness = paramValue;
    }
    else if (paramName == "DetSizeX")
    {
        gwlData.detSizeX = paramValue;
    }
    else if (paramName == "DetSizeY")
    {
        gwlData.detSizeY = paramValue;
    }
    // 过滤层参数
    else if (paramName == "Filter1Material")
    {
        gwlData.filter1Material = paramValue;
    }
    else if (paramName == "Filter1Thickness")
    {
        gwlData.filter1Thickness = paramValue;
    }
    else if (paramName == "Filter1Position")
    {
        gwlData.filter1Position = paramValue;
    }
    else if (paramName == "Filter2Material")
    {
        gwlData.filter2Material = paramValue;
    }
    else if (paramName == "Filter2Thickness")
    {
        gwlData.filter2Thickness = paramValue;
    }
    else if (paramName == "Filter2Position")
    {
        gwlData.filter2Position = paramValue;
    }
    // 物理过程
    else if (paramName == "EnableIonisation")
    {
        gwlData.enableIonisation = paramValue;
    }
    else if (paramName == "EnableBremsstrahlung")
    {
        gwlData.enableBremsstrahlung = paramValue;
    }
    else if (paramName == "EnableScatter")
    {
        gwlData.enableScatter = paramValue;
    }
    else if (paramName == "EnableAnnihilation")
    {
        gwlData.enableAnnihilation = paramValue;
    }
    // 仿真参数
    else if (paramName == "ElectronCount")
    {
        gwlData.electronCount = paramValue;
    }
    else if (paramName == "GridSizeX")
    {
        gwlData.gridSizeX = paramValue;
    }
    else if (paramName == "GridSizeY")
    {
        gwlData.gridSizeY = paramValue;
    }
    else if (paramName == "EnergyBinSize")
    {
        gwlData.energyBinSize = paramValue;
    }
    // 输入输出
    else if (paramName == "InputFilePath")
    {
        gwlData.inputFilePath = paramValue;
    }
    else if (paramName == "OutputFolder")
    {
        gwlData.outputFolder = paramValue;
    }
    else if (paramName == "OutputFilename")
    {
        gwlData.outputFilename = paramValue;
    }
    else
    {
        return false; // 未知参数
    }

    return true;
}

bool TransSpectrum::generateMacFile(const QString &macFilePath, const GWLData &gwlData)
{
    QFile macFile(macFilePath);
    if (!macFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create Mac file: " + macFilePath);
        return false;
    }

    QTextStream out(&macFile);

    // 写入文件头注释
    out << "# Generated Mac file from GWL conversion\n";
    out << "# Conversion performed by iiCTs TransSpectrum module\n";
    out << "\n";

    // 按照Geant4规范顺序写入命令

    // 1. 物理过程配置 (必须在初始化之前)
    out << "# Physics processes configuration\n";
    if (!gwlData.enableIonisation.isEmpty())
    {
        out << "/testem/phys/EnableIonisation " << gwlData.enableIonisation << "\n";
    }
    if (!gwlData.enableBremsstrahlung.isEmpty())
    {
        out << "/testem/phys/EnableBremsstrahlung " << gwlData.enableBremsstrahlung << "\n";
    }
    if (!gwlData.enableScatter.isEmpty())
    {
        out << "/testem/phys/EnableScatter " << gwlData.enableScatter << "\n";
    }
    if (!gwlData.enableAnnihilation.isEmpty())
    {
        out << "/testem/phys/EnableAnnihilation " << gwlData.enableAnnihilation << "\n";
    }
    out << "\n";

    // 2. 输入文件路径设置
    out << "# Input configuration\n";
    if (!gwlData.inputFilePath.isEmpty())
    {
        out << "/Input/setInputFilePath " << gwlData.inputFilePath << "\n";
    }
    out << "\n";

    // 3. 输出文件路径设置
    out << "# Output configuration\n";
    if (!gwlData.outputFolder.isEmpty() && !gwlData.outputFilename.isEmpty())
    {
        QString outputPath = gwlData.outputFolder;
        if (!outputPath.endsWith("/"))
        {
            outputPath += "/";
        }
        outputPath += gwlData.outputFilename;
        out << "/Output/setOutputFilePath " << outputPath << "\n";
    }
    if (!gwlData.gridSizeX.isEmpty())
    {
        out << "#/Output/setGridSizeX " << gwlData.gridSizeX << " mm\n";
    }
    if (!gwlData.gridSizeY.isEmpty())
    {
        out << "#/Output/setGridSizeY " << gwlData.gridSizeY << " mm\n";
    }
    if (!gwlData.energyBinSize.isEmpty())
    {
        out << "#/Output/setEnergyBinSize " << gwlData.energyBinSize << "\n";
    }
    out << "\n";

    // 4. 几何组件配置
    out << "# Geometry configuration\n";
    if (!gwlData.anodeAngle.isEmpty())
    {
        out << "/Anode/setAnodeAngle " << gwlData.anodeAngle << "\n";
    }
    if (!gwlData.anodeMaterial.isEmpty())
    {
        QString material = gwlData.anodeMaterial;
        if (material == "Tungsten")
        {
            material = "W"; // 转换材料名称
        }
        out << "/Anode/setAnodeMaterial " << material << "\n";
    }
    if (!gwlData.targetMode.isEmpty())
    {
        out << "#/Target/setTargetMode " << gwlData.targetMode << "\n";
    }
    out << "\n";

    // 5. 探测器配置
    out << "# Detector configuration\n";
    if (!gwlData.detMaterial.isEmpty())
    {
        QString material = gwlData.detMaterial;
        if (material == "vacuum")
        {
            material = "Galactic"; // 转换材料名称
        }
        out << "/detector/setDetectorMaterial " << material << "\n";
    }
    if (!gwlData.detThickness.isEmpty())
    {
        double z = gwlData.detThickness.toDouble() / 2.0;
        out << "/detector/setDetectorZ " << z << " mm\n";
    }
    if (!gwlData.detSizeX.isEmpty())
    {
        double x = gwlData.detSizeX.toDouble() / 2.0;
        out << "/detector/setDetectorX " << x << " mm\n";
    }
    if (!gwlData.detSizeY.isEmpty())
    {
        double y = gwlData.detSizeY.toDouble() / 2.0;
        out << "/detector/setDetectorY " << y << " mm\n";
    }
    out << "\n";

    // 6. 过滤器配置
    out << "# Filter configuration\n";
    if (!gwlData.filter1Material.isEmpty())
    {
        out << "/Filter/setFilter1Material " << gwlData.filter1Material << "\n";
    }
    if (!gwlData.filter1Thickness.isEmpty())
    {
        double z1 = gwlData.filter1Thickness.toDouble() / 2.0;
        out << "/Filter/setFilter1Z " << z1 << " mm\n";
    }
    if (!gwlData.filter1Position.isEmpty())
    {
        out << "/Filter/Filter1Position " << gwlData.filter1Position << " mm\n";
    }
    if (!gwlData.filter2Material.isEmpty())
    {
        out << "/Filter/setFilter2Material " << gwlData.filter2Material << "\n";
    }
    if (!gwlData.filter2Thickness.isEmpty())
    {
        double z2 = gwlData.filter2Thickness.toDouble() / 2.0;
        out << "/Filter/setFilter2Z " << z2 << " mm\n";
    }
    if (!gwlData.filter2Position.isEmpty())
    {
        out << "/Filter/Filter2Position " << gwlData.filter2Position << " mm\n";
    }
    out << "\n";

    // 7. 运行命令 (必须在最后)
    out << "# Run simulation\n";
    out << "/run/initialize\n";
    if (!gwlData.electronCount.isEmpty())
    {
        out << "/run/beamOn " << gwlData.electronCount << "\n";
    }

    macFile.close();
    return true;
}

QString TransSpectrum::getLastError() const
{
    return lastError;
}

// Qt接口方法
bool TransSpectrum::convertGWLFile(const QString &gwlPath, const QString &outputDir)
{
    // 验证输入文件
    QFile gwlFile(gwlPath);
    if (!gwlFile.exists())
    {
        lastError = "GWL file does not exist: " + gwlPath;
        emit conversionError(lastError);
        return false;
    }

    // 创建输出目录
    QDir outDir(outputDir);
    if (!outDir.exists())
    {
        if (!outDir.mkpath("."))
        {
            lastError = "Failed to create output directory: " + outputDir;
            emit conversionError(lastError);
            return false;
        }
    }

    // 生成输出文件名
    QFileInfo gwlInfo(gwlPath);
    QString baseName = gwlInfo.baseName();
    QString macFileName = baseName + ".mac";
    QString macFilePath = outDir.absoluteFilePath(macFileName);

    // 执行转换
    return convertGWLToMac(gwlPath, macFilePath);
}

QStringList TransSpectrum::getSupportedParameters() const
{
    return QStringList()
           << "AnodeAngle" << "TubeVoltage" << "AnodeMaterial" << "TargetMode"
           << "DetMaterial" << "DetThickness" << "DetSizeX" << "DetSizeY"
           << "Filter1Material" << "Filter1Thickness" << "Filter1Position"
           << "Filter2Material" << "Filter2Thickness" << "Filter2Position"
           << "EnableIonisation" << "EnableBremsstrahlung" << "EnableScatter" << "EnableAnnihilation"
           << "ElectronCount" << "GridSizeX" << "GridSizeY" << "EnergyBinSize"
           << "InputFilePath" << "OutputFolder" << "OutputFilename";
}

bool TransSpectrum::validateGWLFile(const QString &gwlPath)
{
    QFile gwlFile(gwlPath);
    if (!gwlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        lastError = "Cannot open GWL file for validation: " + gwlPath;
        return false;
    }

    QTextStream in(&gwlFile);
    QString line;
    int lineNumber = 0;
    QStringList supportedParams = getSupportedParameters();

    while (!in.atEnd())
    {
        line = in.readLine().trimmed();
        lineNumber++;

        // 跳过注释行和空行
        if (line.isEmpty() || line.startsWith("#"))
        {
            continue;
        }

        // 检查参数格式
        QStringList parts = line.split(QRegularExpression("\\s+"));
        if (parts.size() < 2)
        {
            lastError = QString("Invalid format at line %1: %2").arg(lineNumber).arg(line);
            return false;
        }

        QString paramName = parts[0];
        if (!supportedParams.contains(paramName))
        {
            lastError = QString("Unsupported parameter at line %1: %2").arg(lineNumber).arg(paramName);
            return false;
        }
    }

    gwlFile.close();
    return true;
}

QString TransSpectrum::generateVisMacFilePath(const QString &originalMacPath)
{
    QFileInfo fileInfo(originalMacPath);
    QString baseName = fileInfo.baseName(); // 不带扩展名的文件名
    QString dir = fileInfo.absolutePath();  // 目录路径

    // 生成可视化Mac文件路径：添加"vis"后缀
    return dir + "/" + baseName + "vis.mac";
}

bool TransSpectrum::generateVisMacFile(const QString &visMacFilePath, const GWLData &gwlData)
{
    QFile visMacFile(visMacFilePath);
    if (!visMacFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit conversionError("Failed to create visualization Mac file: " + visMacFilePath);
        return false;
    }

    QTextStream out(&visMacFile);

    // 写入文件头注释
    out << "# Generated Visualization Mac file from GWL conversion\n";
    out << "# Conversion performed by iiCTs TransSpectrum module\n";
    out << "# This file includes visualization commands for Geant4\n";
    out << "\n";

    // 按照Geant4规范顺序写入命令

    // 1. 物理过程配置 (必须在初始化之前)
    out << "# Physics processes configuration\n";
    if (!gwlData.enableIonisation.isEmpty())
    {
        out << "/testem/phys/EnableIonisation " << gwlData.enableIonisation << "\n";
    }
    if (!gwlData.enableBremsstrahlung.isEmpty())
    {
        out << "/testem/phys/EnableBremsstrahlung " << gwlData.enableBremsstrahlung << "\n";
    }
    if (!gwlData.enableScatter.isEmpty())
    {
        out << "/testem/phys/EnableScatter " << gwlData.enableScatter << "\n";
    }
    if (!gwlData.enableAnnihilation.isEmpty())
    {
        out << "/testem/phys/EnableAnnihilation " << gwlData.enableAnnihilation << "\n";
    }
    out << "\n";

    // 2. 输入文件路径设置
    out << "# Input configuration\n";
    if (!gwlData.inputFilePath.isEmpty())
    {
        out << "/Input/setInputFilePath " << gwlData.inputFilePath << "\n";
    }
    out << "\n";

    // 3. 输出文件路径设置
    out << "# Output configuration\n";
    if (!gwlData.outputFolder.isEmpty() && !gwlData.outputFilename.isEmpty())
    {
        QString outputPath = gwlData.outputFolder;
        if (!outputPath.endsWith("/"))
        {
            outputPath += "/";
        }
        outputPath += gwlData.outputFilename;
        out << "/Output/setOutputFilePath " << outputPath << "\n";
    }
    if (!gwlData.gridSizeX.isEmpty())
    {
        out << "#/Output/setGridSizeX " << gwlData.gridSizeX << " mm\n";
    }
    if (!gwlData.gridSizeY.isEmpty())
    {
        out << "#/Output/setGridSizeY " << gwlData.gridSizeY << " mm\n";
    }
    if (!gwlData.energyBinSize.isEmpty())
    {
        out << "#/Output/setEnergyBinSize " << gwlData.energyBinSize << "\n";
    }
    out << "\n";

    // 4. 几何组件配置
    out << "# Geometry configuration\n";
    if (!gwlData.anodeAngle.isEmpty())
    {
        out << "/Anode/setAnodeAngle " << gwlData.anodeAngle << "\n";
    }
    if (!gwlData.anodeMaterial.isEmpty())
    {
        QString material = gwlData.anodeMaterial;
        if (material == "Tungsten")
        {
            material = "W"; // 转换材料名称
        }
        out << "/Anode/setAnodeMaterial " << material << "\n";
    }
    if (!gwlData.targetMode.isEmpty())
    {
        out << "#/Target/setTargetMode " << gwlData.targetMode << "\n";
    }
    if (!gwlData.tubeVoltage.isEmpty())
    {
        out << "#/Beam/setTubeVoltage " << gwlData.tubeVoltage << " keV\n";
    }
    out << "\n";

    // 5. 探测器配置
    out << "# Detector configuration\n";
    if (!gwlData.detMaterial.isEmpty())
    {
        QString material = gwlData.detMaterial;
        if (material == "vacuum")
        {
            material = "Galactic"; // 转换材料名称
        }
        out << "/detector/setDetectorMaterial " << material << "\n";
    }
    if (!gwlData.detThickness.isEmpty())
    {
        double z = gwlData.detThickness.toDouble() / 2.0;
        out << "/detector/setDetectorZ " << z << " mm\n";
    }
    if (!gwlData.detSizeX.isEmpty())
    {
        double x = gwlData.detSizeX.toDouble() / 2.0;
        out << "/detector/setDetectorX " << x << " mm\n";
    }
    if (!gwlData.detSizeY.isEmpty())
    {
        double y = gwlData.detSizeY.toDouble() / 2.0;
        out << "/detector/setDetectorY " << y << " mm\n";
    }
    out << "\n";

    // 6. 过滤器配置
    out << "# Filter configuration\n";
    if (!gwlData.filter1Material.isEmpty())
    {
        out << "/Filter/setFilter1Material " << gwlData.filter1Material << "\n";
    }
    if (!gwlData.filter1Thickness.isEmpty())
    {
        double z1 = gwlData.filter1Thickness.toDouble() / 2.0;
        out << "/Filter/setFilter1Z " << z1 << " mm\n";
    }
    if (!gwlData.filter1Position.isEmpty())
    {
        out << "/Filter/Filter1Position " << gwlData.filter1Position << " mm\n";
    }
    if (!gwlData.filter2Material.isEmpty())
    {
        out << "/Filter/setFilter2Material " << gwlData.filter2Material << "\n";
    }
    if (!gwlData.filter2Thickness.isEmpty())
    {
        double z2 = gwlData.filter2Thickness.toDouble() / 2.0;
        out << "/Filter/setFilter2Z " << z2 << " mm\n";
    }
    if (!gwlData.filter2Position.isEmpty())
    {
        out << "/Filter/Filter2Position " << gwlData.filter2Position << " mm\n";
    }
    out << "\n";

    // 7. 运行命令
    out << "# Run simulation\n";
    out << "/run/initialize\n";

    // 8. 可视化命令 (在 /run/beamOn 之后添加)
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

    visMacFile.close();
    return true;
}
