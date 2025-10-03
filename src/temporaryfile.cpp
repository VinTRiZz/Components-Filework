#include "temporaryfile.hpp"

#include <filesystem>
#include <algorithm>

#include <random>
#include <Components/Common/Utils.h>

namespace Filework
{

TemporaryFile::TemporaryFile(const std::string &targetFile) :
    m_targetFilePath {targetFile} {
    auto tgtFilePath = std::filesystem::path(m_targetFilePath);

    const bool isNewFile = !std::filesystem::exists(targetFile);
    const bool isCorrectFile = std::filesystem::exists(tgtFilePath.parent_path()) && std::filesystem::is_regular_file(targetFile);

    if (isNewFile ^ !isCorrectFile) {
        throw std::invalid_argument(std::string("Temp file: invalid target file: ") + targetFile);
    }

    m_filepath = Common::createRandomString(10);
    m_file.exceptions(std::ios::failbit);

    if (!isNewFile) {
        std::filesystem::copy_file(targetFile, m_filepath, std::filesystem::copy_options::overwrite_existing);
    }

    try {
        m_file.open(m_filepath, std::ios_base::out);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }

    m_file.close();
}

TemporaryFile::~TemporaryFile() {
    std::filesystem::remove(m_filepath);
}

void TemporaryFile::accept() {
    std::filesystem::copy_file(m_filepath, m_targetFilePath, std::filesystem::copy_options::overwrite_existing);
}

std::string TemporaryFile::path() const
{
    return m_filepath;
}

}
