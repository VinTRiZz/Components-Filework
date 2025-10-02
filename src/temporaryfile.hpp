#pragma once

#include <string>
#include <fstream>

namespace Filework
{

class TemporaryFile
{
    TemporaryFile(TemporaryFile&&) = delete;
    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;
    TemporaryFile& operator=(TemporaryFile&&) = delete;

public:
    TemporaryFile(const std::string& targetFile);
    ~TemporaryFile();

    template <typename InputT>
    TemporaryFile& operator <<(InputT&& val) {
        m_file.open(m_filepath, std::ios_base::out);
        m_file << val;
        m_file.close();
        return *this;
    }

    template <typename OutputT>
    void operator >>(OutputT& val) {
        m_file.open(m_filepath, std::ios_base::out);
        m_file >> val;
        m_file.close();
    }

    void accept();

private:
    std::string m_targetFilePath;
    std::string m_filepath;
    std::fstream m_file;
};

}
