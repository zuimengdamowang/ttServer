#pragma once

#include <string>

class Buffer
{
private:
    std::string m_buf_;
public:
    Buffer() = default;
    ~Buffer() = default;
    
    void Append(const std::string &str);
    void Append(const char* str, int size);

    std::string GetStr();

    ssize_t Size();
    const char* ToStr();
    void Clear();
    void Getline();
    bool Empty();

    void SetBuf(const char* buf);
    void SetBuf(const std::string &str);
};