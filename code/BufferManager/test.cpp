#include <bits/stdc++.h>
#include "BufferManager.h"
using namespace std;


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
    
    // // 先填满buffer pool
    // for (int i = 0; i < MAX_BlockNumber; i++)
    // {
    //     myblk = mybuffer->get_block("table0", i);
    //     myblk->data_begin[10] = 'q';
    //     mybuffer->ret_block(myblk);
    // }

    // // 测试bufferpool满的时候的替换
    // for (int i = 0; i < MAX_BlockNumber; i++)
    // {
    //     myblk = mybuffer->get_block("table0", i);
    //     myblk->data_begin[100] = 'y';
    //     mybuffer->ret_block(myblk);
    // }

    return 0;
}