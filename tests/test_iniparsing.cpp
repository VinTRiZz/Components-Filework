#include <gtest/gtest.h>

#include <Components/Filework/ConfigParsing/IniParser.h>
#include <Components/Filework/TemporaryFile.h>
#include <Components/Filework/Common.h>

#include "testini.cpp"

#include <filesystem>

using namespace Filework;

void checkValues(const IniFileParser& parser) {
    // TODO: Check values
}

TEST(ConfigParsing, IniRead) {
    // Prepare
    TemporaryFile tmpFile("test.ini");
    tmpFile << TEST_INI_DATA;
    tmpFile.seekg(0);

    IniFileParser parser;
    ASSERT_TRUE(parser.read(tmpFile.getFilename(), true)) << parser.getLastErrorText();
    checkValues(parser);
}

TEST(ConfigParsing, IniWrite) {
    // Prepare
    TemporaryFile tmpFile("test.ini");
    tmpFile << TEST_INI_DATA;
    tmpFile.seekg(0);

    IniFileParser parser;
    ASSERT_TRUE(parser.read(tmpFile.getFilename(), true)) << parser.getLastErrorText();
    checkValues(parser);

    tmpFile.clearFileData();
    ASSERT_TRUE(parser.write("test_inifwrite.ini")) << parser.getLastErrorText();

    std::string tmpOutput;
    ASSERT_TRUE(Common::readFileData("test_inifwrite.ini", tmpOutput));
    std::filesystem::remove("test_inifwrite.ini");
    ASSERT_EQ(TEST_INI_EXPECTED_DATA, tmpOutput);
}