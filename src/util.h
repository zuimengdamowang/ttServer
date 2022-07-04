#pragma once
#include <string>
#include <vector>


void ErrorIf(bool condition, const char *msg);

void SetNonBlocking(int fd);

bool IsNonBlocking(int fd);

std::vector<std::string> Split(const std::string &str, const std::string &delim); 




