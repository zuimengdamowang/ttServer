#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <condition_variable>
#include <string>

#include "LogBlockQueue.h"

using namespace std;


class Log {
public:
    static Log *get_instance();
    static void flush_log_thread();
public:
    // bool init(const char *file_name, int log_buf_size=8192, int split_lines=5000000, int max_queue_size=0);
    void write_log(int level, const string &format);


private:
    Log();
    ~Log();
    void async_write_log();

private:
    string dir_name;       //路径名
    string log_name;       //log文件名
    int m_split_lines;     //日志最大行数
    int m_log_buf_size;    //日志缓冲区大小 
    long long m_count;     //日志行数记录
    int m_today;
    ofstream ofs;
    std::string m_buf;
    BlockQueue<std::string> *m_log_queue{nullptr};
    std::mutex m_mutex;

};




void LOG_INFO(const std::string &info);


