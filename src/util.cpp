#include <iostream>
#include <fcntl.h>


#include "util.h"


void ErrorIf(bool condition, const char *msg) {
    if (condition) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
}

void SetNonBlocking(int fd) {
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}


bool IsNonBlocking(int fd) {
    return (fcntl(fd, F_GETFL) & O_NONBLOCK) != 0;
}


std::vector<std::string> Split(const std::string &str, const std::string &delim) {
    std::vector<std::string> tokens;
    auto start = str.find_first_not_of(delim, 0);       // 分割到的字符串的第一个字符
    auto position = str.find_first_of(delim, start);    // 分隔符的位置
    while (position != std::string::npos || start != std::string::npos) {
        // [start, position) 为分割下来的字符串
        tokens.emplace_back(std::move(str.substr(start, position - start)));
        start = str.find_first_not_of(delim, position);
        position = str.find_first_of(delim, start);
    }
    return tokens;
}


