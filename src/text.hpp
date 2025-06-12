#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>
#include <vector>

void command_cat(const std::vector<std::string> &args);
void command_grep(const std::vector<std::string> &args);
void command_head(const std::vector<std::string> &args);
void command_tail(const std::vector<std::string> &args);

#endif // TEXT_HPP