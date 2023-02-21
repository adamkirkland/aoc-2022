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

std::pair<char, int> parseInstruction(std::string s) {
    char dir = '\0';
    for (const auto tok : std::views::split(s, ' ')) {
        if (dir == '\0') {
            dir = std::string{tok.begin(), tok.end()}[0];
        }
        else {
            return std::make_pair(dir, stoi(std::string{tok.begin(), tok.end()}));
        }
    }
}

int part1(std::istream& input) {
    int hx = 0, hy = 0;
    int tx = 0, ty = 0;
    std::set<std::pair<int, int>> visited;

    std::string line;
    while (std::getline(input, line)) {
        auto action = parseInstruction(line);
        for (int i = 0; i < action.second; i++) {
            const int oldHx = hx, oldHy = hy;
            // Move head
            switch (action.first) {
                case 'U':
                    hy += 1; break;
                case 'D':
                    hy -= 1; break;
                case 'L':
                    hx -= 1; break;
                case 'R':
                    hx += 1; break;
            }
            // Catch up tail
            if (abs(hx - tx) > 1 || abs(hy - ty) > 1) {
                tx = oldHx;
                ty = oldHy;
            }
            visited.insert(std::make_pair(tx, ty));
        }
    }

    return visited.size();
}

std::pair<int, int> adjustTail(int tx, int ty, int hx, int hy) {
    int dx = abs(hx - tx), dy = abs(hy - ty);

    // No movement
    if (dx <= 1 && dy <= 1)
        return std::make_pair(tx, ty);
    // Lateral movement
    if (dx + dy < 3) {
        if (dx == 0)
            return std::make_pair(tx, hy > ty ? ty+1 : ty-1 );
        if (dy == 0)
            return std::make_pair(hx > tx ? tx+1 : tx-1, ty);
    }
    // Diagonal movement
    return std::make_pair(hx > tx ? tx+1 : tx-1, hy > ty ? ty+1 : ty-1);
}

int part2(std::istream& input) {
    std::vector<std::pair<int, int>> rope(10, std::make_pair(0, 0));  // Head is at index 0
    std::set<std::pair<int, int>> visited;

    std::string line;
    while (std::getline(input, line)) {
        auto action = parseInstruction(line);
        for (int i = 0; i < action.second; i++) {
            // Move head
            auto& head = rope[0];
            switch (action.first) {
                case 'U':
                    head.second += 1; break;
                case 'D':
                    head.second -= 1; break;
                case 'L':
                    head.first -= 1; break;
                case 'R':
                    head.first += 1; break;
            }
            // Now move all the other nodes
            for (int j = 1; j < rope.size(); j++) {
                auto leadingNode = rope[j-1], trailingNode = rope[j];
                rope[j] = adjustTail(trailingNode.first, trailingNode.second, leadingNode.first, leadingNode.second);
            }
            visited.insert(rope[rope.size()-1]);
        }
    }
    return visited.size();
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 13, part1),
        std::make_tuple("Part 2"s, 1, part2)
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
