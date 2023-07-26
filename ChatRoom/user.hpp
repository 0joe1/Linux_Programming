#include <cstdint>
#include <string>
#include <nlohmann/json.hpp>

class User
{

public:
    std::string password;
    uint32_t uid;
    std::string name;

public:
    User(uint32_t uuid,std::string upassword,std::string uname="dog"):
        uid(uuid),name(uname),password(upassword){}
    User(std::string replyStr);
    User(nlohmann::json j);

    nlohmann::json tojson();
};

User::User(std::string replyStr)
{
    nlohmann::json j = nlohmann::json::parse(replyStr);

    uid      =   j["uid"];
    name     =   j["name"];
    password =   j["password"]; 
}

User::User(nlohmann::json j)
{
    uid      =   j["uid"];
    name     =   j["name"];
    password =   j["password"]; 
}

nlohmann::json User::tojson()
{
    nlohmann::json j;
    j["uid"] = uid;
    j["name"] = name;
    j["password"] = password;

    return j;
}
