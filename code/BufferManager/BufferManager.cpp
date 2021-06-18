#include "BufferManager.hpp"
using namespace std;

Block::Block()
{
    
}


bool BlockAttr::operator==(const BlockAttr & x) const 
{
    return tablename == x.tablename && block_id == x.block_id;
}

size_t hashfunc::operator()(const BlockAttr & x) const
{
	return hash<string>()(x.tablename) ^ hash<int>()(x.block_id);
}

BlockAttr::BlockAttr(string TableName, int BlockId)
{
    tablename = TableName;
    block_id = BlockId;
}

BufferManage::BufferManage()
{
    // allocate vector pointer
    BlockInfo = new list<BlockAttr>;
    Hash_info = new unordered_map<BlockAttr, list<BlockAttr>::iterator, hashfunc>;

    // allocate space for Bufferpool
    Buffer_pool = new Block[MAX_BlockNumber];
}

Block* BufferManage::get_block(string TableName, int BlockId, bool is_insert)
{
    /*this function is called when inserting, deleting or selecting*/
    BlockAttr attr(TableName, BlockId);
    // not present in buffer
    if((*Hash_info).find(attr) == (*Hash_info).end())
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
            
            /*** insertion: ***/
            if(is_insert)
            {
                // create new 
            }
        }
    }
    // present in buffer 
    else
    {
        /*** insertion ***/
    }
}

Block* BufferManage::ret_block(Block* blk)
{
    // deletion: blk == NULL
}














