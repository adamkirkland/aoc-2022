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

typedef long answer_t;

typedef std::pair<int, int> range_t;

void mergeOverlaps(std::list<range_t>& existingOverlaps, range_t& overlap) {
    // Check to see if new overlap conflicts with any existing
    for (auto it = existingOverlaps.begin(); it != existingOverlaps.end(); ++it) {
        if (overlap.first > it->second || overlap.second < it->first)
            continue;

        auto existing = *it;
        if (overlap.first < existing.first) {
            existing.first = overlap.first;
        }
        if (overlap.second > existing.second) {
            existing.second = overlap.second;
        }
        existingOverlaps.erase(it);
        mergeOverlaps(existingOverlaps, existing);
        return;
    }

    existingOverlaps.push_back(overlap);
}

answer_t part1(std::istream& input, bool isTest = false) {
    answer_t result = 0;
    int targetRow = isTest ? 10 :  2000000;
    std::list<range_t> overlaps;  // Sections of the target row overlapped by sensor coverage
    std::set<int> beaconsOnTargetRow;

    std::string line;
    auto lineRe = std::regex(R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))");
    while (std::getline(input, line)) {
        std::smatch matches;
        if (!std::regex_search(line, matches, lineRe))
            continue;

        auto sX = stoi(matches[1].str()), sY = stoi(matches[2].str());
        auto bX = stoi(matches[3].str()), bY = stoi(matches[4].str());
        if (bY == targetRow)
            beaconsOnTargetRow.insert(bX);
        //std::cout << "Sensor at (" << sX << ", " << sY << ") and beacon at (" << bX << ", " << bY << ")\n";

        auto areaSize = abs(sX-bX) + abs(sY-bY);
        auto overlapRadius = areaSize - abs(targetRow - sY);
        if (overlapRadius < 0)
            continue;  // Sensor detection did not touch the target row

        range_t overlap = std::make_pair(sX - overlapRadius, sX + overlapRadius);
        //std::cout << "Overlap range of [" << overlap.first << " - " << overlap.second << "]\n";
        mergeOverlaps(overlaps, overlap);
    }

    for (auto& overlap : overlaps) {
        int coverage = overlap.second - overlap.first + 1;
        for (auto beacon : beaconsOnTargetRow) {
            if (beacon >= overlap.first && beacon <= overlap.second)
                coverage--;
        }
        result += coverage;
        //std::cout << "Overlap range of [" << overlap.first << " - " << overlap.second << "]\n";
    }

    return result;
}

answer_t part2(std::istream& input, bool isTest = false) {
    answer_t result = 0;
    std::vector<std::vector<int>> parsedInput;

    std::string line;
    auto lineRe = std::regex(R"(Sensor at x=(-?\d+), y=(-?\d+): closest beacon is at x=(-?\d+), y=(-?\d+))");
    while (std::getline(input, line)) {
        std::smatch matches;
        if (!std::regex_search(line, matches, lineRe))
            continue;

        auto sX = stoi(matches[1].str()), sY = stoi(matches[2].str());
        auto bX = stoi(matches[3].str()), bY = stoi(matches[4].str());
        parsedInput.push_back(std::vector<int>{sX, sY, bX, bY});
    }

    int minX = 0, maxX = isTest ? 20 : 4'000'000;

    for (int targetRow = minX; targetRow < maxX; targetRow++) {
        std::list<range_t> overlaps;
        for (auto& v : parsedInput) {
            auto areaSize = abs(v[0]-v[2]) + abs(v[1]-v[3]);
            auto overlapRadius = areaSize - abs(targetRow - v[1]);
            if (overlapRadius < 0)
                continue;  // Sensor detection did not touch the target row

            range_t overlap = std::make_pair(v[0] - overlapRadius, v[0] + overlapRadius);
            //std::cout << "Overlap range of [" << overlap.first << " - " << overlap.second << "]\n";
            mergeOverlaps(overlaps, overlap);
        }
        range_t merged = overlaps.front();
        for (auto it = std::next(overlaps.begin()); it != overlaps.end(); ++it) {
            if (it->first > merged.second + 1) {
                return long(merged.second+1) * 4'000'000 + targetRow;
            }
            else {
                merged.second = it->second;
            }
            //std::cout << "\tOverlap range of [" << overlap.first << " - " << overlap.second << "]\n";
        }
        if (merged.first > minX || merged.second < maxX) {
            return long(merged.first-1) * 4'000'000l + targetRow;
        }
    }

    return result;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, answer_t, answer_t (*)(std::istream& input, bool isTest)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 26, part1),
        std::make_tuple("Part 2"s, 56'000'011, part2)
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
