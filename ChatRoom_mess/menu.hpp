#ifndef MENU
#define MENU

#include <iostream>
#include <string>
#include <stdint.h>
#include <unistd.h>
const char *pre_login_content = "- 登陆[0] 注册[1] -";
const char *after_login_content =
"好友聊天      [2]\n"
"屏蔽好友      [3]\n"
"解除屏蔽好友  [4]\n"
"显示我的好友  [5]\n"
"添加 好友     [6]\n"
"删除 好友     [7]\n"
"查看好友请求  [8]\n"
"创建群聊      [9]\n"
"申请加群      [10]\n"
"查看加群请求  [11]\n"
"发送文件      [12]\n"
"接收文件      [13]\n"
"群操作[777]";

const char *group_content =
"群内沟通     [14]\n"
"查看群成员   [15]\n"
"解散 群      [16]\n"
"添加 群成员  [17]\n"
"踢   群成员  [18]\n"
"添加 管理员  [19]\n"
"撤销 管理员  [20]";

extern int  sfd;
extern bool islog;
bool isNumeric(std::string const &str)
{
    auto it = str.begin();
    while (it != str.end() && std::isdigit(*it)) {
        it++;
    }
    return !str.empty() && it == str.end();
}
bool in_aftlog_field(std::string choice_){
    int choice = std::stoi(choice_);
    return (choice >=0 && choice<=6)||(choice==777)||(choice==666);
}
bool input_num(int* choice)
{
    std::string t;
    getline(std::cin,t);
    while (!isNumeric(t)){
        if (std::cin.eof()){
            std::cout << "强制退出" << std::endl;
            close(sfd);
            _exit(EXIT_SUCCESS);
        }
        std::cout << "怎么有人连数字都不会输？\\流汗黄豆 (按666刷新)" << std::endl;
        getline(std::cin,t);
    }
    *choice = std::stoi(t);
    return 0;
}
bool input_uint(uint32_t* choice)
{
    std::string t;
    getline(std::cin,t);
    while (!isNumeric(t)){
        if (std::cin.eof()){
            std::cout << "强制退出" << std::endl;
            close(sfd);
            _exit(EXIT_SUCCESS);
            return 1;
        }
        std::cout << "怎么有人连数字都不会输？\\流汗黄豆 (按666刷新)" << std::endl;
        getline(std::cin,t);
    }
    *choice = std::stoul(t);
    return 0;
}

void showMenu(int* choice,uint32_t *gid)
{
    if (!islog)
        std::cout << pre_login_content << std::endl;
    else
        std::cout << after_login_content << std::endl;

    input_num(choice);
    if (*choice != 777){
        return;
    }

    std::cout << "请输入您想要操作的群聊id" << std::endl;
    input_uint(gid);

    std::cout << "选择执行的操作" << std::endl;
    std::cout << group_content << std::endl;
    input_num(choice);
}


#endif
