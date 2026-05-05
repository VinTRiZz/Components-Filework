#include "fileimportmanager.hpp"

#include <filesystem>
#include <fstream>
#include <algorithm>

#include <Components/Logger/Logger.h>

namespace Common {

FileImportManager::FileImportManager()
{
    m_fileTypeDetector = [](const std::string&) -> FileType {
        return FileType::RegularFile;
    };

    m_hashCalculator = [](const std::string& iString) -> std::string {
        return iString;
    };
}

FileImportManager::~FileImportManager()
{

}

FileImportManager &FileImportManager::getInstance()
{
    static FileImportManager inst;
    return inst;
}

void FileImportManager::updateCache()
{
    for (auto& dirEntr : std::filesystem::directory_iterator(m_rootDirectory)) {
        addToCache(std::filesystem::path(dirEntr).generic_string());
    }
    COMPLOG_OK("Cache updated. Found files:", getCacheFileCount());
}

void FileImportManager::clearCache()
{
    for (const auto& file : std::filesystem::directory_iterator(m_rootDirectory)) {
        if (file.is_regular_file()) {
            std::filesystem::remove(file.path());
        }
    }

    m_lastUsedFiles.clear();
    m_filePathCache.clear();
    m_filesCache.clear();
    COMPLOG_INFO("Cache cleared");
}

void FileImportManager::setRootDirectory(const std::string &rDir)
{
    m_rootDirectory = rDir;
    if (!std::filesystem::exists(m_rootDirectory)) {
        throw std::invalid_argument("Invalid directory to be root for file import manager");
    }
    clearCache();
}

void FileImportManager::setFileCacheSize(unsigned int cacheFileCount)
{
    m_fileCacheSize = cacheFileCount;
    cleanupCache();
}

void FileImportManager::setMaxCacheSizeMb(unsigned int maxSizeMb)
{
    m_maxFileCacheSizeMb = maxSizeMb;
    cleanupCache();
}

void FileImportManager::setFileTypeDetector(const std::function<FileType (const std::string &)> &ftypeDetector)
{
    if (ftypeDetector) {
        m_fileTypeDetector = ftypeDetector;
    }
}

void FileImportManager::setHashCalculator(const std::function<std::string (const std::string &)> &hashCalculator)
{
    m_hashCalculator = hashCalculator;
}

std::string FileImportManager::addToCache(const std::string &targetPath, bool forceUpdate)
{
    if (!forceUpdate) [[likely]] {
        if (m_filePathCache.count(targetPath)) {
            updateFileAccessTime(targetPath);
            return m_filePathCache.at(targetPath);
        }
    }

    if (!std::filesystem::exists(targetPath)) {
        return "";
    }

    std::string hash = targetPath;

    auto copyDir = std::filesystem::path(targetPath).parent_path();
    if (copyDir.generic_string() != m_rootDirectory) {
        hash = m_hashCalculator(targetPath);
        if (!copyFileToCache(targetPath, hash)) {
            return "";
        }
    }

    FileInfo info;
    info.hash = hash;
    info.filePath = targetPath;
    info.type = m_fileTypeDetector(targetPath);
    info.size = std::filesystem::file_size(targetPath);
    info.lastAccess = std::chrono::system_clock::now();
    info.creationTime = std::chrono::system_clock::now();

    m_filePathCache[targetPath] = hash;
    m_filesCache[hash] = targetPath;
    m_lastUsedFiles[targetPath] = info;

    COMPLOG_OK("Added file to cache:", targetPath);
    cleanupCache();
    return hash;
}

void FileImportManager::removeFromCache(const std::string &targetPath) const
{
    auto it = m_filePathCache.find(targetPath);
    if (it != m_filePathCache.end()) {
        std::string hash = it->second;
        std::filesystem::remove(m_rootDirectory + "/" + hash);
        m_filePathCache.erase(it);
        m_filesCache.erase(hash);
        m_lastUsedFiles.erase(targetPath);
        COMPLOG_OK("File removed from cache:", targetPath);
    }
}

bool FileImportManager::isCached(const std::string &targetPath) const
{
    return m_filePathCache.count(targetPath) > 0;
}

size_t FileImportManager::getCacheSize() const
{
    size_t totalSize = 0;
    for (const auto& file : std::filesystem::directory_iterator(m_rootDirectory)) {
        if (file.is_regular_file()) {
            totalSize += file.file_size();
        }
    }
    return totalSize;
}

size_t FileImportManager::getCacheFileCount() const
{
    return m_filePathCache.size();
}

std::vector<std::string> FileImportManager::getCachedFiles() const
{
    std::vector<std::string> files;
    for (const auto& item : m_filePathCache) {
        files.push_back(item.first);
    }
    return files;
}

FileImportManager::FileInfo FileImportManager::getFileInfo(const std::string &targetPath) const
{
    auto it = m_lastUsedFiles.find(targetPath);
    if (it != m_lastUsedFiles.end()) {
        return it->second;
    }
    return FileInfo{};
}

bool FileImportManager::validateFile(const std::string &targetPath) const
{
    if (!isCached(targetPath)) {
        return false;
    }

    std::string currentHash = m_hashCalculator(targetPath);
    std::string cachedHash = m_filePathCache.at(targetPath);

    return currentHash == cachedHash;
}

void FileImportManager::cleanupExpiredFiles()
{
    auto now = std::chrono::system_clock::now();
    auto expiryTime = std::chrono::hours(24 * 7);

    for (auto it = m_lastUsedFiles.begin(); it != m_lastUsedFiles.end(); ) {
        if (now - it->second.lastAccess > expiryTime) {
            removeFromCache(it->first);
            it = m_lastUsedFiles.erase(it);
        } else {
            ++it;
        }
    }
    COMPLOG_OK("Expired files removed");
}

void FileImportManager::preloadDirectory(const std::string &directory)
{
    COMPLOG_INFO("Preloading directory");
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            addToCache(entry.path().generic_string());
        }
    }
    COMPLOG_OK("Directory preloaded");
}

