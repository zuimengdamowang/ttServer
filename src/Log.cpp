#include "Log.h"

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <sys/time.h>
#include <stdarg.h>
#include <iostream>

Log *
Log::get_instance() {
    static Log log;
    return &log;
}


Log::Log() {
    string file_name("ServerLog.txt");
    int log_buf_size = 8192;
    int split_lines = 500000;
    int max_queue_size = 16;

    m_log_queue = new BlockQueue<std::string>(max_queue_size);

    std::thread th(std::bind(&Log::flush_log_thread));
    th.detach();

    // 日志缓冲区大小 
    m_log_buf_size = log_buf_size;
    //日志的最大行数
    m_split_lines = split_lines;

    // 当前时间
    time_t t = time(nullptr);
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm   = *sys_tm;


    //从后往前找到第一个'/'的位置
    int p = file_name.rfind('/');
    string log_full_name;

    //若输入的文件名没有/，则直接将时间+文件名作为日志名
    if (p == -1) {
        log_full_name = to_string(my_tm.tm_year + 1900) + "_" + 
                        to_string(my_tm.tm_mon + 1) + "_" + 
                        to_string(my_tm.tm_mday) + "_" + 
                        file_name;
    } else {
        log_name = file_name.substr(p + 1, file_name.size() - p - 1);
        dir_name = file_name.substr(0, p - 0 + 1);
        log_full_name = dir_name + to_string(my_tm.tm_year + 1900) + "_" + 
                        to_string(my_tm.tm_mon + 1) + "_" +
                        to_string(my_tm.tm_mday) + "_" + 
                        log_name;
    }
    
    m_today = my_tm.tm_mday;

    ofs.open(log_full_name, ios::app);

}

Log::~Log() {
    if (ofs.is_open()) {
        ofs.close();
    }
    if (m_log_queue) {
        delete m_log_queue;
    }
}

void
Log::async_write_log() {
    std::string single_one;

    while (true) 
    {   
        single_one = m_log_queue->front();
        m_log_queue->pop();
        std::unique_lock<std::mutex> locker(m_mutex);
        ofs << single_one;
    }
}


void
Log::flush_log_thread() {
    Log::get_instance()->async_write_log();
}





void
Log::write_log(int level, const string &format) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    string s;
    
    // 日志分级
    switch (level)
    {
    case 0:
        s = std::move("[debug]:");
        break;
    case 1: 
        s = std::move("[info]:");
        break;
    case 2:
        s = std::move("[warn]:");
        break;
    case 3:
        s = std::move("[erro]:");
        break;
    
    default:
        s = std::move("[info]:");
        break;
    }

    {
        std::unique_lock<std::mutex> locker(m_mutex);

        // 现有行数+1
        ++m_count;

        // 日志不是今天或写入的日志行数是最大行的倍数
        if (m_today != my_tm.tm_mday || m_count % m_split_lines == 0) {
            string new_log;
            string tail;
            if (ofs.is_open()) {
                ofs.close();
            }
            //格式化日志名中的时间部分
            tail = to_string(my_tm.tm_year + 1900) + "_" +
                   to_string(my_tm.tm_mon + 1) + "_" + 
                   to_string(my_tm.tm_mday) + "_";
            
           

            // 如果是时间不是今天,则创建今天的日志，更新m_today和m_count
            if (m_today != my_tm.tm_mday) {
                new_log = dir_name + tail + log_name;
                m_today = my_tm.tm_mday;
                m_count = 0;
            } else {
                // 超过了最大行，在之前的日志名基础上加后缀, m_count/m_split_lines
                new_log = dir_name + tail + log_name + "." + to_string((long long)(m_count / m_split_lines));
            }
            ofs.open(new_log, ios::app);
        }

    }
    
   

    std::string log_str;
    
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        //写入的具体时间内容格式
        m_buf = to_string(my_tm.tm_year + 1900) + "-" + 
                to_string(my_tm.tm_mon + 1) + "-" +
                to_string(my_tm.tm_mday) + "-" +
                to_string(my_tm.tm_hour) + ":" + to_string(my_tm.tm_min) + ":" + to_string(my_tm.tm_sec) + "." +to_string(now.tv_usec) + " " + 
                s;

        m_buf += format;
        m_buf += "\n\0";

        log_str = m_buf;
    }
    m_log_queue->push(log_str);


}








void LOG_INFO(const std::string &info) {
    Log::get_instance()->write_log(1, info);
}







// bool
// Log::init(const char *file_name,
//           int log_buf_size, 
//           int split_lines, 
//           int max_queue_size) {

//     m_log_queue = new BlockQueue<std::string>(max_queue_size);

//     std::thread th(std::bind(&Log::flush_log_thread));
//     th.detach();


//     m_log_buf_size = log_buf_size;
    


//     //日志的最大行数
//     m_split_lines = split_lines;

//     // 当前时间
//     time_t t = time(nullptr);
//     struct tm *sys_tm = localtime(&t);
//     struct tm my_tm   = *sys_tm;


//     //从后往前找到第一个'/'的位置
//     const char *p = strrchr(file_name, '/');
//     char log_full_name[256] = {0};

//     //若输入的文件名没有/，则直接将时间+文件名作为日志名
//     if (!p) {
//         log_full_name = to_string(my_tm.tm_year + 1900);
//         snprintf(log_full_name, 255, "%d_%02d_%02d_%s", my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, file_name);
//     } else {
//         strcpy(log_name, p + 1);
//         strncpy(dir_name, file_name, p - file_name + 1);
//         snprintf(log_full_name, 255, "%s%d_%02d_%02d_%s", dir_name, my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday, log_name);
//     }
    
    

//     m_today = my_tm.tm_mday;

//     m_fp = fopen(log_full_name, "a");
//     if (m_fp == nullptr) {
//         perror("file open error\n");
//         return false;
//     }
//     return true;
// }








