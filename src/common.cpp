#include "common.hpp"

#include <Components/Logger/Logger.h>

#include <cstring>
#include <regex>

#if (__cplusplus >= 201701UL)
#include <filesystem>
namespace stdfs = std::filesystem;
#else
#include <experimental/filesystem>
namespace stdfs = std::experimental::filesystem;
#endif // C++17

namespace Filework::Common
{

std::vector<std::string> getContentNames(const std::string &dirPath, const std::string nameFilterRegexp)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return {};

    std::regex filterRegexp(nameFilterRegexp, std::regex::flag_type::_S_ECMAScript);

    std::vector<std::string> result;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt) {
        std::string fileName = entry.path().filename();
        if (std::regex_search(fileName, filterRegexp)) {
            result.push_back(fileName);
        }
    }
    return result;
}

std::vector<std::string> getContentPaths(const std::string &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return {};

    std::vector<std::string> result;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
        result.push_back(entry.path());
    return result;
}

bool objectExist(const std::string &objectPath)
{
    return stdfs::exists(objectPath);
}

bool contains(const std::string &dirPath, const std::string &fileName)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return false;

    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
    {
        if (entry.path().filename() == fileName)
            return true;
    }
    return false;
}

int16_t objectCount(const std::string &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t objCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
    {
        objCount++;
    }
    return objCount;
}

int16_t filesCount(const std::string &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t fileCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
    {
        if (stdfs::is_regular_file(entry))
            fileCount++;
    }
    return fileCount;
}

int16_t dirsCount(const std::string &dirPath)
{
    if (!stdfs::exists(dirPath) || !stdfs::is_directory(dirPath))
        return 0;

    int16_t dirCount = 0;
    auto dirIt = stdfs::directory_iterator(dirPath);
    for (const stdfs::directory_entry& entry : dirIt)
    {
        if (stdfs::is_directory(entry))
            dirCount++;
    }
    return dirCount;
}

bool readFileData(const std::string &filePath, std::string &output)
{
    output.clear();
    output.reserve(std::filesystem::file_size(filePath));

    std::ifstream ifile;
    ifile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    ifile.open(filePath, std::ios_base::in | std::ios_base::binary);
    if (!ifile.is_open()) {
        return false;
    }

    const uint16_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    size_t bytesRead = ifile.readsome(buffer, BUFFER_SIZE);
    output.append(buffer, bytesRead);
    while (bytesRead > 0) {
        bytesRead = ifile.readsome(buffer, BUFFER_SIZE);
        output.append(buffer, bytesRead);
    }
    return true;
}

bool saveFileData(const std::string &filePath, const std::string &input)
{
    std::ofstream ofile;
    ofile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    ofile.open(filePath, std::ios_base::out | std::ios_base::binary);
    if (!ofile.is_open()) {
        return false;
    }
    ofile << input;
    ofile.flush();
    return true;
}

bool replaceFileData(const std::string &filePath, const std::string &dataString)
{
   std::fstream cardFile(filePath, std::ios_base::trunc | std::ios_base::out);
   if (!cardFile.is_open()) {
       return false;
   }

   cardFile << dataString;
   cardFile.close();
   return true;
}

bool appendFileData(const std::string &filePath, const std::string &dataString)
{
   std::fstream cardFile(filePath, std::ios_base::app | std::ios_base::out);
   if (!cardFile.is_open()) {
       return false;
   }

   cardFile << dataString;
   cardFile.close();
   return true;
}

}
