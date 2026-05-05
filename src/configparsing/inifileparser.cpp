#include "inifileparser.hpp"

#include <filesystem>
#include <algorithm>
#include <list>

#if __cplusplus >= 202002l
#include <ranges>
#endif // C++20

#include <Components/Filework/TemporaryFile.h>
#include <Components/Filework/Common.h>



#include <iostream>



namespace Filework {

// Utility
std::string_view trimView(std::string_view sv) {
    // Left
    sv.remove_prefix(std::min(sv.find_first_not_of(" \t\n\r\f\v"), sv.size()));

    // Right
    auto last = sv.find_last_not_of(" \t\n\r\f\v");
    if (last != std::string_view::npos) {
        sv.remove_suffix(sv.size() - last - 1);
    }

    return sv;
}


using sectionValues_t = std::map<std::string, std::string>;

struct IniFileParser::Impl
{
    std::map<std::string, std::string>      settingComments;
    std::map<std::string, sectionValues_t>  configData;
    std::string                             lastError;

    void setErrorText(
        const std::string_view& filePath,
        std::size_t lineNo,
        const std::string& errorText,
        const std::string_view& errorContext) {

        lastError = filePath;
        lastError += ":";
        lastError += std::to_string(lineNo) + " ";
        lastError += errorText + " : \"";
        lastError += errorContext;
        lastError += "\"";
    }
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

    d->configData.clear();

#if __cplusplus >= 202002l
    auto lines = fileData
                 | std::views::split('\n');
#else
    std::list<std::string_view> lines;
    {
        std::size_t begPos {};
        std::size_t endPos {};
        for (auto c : fileData) {
            if (c != '\n') {
                ++endPos;
                continue;
            }
            lines.emplace_back(std::string_view(fileData.data() + begPos, endPos - begPos));
            begPos = endPos + 1;
            ++endPos;
        }
    }
#endif // C++20

    // Parsing cases:
    // 1) '[abc]'   --> Section
    // 2) ; abc     --> Comment (skip all up to newline)
    // 3) 'a=value' --> Values (even global)
    // 4) 'a='      --> NULL value

    // Parse lines
    std::size_t lineNo {};
    std::string currentSection;
    for (auto&& l : lines) {
        ++lineNo;
        if (l.size() < 3) { // 3 is MINIMAL length (like '[a]' or 'a=1')
            continue;
        }

        l = trimView(l); // Remove spaces

        std::size_t objectStartPos {std::numeric_limits<std::size_t>::max()};
        std::size_t objectEndPos {};
        std::size_t commentStartPos {l.size()};
        for (auto c : l) {
            if ((c == ';') | (c == '#')) {
                commentStartPos = objectEndPos - (objectEndPos > 0);
                break; // Comments are skipped ones
            }

            if (objectStartPos == std::numeric_limits<std::size_t>::max() && !std::isspace(c)) {
                objectStartPos = objectEndPos;
            }
            ++objectEndPos;
        }
        if (objectEndPos < objectStartPos) { // Case 2: Comments are totally ignored
            continue;
        }
        auto object = trimView({l.data() + objectStartPos, objectEndPos - objectStartPos});

        // Case 1: Process section
        if (object.at(objectStartPos) == '[') {
            if (object.find_first_of(']') != (object.size() - 1)) {
                if (!ignoreInvalidValues) {
                    d->setErrorText(filePath, lineNo, "Syntax error (invalid braces)", object);
                    return false;
                }
                continue;
            }
            if (object.size() < 3) {
                if (!ignoreInvalidValues) {
                    d->setErrorText(filePath, lineNo, "Empty section name", object);
                    return false;
                }
                continue;
            }
            currentSection = trimView({object.data() + 1, object.size() - 2});
            continue;
        }

        // Case 3: Process value
        auto equalSignPos = object.find_first_of('=');
        if (!ignoreInvalidValues && (equalSignPos == objectEndPos || equalSignPos == objectStartPos || equalSignPos == std::string::npos)) { // Invalid value
            d->setErrorText(filePath, lineNo, "Value syntax error", l);
            return false;
        }

        // Get and check name
        auto settingName = trimView({l.data() + objectStartPos, equalSignPos - objectStartPos});
        bool isNameValid {false};
        for (auto& c : settingName) {
            isNameValid = std::isalnum(c) | std::isalpha(c) | (c == '_') | (c == '-');
            if (!isNameValid) {
                break;
            }
        }
        if (!isNameValid) {
            if (!ignoreInvalidValues) {
                d->setErrorText(filePath, lineNo, "Invalid variable name", l);
                return false;
            }
            continue;
        }

        // Get value
        auto settingValue = trimView({l.data() + equalSignPos + 1, commentStartPos - equalSignPos - 1});
        d->configData[currentSection][settingName.data()] = settingValue.data();

        // Add comment
        if (commentStartPos != l.size()) {
            auto comment = trimView({l.data() + commentStartPos, l.size() - commentStartPos});
            comment = trimView({comment.data() + 1, comment.size() - 1});
            d->settingComments[currentSection + ":" + settingName.data()] = comment;
        }
    }

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
    auto targetSection = d->configData.find(sectionName);
    if (targetSection != d->configData.end()) {
        return {};
    }
    return targetSection->second;
}

std::string IniFileParser::getSettingComment(const std::string &setting, const std::string &section) const
{
    auto targetSection = d->settingComments.find(section + ":" + setting);
    if (targetSection != d->settingComments.end()) {
        return {};
    }
    return targetSection->second;
}

} // namespace Filework
