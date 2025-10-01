#include "settingsfileworker.hpp"

#include "common.hpp"

#include <Components/Logger/Logger.h>

#include <boost/algorithm/string.hpp>

namespace Filework
{

SettingsFileWorker::SettingsFileWorker()
{
}

void SettingsFileWorker::setSetting(const std::string &fileName, int64_t value)
{
    if (!Common::replaceFileData(m_settingFilesDir + "/" + fileName, std::to_string(value))) {
        LOG_WARNING("Replace data failed for filename:", fileName);
    }
}


void SettingsFileWorker::setSetting(const std::string &fileName, const std::string &value)
{
    if (!Common::replaceFileData(m_settingFilesDir + "/" + fileName, value)) {
        LOG_WARNING("Replace data failed for filename:", fileName);
    }
}

ExtraClasses::JOptional<std::string> SettingsFileWorker::getSetting(const std::string &fileName) const
{
    std::string result;
    if (!Common::readFileData(m_settingFilesDir + "/" + fileName, result)) {
        LOG_WARNING("Read data failed for filename:", fileName);
        return {};
    }
    boost::algorithm::trim_left(result);
    boost::algorithm::trim_right(result);
    return result;
}

void SettingsFileWorker::setSettingsDir(const std::string &dirPath)
{
    m_settingFilesDir = dirPath;
}

std::string &SettingsFileWorker::getSettingsDir()
{
    return m_settingFilesDir;
}

} // namespace Libraries
