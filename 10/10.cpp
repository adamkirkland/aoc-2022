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

struct Instruction {
    int ticksRemaining;

    Instruction(int ticks) : ticksRemaining(ticks) {}

    virtual void tick(int &rx) {
        ticksRemaining--;
    }
};

struct Noop : Instruction {
    Noop() : Instruction(1) {}
};

struct AddX : Instruction {
    const int amount;

    AddX(int val) : Instruction(2), amount(val) {}

    void tick(int &rx) {
        if (ticksRemaining == 1) {
            rx += amount;
        }
        Instruction::tick(rx);
    }
};

std::shared_ptr<Instruction> parseInstruction(std::string s) {
    std::vector<std::string> toks;
    for (const auto tok : std::views::split(s, ' ')) {
        toks.push_back(std::string{tok.begin(), tok.end()});
    }
    if (toks.size() == 1 && toks[0] == "noop"s) {
        return std::make_shared<Noop>(Noop());
    }
    if (toks.size() == 2 && toks[0] == "addx"s) {
        return std::make_shared<AddX>(AddX(stoi(toks[1])));
    }
}

int part1(std::istream& input) {
    int tickNumber = 0;
    int registerX = 1;
    int result = 0;

    std::string line;
    while (std::getline(input, line)) {
        auto instruction = parseInstruction(line);

        while (instruction->ticksRemaining > 0) {
            // Start tick
            tickNumber++;

            if ((tickNumber - 20) % 40 == 0) {
                result += tickNumber * registerX;
            }

            // Complete tick
            instruction->tick(registerX);
        }
    }

    return result;
}

int part2(std::istream& input) {
    int tickNumber = 0;
    int registerX = 1;

    std::string line;
    while (std::getline(input, line)) {
        auto instruction = parseInstruction(line);

        while (instruction->ticksRemaining > 0) {
            // Start tick
            tickNumber++;

            const int col = (tickNumber - 1) % 40;
            if (abs(registerX - col) < 2)
                std::cout << "#";
            else
                std::cout << ".";

            if (col == 39)
                std::cout << std::endl;

            // Complete tick
            instruction->tick(registerX);
        }
    }

    return 0;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 13140, part1),
        std::make_tuple("Part 2"s, 0, part2)
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
