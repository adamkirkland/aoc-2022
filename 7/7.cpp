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

class File {
public:
    std::string name;
    int size;

    File(std::string n, int s) : name(n), size(s) {}
};

class Directory {
public:
    std::string name;
    std::vector<std::shared_ptr<File>> files;
    std::vector<std::shared_ptr<Directory>> dirs;
    std::weak_ptr<Directory> parent;

    Directory(std::string n): name(n) {}

    int size() {
        int result = 0;
        for (const auto &dir : dirs) {
            result += dir->size();
        }
        for (const auto &file : files) {
            result += file->size;
        }
        return result;
    }

    int totalSizeOfSubdirsUnder(int limit) {
        int result = 0;
        for (const auto &dir : dirs) {
            result += dir->totalSizeOfSubdirsUnder(limit);
            const auto size = dir->size();
            if (size < limit) {
                result += size;
            }
        }
        return result;
    }

    int smallestDirAtLeast(int s) {
        int result = this->size();
        if (result < s) {
            return -1;
        }

        for (const auto &dir : dirs) {
            const auto smallest = dir->smallestDirAtLeast(s);
            if (smallest > -1 && smallest < result) {
                result = smallest;
            }
        }
        return result;
    }
};

// I can't believe I have to write my own function for this
std::vector<std::string> splitString(std::string s, char delim) {
    std::vector<std::string> result;
    for (const auto tok : std::views::split(s, delim)) {
        result.push_back(std::string{tok.begin(), tok.end()});
    }
    return result;
}

std::shared_ptr<Directory> parseFileStructure(std::istream& input) {
    auto cmdRe = std::regex(R"(\$ ([^ ]+) ?(.*))");
    auto root = std::make_shared<Directory>("/"s);
    auto cwd = root;

    std::string line;
    while (std::getline(input, line)) {
        std::smatch matches;
        if (std::regex_search(line, matches, cmdRe)) {
            auto cmd = matches[1].str();
            auto arg = matches[2].str();
            // cmd 'ls' won't have an arg, but we can actually just ignore the ls lines
            if (cmd == "cd"s) {
                if (arg == "/"s) {
                    cwd = root;
                }
                else if (arg == ".."s) {
                    // Move to parent
                    cwd = cwd->parent.lock();
                    if (!cwd) {
                        std::cerr << "Unable to navigate backwards - no parent found\n";
                    }
                }
                else {
                    // Move to sub-dir
                    const auto existing = std::find_if(
                        cwd->dirs.begin(),
                        cwd->dirs.end(),
                        [arg](const auto &dir) { return dir->name == arg; }
                    );
                    if (existing == cwd->dirs.end()) {
                        std::cerr << "Unable to find sub-directory " << arg << std::endl;
                    }
                    else {
                        cwd = *existing;
                    }
                }
            }
        }
        else {
            const auto toks = splitString(line, ' ');
            if (toks[0] == "dir"s) {
                const auto existing = std::find_if(
                    cwd->dirs.begin(),
                    cwd->dirs.end(),
                    [toks](const auto &d) { return d->name == toks[1]; }
                );
                if (existing == cwd->dirs.end()) {
                    // Need to make directory
                    auto dir = std::make_shared<Directory>(toks[1]);
                    dir->parent = cwd;
                    cwd->dirs.push_back(dir);
                }
            }
            else {
                cwd->files.push_back(std::make_shared<File>(toks[1], stoi(toks[0])));
            }
        }
    }
    return root;
}

int part1(std::istream& input) {
    auto root = parseFileStructure(input);
    return root->totalSizeOfSubdirsUnder(100000);
}

int part2(std::istream& input) {
    auto root = parseFileStructure(input);
    const int total = 70'000'000;
    const int unused = total - root->size();
    return root->smallestDirAtLeast(30'000'000 - unused);
    
    return 0;
}

int main(int argc, char* argv[]) {
    using Problem = std::tuple<std::string, int, int (*)(std::istream& input)>;
    auto problemPairs = std::vector<Problem> {
        std::make_tuple("Part 1"s, 95'437, part1),
        std::make_tuple("Part 2"s, 24'933'642, part2)
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
