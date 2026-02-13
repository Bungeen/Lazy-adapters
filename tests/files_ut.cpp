#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>

void PrepairDir(const std::string& temp_dir) {
    std::string sub_temp_dir = temp_dir + "/temp";
    std::filesystem::create_directory(temp_dir);
    std::filesystem::create_directory(sub_temp_dir);

    std::string filename = temp_dir + "/test_file.txt";
    std::ofstream out(filename);
    out << "Test content";
    out.close();

    std::string filename_second = temp_dir + "/test_file_second.txt";
    out.open(filename_second);
    out << "Another text";
    out.close();

    std::string filename_inside = sub_temp_dir + "/test_file_inside.txt";
    out.open(filename_inside);
    out << "Inside surprise";
    out.close();
}

TEST(FileTest, ReadRecursionByDefault) {
    std::string temp_dir = "temp_dir_" + std::to_string(rand());
    PrepairDir(temp_dir);

    auto result = Dir(temp_dir)
        | OpenFiles()
        | Split(" ")
        | AsVector();
    
    std::filesystem::remove_all(temp_dir);

    ASSERT_THAT(result, testing::ElementsAre(
        "Inside", "surprise", "Test", "content", "Another", "text"));
}

TEST(FileTest, ReadRecursionForced) {
    std::string temp_dir = "temp_dir_" + std::to_string(rand());
    PrepairDir(temp_dir);

    auto result = Dir(temp_dir, true)
        | OpenFiles()
        | Split(" ")
        | AsVector();

    std::filesystem::remove_all(temp_dir);

    ASSERT_THAT(result, testing::ElementsAre(
        "Inside", "surprise", "Test", "content", "Another", "text"));

}

TEST(FileTest, ReadSimple) {
    std::string temp_dir = "temp_dir_" + std::to_string(rand());
    PrepairDir(temp_dir);

    auto result = Dir(temp_dir, false)
        | OpenFiles()
        | Split(" ")
        | AsVector();

    std::filesystem::remove_all(temp_dir);

    ASSERT_THAT(result, testing::ElementsAre(
            "Test", "content", "Another", "text"));
}

TEST(FileTest, ComplexRead) {
    std::string temp_dir = "temp_dir_" + std::to_string(rand());
    PrepairDir(temp_dir);
    std::string filename = temp_dir + "/test_file.tsv";
    std::ofstream out(filename);
    out << "BAD\tCONTENT";
    out.close();

    auto result = Dir(temp_dir, false)
        | Filter([](std::filesystem::path& p){
            return p.extension() == ".txt"; })
        | OpenFiles()
        | Split(" ")
        | AsVector();
    
    std::filesystem::remove_all(temp_dir);

    ASSERT_THAT(result, testing::ElementsAre(
            "Test", "content", "Another", "text"));
}

TEST(FileTest, ComplexBadRead) {
    std::string temp_dir = "temp_dir_" + std::to_string(rand());
    PrepairDir(temp_dir);
    std::string filename = temp_dir + "/test_file.tsv";
    std::ofstream out(filename);
    out << "BAD\tCONTENT\tCONTENT";
    out.close();

    auto result = Dir(temp_dir, false)
        | Filter([](std::filesystem::path& p){
            return p.extension() == ".tsv"; })
        | OpenFiles()
        | Split("\t")
        | Transform(
            [](const std::string& token) { 
                std::string result = token;
                std::transform(result.begin(), result.end(), result.begin(), [](char c){return std::tolower(c);});
                return result;
            })
        | AsVector();
    
    std::filesystem::remove_all(temp_dir);

    ASSERT_THAT(result, testing::ElementsAre(
            "bad", "content", "content"));
}