std::string FileImportManager::getFileHash(const std::string &targetPath) const
{
    auto it = m_filePathCache.find(targetPath);
    if (it != m_filePathCache.end()) {
        return it->second;
    }
    return "";
}

void FileImportManager::cleanupCache()
{
    COMPLOG_INFO("Removing old cached files");
    while (m_filePathCache.size() > m_fileCacheSize) {
        auto oldest = std::min_element(m_lastUsedFiles.begin(), m_lastUsedFiles.end(),
            [](const auto& a, const auto& b) {
                return a.second.lastAccess < b.second.lastAccess;
            });

        if (oldest != m_lastUsedFiles.end()) {
            removeFromCache(oldest->first);
            m_lastUsedFiles.erase(oldest);
        }
    }

    COMPLOG_INFO("Removing small cached files");
    size_t currentSizeMb = getCacheSize() / (1024 * 1024);
    while (currentSizeMb > m_maxFileCacheSizeMb) {
        auto smallest = std::min_element(m_lastUsedFiles.begin(), m_lastUsedFiles.end(),
            [](const auto& a, const auto& b) {
                return a.second.size < b.second.size;
            });

        if (smallest != m_lastUsedFiles.end()) {
            currentSizeMb -= smallest->second.size / (1024 * 1024);
            removeFromCache(smallest->first);
            m_lastUsedFiles.erase(smallest);
        }
    }
    COMPLOG_INFO("Cache cleaned up");
}

void FileImportManager::updateFileAccessTime(const std::string &targetPath)
{
    auto it = m_lastUsedFiles.find(targetPath);
    if (it != m_lastUsedFiles.end()) {
        it->second.lastAccess = std::chrono::system_clock::now();
    }
}

bool FileImportManager::copyFileToCache(const std::string &sourcePath, const std::string &hash)
{
    try {
        auto copyPath = m_rootDirectory + "/" + hash;
        if (sourcePath == copyPath) {
            return true; // Is from cache already
        }
        std::filesystem::copy_file(sourcePath, copyPath,
                                 std::filesystem::copy_options::overwrite_existing);
        return true;
    } catch (std::exception& ex) {
        COMPLOG_ERROR("Failed to copy file into cache:", std::string(ex.what()));
        COMPLOG_ERROR("File path:", sourcePath);
        return false;
    }
}

}
