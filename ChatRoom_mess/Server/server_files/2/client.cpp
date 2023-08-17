#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sstream>
#include <fstream>
#include <dirent.h>
#include "menu.hpp"
#include "../Common/socket.hpp"
#include "../Common/srMsg.hpp"
#include "fileio.hpp"

#define MAXEVENTS 20
#define CHUNKSIZE 1024
#define MIN(a,b) ((a<b)? a : b)


const char* blank = "                                   ";
const char* store_files     = "./store_files/";
const char* friend_requests = "./store_files/friend_requests";
const char* group_requests  = "./store_files/group_requests";
const char* prvchat_message = "./store_files/prvchat_message";
const char* grpchat_message = "./store_files/grpchat_message";
const char* temp            = "temp";
const char* temp_aft        = "tmp.txt";
const char* rec_files       = "./rec_files/";

int sfd;
bool islog;
uint32_t myid;
uint32_t talkto;

std::map<uint32_t,std::vector<std::string>> fileMap;
uint32_t blocked;
uint32_t block_list[1000];


pthread_mutex_t mlog  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx   = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t load  = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mfile = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  colog = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cm    = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cl    = PTHREAD_COND_INITIALIZER;
pthread_cond_t  cfile = PTHREAD_COND_INITIALIZER;


void sendone(int sfd) {
    std::string msg;
    std::cin >> msg;
    sendMsg(sfd, msg.c_str());
}
std::string readone(int fd) {
    std::string msg;
    msg = readMsg(fd);
    std::cout << msg << std::endl;
    return msg;
}

std::string get_yn(void)
{
    std::string choice;
    getline(std::cin,choice);
    while (choice != "y" && choice != "n"){
        std::cout << "Please input y or n" << std::endl;
        getline(std::cin,choice);
    }
    return choice;
}


void cliSign(int fd) {
    Msg msg;
    msg.flag = 1;
    std::cout << "Please input UID" << std::endl;
    input_uint(&msg.uid);

    myid = msg.uid;
    std::cout << "Please input your password" << std::endl;
    getline(std::cin,msg.password);

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
    pthread_mutex_unlock(&mlog);
}
void cliLog(int fd) {
    Msg msg;
    msg.flag = 0;
    std::cout << "Please input UID" << std::endl;
    input_uint(&msg.uid);
    myid = msg.uid;

    std::cout << "Please input your password" << std::endl;
    getline(std::cin,msg.password);

    sendMsg(fd, msg.toStr().c_str());
    pthread_mutex_lock(&mlog);
    pthread_cond_wait(&colog,&mlog);
    pthread_mutex_unlock(&mlog);
}

void load_prvchat_message(uint32_t id)
{
    std::vector<chatMsg> messages;
    std::string filename = prvchat_message + std::to_string(myid) + ".txt";
    std::string tempfilename = temp + filename;

    std::ifstream file(filename);
    std::ofstream tempFile(tempfilename);
    if (file.is_open())
    {
        std::string line;
        while (getline(file,line))
        {
            chatMsg msg;
            std::istringstream record(line);
            record >> msg.fid >> msg.content;
            if (msg.fid == id)
                std::cout << msg.content << std::endl;
            else
                tempFile << line << std::endl;
        }
    }
    file.close();
    tempFile.close();
    std::remove(prvchat_message);
    std::rename(tempfilename.c_str(),filename.c_str());
}

void load_grpchat_message(uint32_t id)
{
    std::string filename = grpchat_message + std::to_string(myid) + ".txt";
    std::string tmpfilename = "tmp" + filename;
    std::ifstream file(filename);
    std::ofstream tempFile(tmpfilename);
    if (file.is_open())
    {
        std::string line;
        while (getline(file,line))
        {
            chatMsg msg;
            std::istringstream record(line);
            record >> msg.gid >> msg.fid >> msg.content;
            if (msg.gid == id)
                std::cout << msg.fid << ": " << msg.content << std::endl;
            else
                tempFile << line << std::endl;
        }
    }
    else
    {
        puts("error when open file");
    }
    file.close();
    tempFile.close();
    std::remove(filename.c_str());
    std::rename(tmpfilename.c_str(),filename.c_str());
}

