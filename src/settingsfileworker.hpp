#pragma once

#include <string>
#include <Components/ExtraClasses/JOptional.h>

namespace Filework
{

class SettingsFileWorker
{
  public:
    SettingsFileWorker();

  private:
    std::string m_settingFilesDir;

  protected:
    virtual void setSetting(const std::string& fileName, int64_t value) final;
    virtual void setSetting(const std::string& fileName, const std::string& value) final;
    virtual ExtraClasses::JOptional<std::string> getSetting(const std::string& fileName) const final;

    virtual void setSettingsDir(const std::string& dirPath) final;
    virtual std::string& getSettingsDir() final;
};

} // namespace Libraries
