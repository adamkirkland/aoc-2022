#include <string>
#include <fstream>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <deque>
#include <queue>
#include <stack>
#include <set>
#include <assert.h>
#include <regex>
#include <iomanip>
#include <ctype.h>

using namespace std::string_literals;

const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

typedef int answer_t;

struct SignalBase {
    virtual ~SignalBase() = default;
};
typedef std::shared_ptr<SignalBase> signal_t;

struct SignalDigit : SignalBase {
    int value;
    SignalDigit(int v) : value(v) {}
};
typedef std::shared_ptr<SignalDigit> digit_t;

struct SignalList : SignalBase {
    std::vector<signal_t> vec;
};
typedef std::shared_ptr<SignalList> list_t;

std::ostream &operator << (std::ostream &os, SignalList const &m) {
    os << "[";
    for (int i = 0; i < m.vec.size(); i++) {
        if (digit_t d = dynamic_pointer_cast<SignalDigit>(m.vec[i]))
            os << d->value;
        else if (list_t l = dynamic_pointer_cast<SignalList>(m.vec[i]))
            os << (*l);
        else
            os << "i-" << i << "ERR";

        if (i < m.vec.size()-1)
            os << ", ";
    }
    os << "]";
}

// 1 for correct, -1 for incorrect, 0 for equal
int correctOrder(signal_t a, signal_t b, bool print = false) {
    if (auto ad = dynamic_pointer_cast<SignalDigit>(a)) {
        if (auto bd = dynamic_pointer_cast<SignalDigit>(b)) {
            // Digit vs digit
            if (print)
                std::cout << "Comparing digits " << ad->value << " vs " << bd->value << std::endl;
            if (ad->value < bd->value)
                return 1;
            else if (ad->value > bd->value)
                return -1;
            return 0;
        }
        else if (auto bl = dynamic_pointer_cast<SignalList>(b)) {
            // Convert A to list and recurse
            if (print)
                std::cout << "Converting A to list" << std::endl;
            auto al = std::make_shared<SignalList>();
            al->vec.push_back(ad);
            return correctOrder(al, bl, print);
        }
    }
    else if (auto al = dynamic_pointer_cast<SignalList>(a)) {
        if (auto bd = dynamic_pointer_cast<SignalDigit>(b)) {
            // Convert B to list and recurse
            if (print)
                std::cout << "Converting B to list" << std::endl;
            auto bl = std::make_shared<SignalList>();
            bl->vec.push_back(bd);
            return correctOrder(al, bl, print);
        }
        else if (auto bl = dynamic_pointer_cast<SignalList>(b)) {
            // List vs list
            if (print)
                std::cout << "Comparing lists " << *(al.get()) << " vs " << *(bl.get()) << std::endl;
            auto minLength = std::min<int>(al->vec.size(), bl->vec.size());
            for (int i = 0; i < minLength; i++) {
                // Piecewise comparison
                auto comp = correctOrder(al->vec[i], bl->vec[i], print);
                if (comp != 0)
                    return comp;
            }
            // If not yet determined, go by whichever list is shortest
            if (print)
                std::cout << "Deciding by list size\n";
            if (al->vec.size() < bl->vec.size())
                return 1;
            else if (al->vec.size() > bl->vec.size())
                return -1;
        }
    }

    return 0;
}

list_t parseLine(std::string line) {
    std::stack<list_t> stack;
    std::vector<char> digits;  // Pretty smelly way of parsing
    list_t result;
    for (auto c : line) {
        if ((c == ',' || c == ']') && !digits.empty()) {
            auto value = stoi(std::string(digits.begin(), digits.end()));
            stack.top()->vec.push_back(std::make_shared<SignalDigit>(value));
            digits.clear();
        }

        if (c == '[')
            stack.push(std::make_shared<SignalList>());

        if (c == ']') {
            auto finishedList = stack.top();
            stack.pop();
            if (stack.empty()) {
                result = finishedList;
                break;
            }
            else
                stack.top()->vec.push_back(finishedList);
        }

        if (isdigit(c)) {
            digits.push_back(c);
        }
    }

    return result;
}

std::vector<list_t> parseInput(std::istream& input) {
    std::vector<list_t> result;
    std::string line;
    while (std::getline(input, line)) {
        if (line.length() < 1)
            continue;
        result.push_back(parseLine(line));
    }
    return result;
}

answer_t part1(std::istream& input) {
    answer_t result = 0;
    auto signalLists = parseInput(input);

    for (int i = 1; i < signalLists.size(); i += 2) {
        auto a = signalLists[i-1], b = signalLists[i];

        if (correctOrder(a, b) == 1) {
            result += (i + 1) / 2;
        }
    }

    return result;
}

answer_t part2(std::istream& input) {
    auto signalLists = parseInput(input);
    auto div1 = parseLine("[[2]]"s), div2 = parseLine("[[6]]"s);
    signalLists.push_back(div1);
    signalLists.push_back(div2);

    std::sort(signalLists.begin(), signalLists.end(), [](list_t a, list_t b) {
        return correctOrder(a, b) == 1;
    });

    auto div1It = std::find(signalLists.begin(), signalLists.end(), div1);
    auto div2It = std::find(signalLists.begin(), signalLists.end(), div2);
    return (std::distance(signalLists.begin(), div1It) + 1) * (std::distance(signalLists.begin(), div2It) + 1);
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, answer_t, answer_t (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 13, part1),
        std::make_tuple("Part 2"s, 140, part2)
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
        std::cout << "\tAnswer: " << solution(inputFile) << "\n\n";
    });
    return 0;
}
