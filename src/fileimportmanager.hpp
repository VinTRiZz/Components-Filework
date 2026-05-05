#pragma once

#include <boost/noncopyable.hpp>
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <memory>

namespace Common {

class FileImportManager : public boost::noncopyable
{
    FileImportManager();
public:
    ~FileImportManager();

    enum class FileType : int {
        ErrorFile,
        RegularFile,
        ImageFile,
    };

    struct FileInfo {
        std::string hash;
        std::string filePath;
        FileType type;
        size_t size;
        std::chrono::system_clock::time_point lastAccess;
        std::chrono::system_clock::time_point creationTime;
    };

    static FileImportManager& getInstance();

    void updateCache();
    void clearCache();

    void setRootDirectory(const std::string& rDir);
    void setFileCacheSize(unsigned cacheFileCount);
    void setMaxCacheSizeMb(unsigned maxSizeMb);

    void setFileTypeDetector(const std::function<FileType(const std::string&)>& ftypeDetector);
    void setHashCalculator(const std::function<std::string(const std::string&)>& hashCalculator);

    std::string addToCache(const std::string& targetPath, bool forceUpdate = false);
    void removeFromCache(const std::string& targetPath) const;

    bool isCached(const std::string& targetPath) const;
    size_t getCacheSize() const;
    size_t getCacheFileCount() const;
    std::vector<std::string> getCachedFiles() const;
    FileInfo getFileInfo(const std::string& targetPath) const;
    bool validateFile(const std::string& targetPath) const;
    void cleanupExpiredFiles();
    void preloadDirectory(const std::string& directory);
    std::string getFileHash(const std::string& targetPath) const;

private:
    void cleanupCache();
    void updateFileAccessTime(const std::string& targetPath);
    bool copyFileToCache(const std::string& sourcePath, const std::string& hash);

    unsigned m_fileCacheSize {50};
    unsigned m_maxFileCacheSizeMb {100};

    std::string m_rootDirectory;
    std::function<FileType(const std::string&)> m_fileTypeDetector;
    std::function<std::string(const std::string&)> m_hashCalculator;

    mutable std::unordered_map<std::string, FileInfo> m_lastUsedFiles;
    mutable std::unordered_map<std::string, std::string> m_filePathCache;
    mutable std::unordered_map<std::string, std::string> m_filesCache;
};

}
