#ifndef HISTORY
#define HISTORY

#include <iostream>
#include <hiredis/hiredis.h>
#include "../Common/myred.hpp"

class History{
private:
    uint32_t      uid = 0;
    std::string   key = "";
    Hred          hred = 0;
    int           limit = 0;
    uint32_t      requid{0};

public:
    History(redisContext* context_,uint32_t uid_,std::string type_,int limit_,uint32_t requid_=0,bool choice=0):
        hred(context_),uid(uid_),limit(limit_),requid(requid_){
            if (requid == 0)
                key = std::to_string(uid) + type_;
            else if (choice == 0)
                key = std::to_string(uid) + type_ + std::to_string(requid);
            else{
                std::cout << "choice "<< std::endl;
                key = type_ + std::to_string(requid_);
            }
        }
    History(std::string k):
        key(k){}

    bool history_empty(){
        return hred.isnull(key);
    }
    bool history_exist(std::string value){
        return hred.hasValue(key,value);
    }

    void add_new(std::string value);

    std::string get_hismsg(int option=0){
        return hred.rpop(key);
    }
    std::vector<std::string> get_keylist(){
        return hred.getkeys(key);
    }
};

void History::add_new(std::string value)
{
    if (hred.getlen(key) == limit){
        hred.rpop(key);
    }
    hred.lpush(key,value);
}


#endif
