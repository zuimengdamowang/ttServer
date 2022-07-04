#include "Buffer.h"
#include <string.h>
#include <iostream>




void Buffer::Append(const char* _str, int _size){
    for(int i = 0; i < _size; ++i){
        // if(_str[i] == '\0') break;
        m_buf_.push_back(_str[i]);
    }
}
void Buffer::Append(const std::string &_str) {
    m_buf_ = _str;
}

ssize_t Buffer::Size(){
    return m_buf_.size();
}

const char* Buffer::ToStr(){
    return m_buf_.c_str();
}

void Buffer::Clear(){
    m_buf_.clear();
}

void Buffer::Getline(){
    m_buf_.clear();
    std::getline(std::cin, m_buf_);
}

void Buffer::SetBuf(const char* buf){
    m_buf_.clear();
    m_buf_.append(buf);
}

void Buffer::SetBuf(const std::string &str) {
    m_buf_.clear();
    m_buf_.append(str);
}

bool Buffer::Empty() {
    return m_buf_.empty();
}

std::string Buffer::GetStr() {
    return m_buf_;
}


