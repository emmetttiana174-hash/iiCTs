#ifndef FilePathManager_h
#define FilePathManager_h 1

#include "globals.hh"

class FilePathManager
{
public:
    static FilePathManager *GetInstance();

    void SetInputFilePath(const G4String &path) { fInputFilePath = path; }
    void SetOutputFilePath(const G4String &path) { fOutputFilePath = path; }

    G4String GetInputFilePath() const { return fInputFilePath; }
    G4String GetOutputFilePath() const { return fOutputFilePath; }

private:
    FilePathManager() : fInputFilePath("source.txt"), // 使用相对路径，默认在当前目录查找
                        fOutputFilePath("output")
    {
    }

    static FilePathManager *fInstance;

    G4String fInputFilePath;
    G4String fOutputFilePath;
};

#endif
