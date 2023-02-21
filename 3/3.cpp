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

int priority(char in) {
    const auto ascii = int(in);
    if (ascii >= 97 && ascii <= 122) {
        return ascii - 96;
    }
    if (ascii >= 65 && ascii <= 90) {
        return ascii - 38;
    }
    return -1;
}

int part1(std::istream& input) {
    int total = 0;
    std::string line;
    while (std::getline(input, line)) {
        const auto sv = std::string_view(line);
        const auto p1 = sv.substr(0, sv.size()/2);
        const auto p2 = sv.substr(sv.size()/2, sv.size());
        for (const auto c : p1) {
            if (p2.find(c) < std::string::npos) {
                total += priority(c);
                break;
            }
        }
    }
    return total;
}

int part2(std::istream& input) {
    int total = 0;
    auto group = std::vector<std::string>(3);
    unsigned int i = 0;
    std::string line;
    while (std::getline(input, line)) {
        group[i] = line;
        if (i == 2) {
            // Got a full triplet of lines now
            const auto l1 = std::string_view(group[0]);
            const auto l2 = std::string_view(group[1]);
            const auto l3 = std::string_view(group[2]);
            for (const auto c1 : l1) {
                if (l2.find(c1) == std::string::npos) continue;
                if (l3.find(c1) == std::string::npos) continue;
                total += priority(c1);
                break;
            }
            i = 0;
        }
        else {
            i++;
        }
    }
    return total;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 157, part1),
        std::make_tuple("Part 2"s, 70, part2)
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
