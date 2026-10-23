#include "temporaryfile.hpp"

#include <filesystem>
#include <random>

namespace Filework
{

static const auto OPENFLAGS {std::ios_base::trunc | std::ios_base::in | std::ios_base::out | std::ios_base::binary};

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

TemporaryFile::TemporaryFile(const std::string &targetFile) noexcept(false)
{
    init();
    setTargetFile(targetFile);
}

TemporaryFile::TemporaryFile() noexcept(false) {
    init();
}

TemporaryFile::~TemporaryFile() {
    m_file.close();
    std::filesystem::remove(m_filepath);
}

bool TemporaryFile::setTargetFile(const std::string &targetFile)
{
    m_targetFilePath = targetFile;
    if (m_targetFilePath.empty()) {
        // Reset file path is allowed
        clearFileData();
        return true;
    }
    auto tgtFilePath = std::filesystem::path(m_targetFilePath);

    const bool isNewFile = !std::filesystem::exists(tgtFilePath);
    const bool isDirectory = std::filesystem::is_directory(tgtFilePath);

    auto expectedRights = (std::filesystem::perms::owner_read | std::filesystem::perms::owner_write);
    const bool isEditable = expectedRights == (expectedRights & std::filesystem::status(tgtFilePath).permissions());

    if (isDirectory || (!isNewFile && !isEditable)) {
        throw std::runtime_error(
            std::string("Invalid target file:") +
                (isNewFile ? " new" : " existing") +
                (isEditable ? " ReadWrite" : " not ReadWrite") +
                (isDirectory ? " directory" : " file")
            );
    }
    m_file.close();
    if (std::filesystem::exists(m_targetFilePath)) {
        std::filesystem::copy_file(m_targetFilePath, m_filepath, std::filesystem::copy_options::overwrite_existing);
    }
    try {
        m_file.open(m_filepath, std::ios_base::in | std::ios_base::out);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }
    return true;
}

void TemporaryFile::accept() noexcept(false) {
    if (m_targetFilePath.empty()) { return; }
    std::filesystem::copy_file(m_filepath, m_targetFilePath, std::filesystem::copy_options::overwrite_existing);
}

std::string_view TemporaryFile::getFilename() const
{
    return m_filepath;
}

std::string_view TemporaryFile::getTargetFilename() const
{
    return m_targetFilePath;
}

bool TemporaryFile::seekg(std::size_t pos, const std::ios_base::seekdir& moveDir)
{
    if (!m_file.is_open()) {
        return false;
    }
    m_file.seekg(pos, moveDir);
    return true;
}

std::size_t TemporaryFile::tellg()
{
    return m_file.tellg();
}

bool TemporaryFile::empty() const
{
    return (0 == size());
}

std::size_t TemporaryFile::size() const
{
    return std::filesystem::file_size(m_filepath);
}

void TemporaryFile::clearFileData() noexcept(false)
{
    m_file.close();
    try {
        m_file.open(m_filepath, OPENFLAGS);
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }
}

void TemporaryFile::init() noexcept(false)
{
    m_fileDeleter = {reinterpret_cast<bool*>(123),
        [this](auto*) {
            if (std::filesystem::exists(m_filepath)) {
                std::filesystem::remove(m_filepath);
            }
        }
    };

    m_filepath = createRandomString(10);
    m_file.exceptions(std::ios::failbit);

    try {
        if (std::filesystem::exists(m_filepath)) {
            m_file.open(m_filepath, OPENFLAGS & ~std::ios_base::trunc);
        } else {
            m_file.open(m_filepath, OPENFLAGS);
        }
    } catch (const std::ios_base::failure& e) {
        throw std::runtime_error(std::string("Temporary file open error: ") + e.what());
    }
}

}
