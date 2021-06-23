#include"RecordManager.h"
#include"../IndexManager/IndexManager.h"
#include"../BufferManager/BufferManager.h"
#include<fstream>
#include<iomanip>
extern BufferManage bm;
extern IndexManager im;
//打印结果
//Result类型的字符串组
void RecordManager::print(const Result &res) const{ //打印结果
    for(auto const &row:res.row){
        cout<<" | ";
        for(auto const &col:row.col){
            cout<<setw(10)<<col<<" | ";
        }
        cout<<"\n";
    }
}  

//检查元组是否符合条件
bool RecordManager::validCheck(const vector<condition> conditions, const Tuple tu)
{
    int pos;
    for(condition con: conditions){
        pos=-1;
        //找到条件对应的属性是第几列
        for(size_t i=0;i<tu.element.size();i++){
            if(con.name==tu.element[i].type.attri_name){
                pos=i;
            }
        }
        //核对属性，按理来说API中应该已经核对过了
        if(pos==-1) return false;
        if(con.val.type.type!=tu.element[pos].type.type) return false;
        
        //核对条件是否成立
        switch(con.op){
            case 0:{
                if(con.val!=tu.element[pos]) return false;
                break;
            }
            case 1:{
                if(con.val==tu.element[pos]) return false;
                break;
            }
            case 2:{
                if(con.val>=tu.element[pos]) return false;
                break;
            }
            case 3:{
                if(con.val>tu.element[pos]) return false;
                break;
            }
            case 4:{
                if(con.val<=tu.element[pos]) return false;
                break;
            }
            case 5:{
                if(con.val<tu.element[pos]) return false;
                break;
            }
            default: return false;
        }

    }
    return true;
}
    
//从内存中读取元组，给定block和offset
void RecordManager::readTuple(const char *blockBuffer,int offset, const vector<attri_type> &attris, Tuple&tu)
{
    sqlvalue v;
    tu.element.clear();
    const char *block = blockBuffer+offset+1;
    for(size_t i=0;i<attris.size();i++){
        v.reset();
        v.type = attris[i];
        if(attris[i].type==AType::Integer){
            memcpy(&v.i,block,sizeof(int));
            block+=sizeof(int);
        }
        else if(attris[i].type==AType::Float){
            memcpy(&v.f,block,sizeof(float));
            block+=sizeof(float);
        }
        else if(attris[i].type==AType::String){
            v.str=block;
            block+=attris[i].char_sz+1;
        }
        tu.element.push_back(v);
    }
}

//创建表文件
bool RecordManager::createTable(const string tablename)
{
    //ofstream out(tablename+".dbf");
    return true;
}

//删除表文件
bool RecordManager::dropTable(const string tablename)
{
    //BufferManager_dropTableFile(tablename+".dbf");
    return true;
}

//查找记录,返回条数,无须辅助
//可能每个块头的偏移要计算一下
int  RecordManager::selectRecord(const Table &table, const vector<string> &attr, const vector<condition> conditions,bool output)
{   try{
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock = (4096-BLOCK_HEADER_SIZE)/length;
    int blockID=0;
    int total=get_total_block_num(table.tablename);
    Block *B=bm.get_block(table.tablename,blockID);
    char* block=B->data_begin;

    //output attris
    if(output){
        cout<<" | ";
        for(auto itr=attr.begin(); itr!= attr.end();itr++){
            cout<<setw(10)<<*itr<<" | ";
        }
        cout<<endl;
    }

    Tuple t;
    int rownum = 0;
    Row r;
    
        
        
    //搜索所有条目
    while(block){
        Result res;
        for(int i=0;i<rcdPerBlock;i++){
            if(block[i*length]!=1) continue;
            readTuple(block,i*length,table.attri_types,t);
            if(validCheck(conditions,t)){
                r=t.fetchRow(table.attri_names,attr);
                res.row.push_back(r);
            }
        }
        if(output) print(res);
        rownum+=res.row.size();
        blockID++;
        if(blockID>total) break;
        B=bm.get_block(table.tablename,blockID);  
        block=B->data_begin;
    }
    if(output) cout<<rownum<<" selected."<<endl;
    return rownum;
    }
    catch(std::runtime_error &error){
        cout << "[Error] " << error.what() << endl;
    }
}

//查找记录，返回条数，有索引,可以传进来一个有索引的条件
int  RecordManager::selectRecord_index(const Table &table, const vector<string> &attr, const vector<condition> conditions, const condition indexcon,bool output)
{   
    vector<Position> positions;
    positions=im.GetPosition(table.tablename,indexcon);

#ifdef  INDEX_DEBUG
    cout<<"positions's information:"<<endl;
    for(auto t=positions.begin();t!=positions.end();t++){
        cout<<"BlockID: "<<t->blockID<<" offset: "<<t->offset<<endl;
    }
#endif

    if(output){
        cout<<" | ";
        for(auto itr=attr.begin(); itr!= attr.end();itr++){
            cout<<setw(10)<<*itr<<" | ";
        }
        cout<<endl;
    }

    //先用索引信息定位，然后进一步判断
    auto itr=positions.begin();
    Tuple t;
    Result res;
    Row r;
    for(;itr!=positions.end();itr++){
        Block *B=bm.get_block(table.tablename,itr->blockID);
        char *block=B->data_begin;
        readTuple(block,itr->offset,table.attri_types,t);
        if(validCheck(conditions,t)){
            r=t.fetchRow(table.attri_names,attr);
            res.row.push_back(r);
        }
    }
    if(output) print(res);
    return res.row.size();
}

