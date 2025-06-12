#include "filesystem.hpp"
#include <cstdio> // For printf
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>

// ANSI escape codes for colors
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

std::string GetModeString(DWORD dwFileAttributes) {
  std::string mode = "------";
  if (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    mode[0] = 'd';
  if (dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
    mode[1] = 'a';
  if (dwFileAttributes & FILE_ATTRIBUTE_READONLY)
    mode[2] = 'r';
  if (dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
    mode[3] = 'h';
  if (dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
    mode[4] = 's';
  return mode;
}

std::string FileTimeToString(const FILETIME &ft) {
  SYSTEMTIME stUTC, stLocal;
  char buffer[128];

  FileTimeToSystemTime(&ft, &stUTC);
  SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

  char dateStr[32];
  sprintf_s(dateStr, "%d/%d/%d", stLocal.wMonth, stLocal.wDay, stLocal.wYear);

  char timeStr[32];
  sprintf_s(timeStr, "%d:%02d %s",
            (stLocal.wHour % 12 == 0) ? 12 : stLocal.wHour % 12,
            stLocal.wMinute, (stLocal.wHour < 12) ? "AM" : "PM");

  sprintf_s(buffer, "%-14s %s", dateStr, timeStr);

  return buffer;
}

void list_directory(const std::string &path) {
  WIN32_FIND_DATAA findFileData;
  HANDLE hFind = INVALID_HANDLE_VALUE;

  std::string searchPath = path + "\\*";
  hFind = FindFirstFileA(searchPath.c_str(), &findFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
    fprintf(stderr,
            "ls: cannot access '%s': No such file or directory. Error %lu\n",
            path.c_str(), GetLastError());
    return;
  }

  printf("%-10s %-22s %12s %s\n", "Mode", "LastWriteTime", "Length", "Name");
  printf("%-10s %-22s %12s %s\n", "----", "-------------", "------", "----");

  do {
    const std::string filename = findFileData.cFileName;
    if (filename == "." || filename == "..") {
      continue;
    }

    std::string mode_str = GetModeString(findFileData.dwFileAttributes);
    std::string time_str = FileTimeToString(findFileData.ftLastWriteTime);

    if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      printf("%-10s %-22s %12s " ANSI_COLOR_CYAN "%s/" ANSI_COLOR_RESET "\n",
             mode_str.c_str(), time_str.c_str(),
             "", // No length for directories
             filename.c_str());
    } else {
      ULONGLONG file_size = ((ULONGLONG)findFileData.nFileSizeHigh << 32) |
                            findFileData.nFileSizeLow;
      char size_str[32];
      sprintf_s(size_str, sizeof(size_str), "%llu", file_size);

      printf("%-10s %-22s %12s %s\n", mode_str.c_str(), time_str.c_str(),
             size_str, filename.c_str());
    }
  } while (FindNextFileA(hFind, &findFileData) != 0);

  FindClose(hFind);

  DWORD dwError = GetLastError();
  if (dwError != ERROR_NO_MORE_FILES) {
    fprintf(stderr, "ls: FindNextFileA failed with error %lu\n", dwError);
  }
}

void command_mkdir(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "mkdir: missing operand\n");
    return;
  }
  for (const auto &dir_name : args) {
    if (!CreateDirectoryA(dir_name.c_str(), NULL)) {
      fprintf(stderr, "mkdir: cannot create directory '%s': Error %lu\n",
              dir_name.c_str(), GetLastError());
    }
  }
}

void command_rm(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "rm: missing operand\n");
    return;
  }

  for (const auto &file_name : args) {
    DWORD attrs = GetFileAttributesA(file_name.c_str());
    if (attrs == INVALID_FILE_ATTRIBUTES) {
      fprintf(stderr, "rm: cannot remove '%s': No such file or directory\n",
              file_name.c_str());
      continue;
    }

    if (attrs & FILE_ATTRIBUTE_DIRECTORY) {
      // It's a directory, try to remove it with RemoveDirectoryA
      if (!RemoveDirectoryA(file_name.c_str())) {
        fprintf(stderr,
                "rm: cannot remove '%s': Directory not empty or permission "
                "denied. Error %lu\n",
                file_name.c_str(), GetLastError());
      }
    } else {
      // It's a file, use DeleteFileA
      if (!DeleteFileA(file_name.c_str())) {
        fprintf(stderr,
                "rm: cannot remove '%s': Permission denied. Error %lu\n",
                file_name.c_str(), GetLastError());
      }
    }
  }
}

void command_touch(const std::vector<std::string> &args) {
  if (args.empty()) {
    fprintf(stderr, "touch: missing file operand\n");
    return;
  }
  for (const auto &file_name : args) {
    HANDLE hFile = CreateFileA(file_name.c_str(), GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
      fprintf(stderr, "touch: cannot touch '%s': Error %lu\n",
              file_name.c_str(), GetLastError());
    } else {
      CloseHandle(hFile);
    }
  }
}

void command_cp(const std::vector<std::string> &args) {
  if (args.size() != 2) {
    fprintf(stderr, "cp: missing destination file operand after '%s'\n",
            args.empty() ? "file" : args[0].c_str());
    return;
  }
  const std::string &source = args[0];
  const std::string &dest = args[1];

  if (!CopyFileA(source.c_str(), dest.c_str(), FALSE)) { // FALSE to overwrite
    fprintf(stderr, "cp: cannot copy '%s' to '%s': Error %lu\n", source.c_str(),
            dest.c_str(), GetLastError());
  }
}

void command_mv(const std::vector<std::string> &args) {
  if (args.size() != 2) {
    fprintf(stderr, "mv: missing destination file operand after '%s'\n",
            args.empty() ? "file" : args[0].c_str());
    return;
  }
  const std::string &source = args[0];
  const std::string &dest = args[1];

  if (!MoveFileA(source.c_str(), dest.c_str())) {
    fprintf(stderr, "mv: cannot move '%s' to '%s': Error %lu\n", source.c_str(),
            dest.c_str(), GetLastError());
  }
}

void command_ls(const std::vector<std::string> &args) {
  if (args.empty()) {
    list_directory(".");
  } else {
    // A more robust implementation could handle multiple paths or flags.
    // For now, we handle the first path provided.
    list_directory(args[0]);
  }
}