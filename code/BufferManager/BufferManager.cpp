#include "BufferManager.h"
using namespace std;

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



/*********************************DISK**********************************/

void write_block_disk(Block*blk)
{
    string TableName = blk->TableName;
    int BlockId = blk->BlockId;
    char* data = blk->data_begin;

    filebuf f;
    f.open(TableName, ios::in | ios::out | ios::binary);
    if(!f.is_open())
    {
        ofstream new_f(TableName);
        new_f.close();
        f.open(TableName, ios::in | ios::out | ios::binary);
    }
    istream fin(&f);
    ostream fout(&f);
    fout.seekp(TABLE_HEADER_SIZE + BlockId*BLOCK_SIZE);
    fout << TableName << BlockId;
    fout.seekp(TABLE_HEADER_SIZE + BlockId*BLOCK_SIZE+BLOCK_HEADER_SIZE,ios::beg);
    for (int i = 0; i < BLOCK_SIZE-BLOCK_HEADER_SIZE; i++)
    {
        fout << data[i];
        if(data[i]=='\n') cout<<"eeeerrrrrooooooorrrrrroo";
    }
    return ;    
}


Block* fetch_block_disk(string TableName, int BlockId)
{
    filebuf f;
    Block* result;

    f.open(TableName, ios::in | ios::out | ios::binary);
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
    f.open(TableName, ios::in | ios::out | ios::binary);
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
    f.open(TableName, ios::in | ios::out | ios::binary);
    istream fin(&f);
    fin >> i;
    i += add;
    ostream fout(&f);
    fout.seekp(0, ios::beg);
    fout << i;
    f.close();
    return ;
}