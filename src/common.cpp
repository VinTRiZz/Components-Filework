#include "common.hpp"

#include <Components/Logger/Logger.h>

#include <algorithm>
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
    const uint16_t BUFFER_SIZE = 128;
    output.clear();

    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytesRead;

    // Open the file in read mode
    file = fopen(filePath.c_str(), "rb");
    if (file == NULL) {
        LOG_ERROR("Failed to open file:", filePath);
        return false;
    }

    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        output.reserve(bytesRead);
        std::copy_n(buffer, bytesRead, std::back_inserter(output));
    }

    if (!output.size())
        return false;

    // Close the file
    fclose(file);
    return true;
}

bool replaceFileData(const std::string &filePath, const std::string &dataString)
{
    /// C++ Code
//    std::fstream cardFile(filePath, std::ios_base::trunc | std::ios_base::out);
//    if (!cardFile.is_open())
//    {
//        LOG_WARNING("Error writing to file by path:", filePath.c_str(), strerror(errno));
//        return false;
//    }

//    cardFile << dataString;
//    cardFile.close();
//    return true;

    /// C CODE (ADDED ONLY IN CASE OF MANY MEMORY ALLOCATIONS)
    // Declare a FILE pointer
    FILE *fp;

    // Open using append flag
    fp = fopen(filePath.c_str(), "w");

    // Check if the file opened successfully
    if (fp == NULL) {
        LOG_ERROR("Error opening file", filePath);
        return false;
    }

    // You can use fputs or fwrite as well
    fputs(dataString.c_str(), fp);

    // Close the file
    if (fclose(fp) != 0) {
        LOG_ERROR("Error closing file", filePath);
        return false;
    }
    return true;
}

bool appendFileData(const std::string &filePath, const std::string &dataString)
{
    /// C++ CODE
//    std::fstream cardFile(filePath, std::ios_base::app | std::ios_base::out);
//    if (!cardFile.is_open())
//    {
//        LOG_WARNING("Error writing to file by path:", filePath.c_str(), strerror(errno));
//        return false;
//    }

//    cardFile << dataString;
//    cardFile.close();
//    return true;

    /// C CODE (ADDED ONLY IN CASE OF MANY MEMORY ALLOCATIONS)
    // Declare a FILE pointer
    FILE *fp;

    // Open using append flag
    fp = fopen(filePath.c_str(), "a");

    // Check if the file opened successfully
    if (fp == NULL) {
        LOG_ERROR("Error opening file", filePath);
        return false;
    }

    // You can use fputs or fwrite as well
    fputs(dataString.c_str(), fp);

    // Close the file
    if (fclose(fp) != 0) {
        LOG_ERROR("Error closing file", filePath);
        return false;
    }
    return true;
}

}
