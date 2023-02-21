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

typedef int answer_t;
typedef std::pair<int, int> coord_t;
typedef std::vector<int> row_t;
typedef std::vector<row_t> grid_t;

std::tuple<grid_t, coord_t, coord_t> parseInput(std::istream& input) {
    grid_t map;
    coord_t start, end;

    std::string line;
    while (std::getline(input, line)) {
        row_t row(line.size());
        for (auto i = 0u; i < line.size(); i++) {
            row[i] = line[i] - 'a';
            if (line[i] == 'S') {
                row[i] = 0;
                start = std::make_pair(map.size(), i);
            }
            else if (line[i] == 'E') {
                row[i] = 25;
                end = std::make_pair(map.size(), i);
            }
        }
        map.push_back(row);
    }
    //std::cout << "Start is at [" << start.first << ", " << start.second << "]\n";
    //std::cout << "End is at [" << end.first << ", " << end.second << "]\n";
    return std::make_tuple(map, start, end);
}

std::vector<coord_t> findNeighbors(coord_t coord, int numRows, int numCols) {
    std::vector<coord_t> result;
    if (coord.first > 0)
        result.push_back(std::make_pair(coord.first-1, coord.second));
    if (coord.first < numRows-1)
        result.push_back(std::make_pair(coord.first+1, coord.second));
    if (coord.second > 0)
        result.push_back(std::make_pair(coord.first, coord.second-1));
    if (coord.first < numCols-1)
        result.push_back(std::make_pair(coord.first, coord.second+1));
    return result;
}

answer_t part1(std::istream& input) {
    const auto parsed = parseInput(input);
    const auto grid = std::get<0>(parsed);
    const auto start = std::get<1>(parsed), end = std::get<2>(parsed);

    grid_t pathMap(grid.size(), row_t(grid[0].size(), -1));
    pathMap[end.first][end.second] = 0;
    std::queue<coord_t> nodes;
    nodes.push(end);

    while (pathMap[start.first][start.second] == -1) {
        auto node = nodes.front(); 
        nodes.pop();
        auto distance = pathMap[node.first][node.second];
        auto height = grid[node.first][node.second];
        //std::cout << "---\nConsidering square [" << node.first << ", " << node.second << "] with height " << height << ", distance " << distance << std::endl;
        auto neighbors = findNeighbors(node, grid.size(), grid[0].size());
        for (auto &n : neighbors) {
            if (grid[n.first][n.second] >= height-1 && pathMap[n.first][n.second] == -1) {
                //std::cout << "\tneighbor [" << n.first << ", " << n.second << "] of height " << grid[n.first][n.second] << " has new distance " << distance+1 << std::endl;
                pathMap[n.first][n.second] = distance + 1;
                nodes.push(n);
            }
        }
    }
    return pathMap[start.first][start.second];
}

answer_t part2(std::istream& input) {
    const auto parsed = parseInput(input);
    const auto grid = std::get<0>(parsed);
    const auto start = std::get<1>(parsed), end = std::get<2>(parsed);

    grid_t pathMap(grid.size(), row_t(grid[0].size(), -1));
    pathMap[end.first][end.second] = 0;
    std::queue<coord_t> nodes;
    nodes.push(end);
    int answer = -1;

    while (answer == -1) {
        auto node = nodes.front(); 
        nodes.pop();
        auto distance = pathMap[node.first][node.second];
        auto height = grid[node.first][node.second];
        //std::cout << "---\nConsidering square [" << node.first << ", " << node.second << "] with height " << height << ", distance " << distance << std::endl;
        auto neighbors = findNeighbors(node, grid.size(), grid[0].size());
        for (auto &n : neighbors) {
            if (grid[n.first][n.second] >= height-1 && pathMap[n.first][n.second] == -1) {
                //std::cout << "\tneighbor [" << n.first << ", " << n.second << "] of height " << grid[n.first][n.second] << " has new distance " << distance+1 << std::endl;
                pathMap[n.first][n.second] = distance + 1;
                nodes.push(n);
                if (grid[n.first][n.second] == 0) {
                    answer = distance + 1;
                }
            }
        }
    }
    return answer;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, answer_t, answer_t (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 31, part1),
        std::make_tuple("Part 2"s, 29, part2)
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
