#ifndef PROJECTINFO_H
#define PROJECTINFO_H

#include <QString>

struct ProjectInfo {
    QString ProjectName = "";
    QString TubeType = "hot";
    QString CTType = "";
    QString TubeFolder = "";
    QString SpectrumFolder = "";
    QString DetectorFolder = "";
    QString CTFolder = "";
    QString ReconFolder = "";
    QString MACFolder = "";

    QString ProjectPath = "/mnt";
    QString SystemScriptPath = "/mnt";
    QString SystemMaterialPath = "";
    QString TubeDatabasePath = "";
    QString SpectrumDatabasePath = "";
};


#endif // PROJECTINFO_H
