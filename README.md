# Linux_Programming


介绍：
C++聊天室，采用epoll ET边缘触发模式，用到了线程池、redis等技术。完成基本聊天室的功能如聊天、好友、群、历史消息、传送文件等。

用法

服务端

进入Server目录，

```
make build
./main
```

客户端

进入Client

```
make build
./client [ip] [port]
```

