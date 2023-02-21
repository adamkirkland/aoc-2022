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
typedef std::pair<int, int> xy_t;
typedef std::vector<std::vector<char>> grid_t;
const char AIR = '.';
const char ROCK = '#';
const char SAND = 'o';
const char SOURCE = '+';

int parseInput(std::istream& input, grid_t& output, bool addFloor = false) {
    std::vector<std::vector<xy_t>> lineSegments;
    int minX = 500, minY = 0;
    int maxX = 500, maxY = 0;
    std::string line;
    while (std::getline(input, line)) {
        std::vector<xy_t> lineSegment;
        for (const auto lineTok : std::views::split(line, " -> "s)) {
            auto lineTokStr = std::string{lineTok.begin(), lineTok.end()};
            int x = -1;
            for (const auto numTok : std::views::split(lineTokStr, ',')) {
                auto num = stoi(std::string{numTok.begin(), numTok.end()});
                if (x == -1)
                    x = num;
                else {
                    minX = std::min<int>(minX, x);
                    maxX = std::max<int>(maxX, x);
                    maxY = std::max<int>(maxY, num);
                    lineSegment.push_back(std::make_pair(x, num));
                }
            }
        }
        lineSegments.push_back(lineSegment);
    }

    // Init the output grid
    if (addFloor) {
        maxY += 2;
        minX = 500 - (maxY+1);
        maxX = 500 + (maxY+1);
    }
    auto height = maxY - minY + 1, width = maxX - minX + 1;
    output.clear();
    output.resize(height, std::vector<char>(width, AIR));

    for (auto &rockLine : lineSegments) {
        if (rockLine.size() < 2)
            continue;
        xy_t &prev = rockLine[0];
        for (int i = 1; i < rockLine.size(); i++) {
            auto& curr = rockLine[i];
            for (int x = std::min<int>(curr.first, prev.first) - minX; x <= std::max<int>(curr.first, prev.first) - minX; x++) {
                output[curr.second-minY][x] = ROCK;
            }
            for (int y = std::min<int>(curr.second, prev.second) - minY; y <= std::max<int>(curr.second, prev.second) - minY; y++) {
                output[y][curr.first-minX] = ROCK;
            }
            prev = curr;
        }
    }

    // Final touches: source tile, and floor (if there is one)
    output[0][500-minX] = SOURCE;
    if (addFloor) {
        for (int i = 0; i < output[height-1].size(); i++) {
            output[height-1][i] = ROCK;
        }
    }
}

void printGrid(grid_t& grid) {
    for (int y = 0; y < grid.size(); y++) {
        auto& row = grid[y];
        for (int x = 0; x < row.size(); x++) {
            std::cout << row[x];
        }
        std::cout << std::endl;
    }
}

bool placeSand(grid_t& grid) {
    auto& topRow = grid[0];
    xy_t sand;
    for (int i = 0; i < topRow.size(); i++) {
        if (topRow[i] == SOURCE) {
            sand.first = i;
            sand.second = 0;
            break;
        }
    }

    while (true) {
        //std::cout << "in while loop, sand is [" << sand.first << ", " << sand.second << "]\n";
        if (sand.second == grid.size()-1)
            return false; // Sand is falling off the bottom
        if (grid[sand.second+1][sand.first] == AIR) {
            sand.second++;
            continue;
        }

        if (sand.first == 0)
            return false; // Sand is falling off the left side
        if (grid[sand.second+1][sand.first-1] == AIR) {
            sand.second++;
            sand.first--;
            continue;
        }

        if (sand.first == grid[0].size()-1)
            return false; // Sand is falling off the right side
        if (grid[sand.second+1][sand.first+1] == AIR) {
            sand.second++;
            sand.first++;
            continue;
        }

        // Has sand filled up all the way to the top?
        if (grid[sand.second][sand.first] == SOURCE) {
            return false;
        }

        // Sand has settled
        grid[sand.second][sand.first] = SAND;
        break;
    }
    return true;
}

answer_t part1(std::istream& input) {
    answer_t result = 0;

    grid_t grid;
    parseInput(input, grid);

    while (placeSand(grid)) {
        result++;
    }
    //printGrid(grid);

    return result;
}

answer_t part2(std::istream& input) {
    answer_t result = 0;

    grid_t grid;
    parseInput(input, grid, true);

    while (placeSand(grid)) {
        result++;
    }
    //printGrid(grid);

    return result + 1;  // 1 extra to account for the source tile
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, answer_t, answer_t (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 24, part1),
        std::make_tuple("Part 2"s, 93, part2)
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
