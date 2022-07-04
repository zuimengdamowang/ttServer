#include "HttpConn.h"
#include "util.h"
#include "Log.h"
#include "MySQLConnPool.h"

#include <functional>
#include <regex>
#include <unordered_set>
#include <unordered_map>

#include<mysql/mysql.h>


const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const std::unordered_map<std::string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };



bool
HttpRequest::Parse(const std::string &buf) {
    const std::string CRLF("\r\n");
    std::vector<std::string> lines = Split(buf, CRLF);
    int idx = 0;
    while (idx < lines.size() && state_ != FINISH) {
        std::string line = lines[idx++];
        switch (state_)
        {
        case REQUIRE_LINE:
            if (!ParseRequestLine(line)) {
                return false;
            }
            ParsePath();
            break;

        case HEADERS:
            ParseHeader(line);
            break;

        case BODY:
            ParseBody(line);
            break;
        default:
            break;
        }
    }
}


bool
HttpRequest::ParseRequestLine(const std::string &line) {
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if(regex_match(line, subMatch, patten)) {   
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    return false;
}

void
HttpRequest::ParseHeader(const std::string &line) {
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(regex_match(line, subMatch, patten)) {
        header_[subMatch[1]] = subMatch[2];
    }
    else {
        state_ = BODY;
    }
}

void
HttpRequest::ParseBody(const std::string &line) {
    body_ = line;
    ParsePost();
    state_ = FINISH;
}





void
HttpRequest::ParsePost() {
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-form-urlencoded") {
        ParseFromUrlencoded();
        if(DEFAULT_HTML_TAG.count(path_)) {
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            if(tag == 0 || tag == 1) {
                bool isLogin = (tag == 1);
                if(UserVerify(post_["username"], post_["password"], isLogin)) {
                    path_ = "/welcome.html";
                } 
                else {
                    path_ = "/error.html";
                }
            }
        }
    }
}



bool
HttpRequest::UserVerify(const std::string& name, const std::string& pwd, bool isLogin) {
    if(name == "" || pwd == "") { 
        return false; 
    }
    LOG_INFO("Verify name:" + name + ", pwd" + pwd);
    MYSQL* sql = ConnPool::GetConnPool()->GetConn();  // 取出一个mysql连接
    assert(sql);
    
    bool flag = false;
    unsigned int j = 0;
    char order[256] = { 0 };
    MYSQL_FIELD *fields = nullptr;
    MYSQL_RES *res = nullptr;
    
    if(!isLogin) { flag = true; }
    /* 查询用户及密码 */
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", name.c_str());

    if(mysql_query(sql, order)) { 
        mysql_free_result(res);
        return false; 
    }
    res = mysql_store_result(sql);
    j = mysql_num_fields(res);
    fields = mysql_fetch_fields(res);

    while(MYSQL_ROW row = mysql_fetch_row(res)) {
        string password(row[1]);
        /* 注册行为 且 用户名未被使用*/
        if(isLogin) {
            if(pwd == password) { flag = true; }
            else {
                flag = false;
            }
        } 
        else { 
            flag = false; 
        }
    }
    mysql_free_result(res);

    /* 注册行为 且 用户名未被使用*/
    if(!isLogin && flag == true) {
        bzero(order, 256);
        snprintf(order, 256,"INSERT INTO user(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
        if(mysql_query(sql, order)) { 
            flag = false; 
        }
        flag = true;
    }
    ConnPool::GetConnPool()->ReleaseConn(sql);
    return flag;
}