void rec_file()
{
    std::string buf;
    char buffer[1024];

    buf = readMsg(sfd);
    fileMsg fmsg(buf);
    std::string filename = fmsg.filename;
    filename = rec_files + filename;

    int64_t fileSize,recvd;
    fileSize = fmsg.fileSize;
    recvd =0;
    std::ofstream file(filename,std::ios::binary);
    while (recvd < fileSize)
    {
        buf = readMsg(sfd);
        fileMsg fmsg(buf);
        file << fmsg.content;
        recvd += fmsg.content.size();
    }
    file.close();
    std::cout << "传输成功！" << std::endl;
}

void friChat(int fd)
{
    Msg msg;
    msg.flag = FRIENDCHAT;

    msg.uid = myid;
    std::cout << "请输入您想聊天的用户的UID" << std::endl;
    input_uint(&msg.touid);
    talkto = msg.touid;

    printf("--------与用户%d的聊天(按Q退出)-----------\n",talkto);
    load_prvchat_message(talkto);
    std::string content;
    getline(std::cin,content);
    if (content.size() > 30){
        std::cout << "一次发送的长度不能超过30" << std::endl;
    }
    while (content != "Q")
    {
        msg.content = content;
        sendMsg(fd,msg.toStr().c_str());
        getline(std::cin,content);
        if (content.size() > 30){
            std::cout << "一次发送的长度不能超过30" << std::endl;
        }
    }
    talkto = 0;
}

void blockFriend(int fd)
{
    Msg msg;
    msg.flag = BLOCKFRIEND;

    msg.uid = myid;
    std::cout << "请输入您想屏蔽的用户的UID" << std::endl;
    input_uint(&msg.touid);

    sendMsg(fd,msg.toStr().c_str());
}

void unblockFriend(int fd)
{
    Msg msg;
    msg.flag = UNBLOCKFRIEND;

    msg.uid = myid;
    std::cout << "请输入您想解除屏蔽的用户" << std::endl;
    scanf("%u",&msg.touid);
    while (msg.touid <= 0){
        std::cout << "请输入大于0的uid号" << std::endl;
        scanf("%u",&msg.touid);
    }
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void groupChat(int fd,uint32_t gid)
{
    Msg msg;
    msg.flag = GROUPCHAT;

    msg.uid   =  myid;
    msg.touid =  gid;
    talkto    =  msg.touid;

    printf("--------在群%d内的聊天(按Q退出)-----------\n",talkto);
    load_grpchat_message(msg.touid);
    std::string content;
    getline(std::cin,content);
    if (content.size() > 30){
        std::cout << "一次发送的长度不能超过30" << std::endl;
    }
    while (content != "Q")
    {
        msg.content = content;
        sendMsg(fd,msg.toStr().c_str());
        getline(std::cin,content);
        if (content.size() > 30){
            std::cout << "一次发送的长度不能超过30" << std::endl;
        }
    }
    talkto = 0;
}

void showFriend(int fd)
{
    Msg msg;
    msg.flag = SHOWFRIEND;

    msg.uid = myid;
    sendMsg(fd,msg.toStr().c_str());
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cm,&mtx);
    pthread_mutex_unlock(&mtx);
}

void addFriend(int fd)
{
    Msg msg;
    msg.flag = ADDFRIEND;

    msg.uid = myid;
    std::cout << "请选择您想要的伙伴(uid)" << std::endl;
    input_uint(&msg.touid);

    sendMsg(fd,msg.toStr().c_str());
}

