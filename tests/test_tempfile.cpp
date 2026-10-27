#include <gtest/gtest.h>

#include <Components/Filework/TemporaryFile.h>

#include <filesystem>

TEST(Components_Filework, TemporaryFileNoTarget) {
    const std::string TEST_TEXT {"Test_text"};
    Filework::TemporaryFile tmpFile;

    try {
        tmpFile << TEST_TEXT;
    } catch (...) {
        std::cerr << "Temporary file writing error" << std::endl;
        throw;
    }

    try {
        std::string fileContents;
        tmpFile.seekg(0, std::ios_base::beg);
        tmpFile >> fileContents;
        ASSERT_EQ(TEST_TEXT, fileContents);
    } catch (...) {
        std::cerr << "Temporary file reading error" << std::endl;
        throw;
    }

    try {
        tmpFile.clearFileData();
        ASSERT_TRUE(tmpFile.empty()) << "File not cleaned (size is " << tmpFile.size() << ")";
    } catch (...) {
        std::cerr << "Temporary file cleaning error" << std::endl;
        throw;
    }
}

TEST(Components_Filework, TemporaryFileRAII) {
    std::string fileName;
    {
        const std::string testfilePath {"tempfile_input.txt"};
        std::unique_ptr<bool, std::function<void(bool*)> > m_fileDeleter = {
            reinterpret_cast<bool*>(1),
            [&](bool*){
                if (std::filesystem::exists(testfilePath)) {
                    std::filesystem::remove(testfilePath);
                }
            }
        }; // Deletes file

        {
            std::ofstream testfile(testfilePath);
            testfile << "Empty text";
            testfile.flush();
        }

        Filework::TemporaryFile tmpFile(testfilePath);
        fileName = tmpFile.getFilename();
    }
    ASSERT_FALSE(std::filesystem::exists(fileName)) << "RAII broken";
}

TEST(Components_Filework, TemporaryFileTarget) {
    std::string tmpFileName;
    {
        const std::string TEST_TEXT {"Test_text"};
        const std::string TEST_TEXT_REPL {"Replaced_data"};

        const std::string testfilePath {"tempfile_input.txt"};
        if (std::filesystem::exists(testfilePath)) {
            std::filesystem::remove(testfilePath);
        }

        {
            std::ofstream testfile(testfilePath);
            testfile << TEST_TEXT;
            testfile.flush();
        }

        // Check temporary file writing
        Filework::TemporaryFile tmpFile(testfilePath);
        tmpFileName = std::string(tmpFile.getFilename());
        try {
            tmpFile.seekg(0, std::ios_base::end);
            tmpFile << TEST_TEXT_REPL;
            tmpFile.flush();
        } catch (const std::exception& ex) {
            std::cerr << "Failed to write data to temporary file" << std::endl;
            throw;
        }

        // Check that data have even been written
        try {
            std::string fileContents;
            tmpFile.seekg(0, std::ios_base::beg);
            tmpFile >> fileContents;
            ASSERT_EQ(std::string(TEST_TEXT) + TEST_TEXT_REPL, fileContents);
        } catch (const std::exception& ex) {
            std::cerr << "Failed to read data 1 from temporary file" << std::endl;
            throw;
        }

        // Try save
        try {
            tmpFile.accept();
        } catch (const std::exception& ex) {
            std::cerr << "Failed to accept temporary file" << std::endl;
            throw;
        }

        // Reopen file, check if data is correct
        ASSERT_TRUE(tmpFile.setTargetFile(testfilePath)) << "Failed to set target file";
        try {
            std::string fileContents;
            tmpFile.seekg(0, std::ios_base::beg);
            tmpFile >> fileContents;
            ASSERT_EQ(std::string(TEST_TEXT) + TEST_TEXT_REPL, fileContents);
        } catch (const std::exception& ex) {
            std::cerr << "Failed to read data 2 from temporary file" << std::endl;
            throw;
        }

        // Here's must be all good with used methods
        tmpFile.clearFileData();
        tmpFile << TEST_TEXT;
        tmpFile.flush();
        tmpFile.accept();

        {
            std::ifstream testfile(testfilePath);
            std::string fileContents;
            testfile >> fileContents;
            ASSERT_EQ(TEST_TEXT, fileContents);
        }

        if (std::filesystem::exists(testfilePath)) {
            std::filesystem::remove(testfilePath);
        }
    }
    ASSERT_FALSE(std::filesystem::exists(tmpFileName)) << "RAII broken";
}
