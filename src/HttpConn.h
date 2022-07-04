#pragma once
#include <string>
#include <unordered_map>
#include <unordered_set>



class HttpRequest {
public:
    enum PARSE_STATE {REQUIRE_LINE, HEADERS, BODY, FINISH};

public:
    HttpRequest()   = default;
    ~HttpRequest()  = default;

public:
    bool Parse(const std::string &buf);

private:
    bool ParseRequestLine(const std::string &line);
    void ParseHeader(const std::string &line);
    void ParseBody(const std::string &line);
    
    void ParsePath();
    void ParsePost();
    void ParseFromUrlencoded();

    static bool UserVerify(const std::string& name, const std::string& pwd, bool isLogin);

private:
    PARSE_STATE state_      = REQUIRE_LINE;
    std::string method_     = "";
    std::string path_       = "";
    std::string version_    = "";
    std::string body_       = "";
    std::unordered_map<std::string, std::string> header_;
    std::unordered_map<std::string, std::string> post_;

    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string, int> DEFAULT_HTML_TAG;

};





class HttpReponse {
public:
    HttpReponse();
    ~HttpReponse();

private:
    int         code_;
    bool        is_keepalive_;
    std::string path_;
    

};








