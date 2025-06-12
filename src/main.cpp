#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h> // For CreateProcess on Windows

#include "filesystem.hpp"
#include "input.hpp"
#include "system.hpp"
#include "text.hpp"

// Function to split a string by a delimiter
std::vector<std::string> split_string(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  std::istringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

// Function to execute external commands
void execute_command(const std::string &command,
                     const std::vector<std::string> &args) {
  if (command.empty()) {
    return;
  }

  // For demonstration, just print the command and arguments
  // std::cout << "Executing: " << command << std::endl;
  // for(const auto& arg : args) {
  //     std::cout << "Arg: " << arg << std::endl;
  // }

  // Prepare command line string
  std::string command_line = command;
  for (const auto &arg : args) {
    command_line += " " + arg;
  }

  STARTUPINFOA si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  ZeroMemory(&pi, sizeof(pi));

  std::vector<char> command_line_buffer(command_line.begin(),
                                        command_line.end());
  command_line_buffer.push_back('\0');

  // Start the child process.
  if (!CreateProcessA(NULL, // No module name (use command line)
                      command_line_buffer.data(), // Command line
                      NULL,  // Process handle not inheritable
                      NULL,  // Thread handle not inheritable
                      FALSE, // Set handle inheritance to FALSE
                      0,     // No creation flags
                      NULL,  // Use parent's environment block
                      NULL,  // Use parent's starting directory
                      &si,   // Pointer to STARTUPINFO structure
                      &pi)   // Pointer to PROCESS_INFORMATION structure
  ) {
    DWORD error = GetLastError();
    if (error == ERROR_FILE_NOT_FOUND) {
      fprintf(stderr,
              "%s: The term '%s' is not recognized as the name of a command, "
              "function, or operable program.\n",
              command.c_str(), command.c_str());
    } else {
      fprintf(stderr, "CreateProcess failed (%lu).\n", error);
    }
    return;
  }

  // Wait until child process exits.
  WaitForSingleObject(pi.hProcess, INFINITE);

  // Close process and thread handles.
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);
}

int main() {
  // Enable virtual terminal processing for colors
  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
  DWORD dwMode = 0;
  GetConsoleMode(hOut, &dwMode);
  dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  SetConsoleMode(hOut, dwMode);

  printf("Hi there! The Shell is running. v1.0 -- http://sebastiansebs.com\n");
  printf("A simple cross-platform shell implemented in C++\n\n");

  std::string input_line;
  std::vector<std::string> history;

  while (true) {
    char buffer[MAX_PATH];
    if (GetCurrentDirectoryA(MAX_PATH, buffer)) {
      printf("Shell %s> ", buffer);
    } else {
      printf("MyShell> ");
    }

    input_line = get_line_with_history(history);

    if (input_line.empty()) {
      continue;
    }

    history.push_back(input_line);

    std::vector<std::string> tokens = split_string(input_line, ' ');

    if (tokens.empty()) {
      continue;
    }

    std::string command = tokens[0];
    std::vector<std::string> arguments;
    if (tokens.size() > 1) {
      arguments.assign(tokens.begin() + 1, tokens.end());
    }

    if (command == "exit") {
      std::cout << "Exiting MyShell." << std::endl;
      break;
    } else if (command == "pwd") {
      char buffer[MAX_PATH];
      if (GetCurrentDirectoryA(MAX_PATH, buffer)) {
        printf("Path\n");
        printf("----\n");
        printf("%s\n", buffer);
      } else {
        fprintf(stderr, "pwd: could not get current directory (Error %lu)\n",
                GetLastError());
      }
    } else if (command == "echo") {
      for (size_t i = 0; i < arguments.size(); ++i) {
        std::cout << arguments[i] << (i == arguments.size() - 1 ? "" : " ");
      }
      std::cout << std::endl;
    }
    // Rudimentary cd command
    else if (command == "cd") {
      if (arguments.empty()) {
        std::cerr << "cd: missing argument" << std::endl;
      } else {
        if (SetCurrentDirectoryA(arguments[0].c_str())) {
          // Optionally print new directory or handle errors
        } else {
          fprintf(stderr,
                  "cd: could not change directory to '%s' (Error %lu)\n",
                  arguments[0].c_str(), GetLastError());
        }
      }
    } else if (command == "ls") {
      command_ls(arguments);
    } else if (command == "mkdir") {
      command_mkdir(arguments);
    } else if (command == "touch") {
      command_touch(arguments);
    } else if (command == "rm") {
      command_rm(arguments);
    } else if (command == "mv") {
      command_mv(arguments);
    } else if (command == "cp") {
      command_cp(arguments);
    } else if (command == "history") {
      for (size_t i = 0; i < history.size(); ++i) {
        printf(" %zu: %s\n", i + 1, history[i].c_str());
      }
    } else if (command == "date") {
      command_date(arguments);
    } else if (command == "hostname") {
      command_hostname(arguments);
    } else if (command == "cat") {
      command_cat(arguments);
    } else if (command == "grep") {
      command_grep(arguments);
    } else if (command == "head") {
      command_head(arguments);
    } else if (command == "tail") {
      command_tail(arguments);
    } else {
      execute_command(command, arguments);
    }
  }

  return 0;
}