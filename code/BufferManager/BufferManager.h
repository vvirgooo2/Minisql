#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <bits/stdc++.h>
#include <string>
#include "../SqlDataType.h"
using namespace std;
#define HEADER_SIZE 36

// 读内存块的时候，默认的是char(n)类型的要存n+1个字节
class Block
{
/* 我们定义磁盘上前32+4 = 36个字节为块头，从第37个字节开始存储记录的信息
 * 解析外存时就按照这个规则还原成员变量*/
public:
    char* data_begin;  // 有效记录的首地址 8 Byte
    string TableName;  //32 Byte
    int BlockId;  //4 Byte
    Block();
    Block(string tn, int bi);
    bool is_empty();
};


class BufferManage
{
    /* free_list 存储了在buffer里面的块的index
     *  存储了在buffer里面的BlockAttr对应的数组索引（int类型）
     */
public:
    Block *Buffer_pool;

    BufferManage();
    // 遍历bufferpool数组，根据表名和blockID搜索非满的块，有则返回块指针，没有则返回null */
    Block* get_block(string TableName, int BlockId);  
    
    // 返回处理过的块
    void ret_block(Block* blk);  
};

// belows are disk operating functions
Block* fetch_block_disk(string TableName, int BlockId);
void write_block_disk(Block*blk);

#endif

