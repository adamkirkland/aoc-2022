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

int part1(std::istream& input) {
    std::vector<std::vector<int>> trees;
    std::vector<std::vector<bool>> visibility;

    // Parse input into trees
    std::string line;
    while (std::getline(input, line)) {
        std::vector<int> row(line.size());
        for (int i = 0; i < line.size(); i++) {
            row[i] = line[i] - '0';
        }
        trees.push_back(row);
        visibility.push_back(std::vector<bool>(line.size(), false));
    }

    // Sweep left and right
    for (int i = 0; i < trees.size(); i++) {
        const auto row = trees[i];
        int h = -1;
        for (int j = 0; j < row.size(); j++) {
            if (row[j] > h) {
                visibility[i][j] = true;
                h = row[j];
            }
        }
        h = -1;
        for (int j = row.size()-1; j >= 0; j--) {
            if (row[j] > h) {
                visibility[i][j] = true;
                h = row[j];
            }
        }
    }
    // Sweep up and down
    for (int j = 0; j < trees[0].size(); j++) {
        int h = -1;
        for (int i = 0; i < trees.size(); i++) {
            if (trees[i][j] > h) {
                visibility[i][j] = true;
                h = trees[i][j];
            }
        }
        h = -1;
        for (int i = trees.size()-1; i >= 0; i--) {
            if (trees[i][j] > h) {
                visibility[i][j] = true;
                h = trees[i][j];
            }
        }
    }

    // Count visibles
    auto total = 0;
    for (const auto& row : visibility) {
        total += std::count(row.begin(), row.end(), true);
    }
    return total;
}

int part2(std::istream& input) {
    std::vector<std::vector<int>> trees;
    std::vector<std::vector<bool>> visibility;

    // Parse input into trees
    std::string line;
    while (std::getline(input, line)) {
        std::vector<int> row(line.size());
        for (int i = 0; i < line.size(); i++) {
            row[i] = line[i] - '0';
        }
        trees.push_back(row);
        visibility.push_back(std::vector<bool>(line.size(), false));
    }

    int result = 0;
    for (int i = 0; i < trees.size(); i++) {
        for (int j = 0; j < trees[i].size(); j++) {
            int treeScore = 1;

            int dirScore = 0;
            for (int x = j-1; x >= 0; x--, dirScore++) {
                if (trees[i][x] >= trees[i][j]) {
                    dirScore++;
                    break;
                }
            }
            treeScore *= dirScore;

            dirScore = 0;
            for (int x = j+1; x < trees[i].size(); x++, dirScore++) {
                if (trees[i][x] >= trees[i][j]) {
                    dirScore++;
                    break;
                }
            }
            treeScore *= dirScore;

            dirScore = 0;
            for (int y = i-1; y >= 0; y--, dirScore++) {
                if (trees[y][j] >= trees[i][j]) {
                    dirScore++;
                    break;
                }
            }
            treeScore *= dirScore;

            dirScore = 0;
            for (int y = i+1; y < trees.size(); y++, dirScore++) {
                if (trees[y][j] >= trees[i][j]) {
                    dirScore++;
                    break;
                }
            }
            treeScore *= dirScore;

            if (treeScore > result) {
                result = treeScore;
            }
        }
    }
    return result;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 21, part1),
        std::make_tuple("Part 2"s, 8, part2)
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
