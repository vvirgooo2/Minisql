#include "BufferManager.h"
using namespace std;

Block::Block()
{
    BlockId = -1; // mark the block is empty
    data_begin = new char[BLOCK_SIZE-HEADER_SIZE];
    memset(data_begin, 0, (BLOCK_SIZE-HEADER_SIZE)*sizeof(char));
}

Block::Block(string tn, int bi)
{
    TableName = tn;
    BlockId = bi;
    data_begin = new char[BLOCK_SIZE];
}

bool Block::is_empty()
{
    return BlockId >= 0;
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
            }
        }
        // if buffer is full, overwrite the first one to lay the bew block
        Buffer_pool[0] = *blk;   
    }

    // write to disk
    write_block_disk(blk);
}

void write_block_disk(Block*blk)
{
    string TableName = blk->TableName;
    int BlockId = blk->BlockId;
    char* data = blk->data_begin;

    filebuf f;
    f.open(TableName, ios::in | ios::out);
    istream fin(&f);
    ostream fout(&f);
    fout.seekp(BlockId*BLOCK_SIZE);
    fout << TableName << BlockId;
    for (int i = 0; i < BLOCK_SIZE-HEADER_SIZE; i++)
    {
        fout << data[i];
    }
    return ;    
}


Block* fetch_block_disk(string TableName, int BlockId)
{
    filebuf f;
    f.open(TableName, ios::in | ios::out);
    istream fin(&f);
    ostream fout(&f);

    // create new block to return to the caller
    Block* result;
    result = new Block(TableName, BlockId);

    fin.seekg(BlockId * BLOCK_SIZE + HEADER_SIZE);
    // read into databegin byte by byte, (BSize-HSize) in total
    for (int i = 0; i < BLOCK_SIZE-HEADER_SIZE; i++)
    {
        fin >> (result->data_begin)[i];
    }
    return result;
}
