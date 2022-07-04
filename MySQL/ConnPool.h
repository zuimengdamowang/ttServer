#pragma once

#include <iostream>
#include <mysql/mysql.h>
#include <queue>
#include <string>
#include <mutex>
#include <list>
#include <condition_variable>

using namespace std;



class ConnPool {
public:
    using size_type = unsigned int;

public:
    static ConnPool *GetConnPool();
    
    MYSQL *GetConn();
    bool ReleaseConn(MYSQL *conn);
    size_type GetCurConnNum();
    size_type GetFreeConnNum();

public:    
    ~ConnPool();

    ConnPool(const ConnPool &)              = delete;
    ConnPool & operator=(const ConnPool &)  = delete;

private:
    ConnPool();
    bool ParseJsonFile();
    void DestroyAllConn();

private:
    queue<MYSQL *, list<MYSQL *>> connections_;  // 连接池
    mutex mutex_;  // 互斥锁
    condition_variable cv_;  // 条件变量

private:
    size_type max_conn_num_{0};
    // size_type min_conn_num_{0};

    size_type cur_conn_num_{0};
    size_type free_conn_num_{0};

private:
    string      ip_;
    size_type   port_;
    string      user_;
    string      pwd_;
    string      database_name_;

};


