#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h> // For CreateProcess on Windows

#include "filesystem.hpp"

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
    std::cerr << "CreateProcess failed (" << GetLastError() << ").\n";
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

  std::string input_line;

  while (true) {
    std::cout << "MyShell> ";
    if (!std::getline(std::cin, input_line)) {
      break; // Handle EOF (Ctrl+D or Ctrl+Z)
    }

    if (input_line.empty()) {
      continue;
    }

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
        std::cout << buffer << std::endl;
      } else {
        std::cerr << "pwd: could not get current directory (Error "
                  << GetLastError() << ")" << std::endl;
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
          std::cerr << "cd: could not change directory to " << arguments[0]
                    << " (Error " << GetLastError() << ")" << std::endl;
        }
      }
    } else if (command == "ls") {
      command_ls(arguments);
    } else {
      execute_command(command, arguments);
    }
  }

  return 0;
}