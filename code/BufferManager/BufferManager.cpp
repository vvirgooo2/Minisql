#include "BufferManager.hpp"
using namespace std;

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

Block* BufferManage::get_block(string TableName, int BlockId)
{
    // not present in buffer
    
}

Block* BufferManage::ret_block(Block* blk)
{
    
}














