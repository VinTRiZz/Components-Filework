#pragma once

#include <string>
#include <vector>

#ifdef QT_CORE_LIB
#include <QString>
#include <QFile>
#endif // QT_CORE_LIB

namespace Filework
{

namespace Common
{
std::vector<std::string> getContentPaths(const std::string& dirPath);
std::vector<std::string> getContentNames(const std::string& dirPath, const std::string nameFilterRegexp = ".*");

bool objectExist(const std::string& objectPath);
bool contains(const std::string& dirPath, const std::string& fileName);

int16_t objectCount(const std::string& dirPath);
int16_t filesCount(const std::string& dirPath);
int16_t dirsCount(const std::string& dirPath);

bool readFileData(const std::string& filePath, std::string& output);
bool replaceFileData(const std::string& filePath, const std::string& dataString);
bool appendFileData(const std::string& filePath, const std::string& dataString);

bool copyFile(const std::string& sourcePath, const std::string& targetPath, bool replaceExisting = true);

}

}