void friend_req(int fd)
{
    Msg msg;
    msg.flag = FRIENDREQUEST;

    std::string filename = friend_requests + std::to_string(myid) + ".txt";
    std::string tmpfilename = temp + filename;

    std::cout <<filename << std::endl;
    std::ifstream file(filename);
    std::ofstream tmpfile(tmpfilename);
    if (!file.is_open()){
        std::cout << "暂无好友请求" << std::endl;
        return;
    }

    std::string line;
    std::string choice("y");
    while(getline(file,line))
    {
        if (line == "")
            continue;
        if (choice == "n")
        {
            tmpfile << line << std::endl;
            continue;
        }
        uint32_t fuid = std::stoul(line);
        std::string request = std::to_string(fuid) + " wants you! Accept?(y/n)";
        std::cout << request << std::endl;
        std::string ac = get_yn();
        msg.uid     =  myid;
        msg.touid   =  fuid;
        msg.content =  ac;
        sendMsg(fd,msg.toStr().c_str());

        std::cout << "继续读取下一条？(y/n)" << std::endl;
        choice = get_yn();
    }

    file.close();
    tmpfile.close();
    std::remove(filename.c_str());
    std::rename(filename.c_str(),tmpfilename.c_str());
}

void group_req(int fd)
{
    Msg msg;
    msg.flag = GROUPREQUEST;

    std::string filename = group_requests + std::to_string(myid) + ".txt";
    std::string tmpfilename = temp + filename;

    std::ifstream file(filename);
    std::ofstream tmpfile(tmpfilename);
    if (!file.is_open()){
        std::cout << "error when open group_requests file" << std::endl;
        return;
    }

    std::string line;
    std::string choice("y");
    while(getline(file,line))
    {
        if (line == "")
            continue;
        if (choice == "n")
        {
            tmpfile << line << std::endl;
            continue;
        }

        puts("test1");
        groupReq gq(line);
        puts("test2");
        std::string ask = std::to_string(gq.uid) + "想要加入群聊" + std::to_string(gq.gid);
        switch(gq.status)
        {
            case 0:
                std::cout << ask << "  [同意(y)/拒绝(n)]" << std::endl;
                msg.uid     = gq.uid;
                msg.touid   = gq.gid;
                msg.adduid  = myid;
                msg.content = get_yn();
                sendMsg(fd,msg.toStr().c_str());
                break;
            case 1:
                std::cout << ask << "  然而已被拒绝" << std::endl;
                break;
            case 2:
                std::cout << ask << "  已同意" << std::endl;
                break;
            default:;
        }
        std::cout << "继续读取下一条？(y/n)" << std::endl;
        choice = get_yn();
        puts("test3");
    }
    file.close();
    tmpfile.close();
    std::remove(filename.c_str());
    std::rename(filename.c_str(),tmpfilename.c_str());
}

void delfriend(int fd)
{
    Msg msg;
    msg.flag = DELFRIEND;

    uint32_t touid;
    std::cout << "想删谁？" << std::endl;
    scanf("%u",&touid);
    getchar();

    msg.uid   = myid;
    msg.touid = touid;
    sendMsg(fd,msg.toStr().c_str());
}

