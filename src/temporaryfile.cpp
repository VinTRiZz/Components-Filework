#include "temporaryfile.hpp"

#include <filesystem>
#include <random>

namespace Filework
{

std::string createRandomString(unsigned int stringLength) {
    std::random_device rd;
    std::mt19937 gen(rd());

    const std::string characters =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::uniform_int_distribution<> dis(0, characters.size() - 1);

    std::string result;
    result.reserve(stringLength);
    for (unsigned int i = 0; i < stringLength; ++i)
        result += characters[dis(gen)];

    return result;
}

TemporaryFile::TemporaryFile(const std::string &targetFile) noexcept(false) :
    m_targetFilePath {targetFile} {
    auto tgtFilePath = std::filesystem::path(m_targetFilePath);

    const bool isNewFile = !std::filesystem::exists(targetFile);
    const bool isCorrectFile = std::filesystem::exists(tgtFilePath.parent_path()) && std::filesystem::is_regular_file(targetFile);

    if (isNewFile ^ !isCorrectFile) {
        throw std::invalid_argument(std::string("Temp file: invalid target file: ") + targetFile);
    }

    m_filepath = createRandomString(10);
    m_file.exceptions(std::ios::failbit);

    try {
        m_file.open(m_filepath, std::ios_base::in | std::ios_base::out | std::ios_base::app);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }
}

TemporaryFile::~TemporaryFile() {
    m_file.close();
    std::filesystem::remove(m_filepath);
}

std::string_view TemporaryFile::getFilename() const
{
    return m_filepath;
}

std::string_view TemporaryFile::getTargetFilename() const
{
    return m_targetFilePath;
}

void TemporaryFile::seekg(std::size_t pos)
{
    m_file.seekg(pos);
}

std::size_t TemporaryFile::tellg()
{
    return m_file.tellg();
}

void TemporaryFile::accept() noexcept(false) {
    std::filesystem::copy_file(m_filepath, m_targetFilePath, std::filesystem::copy_options::overwrite_existing);
}

}
