#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <bits/stdc++.h>
#include "../SqlDataType.h"
using namespace std;
// 读内存块的时候，默认的是char(n)类型的要存n+1个字节
class Block
{
/* 我们定义磁盘上前4个字节为块头，从第5个字节开始存储记录的信息
 * 解析外存时就按照这个规则还原is_full 和data_begin指针*/
private:
    int is_full;  // 判断块是否已满
    char* data_begin;  // 有效记录的首地址
public:
    Block();
    char * fetch_begin();  // 得到可写首地址的函数
    void set_full();  // block.set_full() 调用后会使得本块的is_full变为1
};

struct BlockAttr
{
    string tablename;
    int block_id;
    BlockAttr(string TableName, int BlockId);
    bool operator == (const BlockAttr & x) const;
};

struct hashfunc
{
    size_t operator()(const BlockAttr & x) const;
};


class BufferManage
{
    /* 记录了内存中的block信息，buffer managerment
     *据此在buffer pool(实际上是数组)里面找块的索引
     */
    list<BlockAttr>* BlockInfo; //elements in the buffer
    unordered_map<BlockAttr, list<BlockAttr>::iterator, hashfunc>* Hash_info;
    Block *Buffer_pool;
public:
    BufferManage();
    // 单条记录的处理只需要提供相应的块即可，is_insert决定此次的块操作类型
    Block* get_block(string TableName, int BlockId, bool is_insert);  
    Block* ret_block(Block* blk);  // 返回处理过的块，如果该块没有被修改，返回null
};

// belows are disk operating functions
Block* fetch_block_disk(BlockAttr* attr);
Block* alloc_block_disk(BlockAttr* attr);  // ???

#endif

