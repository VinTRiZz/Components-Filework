#pragma once

#include <string>
#include <memory>
#include <functional>
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

    // Constructor-set of a target has no warranty,
    // that file will be set
    TemporaryFile(const std::string& targetFile) noexcept(false);

    TemporaryFile() noexcept(false);
    ~TemporaryFile();

    /**
     * @brief setTargetFile Set file to save data into on accept()
     * @param targetFile    Target file path
     * @return              true if target exist and writable or creatable
     */
    bool setTargetFile(const std::string& targetFile);

    /**
     * @brief setZerofill   Enable zero-filling (replace all data with zeros before remove)
     * @param isEnabled
     * @note Mostly for security purposes
     */
    void setZerofill(bool isEnabled);

    /**
     * @brief accept Write changes into a target file
     * @throws Same errors as std::filesystem::copy()
     */
    void accept() noexcept(false);

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
     * @param moveDir   Same logic as in std::fstream
     * @return          true if temporary file is correctly inited
     */
    bool seekg(std::size_t pos, const std::ios_base::seekdir &moveDir = std::ios_base::cur);

    /**
     * @brief tellg Get current position of a cursor in a file
     * @return
     */
    std::size_t tellg();

    /**
     * @brief empty Return true if file size is 0
     * @return
     */
    bool empty() const;

    /**
     * @brief size Return size of file
     * @return
     */
    std::size_t size() const;

    /**
     * @brief operator << Same as std::fstream operator
     * @param val
     * @return
     */
    template <typename InputT>
    TemporaryFile& operator <<(InputT&& val) {
        m_file << val;
        return *this;
    }

    /**
     * @brief operator >> Same as std::fstream operator
     * @param val
     * @return
     */
    template <typename OutputT>
    TemporaryFile& operator >>(OutputT& val) {
        m_file >> val;
        return *this;
    }

    /**
     * @brief write     Same as std::fstream write
     * @param input     Input string
     * @param byteCount Byte count to write
     * @return
     */
    TemporaryFile& write(const char* input, const std::streamsize byteCount);

    /**
     * @brief flush     Same as std::fstream flush
     */
    void flush();

    /**
     * @brief readsome  Same as std::fstream readsome
     * @param output    Output array
     * @param byteCount Byte count to read
     * @return
     */
    std::streamsize readsome(char* output, const std::streamsize byteCount);

    /**
     * @brief clearFileData Wipes all data written to file (reopen it using TRUNCATE)
     */
    void clearFileData() noexcept(false);

private:
    std::string     m_targetFilePath    {};
    std::string     m_tmpFilePath       {createRandomString(16)};
    bool            m_isZerofillEnabled {false}; // Security
    std::fstream    m_file;
    std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter; // Deletes file

    // Utility
    static std::string createRandomString(unsigned int stringLength);

    void init() noexcept(false);
    void wipeData();
    void openTmpFile() noexcept(false);
};

}
