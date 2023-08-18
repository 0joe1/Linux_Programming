#include <iostream>
#include <sstream>
#include <vector>
#include <stdint.h>

void friendShow(std::string buf)
{
    struct FriendInfo {
        uint32_t  frid;
        int       online;
    };

    FriendInfo info;
    std::vector<FriendInfo> frv;
    std::istringstream record(buf);

    while (record >> info.frid >> info.online ){
        frv.push_back(info);
    }

    for (auto frd : frv)
    {
        const char* status;
        if (frd.online)
            status = "online";
        else
            status = "offline";
        printf("user:%d status:%s \n",frd.frid,status);
    }
}

int main(void)
{
    std::string buf = "1 0 2 1 3 1 4 1 5 0";
    friendShow(buf);
    return 0;
}
