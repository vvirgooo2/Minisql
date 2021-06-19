#include "BufferManager.hpp"
using namespace std;

Block::Block()
{
    is_full = 0;
    data_begin = new char[BLOCK_SIZE];
}

void Block::set_full()
{
    is_full = 1; return ;
}

char* Block::fetch_begin()
{
    return data_begin;
}

BlockAttr::BlockAttr(string TableName, int BlockId)
{
    tablename = TableName;
    block_id = BlockId;
}

bool BlockAttr::operator==(const BlockAttr & x) const 
{
    return tablename == x.tablename && block_id == x.block_id;
}

size_t hashfunc::operator()(const BlockAttr & x) const
{
	return hash<string>()(x.tablename) ^ hash<int>()(x.block_id);
}


BufferManage::BufferManage()
{
    // allocate vector pointer
    BlockInfo = new list<BlockAttr>;
    Hash_info = new unordered_map<BlockAttr, list<BlockAttr>::iterator, hashfunc>;

    // allocate space for Bufferpool
    Buffer_pool = new Block[MAX_BlockNumber];
}

Block* BufferManage::get_block(string TableName, int BlockId, int operation)
{
    /*this function is called when inserting, deleting or selecting*/
    BlockAttr* attr = new BlockAttr(TableName, BlockId);
    Block* result;

    /**************** select *****************/
    if(operation == SELECT_OP)
    {
        // not present in buffer
        if((*Hash_info).find(*attr) == (*Hash_info).end())
        {
            // no such block found in buffer, turn to disk for help
            result = fetch_block_disk(attr);
            // even not in disk, return failure and maintain current status
            if(!result) return NULL;

            // if the buffer pool is full, delete the least recently used block
            if((*Hash_info).size() == MAX_BlockNumber)
            {
                // grab least recently refered block
                BlockAttr last = (*BlockInfo).back();
                // delete the last block from BlockInfo
                (*BlockInfo).pop_back();
                // erase the last in unordered_map according to the key
                (*Hash_info).erase(last);
            }
            // update buffer pool(bring the most recently used block to the front of the list)
            (*BlockInfo).push_front(*attr);
            (*Hash_info)[*attr] = (*BlockInfo).begin();
        }

    }

    /**************** insert ****************/
    else if(operation == INSERT_OP)
    {
        // if buffer is full, swap it out using LRU scheme
        if((*Hash_info).size() == MAX_BlockNumber)
        {
            // grab least recently refered block
            BlockAttr last = (*BlockInfo).back();
            // delete the block from BlockInfo
            (*BlockInfo).pop_back();
            // erase the last in unordered_map
            (*Hash_info).erase(last);
            
        }
    }
    
}

Block* BufferManage::ret_block(Block* blk)
{
    // deletion: blk == NULL
}














