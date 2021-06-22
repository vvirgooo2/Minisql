#include <bits/stdc++.h>
#include <string>
#include "../SqlDataType.h"
using namespace std;
#define TABLE_HEADER_SIZE 4
#define BLOCK_HEADER_SIZE 36
#define NUM_TABLE 10000

// 读内存块的时候，默认的是char(n)类型的要存n+1个字节
class Block
{
/* 我们定义磁盘上前32+4 = 36个字节为块头，从第41个字节开始存储记录的信息
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
    
private:
    // 各个表的当前 最后一块的ID

public:
    Block *Buffer_pool;

    BufferManage();
    // 遍历bufferpool数组，根据表名和blockID搜索非满的块，有则返回块指针，没有则返回null 
    Block* get_block(string TableName, int BlockId);  
    
    // 返回处理过的块
    void ret_block(Block* blk);  

};

// 下面是磁盘操作函数
Block* fetch_block_disk(string TableName, int BlockId);
void write_block_disk(Block*blk);
int get_total_block_num(string TableName);
void edit_total_block_num(string TableName, int add);



Block::Block()
{
    BlockId = -1; // mark the block is empty
    data_begin = new char[BLOCK_SIZE-BLOCK_HEADER_SIZE];
    memset(data_begin, 0, (BLOCK_SIZE-BLOCK_HEADER_SIZE)*sizeof(char));
}

Block::Block(string tn, int bi)
{
    TableName = tn;
    BlockId = bi;
    data_begin = new char[BLOCK_SIZE];
}

bool Block::is_empty()
{
    return BlockId == -1;
}

BufferManage::BufferManage()
{
    // allocate block pointer
    Buffer_pool = new Block[MAX_BlockNumber];
    // empty the buffer_pool
    for (int i = 0; i < MAX_BlockNumber; i++)
    {
        Buffer_pool[i].BlockId = -1;
    }
}


Block* BufferManage::get_block(string TableName, int BlockId)
{
    /*this function is called when inserting, deleting or selecting*/
    
    // present in buffer
    for (int i = 0; i < MAX_BlockNumber; i++)
    {
        if(Buffer_pool[i].TableName == TableName && Buffer_pool[i].BlockId == BlockId)
        {
            return &(Buffer_pool[i]);
        }
    }
    // not present in buffer
    return fetch_block_disk(TableName, BlockId);
}

void BufferManage::ret_block(Block* blk)
{
    string TableName = blk->TableName;
    int BlockId = blk->BlockId;
    bool in_buffer = false;
    // blk can be NULL
    // if in buffer, find and update it
    for (int i = 0; i < MAX_BlockNumber; i++)
    {
        if(Buffer_pool[i].TableName == TableName && Buffer_pool[i].BlockId == BlockId)
        {
            in_buffer = true;
            Buffer_pool[i] = *blk;
            break;
        }
    }
    // not present in buffer, insert it into the buffer
    if(!in_buffer)
    {
        bool is_available = false;
        for (int i = 0; i < MAX_BlockNumber; i++)
        {
            if(Buffer_pool[i].is_empty())
            {
                is_available = true;
                Buffer_pool[i] = *blk;
                break;
            }
        }
        // if buffer is full, overwrite the first one to lay the bew block
        if(!is_available)  Buffer_pool[0] = *blk;   
    }

    // write to disk
    write_block_disk(blk);
}

int get_num_table(string TableName)
{
    filebuf f;
    f.open(TableName, ios::in | ios::out);
    if(!f.is_open())
    {
        return 0;
    }
    istream fin(&f);
}

void write_block_disk(Block*blk)
{
    string TableName = blk->TableName;
    int BlockId = blk->BlockId;
    char* data = blk->data_begin;

    filebuf f;
    f.open(TableName, ios::in | ios::out);
    if(!f.is_open())
    {
        ofstream new_f(TableName);
        new_f.close();
        f.open(TableName, ios::in | ios::out);
    }
    istream fin(&f);
    ostream fout(&f);
    fout.seekp(BlockId*BLOCK_SIZE);
    fout << TableName << BlockId;
    for (int i = 0; i < BLOCK_SIZE-BLOCK_HEADER_SIZE; i++)
    {
        fout << data[i];
    }
    return ;    
}



Block* fetch_block_disk(string TableName, int BlockId)
{
    filebuf f;
    Block* result;

    f.open(TableName, ios::in | ios::out);
    // there is no such block
    if(!f.is_open())
    {
        result = new Block(TableName, -1);
        return result;
    }
    
    istream fin(&f);
    ostream fout(&f);

    // create new block to return to the caller
    result = new Block(TableName, BlockId);

    fin.seekg(TABLE_HEADER_SIZE + BlockId * BLOCK_SIZE + BLOCK_HEADER_SIZE, ios::beg);
    for (int i = 0; i < BLOCK_SIZE-BLOCK_HEADER_SIZE; i++)
    {
        fin >> (result->data_begin)[i];
    }
    return result;
}

int get_total_block_num(string TableName)
{
    filebuf f;
    f.open(TableName, ios::in | ios::out);
    // 新开文件即初始化为0
    if(!f.is_open())
    {
        ofstream new_f(TableName);
        int b_n = 0;
        new_f << b_n;
        new_f.close();
        f.close();
        return 0;
    }
    istream fin(&f);
    int i;
    fin >> i;
    f.close();
    return i;
}

void edit_total_block_num(string TableName, int add)
{
    filebuf f;
    int i;
    f.open(TableName, ios::in | ios::out);
    istream fin(&f);
    fin >> i;
    i += add;
    ostream fout(&f);
    fout << i;
    f.close();
    return ;
}

int main()
{
    // create a buffer manager
    BufferManage* mybuffer;
    mybuffer = new BufferManage;
	Block *myblk;
    myblk = new Block;
    int blk_num;
    bool need_add = false;

    /* 测试insert
     * insert操作可以直接先查最后一个块的ID，
     * 直接再调用get_block（表名，ID）即可
     */
    for (int i = 0; i < 10000; i++)
    {
        blk_num = get_total_block_num("table0");
        myblk = mybuffer->get_block("table0", blk_num);
        myblk->data_begin[1] = 'x';
        edit_total_block_num("table0", need_add);
        mybuffer->ret_block(myblk);
        need_add = !need_add; // flip to test
    }
    
    // 先填满buffer pool
    for (int i = 0; i < MAX_BlockNumber; i++)
    {
        myblk = mybuffer->get_block("table0", i);
        myblk->data_begin[10] = 'q';
        mybuffer->ret_block(myblk);
    }

    // 测试bufferpool满的时候的替换
    for (int i = 0; i < MAX_BlockNumber; i++)
    {
        myblk = mybuffer->get_block("table0", i);
        myblk->data_begin[100] = 'y';
        mybuffer->ret_block(myblk);
    }

    return 0;
}