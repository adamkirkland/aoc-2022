#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <vector>
#include <set>
#include <assert.h>
#include <regex>

using namespace std::string_literals;
const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

std::string part1(std::istream& input) {
    std::vector<std::vector<char>> crates;
    auto crateRe = std::regex(R"(\[(.)\])");
    auto instructionRe = std::regex(R"(move (.+) from (.+) to (.+))");
    bool cratesFlipped = false;

    std::string line;
    while (std::getline(input, line)) {
        std::smatch matches;
        if (std::regex_search(line, matches, crateRe)) {
            auto begin = std::sregex_iterator(line.begin(), line.end(), crateRe);
            auto end = std::sregex_iterator();
            for (std::sregex_iterator i = begin; i != end; i++) {
                std::smatch match = *i;
                const int towerIndex = match.position() / 4;
                while (crates.size() < towerIndex+1) {
                    crates.push_back(std::vector<char>());
                }
                crates[towerIndex].push_back(match[1].str()[0]);
            }
        }
        else if (std::regex_search(line, matches, instructionRe)) {
            if (!cratesFlipped) {
                for (auto &crateStack : crates) {
                    std::reverse(crateStack.begin(), crateStack.end());
                }
                cratesFlipped = true;
            }
            int numMoves = stoi(matches[1].str());
            int start = stoi(matches[2].str()) - 1;
            int end = stoi(matches[3].str()) - 1;
            for (int i = 0; i < numMoves; i++) {
                char crate = crates[start].back();
                crates[start].pop_back();
                crates[end].push_back(crate);
            }
        }
    }

    std::string result;
    for (auto &crateStack : crates) {
        result += crateStack.back();
    }

    return result;
}

std::string part2(std::istream& input) {
    std::vector<std::vector<char>> crates;
    auto crateRe = std::regex(R"(\[(.)\])");
    auto instructionRe = std::regex(R"(move (.+) from (.+) to (.+))");
    bool cratesFlipped = false;

    std::string line;
    while (std::getline(input, line)) {
        std::smatch matches;
        if (std::regex_search(line, matches, crateRe)) {
            auto begin = std::sregex_iterator(line.begin(), line.end(), crateRe);
            auto end = std::sregex_iterator();
            for (std::sregex_iterator i = begin; i != end; i++) {
                std::smatch match = *i;
                const int towerIndex = match.position() / 4;
                while (crates.size() < towerIndex+1) {
                    crates.push_back(std::vector<char>());
                }
                crates[towerIndex].push_back(match[1].str()[0]);
            }
        }
        else if (std::regex_search(line, matches, instructionRe)) {
            // We could avoid flipping the crate stacks and just use reverse iterators etc, but the logic
            // becomes quite convoluted
            if (!cratesFlipped) {
                for (auto &crateStack : crates) {
                    std::reverse(crateStack.begin(), crateStack.end());
                }
                cratesFlipped = true;
            }
            int numMoves = stoi(matches[1].str());
            int start = stoi(matches[2].str()) - 1;
            int end = stoi(matches[3].str()) - 1;
            crates[end].insert(
                crates[end].end(),
                crates[start].end()-numMoves,
                crates[start].end()
            );
            crates[start].erase(
                crates[start].end()-numMoves,
                crates[start].end()
            );
        }
    }

    // TODO there's surely a better, more functional way to do this
    std::string result;
    for (const auto &crateStack : crates) {
        result += crateStack.back();
    }

    return result;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, std::string, std::string (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, "CMZ"s, part1),
        std::make_tuple("Part 2"s, "MCD"s, part2)
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
