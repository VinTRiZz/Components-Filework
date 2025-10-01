#include "configparser.hpp"

#include <cstring>
#include <iostream>

namespace Filework
{

ConfigParser::ConfigParser(const std::string& filePath) : configFile{filePath}
{
    if (!configFile.is_open())
    {
        std::cout << "-------------------------------------------" << std::endl;
        std::cout << "-------------------------------------------" << std::endl;
        std::cout << "Error opening config file with path: " << filePath
                  << std::endl;
        std::cout << "Error text: " << strerror(errno) << std::endl;
        std::cout << "-------------------------------------------" << std::endl;
        std::cout << "-------------------------------------------" << std::endl;

        exit(-1);
    }

    parseConfig();
}

std::string ConfigParser::configValue(const std::string& key)
{
    auto valIt = m_config.find(key);
    if (valIt == m_config.end())
        throw std::invalid_argument(std::string("Not found key in config: [ ") +
                                    key + " ]");
    return valIt.value();
}

void ConfigParser::parseConfig()
{
    char buffer[4096];
    memset(buffer, '\0', 4096);

    configFile.read(buffer, 4096);

    try
    {
        m_config = nlohmann::json::parse(buffer);
    } catch (nlohmann::json::exception& ex)
    {
        std::cout << "Error parsing config: " << ex.what() << std::endl;
        exit(-1);
    }
}

} // namespace Libraries
