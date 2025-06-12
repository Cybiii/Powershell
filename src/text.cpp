#include "text.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// Helper function to read a file into a vector of strings
bool read_file_lines(const std::string &path, std::vector<std::string> &lines) {
  std::ifstream file(path);
  if (!file) {
    return false;
  }
  std::string line;
  while (std::getline(file, line)) {
    lines.push_back(line);
  }
  return true;
}

void command_cat(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "cat: missing file operand\n");
    return;
  }

  for (const auto &file_path : args) {
    std::vector<std::string> lines;
    if (!read_file_lines(file_path, lines)) {
      fprintf(stderr, "cat: %s: No such file or directory\n",
              file_path.c_str());
      continue;
    }
    for (const auto &line : lines) {
      printf("%s\n", line.c_str());
    }
  }
}

void command_grep(const std::vector<std::string> &args) {
  if (args.size() < 2) {
    fprintf(stderr, "grep: usage: grep [pattern] [file]\n");
    return;
  }

  const std::string &pattern = args[0];
  const std::string &file_path = args[1];

  std::vector<std::string> lines;
  if (!read_file_lines(file_path, lines)) {
    fprintf(stderr, "grep: %s: No such file or directory\n", file_path.c_str());
    return;
  }

  for (const auto &line : lines) {
    if (line.find(pattern) != std::string::npos) {
      printf("%s\n", line.c_str());
    }
  }
}

void command_head(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "head: missing file operand\n");
    return;
  }

  long long n = 10;
  std::string file_path;

  if (args.size() > 1 && args[0] == "-n") {
    if (args.size() < 3) {
      fprintf(stderr, "head: option requires an argument -- 'n'\n");
      return;
    }
    try {
      n = std::stoll(args[1]);
    } catch (...) {
      fprintf(stderr, "head: invalid number of lines: '%s'\n", args[1].c_str());
      return;
    }
    file_path = args[2];
  } else {
    file_path = args[0];
  }

  std::vector<std::string> lines;
  if (!read_file_lines(file_path, lines)) {
    fprintf(stderr, "head: %s: No such file or directory\n", file_path.c_str());
    return;
  }

  for (long long i = 0; i < std::min((long long)lines.size(), n); ++i) {
    printf("%s\n", lines[i].c_str());
  }
}

void command_tail(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "tail: missing file operand\n");
    return;
  }

  long long n = 10;
  std::string file_path;

  if (args.size() > 1 && args[0] == "-n") {
    if (args.size() < 3) {
      fprintf(stderr, "tail: option requires an argument -- 'n'\n");
      return;
    }
    try {
      n = std::stoll(args[1]);
    } catch (...) {
      fprintf(stderr, "tail: invalid number of lines: '%s'\n", args[1].c_str());
      return;
    }
    file_path = args[2];
  } else {
    file_path = args[0];
  }

  std::vector<std::string> lines;
  if (!read_file_lines(file_path, lines)) {
    fprintf(stderr, "tail: %s: No such file or directory\n", file_path.c_str());
    return;
  }

  long long start = std::max(0LL, (long long)lines.size() - n);
  for (size_t i = start; i < lines.size(); ++i) {
    printf("%s\n", lines[i].c_str());
  }
}