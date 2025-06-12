#include "input.hpp"
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>


void clear_line() {
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  GetConsoleScreenBufferInfo(hConsole, &csbi);
  DWORD count;
  DWORD cellCount = csbi.dwSize.X;
  COORD homeCoords = {0, csbi.dwCursorPosition.Y};

  FillConsoleOutputCharacter(hConsole, (TCHAR)' ', cellCount, homeCoords,
                             &count);
  FillConsoleOutputAttribute(hConsole, csbi.wAttributes, cellCount, homeCoords,
                             &count);
  SetConsoleCursorPosition(hConsole, homeCoords);
}

void print_prompt() {
  char buffer[MAX_PATH];
  if (GetCurrentDirectoryA(MAX_PATH, buffer)) {
    printf("Shell %s> ", buffer);
  } else {
    printf("MyShell> ");
  }
}

// Helper to convert string to lowercase
std::string to_lower(const std::string &str) {
  std::string lower_str = str;
  std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return lower_str;
}

std::vector<std::string> get_completions(const std::string &partial) {
  std::vector<std::string> matches;
  std::string search_path = ".\\*";
  WIN32_FIND_DATAA findFileData;
  HANDLE hFind = FindFirstFileA(search_path.c_str(), &findFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    return matches;
  }

  std::string lower_partial = to_lower(partial);

  do {
    std::string filename = findFileData.cFileName;
    std::string lower_filename = to_lower(filename);
    if (lower_filename.rfind(lower_partial, 0) ==
        0) { // Check if filename starts with partial
      matches.push_back(filename);
    }
  } while (FindNextFileA(hFind, &findFileData) != 0);

  FindClose(hFind);
  return matches;
}

std::string get_line_with_history(const std::vector<std::string> &history) {
  HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
  INPUT_RECORD irInBuf[128];
  DWORD cNumRead;
  std::string current_line;
  int history_index = history.size();

  while (true) {
    if (!ReadConsoleInput(hConsole, irInBuf, 1, &cNumRead)) {
      return ""; // Error
    }

    if (cNumRead > 0 && irInBuf[0].EventType == KEY_EVENT &&
        irInBuf[0].Event.KeyEvent.bKeyDown) {
      KEY_EVENT_RECORD ker = irInBuf[0].Event.KeyEvent;

      if (ker.wVirtualKeyCode == VK_RETURN) {
        printf("\n");
        break;
      } else if (ker.wVirtualKeyCode == VK_BACK) {
        if (!current_line.empty()) {
          current_line.pop_back();
          clear_line();
          print_prompt();
          printf("%s", current_line.c_str());
        }
      } else if (ker.wVirtualKeyCode == VK_UP) {
        if (!history.empty() && history_index > 0) {
          history_index--;
          current_line = history[history_index];
          clear_line();
          print_prompt();
          printf("%s", current_line.c_str());
        }
      } else if (ker.wVirtualKeyCode == VK_DOWN) {
        if (!history.empty() && history_index < history.size() - 1) {
          history_index++;
          current_line = history[history_index];
          clear_line();
          print_prompt();
          printf("%s", current_line.c_str());
        } else if (history_index == history.size() - 1) {
          history_index++;
          current_line = "";
          clear_line();
          print_prompt();
        }
      } else if (ker.wVirtualKeyCode == VK_TAB) {
        size_t last_space = current_line.find_last_of(" \t");
        std::string to_complete = (last_space == std::string::npos)
                                      ? current_line
                                      : current_line.substr(last_space + 1);

        if (!to_complete.empty()) {
          std::vector<std::string> matches = get_completions(to_complete);
          if (matches.size() == 1) {
            std::string completed = matches[0];
            if (last_space != std::string::npos) {
              current_line = current_line.substr(0, last_space + 1) + completed;
            } else {
              current_line = completed;
            }
            clear_line();
            print_prompt();
            printf("%s", current_line.c_str());
          }
        }
      } else if (ker.uChar.AsciiChar >= ' ') {
        current_line += ker.uChar.AsciiChar;
        printf("%c", ker.uChar.AsciiChar);
      }
    }
  }
  return current_line;
}