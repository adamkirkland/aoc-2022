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
#include <list>
#include <assert.h>
#include <regex>
#include <iomanip>
#include <ctype.h>

using namespace std::string_literals;

const auto testPath = "test.txt"s;
const auto inputPath = "input.txt"s;

typedef int answer_t;

class Node;

class Node {
    int flow;
    std::string name;
    std::vector<std::pair<std::shared_ptr<Node>, int>> edges;

    Node(int f, std::string n) : flow(f), name(n) {}

    void addEdge(Node& node, int time = 1, bool makeTwoWay = true) {
        // Check if we already have this edge
        for (auto& edge : edges) {
            if (edge.first->name == node.name) {
                if (edge.second > time) {
                    edge.second = time;
                }
                return;
            }
        }
        edges.push_back(std::make_pair(std::make_shared<Node>(node), time));
        if (makeTwoWay) {
            node.addEdge(*this, time, false);
        }
    }
};

answer_t part1(std::istream& input, bool isTest = false) {
    answer_t result = 0;
    auto lineRe = std::regex(R"(Valve (.+) has flow rate=(.+); tunnels* leads* to valves* (.+))");

    return result;
}

answer_t part2(std::istream& input, bool isTest = false) {
    answer_t result = 0;

    return result;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, answer_t, answer_t (*)(std::istream& input, bool isTest)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 1651, part1),
        //std::make_tuple("Part 2"s, 56'000'011, part2)
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
        auto testResult = solution(testFile, true);
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
        std::cout << "\tAnswer: " << solution(inputFile, false) << "\n\n";
    });
    return 0;
}
