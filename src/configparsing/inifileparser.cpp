#include "inifileparser.hpp"

#include <filesystem>
#include <fstream>

#include <Components/Filework/TemporaryFile.h>
#include <Components/Filework/Common.h>

namespace Filework {

using sectionValues_t = std::map<std::string, std::string>;

struct IniFileParser::Impl
{
    sectionValues_t                         globalValues;
    std::map<std::string, sectionValues_t>  configData;
    std::string                             lastError;
};

IniFileParser::IniFileParser() :
    d {new Impl}
{

}

IniFileParser::~IniFileParser()
{

}

bool IniFileParser::read(const std::string_view &filePath, bool ignoreInvalidValues)
{
    if (!std::filesystem::exists(filePath)) {
        d->lastError = "File not exist";
        return false;
    }

    std::string fileData;
    if (!Common::readFileData(filePath.data(), fileData)) {
        d->lastError = "Failed to read data from a file";
        return false;
    }

    d->globalValues.clear();
    d->configData.clear();

    // Get global section data


    // TODO: Use ignoreErrors

    return true;
}

bool IniFileParser::write(const std::string_view &filePath)
{
    auto targetFile = std::filesystem::path(filePath);
    auto subdir = targetFile.parent_path();
    if (!std::filesystem::exists(subdir)) {
        d->lastError = "File directory not exist";
        return false;
    }

    TemporaryFile tmpf(filePath.data());

    return true;
}

std::string IniFileParser::getLastErrorText() const
{
    return d->lastError;
}

std::vector<std::string> IniFileParser::getSections() const
{
    std::vector<std::string> res;
    res.reserve(d->configData.size());
    for (auto& sect : d->configData) {
        res.push_back(sect.first);
    }
    return res;
}

void IniFileParser::addSection(const std::string &sectionName, const std::map<std::string, std::string> &values)
{
    d->configData[sectionName] = values;
}

void IniFileParser::addSection(const std::string &sectionName, std::map<std::string, std::string> &&values)
{
    d->configData[sectionName] = std::move(values);
}

std::map<std::string, std::string> IniFileParser::getSection(const std::string &sectionName) const
{
    if (sectionName.empty()) {
        return d->globalValues;
    }
    auto targetSection = d->configData.find(sectionName);
    if (targetSection != d->configData.end()) {
        return {};
    }
    return targetSection->second;
}

} // namespace Filework
