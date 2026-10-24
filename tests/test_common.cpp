#include <gtest/gtest.h>

#include <Components/Filework/Common.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

class Components_FileworkCommon : public ::testing::Test {
protected:
    const std::filesystem::path testDir = "filework_test_dir";

    void SetUp() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
        std::filesystem::create_directories(testDir);
    }

    void TearDown() override {
        if (std::filesystem::exists(testDir)) {
            std::filesystem::remove_all(testDir);
        }
    }

    std::string path(const std::string& name) const {
        return testDir / name;
    }

    void createFile(const std::string& name, const std::string& content = "") {
        std::ofstream f(path(name));
        f << content;
    }

    void createDir(const std::string& name) {
        std::filesystem::create_directories(path(name));
    }
};

// ---------------- Filework::Common::objectExist / Filework::Common::contains ----------------

TEST_F(Components_FileworkCommon, ObjectExist_FileAndDirAndMissing) {
    createFile("a.txt");
    createDir("subdir");

    ASSERT_TRUE(Filework::Common::objectExist(path("a.txt")));
    ASSERT_TRUE(Filework::Common::objectExist(path("subdir")));
    ASSERT_FALSE(Filework::Common::objectExist(path("missing.txt")));
}

TEST_F(Components_FileworkCommon, Contains_PresentAndAbsent) {
    createFile("a.txt");
    ASSERT_TRUE(Filework::Common::contains(testDir, "a.txt"));
    ASSERT_FALSE(Filework::Common::contains(testDir, "b.txt"));
}

// ---------------- Counts ----------------

TEST_F(Components_FileworkCommon, ObjectCount_MixedContent) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createFile("d.txt");
    createFile("e.txt");
    createDir("dir1");
    createDir("dir2");
    createDir("dir3");

    // 5 files, 3 dirs. In count error must be 10, 6, or 4
    ASSERT_EQ(static_cast<int>(Filework::Common::objectCount(testDir)), 8);
}

TEST_F(Components_FileworkCommon, ObjectCount_EmptyDir) {
    ASSERT_EQ(static_cast<int>(Filework::Common::objectCount(testDir)), 0);
}

TEST_F(Components_FileworkCommon, FilesCount_OnlyRegularFiles) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createFile("d.txt");
    createFile("e.txt");
    createDir("subdir1");
    createDir("subdir2");
    ASSERT_EQ(static_cast<int>(Filework::Common::filesCount(testDir)), 5);
}

TEST_F(Components_FileworkCommon, DirsCount_OnlyDirs) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.txt");
    createDir("dir1");
    createDir("dir2");
    createDir("dir3");
    createDir("dir4");
    createDir("dir5");
    ASSERT_EQ(static_cast<int>(Filework::Common::dirsCount(testDir)), 5);
}

// ---------------- getContentNames / getContentPaths ----------------

TEST_F(Components_FileworkCommon, GetContentNames_AllNames) {
    createFile("a.txt");
    createFile("b.log");
    createDir("subdir");

    auto names = Filework::Common::getContentNames(testDir);
    ASSERT_EQ(names.size(), 3);
    ASSERT_EQ(names[0], "a.txt");
    ASSERT_EQ(names[1], "b.log");
    ASSERT_EQ(names[2], "subdir");
}

TEST_F(Components_FileworkCommon, GetContentNames_WithRegexpFilter) {
    createFile("a.txt");
    createFile("b.txt");
    createFile("c.log");

    auto names = Filework::Common::getContentNames(testDir, ".*\\.txt");
    ASSERT_EQ(names.size(), 2);
    for (const auto& n : names) {
        ASSERT_NE(n.find(".txt"), std::string::npos);
    }
}

TEST_F(Components_FileworkCommon, GetContentPaths_AllPaths) {
    createFile("a.txt");
    createFile("b.log");
    createDir("subdir");

    auto paths = Filework::Common::getContentPaths(testDir);
    ASSERT_EQ(paths.size(), 3u);
    for (const auto& p : paths) {
        ASSERT_TRUE(std::filesystem::exists(p));
        ASSERT_EQ(p.rfind(testDir, 0), 0); // префикс каталога
    }
}

// ---------------- read/save/replace/append ----------------

TEST_F(Components_FileworkCommon, ReadData) {
    const std::string content = "hello world";

    const std::string tstFile {"test.txt"};
    std::ofstream ofile(tstFile, std::ios_base::trunc | std::ios_base::out);
    ASSERT_TRUE(ofile.is_open()) << "Error opening test file: " << strerror(errno);
    ofile.write(content.c_str(), content.size());
    ofile.flush();
    ofile.close();

    std::string out;
    ASSERT_TRUE(Filework::Common::readFileData(tstFile, out));
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, SaveData) {
    const std::string content = "hello world";
    ASSERT_TRUE(Filework::Common::saveFileData(path("f.txt"), content));

    std::string out;
    ASSERT_TRUE(Filework::Common::readFileData(path("f.txt"), out));
    ASSERT_EQ(out, content);
}

TEST_F(Components_FileworkCommon, ReadNonexistentFile_ReportsFailure) {
    std::string out;
    bool result = true;
    try {
        result = Filework::Common::readFileData(path("missing.txt"), out);
    } catch (...) {
        result = false; // noexcept(false) — допускает исключение
    }
    ASSERT_FALSE(result);
}

TEST_F(Components_FileworkCommon, ReplaceFileData_Overwrites) {
    ASSERT_TRUE(Filework::Common::saveFileData(path("f.txt"), "original"));
    std::string out;
    ASSERT_TRUE(Filework::Common::readFileData(path("f.txt"), out));
    ASSERT_EQ(out, "original");

    out.clear();
    ASSERT_TRUE(Filework::Common::replaceFileData(path("f.txt"), "replaced"));
    ASSERT_TRUE(Filework::Common::readFileData(path("f.txt"), out));
    ASSERT_EQ(out, "replaced");
}

TEST_F(Components_FileworkCommon, AppendFileData_Appends) {
    ASSERT_TRUE(Filework::Common::saveFileData(path("f.txt"), "first"));
    std::string out;
    ASSERT_TRUE(Filework::Common::readFileData(path("f.txt"), out));
    ASSERT_EQ(out, "first");

    out.clear();
    ASSERT_TRUE(Filework::Common::appendFileData(path("f.txt"), " second"));
    ASSERT_TRUE(Filework::Common::readFileData(path("f.txt"), out));
    ASSERT_EQ(out, "first second");
}