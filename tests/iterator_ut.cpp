#include <processing.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <expected>
#include <cctype>

struct Department {
    std::string name;

    bool operator==(const Department& other) const {
        return name == other.name;
    }
};

std::expected<Department, std::string> ParseDepartmentLocal(const std::string& str) {
    if (str.empty()) {
        return std::unexpected("Department name is empty");
    }
    if (str.contains(' ')) {
        return std::unexpected("Department name contains space");
    }
    return Department{str};
}

TEST(IteratorTest, FilterIterator) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Filter([](int x) { return x % 2 == 0; });
    ASSERT_THAT(result, testing::ElementsAre(2, 4));
}

TEST(IteartorTest, TransformIterator) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input) | Transform([](int x) { return x * x; }) | Transform([](int x) { return x * x; });
    ASSERT_THAT(result, testing::ElementsAre(1, 16, 81, 256, 625));
}

TEST(IteartorTest, SplitIterator) {
    std::vector<std::stringstream> files(2);
    files[0] << "1 2 3 4 5";
    files[1] << "6 7 8 9 10";
    auto result = AsDataFlow(files) | Split(" ");
    ASSERT_THAT(result, testing::ElementsAre("1", "2", "3", "4", "5", "6", "7", "8", "9", "10"));
}


TEST(IteratorTest, AggregateIterator) {
    std::vector<std::string> input = {"name4", "name0", "name1", "name0", "name2", "name0", "name1"};

    auto result =
        AsDataFlow(input)
            | AggregateByKey(
                std::size_t{0},
                [](const std::string&, std::size_t& accumulated) { ++accumulated; },
                [](const std::string& token) { return token; }
            );

    ASSERT_THAT(
        result,
        ::testing::ElementsAre(
            std::make_pair("name4", 1),
            std::make_pair("name0", 3),
            std::make_pair("name1", 2),
            std::make_pair("name2", 1)
        )
    );
}


TEST(IteratorTest, JoinIterator) {
    std::vector<KV<int, std::string>> left = {{0, "a"}, {1, "b"}, {2, "c"}, {3, "d"}, {1, "e"}};
    std::vector<KV<int, std::string>> right = {{0, "f"}, {1, "g"}, {3, "i"}};

    auto left_flow = AsDataFlow(left);
    auto right_flow = AsDataFlow(right);
    auto result = left_flow | Join(right_flow);

    ASSERT_THAT(
        result,
        testing::ElementsAre(
            JoinResult<std::string, std::string>{"a", "f"},
            JoinResult<std::string, std::string>{"b", "g"},
            JoinResult<std::string, std::string>{"c", std::nullopt},
            JoinResult<std::string, std::string>{"d", "i"},
            JoinResult<std::string, std::string>{"e", "g"}
        )
    );
}


TEST(IteratorTest, SplitExpectedIterator) {
    std::vector<std::stringstream> files(1);
    files[0] << "good-department|bad department||another-good-department";

    auto [unexpected_flow, good_flow] = AsDataFlow(files) | Split("|") | Transform(ParseDepartmentLocal) | SplitExpected(ParseDepartmentLocal);

    std::stringstream unexpected_file;
    unexpected_flow | Write(unexpected_file, '.');

    auto expected_result = good_flow;

    ASSERT_EQ(unexpected_file.str(), "Department name contains space.Department name is empty.");
    ASSERT_THAT(expected_result, testing::ElementsAre(Department{"good-department"}, Department{"another-good-department"}));
}


TEST(IteratorTest, AsDataFlowIterator) {
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = AsDataFlow(input);
    ASSERT_THAT(result, testing::ElementsAre(1, 2, 3, 4, 5));
}