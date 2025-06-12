#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>

void command_ls(const std::vector<std::string> &args);
void command_mkdir(const std::vector<std::string> &args);
void command_rm(const std::vector<std::string> &args);
void command_touch(const std::vector<std::string> &args);
void command_cp(const std::vector<std::string> &args);
void command_mv(const std::vector<std::string> &args);

#endif // FILESYSTEM_HPP