### 任务拆分

### 服务端：

##### IO:

主线程epoll等待lfd

accpet后接收fd传来的数据，封装成任务

将任务添加进任务队列



##### 逻辑：

识别并执行任务

发送包给客户端



### 客户端：

各个函数收集信息，封装成一个东西，然后把这个发送给服务端

一个线程专门接收服务器传来的信息

主线程只管发信息







### 对象

服务端给客户端的包：

种类（数字）+ 信息



ChatRoom:

组成：绑定后的lfd

功能：连接新客户端，添加任务



命令解释器：

解释传输过来的命令，据此制造任务

把任务丢给线程池去完成



线程池：

组成：任务队列

功能：处理任务



人物：

组成：唯一ID 名字 密码



数据库操控者

对数据库进行增删改查操作



权限监控器

组成：owner、administrator 分别指向拥有者和管理员

功能：根据命令的发起者和要执行的操作判断有无执行权限



人物列表：

对于个人来讲，gid=本身uid=ownerid

组成：储存人物ID的列表（数据库）   权限监控器

功能：

1.增删改人物





### Redis 内数据组织设计：

单个用户 [string] 

uid:{password,name}



用户会话列表 [List]

"cfuid:tiod" --  聊天内容list





记录谁给我发了消息 [set]

方便刚登陆可以加载离线数据



UserLIst 

一个 hash 数据结构用于存储 gid 对应的拥有者 id
`HSET gid_owner gid owner_id`
一个 set 数据结构用于存储 gid 对应的普通用户 id

```
SADD gid_users gid user_id1
SADD gid_users gid user_id2
```


一个 set 数据结构用于存储 gid 对应的管理者用户 id
```
SADD gid_managers gid manager_id1
SADD gid_managers gid manager_id2
```



