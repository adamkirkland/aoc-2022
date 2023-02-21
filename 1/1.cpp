#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>

using namespace std::string_literals;
const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

int part1(std::istream& input) {
    std::string line;
    int largest = 0;
    int total = 0;
    while (std::getline(input, line)) {
        if (line.length() < 1) {
            total = 0;
            continue;
        }
        int num = stoi(line);
        total += num;
        if (total > largest) {
            largest = total;
        }
    }
    return largest;
}

int part2(std::istream& input) {
    std::string line;
    std::vector<int> calories;
    int total = 0;
    while (std::getline(input, line)) {
        if (line.length() < 1) {
            calories.push_back(total);
            total = 0;
            continue;
        }
        total += stoi(line);
    }
    if (total > 0) {
        calories.push_back(total);
    }
    std::ranges::sort(calories, std::ranges::greater());
    return std::reduce(calories.begin(), calories.begin() + std::min<int>(3, calories.size()));
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 24000, part1),
        std::make_tuple("Part 2"s, 45000, part2)
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