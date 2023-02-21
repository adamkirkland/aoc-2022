#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <deque>
#include <queue>
#include <set>
#include <assert.h>
#include <regex>
#include <iomanip>

using namespace std::string_literals;

const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

enum Operator {
    PLUS,
    TIMES,
    SQUARE
};

struct Monkey {
    int inspectCount;
    std::queue<long> items;

    Operator op;
    int operand; // Ignored if op is SQUARE

    int testDivisor;
    int trueMonkey, falseMonkey;

    Monkey() : inspectCount(0) {}

    long inspect(long item) {
        inspectCount++;
        switch (op) {
            case Operator::SQUARE:
                return item * item;
            case Operator::TIMES:
                return item * operand;
            case Operator::PLUS:
                return item + operand;
            default:
                return 0;
        }
    }
};

std::vector<std::shared_ptr<Monkey>> parseInput(std::istream& input) {
    std::vector<std::shared_ptr<Monkey>> result;
    std::shared_ptr<Monkey> curr;
    auto itemsRe = std::regex(R"(Starting items: (.*))");
    auto opRe = std::regex(R"(Operation: new = old ([+*]) (.+))");
    auto testRe = std::regex(R"(Test: divisible by (\d+))");
    auto throwRe = std::regex(R"(If (true|false): throw to monkey (\d))");

    std::string line;
    while (std::getline(input, line)) {
        std::smatch matches;
        if (std::regex_search(line, matches, itemsRe)) {
            // Time for a new monkey!
            if (curr != nullptr) {
                result.push_back(curr);
            }
            //std::cout << "Making new monkey!\n";
            curr = std::make_shared<Monkey>();

            auto itemStr = matches[1].str();
            for (const auto tok : std::views::split(itemStr, ", "s)) {
                curr->items.push(stol(std::string{tok.begin(), tok.end()}));
            }
        }
        else if (std::regex_search(line, matches, opRe)) {
            auto op = matches[1].str();
            auto operand = matches[2].str();
            if (operand == "old"s) {
                //std::cout << "\tOp is square\n";
                curr->op = Operator::SQUARE;
            }
            else if (op == "*"s) {
                //std::cout << "\tOp is times " << operand << std::endl;
                curr->op = Operator::TIMES;
                curr->operand = stoi(operand);
            }
            else if (op == "+"s) {
                //std::cout << "\tOp is plus " << operand << std::endl;
                curr->op = Operator::PLUS;
                curr->operand = stoi(operand);
            }
        }
        else if (std::regex_search(line, matches, testRe)) {
            curr->testDivisor = stoi(matches[1].str());
        }
        else if (std::regex_search(line, matches, throwRe)) {
            int throwMonkey = stoi(matches[2].str());
            if (matches[1].str() == "true"s) {
                curr->trueMonkey = throwMonkey;
            }
            else {
                curr->falseMonkey = throwMonkey;
            }
        }
    }
    if (curr != nullptr) {
        result.push_back(curr);
    }
    return result;
}

long part1(std::istream& input) {
    auto monkeys = parseInput(input);

    for (auto loop = 0; loop < 20; loop++) {
        for (auto i = 0; i < monkeys.size(); i++) {
            auto monkey = monkeys[i];
            while (monkey->items.size() > 0) {
                auto item = monkey->items.front();
                monkey->items.pop();

                //std::cout << "Monkey " << i << " inspecting " << item << ".";
                item = monkey->inspect(item);
                //std::cout << " Item is now " << item << ".";
                item = item / 3;
                //std::cout << " After it loses interest, item is " << item << ".\n";

                int newMonkeyIndex = monkey->falseMonkey;
                if (item % monkey->testDivisor == 0) {
                    newMonkeyIndex = monkey->trueMonkey;
                }
                //std::cout << "Adding item " << item << " to monkey " << newMonkeyIndex << std::endl;
                monkeys[newMonkeyIndex]->items.push(item);
            }
        }
    }

    std::vector<int> inspectCounts;
    std::transform(monkeys.begin(), monkeys.end(), std::back_inserter(inspectCounts), [](auto &m) {
        return m->inspectCount;
    });
    std::sort(inspectCounts.begin(), inspectCounts.end(), std::greater<int>());
    return inspectCounts[0] * inspectCounts[1];
}

long part2(std::istream& input) {
    auto monkeys = parseInput(input);
    long modulo = 1;
    for (auto &monkey : monkeys) {
        // Could do some LCM work here to make it a smaller mod
        modulo *= monkey->testDivisor;
    }

    for (auto loop = 0; loop < 10000; loop++) {
        for (auto i = 0; i < monkeys.size(); i++) {
            auto monkey = monkeys[i];
            while (monkey->items.size() > 0) {
                auto item = monkey->items.front();
                monkey->items.pop();

                //std::cout << "Monkey " << i << " inspecting " << item << ".";
                item = monkey->inspect(item) % modulo;
                //std::cout << " Item is now " << item << ".";
                //std::cout << " After it loses interest, item is " << item << ".\n";

                int newMonkeyIndex = monkey->falseMonkey;
                if (item % monkey->testDivisor == 0) {
                    newMonkeyIndex = monkey->trueMonkey;
                }
                //std::cout << "Adding item " << item << " to monkey " << newMonkeyIndex << std::endl;
                monkeys[newMonkeyIndex]->items.push(item);
            }
        }
    }

    std::vector<long> inspectCounts;
    std::transform(monkeys.begin(), monkeys.end(), std::back_inserter(inspectCounts), [](auto &m) {
        return m->inspectCount;
    });
    std::sort(inspectCounts.begin(), inspectCounts.end(), std::greater<long>());
    return inspectCounts[0] * inspectCounts[1];
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, long, long (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 10605, part1),
        std::make_tuple("Part 2"s, 2713310158, part2)
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
