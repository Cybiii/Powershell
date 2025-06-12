#include "system.hpp"
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

void command_date(const std::vector<std::string> &args) {
  if (!args.empty()) {
    fprintf(stderr, "date: does not take any arguments\n");
    return;
  }

  auto now = std::chrono::system_clock::now();
  std::time_t now_time = std::chrono::system_clock::to_time_t(now);

  char time_buf[26];
  ctime_s(time_buf, sizeof(time_buf), &now_time);

  printf("%s", time_buf);
}

void command_hostname(const std::vector<std::string> &args) {
  if (!args.empty()) {
    fprintf(stderr, "hostname: does not take any arguments\n");
    return;
  }

  char buffer[MAX_COMPUTERNAME_LENGTH + 1];
  DWORD size = sizeof(buffer);

  if (GetComputerNameA(buffer, &size)) {
    printf("%s\n", buffer);
  } else {
    fprintf(stderr, "hostname: could not get computer name (Error %lu)\n",
            GetLastError());
  }
}