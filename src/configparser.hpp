#pragma once

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Filework
{

class ConfigParser
{
  public:
    ConfigParser(const std::string& filePath);

    std::string configValue(const std::string& key);

  private:
    std::fstream configFile;
    nlohmann::json m_config;

    void parseConfig();
};

static std::shared_ptr<ConfigParser> configP;

} // namespace Libraries
