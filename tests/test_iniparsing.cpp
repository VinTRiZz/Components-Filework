#include <gtest/gtest.h>

#include <Components/Filework/ConfigParsing/IniParser.h>
#include <Components/Filework/TemporaryFile.h>
#include <Components/Filework/Common.h>

#include "testini.cpp"

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
    ASSERT_TRUE(parser.read(tmpFile.getFilename()));
    checkValues(parser);
}

TEST(ConfigParsing, IniWrite) {
    // Prepare
    // TemporaryFile tmpFile("test.ini");
    // tmpFile << TEST_INI_DATA;
    // tmpFile.seekg(0);

    // IniFileParser parser;
    // ASSERT_TRUE(parser.read(tmpFile.getFilename(), true));
    // checkValues(parser);

    // ASSERT_TRUE(parser.write(tmpFile.getFilename()));

    // std::string tmpOutput;
    // ASSERT_TRUE(Common::readFileData(tmpFile.getFilename().data(), tmpOutput));
    // ASSERT_EQ(TEST_INI_DATA, tmpOutput);
}