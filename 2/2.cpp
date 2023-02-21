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

enum Move {
    ROCK = 1,
    PAPER = 2,
    SCISSORS = 3
};

int scoreRound(Move us, Move other) {
    int result = us;
    if ((us == Move::ROCK && other == Move::SCISSORS) || (us > other && !(us == Move::SCISSORS && other == Move::ROCK))) {
        result += 6;
    }
    else if (us == other) {
        result += 3;
    }

    return result;
}

int part1(std::istream& input) {
    int total = 0;
    std::string line;
    while (std::getline(input, line)) {
        Move us, them;
        for (const auto word : std::views::split(line, ' ')) {
            const auto tok = std::string{word.begin(), word.end()};
            if (tok == "A"s) them = Move::ROCK;
            if (tok == "X"s) us = Move::ROCK;
            if (tok == "B"s) them = Move::PAPER;
            if (tok == "Y"s) us = Move::PAPER;
            if (tok == "C"s) them = Move::SCISSORS;
            if (tok == "Z"s) us = Move::SCISSORS;
        }
        //std::cout << "Line '" << line << "' results in a score of " << scoreRound(us, them) << std::endl;
        total += scoreRound(us, them);
    }
    return total;
}

int part2(std::istream& input) {
    int total = 0;
    std::string line;
    const auto delim = std::string_view{" "};
    while (std::getline(input, line)) {
        Move them;
        for (const auto word : std::views::split(line, delim)) {
            const auto tok = std::string{word.begin(), word.end()};
            if (tok == "A"s) them = Move::ROCK;
            if (tok == "B"s) them = Move::PAPER;
            if (tok == "C"s) them = Move::SCISSORS;
            if (tok == "X"s) { // lose
                if (them == Move::ROCK) total += 3;
                else if (them == Move::PAPER) total += 1;
                else total += 2;
            }
            if (tok == "Y"s) { // draw
                total += 3 + them;
            }
            if (tok == "Z"s) { // win
                if (them == Move::ROCK) total += 8;
                else if (them == Move::PAPER) total += 9;
                else total += 7;
            }
        }
    }
    return total;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 15, part1),
        std::make_tuple("Part 2"s, 12, part2)
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