### List

```shell
# 将一个或多个值value插入到key列表的表头(最左边)，最后的值在最前面
LPUSH key value [value ...] 
# 将一个或多个值value插入到key列表的表尾(最右边)
RPUSH key value [value ...]
# 移除并返回key列表的头元素
LPOP key     
# 移除并返回key列表的尾元素
RPOP key 

# 返回列表key中指定区间内的元素，区间以偏移量start和stop指定，从0开始
LRANGE key start stop
```



### Hash、Set

这样设计的好处是，可以方便地进行用户 id 的添加、删除、查询等操作，同时也可以快速地获取 gid 对应的拥有者 id。具体操作如下：

获取 gid 对应的拥有者 id
`HGET gid_owner gid`

获取 gid 对应的所有用户 id（包括普通用户和管理者用户）
`SUNION gid_users gid_managers`

获取 gid 对应的普通用户 id
`SMEMBERS gid_users`
获取 gid 对应的管理者用户 id
`SMEMBERS gid_managers`
向 gid_users 中添加一个普通用户 id
`SADD gid_users gid new_user_id`
从 gid_users 中删除一个普通用户 id
`SREM gid_users gid user_id`
向 gid_managers 中添加一个管理者用户 id
`SADD gid_managers gid new_manager_id`
从 gid_managers 中删除一个管理者用户 id
`SREM gid_managers gid manager_id`
需要注意的是，以上操作都是基于 Redis 的原子性操作，可以保证数据的一致性和可靠性。同时，建议在实际应用中对 Redis 数据库进行适当的优化，以提高性能和稳定性。 