//插入记录
bool RecordManager::insertRecord(const Table &table, const Tuple &record)
{
    int blockID=get_total_block_num(table.tablename);
    //怎么get到最后一块
    Block *B=bm.get_block(table.tablename,blockID);
    char* block=B->data_begin;

    Position pos;
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock= (4096-BLOCK_HEADER_SIZE)/length;  //一块最多多少个
    int i=0;
    bool flag=false;
    for(;i<rcdPerBlock;i++){
        if(block[i*length]==1) continue;  //该条已经有了
        else{
            flag=true;          //如果该块有空间
            block+=i*length;    //把block指针调到插入位置
            pos.blockID=blockID;
            pos.offset=i*length;
            break;
        }
    }

    if(!flag){
        //get到下一块，顺便把这张表的总块数+1
        blockID++;
        B=bm.get_block(table.tablename,blockID);
        edit_total_block_num(table.tablename,1);
        block=B->data_begin;
        pos.blockID=blockID;
        pos.offset=0;
    }

    *block++ = 1;   //valid=1
    string fixstr;
    //插入记录
    for(auto sqlv=record.element.begin();sqlv!=record.element.end();sqlv++){
        switch(sqlv->type.type){
            case AType::String:
                fixstr=sqlv->str;
                if(sqlv->type.char_sz>fixstr.length()) fixstr+="\0"; //如果串小于允许空间，要补一个结束符
                memcpy(block,fixstr.c_str(),sqlv->type.char_sz+1);
                block+=sqlv->type.char_sz+1;
                break;
            case AType::Integer:
                memcpy(block,&sqlv->i,sizeof(int));
                block+=sizeof(int);
                break;
            case AType::Float:
                memcpy(block,&sqlv->f,sizeof(float));
                block+=sizeof(float);
                break;
            default:
                throw runtime_error("Unknown running error");

        }
    }
    //同步索引
    vector<sqlvalue> values;
    for(int i=0;i<table.attri_names.size();i++){
        for(int j=0;j<table.index.size();j++){ 
            if(table.attri_names[i]==table.index[j].first){  //只要有索引就插入，注意sqlvalue里面要有属性名
                sqlvalue v=record.element[i];
                values.push_back(v);  
            }
        }   
    }
    //im.InsertKey(table.tablename,values,pos);
    bm.ret_block(B);
    return true;
}
    
//删除记录，没有索引
bool RecordManager::deleteRecord(const Table &table, const vector<condition> conditions)
{
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock= (4096-BLOCK_HEADER_SIZE)/length;  //一块最多多少个
    int blockID=0;
    int total=get_total_block_num(table.tablename);
    Block *B=bm.get_block(table.tablename,blockID);
    char *block=B->data_begin;
    
    Tuple t;
    
    //搜索所有条目
    while(block){
        for(int i=0;i<rcdPerBlock;i++){
            if(block[i*length]!=1) continue;
            readTuple(block,i*length,table.attri_types,t);
            if(validCheck(conditions,t)){    //此条记录应该被删除
                block[i*length]=0;
                /*delete index on bplus tree*/
                //检查每个属性是否有索引，有的话就更新
                vector<sqlvalue> values;
                for(int i=0;i<table.attri_names.size();i++){
                    for(int j=0;j<table.index.size();j++){ 
                        if(table.attri_names[i]==table.index[j].first){
                            sqlvalue v=t.element[i];
                            values.push_back(v);  
                        }   
                    }
                }
                im.DeleteKey(table.tablename,values);
            }
        }
        bm.ret_block(B);
        blockID++;
        if(blockID>total) break;
        //怎么判断是最后一块
        B=bm.get_block(table.tablename,blockID);
        block=B->data_begin;
    }
    return true;
}

//创建索引
bool RecordManager::CreateIndex(const Table &table, const attri_type indexattr, const string indexname){
    //bool InsertKey(const string &tablename, vector<string> index_name, 
    //vector<sqlvalue> v, const Position& p);
    int attrpos;
    for(int i=0;i<table.attri_names.size();i++){
        if(table.attri_names[i]==indexattr.attri_name){
            attrpos=i;
        }
    }
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock = (4096-BLOCK_HEADER_SIZE)/length;
    int blockID=0;
    int total=get_total_block_num(table.tablename);
    Block *B=bm.get_block(table.tablename,blockID);
    char* block=B->data_begin;

    Tuple t;
    Result res;
    Row r;
    Position pos;
    //搜索所有条目
    while(block){
        for(int i=0;i<rcdPerBlock;i++){
            if(block[i*length]!=1) continue;
            //every key should be added into index
            readTuple(block,i*length,table.attri_types,t);
            pos.clear();
            pos.blockID=blockID;
            pos.offset=i*length;
            vector<sqlvalue> values;
            sqlvalue v=t.element[attrpos];
            values.push_back(v);
            im.InsertKey(table.tablename,values,pos);
        }
        blockID++;
        if(blockID>total) break;
        B=bm.get_block(table.tablename,blockID); 
        block=B->data_begin;
    }
    return true;
}