void createGroup(int fd)
{
    Msg msg;
    msg.flag = CREATGROUP;

    msg.uid = myid;
    std::cout << "请输入您想创建的群聊的id" << std::endl;
    scanf("%u", &msg.touid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void addMember(int fd,uint32_t gid)
{
    Msg msg;
    msg.flag = ADDMEMBER;

    msg.uid   =  myid;
    msg.touid =  gid;
    std::cout << "输入想拉入的新成员的id" << std::endl;
    scanf("%u",&msg.adduid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void kickMember(int fd,uint32_t gid)
{
    Msg msg;
    msg.flag = KICKMEMBER;

    msg.uid   =  myid;
    msg.touid =  gid;
    std::cout << "输入想踢的人" << std::endl;
    scanf("%u",&msg.adduid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void addAdmin(int fd,int gid)
{
    Msg msg;
    msg.flag = ADDADMIN;

    msg.uid   = myid;
    msg.touid = gid;
    std::cout << "输入想提拔的新干部id" << std::endl;
    scanf("%u",&msg.adduid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void delAdmin(int fd,int gid)
{
    Msg msg;
    msg.flag = DELADMIN;

    msg.uid   =  myid;
    msg.touid =  gid;
    std::cout << "想撤销哪位委员的职务" << std::endl;
    scanf("%u",&msg.adduid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void addGroup(int fd)
{
    Msg msg;
    msg.flag = ADDGROUP;

    msg.uid = myid;
    std::cout << "请输入您想加入的群聊id" << std::endl;
    scanf("%u",&msg.touid);
    getchar();

    sendMsg(fd,msg.toStr().c_str());
}

void showGroup(int fd,uint32_t gid)
{
    Msg msg;
    msg.flag = SHOWGROUP;

    msg.uid  =  gid;
    sendMsg(fd,msg.toStr().c_str());

    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cm,&mtx);
    pthread_mutex_unlock(&mtx);
}
void delGroup(int fd,uint32_t gid)
{
    Msg msg;
    msg.flag = DELGROUP;

    msg.uid   = myid;
    msg.touid = gid;
    sendMsg(fd,msg.toStr().c_str());
}


void print_message(std::string buf)
{
    pthread_mutex_lock(&mlog);

    std::cout << buf << std::endl;
    if (buf == "登陆成功" || buf == "注册成功"){
        islog = 1;
    }

    pthread_mutex_unlock(&mlog);
    pthread_cond_signal(&colog);
}

void save_prvchat_message(chatMsg chatmsg)
{
    std::string filename = prvchat_message + std::to_string(myid) + ".txt";
    std::ofstream file(filename,std::ios::app);

    std::string content = std::to_string(chatmsg.fid)+" "+chatmsg.content;
    if (file.is_open())
    {
        std::cout << blank << chatmsg.fid << "send you a message" << std::endl;
        file << content << std::endl;
    }
    else
    {
        std::cout << "failed to open the file" << std::endl;
    }
    file.close();
}
void save_grpchat_message(chatMsg chatmsg)
{
    std::string filename = grpchat_message + std::to_string(myid) + ".txt";
    std::ofstream file(filename,std::ios::app);

    std::string content = std::to_string(chatmsg.gid) +" " + std::to_string(chatmsg.fid)+" "+chatmsg.content;
    if (file.is_open())
    {
        std::cout << blank << chatmsg.gid << "内收到一条消息" << std::endl;
        file << content << std::endl;
    }
    else
    {
        std::cout << "failed to open the file" << std::endl;
    }
}

void prv_recv(std::string buf)
{
    chatMsg chatmsg(buf);
    if (chatmsg.fid == talkto){
        std::cout << chatmsg.fid << ":"<< chatmsg.content << std::endl;
    }
    else
    {
        save_prvchat_message(chatmsg);
    }
}

void grp_recv(std::string buf)
{
    if (buf == "查无此群!")
        std::cout << "查无此群!" << std::endl;

    chatMsg chatmsg(buf);
    if (chatmsg.gid == talkto){
        std::cout << chatmsg.fid << ":"<< chatmsg.content << std::endl;
    }
    else
    {
        save_grpchat_message(chatmsg);
    }
}

void save_friend_request(std::string buf)
{
    if (!isNumeric(buf)){
        std::cout << buf << std::endl;
        return;
    }
    std::cout << "您有一条好友申请，please及时查收哦～" << std::endl;

    uint32_t fuid = std::stoul(buf);

    std::string filename = friend_requests + std::to_string(myid)+".txt";
    std::cout << filename << std::endl;
    std::ofstream file(filename,std::ios::app);

    if (!file.is_open()){
        std::cout << "failed to open the file" << std::endl;
    }
    file << " " << fuid << std::endl;
    file.close();
}
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

    pthread_mutex_lock(&mtx);
    for (auto frd : frv)
    {
        const char* status;
        if (frd.online)
            status = "online";
        else
            status = "offline";
        printf("user:%d status:%s \n",frd.frid,status);
    }
    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cm);
}

void groupShow(std::string buf)
{
    struct groupInfo{
        uint32_t uid;
        int      level;
        int      online;
    };

    groupInfo info;
    std::vector<groupInfo> gpv;
    std::istringstream record(buf);

    while (record >> info.uid >> info.level >> info.online ){
        gpv.push_back(info);
    }

    pthread_mutex_lock(&mtx);
    for (auto gpm : gpv)
    {
        const char *level,*status;
        if (gpm.level == 1)
            level = "平民";
        else if (gpm.level == 2)
            level = "干部";
        else
            level = "群主";

        if (gpm.online)
            status = "online";
        else
            status = "offline";
        printf("user:%d level:%s status:%s \n",gpm.uid,level,status);
    }
    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cm);
}

void save_group_request(std::string buf)
{
    groupReq gq(buf);
    std::string filename = group_requests + std::to_string(myid)+".txt";
    //std::string content  = std::to_string(gq.uid)+" "+std::to_string(gq.gid)+" "+std::to_string(gq.status);
    std::ofstream file(filename,std::ios::app);

    if (file.is_open())
    {
        file << buf << std::endl;
        file.close();
    }
    else
    {
        std::cout << "failed to open the file" << std::endl;
    }
}

void change_status(std::string filename,uint32_t uid,uint32_t gid,int newStatus)
{
    std::string tmpfilename = "tmp"+filename;
    std::string line;
    std::ifstream file(filename);
    std::ofstream tmpfile(tmpfilename);

    while(getline(file,line))
    {
        if (line == "")
            continue;
        groupReq gq(line);
        if (gq.uid != uid || gq.gid != gid)
        {
            tmpfile << line << std::endl;
            continue;
        }
        gq.status = newStatus;
        tmpfile << gq.toStr() << std::endl;
    }
    file.close();
    tmpfile.close();
    std::remove(filename.c_str());
    std::rename(tmpfilename.c_str(),filename.c_str());
}

void handle_group_request(std::string buf)
{
    groupReq gq(buf);
    std::string filename = group_requests + std::to_string(myid)+".txt";
    change_status(filename,gq.uid,gq.gid,gq.status);
}

void print_star(ssize_t big,ssize_t small){
    static int milestone;
    int add = big/10;
    if (small > milestone)
    {
        printf("*");
        milestone += add;
    }
}

void acceptFile(std::string buf)
{
    std::cout << "enter" << std::endl;
    fileMsg fmsg(buf);
    std::string filename = rec_files + fmsg.filename;

    std::cout << "开始接收" << filename << std::endl;
    int fd = open(filename.c_str(),O_WRONLY | O_TRUNC | O_CREAT,0755);
    if (fd == -1){
        myerr("error when open file");
    }

    char buffer[1024]={0};
    ssize_t recvd_bytes = 0,trans = 0,round = 0;
    while(recvd_bytes < fmsg.fileSize){
        if ((trans = recv(sfd,buffer,MIN(CHUNKSIZE,fmsg.fileSize-recvd_bytes),0)) <= 0){
            if (errno == EWOULDBLOCK || errno == EINTR){
                continue;
            }
            std::cout << "something occured" << std::endl;
        }
        recvd_bytes += trans;
        write(fd,buffer,sizeof(buffer));
        memset(buffer,0,sizeof(buffer));
        print_star(fmsg.fileSize,recvd_bytes);
    }
    printf("\n");
    std::cout << "接收完毕(请输入666,再进行下一步操作)" << std::endl;

    close(fd);
}


void sendFile(std::string filename,int fd,ssize_t fileSize)
{

    int64_t sent = 0;
    ssize_t offset = 0;

    while (offset < fileSize)
    {
        ssize_t send_bytes = MIN(CHUNKSIZE,fileSize-offset);
        ssize_t tran = sendfile(sfd,fd,NULL,send_bytes);
        offset += tran;
        print_star(fileSize,offset);
    }
    printf("\n");
    close(fd);
}

void send_file(int fd)
{
    Msg msg;
    msg.flag = SENDFILE;
    msg.uid   = myid;
    std::cout << "您想传给谁呢？" << std::endl;
    input_uint(&msg.touid);

    std::string filename;
    std::cout << "请输入您要传输的文件" << std::endl;
    getline(std::cin,filename);
    int file_fd = open(filename.c_str(),O_RDONLY);
    if (file_fd == -1){
        myerr("error when open file");
    }
    struct stat file_info;
    fstat(file_fd,&file_info);
    ssize_t data_size = file_info.st_size;

    std::cout << data_size << std::endl;
    fileMsg fmsg(msg.uid,msg.touid);
    fmsg.filename = filename;
    fmsg.fileSize = data_size;

    msg.content = fmsg.toStr();
    sendMsg(sfd,msg.toStr().c_str());

    std::cout << "提示：开始向" << msg.touid << "传功" << std::endl;
    sendFile(filename,file_fd,data_size);
    std::cout << "传功成功，等待对方接收" << std::endl;
}

void accept_file(int fd)
{
    Msg msg;
    msg.flag = ACCEPTFILE;
    msg.uid   = myid;
    std::cout << "想接收谁的文件？" << std::endl;
    input_uint(&msg.touid);

    std::string filename;
    std::cout << "请输入您要接收的文件名" << std::endl;
    getline(std::cin,filename);

    fileMsg fmsg(msg.touid,msg.uid);
    fmsg.filename = filename;
    msg.content = fmsg.toStr();

    sendMsg(sfd,msg.toStr().c_str());
}


void accept_load_info(std::string buf)
{
    pthread_mutex_lock(&load);

    if (buf == ""){
        pthread_mutex_unlock(&load);
        pthread_cond_signal(&cl);
        return;
    }

    rMsg rmg(buf);
    switch(rmg.flag)
    {
        case HISTORYPRICHAT:
            save_prvchat_message(rmg.mg);
            break;
        case HISTORYGRPCHAT:
            save_grpchat_message(rmg.mg);
            break;
        case FRIENDREQUEST:
            save_friend_request(rmg.mg);
            break;
    }

    pthread_mutex_unlock(&load);
    pthread_cond_signal(&cl);
}

void load_info()
{
    Msg msg;
    msg.flag = HISTORYY;
    msg.uid = myid;
    sendMsg(sfd,msg.toStr().c_str());

    struct timespec wait_time{2,0};
    pthread_mutex_lock(&load);
    pthread_cond_timedwait(&cl,&load,&wait_time);
    pthread_mutex_unlock(&load);
}

void do_read(int fd)
{
    std::string t = readMsg(fd);
    rMsg rmg(t);

    int choice = rmg.flag;
    std::string str = rmg.mg;
    switch(choice)
    {
        case LOGIN:
        case SIGNUP:
        case DELFRIEND:
        case CREATGROUP:
        case ADDMEMBER:
        case ADDADMIN:
        case DELADMIN:
        case FRIENDREQUEST:
        case KICKMEMBER:
        case BLOCKFRIEND:
        case UNBLOCKFRIEND:
        case DELGROUP:
        case SENDFILE:
            print_message(rmg.mg);
            break;
        case FRIENDCHAT:
            prv_recv(rmg.mg);
            break;
        case ADDFRIEND:
            std::cout << choice << std::endl;
            save_friend_request(rmg.mg);
            break;
        case SHOWFRIEND:
            std::cout << choice << std::endl;
            friendShow(rmg.mg);
            break;
        case ADDGROUP:
            std::cout << choice << std::endl;
            save_group_request(rmg.mg);
            break;
        case GROUPREQUEST:
            handle_group_request(rmg.mg);
            break;
        case SHOWGROUP:
            groupShow(rmg.mg);
            break;
        case GROUPCHAT:
            grp_recv(rmg.mg);
            break;
        case ACCEPTFILE:
            acceptFile(rmg.mg);
            break;
        case HISTORYPRICHAT:
        case HISTORYGRPCHAT:
        case HISTORYFRIREQUEST:
            std::cout << choice << std::endl;
            accept_load_info(t);
            break;

        //case FRIENDREQUEST:
            //std::cout << choice << std::endl;
            //friendRequest(rmg.mg);
            //break;
        default:
            std::cout << "default" << std::endl;
    }
}

void* do_epoll(void *) {
    struct epoll_event ev;
    struct epoll_event evlist[MAXEVENTS];
    int epfd = epoll_create(3);

    ev.data.fd = sfd;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sfd, &ev);
    while (1) {
        epoll_wait(epfd, evlist, MAXEVENTS, -1);
        do_read(sfd);
    }

    close(epfd);

    return NULL;
}

void mainDisplay(int sfd)
{
    int choice;
    while (1)
    {
        load_info();
        uint32_t gid;
        showMenu(&choice,&gid);

        switch(choice)
        {
            case LOGIN:
                cliLog(sfd);
                break;
            case SIGNUP:
                cliSign(sfd);
                break;
            case FRIENDCHAT:
                friChat(sfd);
                break;
            case BLOCKFRIEND:
                blockFriend(sfd);
                break;
            case UNBLOCKFRIEND:
                unblockFriend(sfd);
                break;
            case SHOWFRIEND:
                showFriend(sfd);
                break;
            case ADDFRIEND:
                addFriend(sfd);
                break;
            case FRIENDREQUEST:
                friend_req(sfd);
                break;
            case DELFRIEND:
                delfriend(sfd);
                break;
            case CREATGROUP:
                createGroup(sfd);
                break;
            case ADDMEMBER:
                addMember(sfd,gid);
                break;
            case KICKMEMBER:
                kickMember(sfd,gid);
            case ADDGROUP:
                addGroup(sfd);
                break;
            case GROUPREQUEST:
                group_req(sfd);
                break;
            case SENDFILE:
                send_file(sfd);
                break;
            case ACCEPTFILE:
                accept_file(sfd);
                break;
            case SHOWGROUP:
                showGroup(sfd,gid);
                break;
            case DELGROUP:
                delGroup(sfd,gid);
                break;
            case GROUPCHAT:
                groupChat(sfd,gid);
                break;
            case ADDADMIN:
                addAdmin(sfd,gid);
                break;
            case DELADMIN:
                delAdmin(sfd,gid);
                break;

            default:;
        }
    }
}

void log_out(int sig)
{
    Msg msg;
    msg.flag = LOGOUT;

    msg.uid   =  myid;
    std::cout << "确定要退出吗？QAQ" << std::endl;
    std::string ans = get_yn();

    if (ans == "n"){
        return;
    }
    islog = 0;
    sendMsg(sfd,msg.toStr().c_str());
    close(sfd);
    _exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    if (argc == 2){
        sfd = inetConnect(argv[1], "7679");
    }
    else if (argc == 3){
        sfd = inetConnect(argv[1], argv[2]);
    }
    else
        sfd = inetConnect("127.0.0.1","7679");
    struct stat st;
    if (stat(rec_files, &st) == -1) {
      // 目录不存在,调用mkdir创建
      mkdir(rec_files, 0755);
    }
    DIR *dir = opendir(store_files);
    if (dir){
        closedir(dir);
        rmdir(store_files);
    }
    mkdir(store_files,0777);

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = log_out;
    sigaction(SIGINT,&sa,NULL);


    pthread_t thid;
    if (pthread_create(&thid,NULL,do_epoll,NULL) != 0){
        myerr("creat thread");
    }
    mainDisplay(sfd);

    return 0;
}
                                                                         