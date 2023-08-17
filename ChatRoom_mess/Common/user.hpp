#ifndef USER
#define USER

#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

class User
{
public:
    uint32_t uid;
    std::string password;
    std::string name;
    bool status = 0;

public:
    User(uint32_t uuid,std::string upassword,std::string uname="dog"):
        uid(uuid),name(uname),password(upassword){}
    User(std::string replyStr);
    User(nlohmann::json j);

    nlohmann::json tojson();

    std::string getStr(){
        return this->tojson().dump();
    }
};

User::User(std::string replyStr)
{
    nlohmann::json j = nlohmann::json::parse(replyStr);

    uid      =   j["uid"];
    name     =   j["name"];
    password =   j["password"];
    status   =   j["status"];
}

User::User(nlohmann::json j)
{
    uid      =   j["uid"];
    name     =   j["name"];
    password =   j["password"]; 
    status   =   j["status"];
}

nlohmann::json User::tojson()
{
    nlohmann::json j;
    j["uid"] = uid;
    j["name"] = name;
    j["password"] = password;
    j["status"]  =status;
    return j;
}

#endif
