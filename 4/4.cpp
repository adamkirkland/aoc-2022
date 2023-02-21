#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>
#include <assert.h>

using namespace std::string_literals;
const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

int part1(std::istream& input) {
    int total = 0;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<std::pair<int, int>> sectors;
        for (const auto range : std::views::split(line, ',')) {
            auto rangeEnds = std::views::split(range, '-');
            int firstNum = -1;
            // This is ugly af but string_view ranges and transform_view shit is proving more trouble than it's worth
            for (const auto numstr : rangeEnds) {
                const auto num = stoi(std::string{numstr.begin(), numstr.end()});
                if (firstNum == -1) {
                    firstNum = num;
                    continue;
                }
                for (const auto &sector : sectors) {
                    if ((sector.first <= firstNum && sector.second >= num) ||
                        (sector.first >= firstNum && sector.second <= num)
                    ) {
                        total++;
                    }
                }
                sectors.push_back(std::make_pair(firstNum, num));
            }
        }
    }
    return total;
}

int part2(std::istream& input) {
    int total = 0;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<std::pair<int, int>> sectors;
        for (const auto range : std::views::split(line, ',')) {
            auto rangeEnds = std::views::split(range, '-');
            int firstNum = -1;
            // This is ugly af but string_view ranges and transform_view shit is proving more trouble than it's worth
            for (const auto numstr : rangeEnds) {
                const auto num = stoi(std::string{numstr.begin(), numstr.end()});
                if (firstNum == -1) {
                    firstNum = num;
                    continue;
                }
                for (const auto &sector : sectors) {
                    const auto overlapStart = std::max(sector.first, firstNum);
                    const auto overlapEnd = std::min(sector.second, num);
                    if (overlapStart <= overlapEnd) {
                        total++;
                    }
                }
                sectors.push_back(std::make_pair(firstNum, num));
            }
        }
    }
    return total;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 2, part1),
        std::make_tuple("Part 2"s, 4, part2)
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
