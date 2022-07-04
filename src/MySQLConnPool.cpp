#include "MySQLConnPool.h"
#include "util.h"

#include <jsoncpp/json/json.h>
#include <fstream>




ConnPool *
ConnPool::GetConnPool() {
    static ConnPool pool;
    return &pool;
}

//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MYSQL *
ConnPool::GetConn() {
    MYSQL *conn = nullptr;
    {
        unique_lock<mutex> locker(mutex_);  // 加锁
        cv_.wait(locker, [this]() {return !connections_.empty();});  // 当connection_为空时阻塞
        conn = connections_.front();
        connections_.pop();
        ++cur_conn_num_;
        --free_conn_num_;
    }
    return conn;
}


bool 
ConnPool::ReleaseConn(MYSQL *conn) {
    if (conn == nullptr) {
        return false;
    }

    {
        unique_lock<mutex> locker;
        connections_.push(conn);

        ++free_conn_num_;
        --cur_conn_num_;   
    }
    cv_.notify_one();
    return true;
}

ConnPool::size_type
ConnPool::GetCurConnNum() {
    return cur_conn_num_;
}

ConnPool::size_type
ConnPool::GetFreeConnNum() {
    return free_conn_num_;
}


ConnPool::ConnPool() {
    // 加载json文件
    ErrorIf(ParseJsonFile() == false, "load json file error");
    
    // 创建连接池
    for (int i = 0; i < max_conn_num_; ++i) {
        MYSQL *conn = mysql_init(nullptr);
        ErrorIf(conn == nullptr, "mysql_init error: ");
        // std::cout << ip_.c_str() << ", " << user_.c_str() << ", "
        //           << pwd_.c_str() << ", " << database_name_.c_str() << ", "
        //           << port_ 
        //           << std::endl;
        conn = mysql_real_connect(
            conn, 
            ip_.c_str(), 
            user_.c_str(), 
            pwd_.c_str(), 
            database_name_.c_str(), 
            port_, 
            nullptr, 0
        );
        ErrorIf(conn == nullptr, "mysql_real_connect() error: ");
        mysql_set_character_set(conn, "utf8");
        std::cout << "mysql connection " << i << " build success!" << std::endl; 

        connections_.push(conn);
        ++free_conn_num_;
    }
    
}



bool
ConnPool::ParseJsonFile() {
    ifstream ifs("/home/boyuan/MyServer/MySQL/config.json");
    ErrorIf(ifs.is_open() == false, "ifs open error");
    Json::Reader rd;
    Json::Value root;
    rd.parse(ifs, root);
    if (root.isObject()) {
        ip_    = root["ip"].asString();
        port_  = root["port"].asUInt();
        user_  = root["user"].asString();
        pwd_   = root["pwd"].asString();
        database_name_     = root["database"].asString(); 
        max_conn_num_      = root["maxConnSize"].asUInt();
        // min_conn_num_      = root["maxSize"].asUInt();
        return true;
    }
    return false;
}


void
ConnPool::DestroyAllConn() {
    cv_.notify_one();
    while (!connections_.empty()) {
        auto conn = connections_.front();
        connections_.pop();
        mysql_close(conn);
    }
    cur_conn_num_ = 0;
    free_conn_num_ = 0;
}

ConnPool::~ConnPool() {
    DestroyAllConn();
}



