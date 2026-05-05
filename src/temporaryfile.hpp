#pragma once

#include <string>
#include <fstream>

namespace Filework
{

/**
 * @brief The TemporaryFile class   Simple temporary file handling object, temp file removed in destructor
 */
class TemporaryFile
{
    TemporaryFile(TemporaryFile&&) = delete;
    TemporaryFile(const TemporaryFile&) = delete;
    TemporaryFile& operator=(const TemporaryFile&) = delete;
    TemporaryFile& operator=(TemporaryFile&&) = delete;

public:

    /**
     * @brief TemporaryFile
     * @param targetFile    Path to a file for writing or overwriting
     * @throws  std::invalid_argument   - if targetFile is invalid path;
     *          std::runtime_error      - if failed to open tmp file
     */
    TemporaryFile(const std::string& targetFile) noexcept(false);
    ~TemporaryFile();

    /**
     * @brief getFilename   Get path to a temporary file
     * @return
     */
    std::string_view getFilename() const;

    /**
     * @brief getTargetFilename Get path to a target file
     * @return
     */
    std::string_view getTargetFilename() const;

    /**
     * @brief seekg Set current position of a cursor in file
     * @param pos
     */
    void seekg(std::size_t pos);

    /**
     * @brief tellg Get current position of a cursor in a file
     * @return
     */
    std::size_t tellg();

    template <typename InputT>
    TemporaryFile& operator <<(InputT&& val) {
        m_file << val;
        m_file.flush();
        return *this;
    }

    template <typename OutputT>
    void operator >>(OutputT& val) {
        m_file >> val;
        m_file.flush();
    }

    /**
     * @brief accept Write changes into a target file
     * @throws Same errors as std::filesystem::copy()
     */
    void accept() noexcept(false);

private:
    std::string m_targetFilePath;
    std::string m_filepath;
    std::fstream m_file;
};

}
