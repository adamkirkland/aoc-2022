#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <deque>
#include <set>
#include <assert.h>
#include <regex>
#include <iomanip>

using namespace std::string_literals;
const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

int part1(std::istream& input) {
    std::deque<char> window;
    char c;
    int result = 0;
    while (input >> c) {
        window.push_back(c);
        result++;

        if (window.size() > 4) {
            window.pop_front();
            if (std::set<char>(window.begin(), window.end()).size() == window.size()) {
                break;
            }
        }
    }

    return result;
}

int part2(std::istream& input) {
    std::deque<char> window;
    char c;
    int result = 0;
    while (input >> c) {
        window.push_back(c);
        result++;

        if (window.size() > 14) {
            window.pop_front();
            if (std::set<char>(window.begin(), window.end()).size() == window.size()) {
                break;
            }
        }
    }

    return result;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 7, part1),
        std::make_tuple("Part 2"s, 19, part2)
    };

    std::ranges::for_each(problemPairs, [](const auto& tup) {
        std::cout << std::get<0>(tup) << ":\n";
        auto testAnswer = std::get<1>(tup);
        auto solution = std::get<2>(tup);

        std::ifstream testFile(testPath);
        if (!testFile.is_open()) {
            std::cerr << "Could not open test file\n\n";
            return;
        }
        auto testResult = solution(testFile);
        if (testResult == testAnswer) {
            std::cout << "\tTest passed!\n";
        }
        else {
            std::cout << "\tTest failed : result " << testResult << " did not match expected answer " << testAnswer << "\n\n";
            return;
        }

        std::ifstream inputFile(inputPath);
        if (!inputFile.is_open()) {
            std::cerr << "Could not open input file\n\n";
            return;
        }
        auto result = solution(inputFile);
        std::cout << "\tAnswer: " << result << "\n\n";
    });
    return 0;
}
