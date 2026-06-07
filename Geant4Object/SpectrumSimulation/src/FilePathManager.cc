#include "FilePathManager.hh"
#include "globals.hh"

// 静态成员变量定义
FilePathManager* FilePathManager::fInstance = nullptr;

FilePathManager* FilePathManager::GetInstance()
{
    if (fInstance == nullptr) {
        fInstance = new FilePathManager();
    }
    return fInstance;
}
