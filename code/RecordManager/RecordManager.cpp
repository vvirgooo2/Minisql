#include"RecordManager.h"
#include<fstream>
//#include"../BufferManager.h"
char blockBuffer[4096];
//打印结果
//Result类型的字符串组
void RecordManager::print(const Result &res) const{ //打印结果
    for(auto const &row:res.row){
        cout<<" | ";
        for(auto const &col:row.col){
            cout<<col<<" | ";
        }
        cout<<"\n";
    }
    cout<<res.row.size()<<" selected."<<endl;
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
    
//从内存中读取元组，给定offset
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
    ofstream out(tablename+".dbf");

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
int  RecordManager::selectRecord(const Table &table, const vector<string> &attr, const vector<condition> conditions)
{   
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock = (4096)/length;
    int blockID=0;
    //Block *B=get_block(table.tablename,blockID,0);
    //char* block=B.fetch_begin();
    char *block=blockBuffer;
    
    cout<<" | ";
    for(auto itr=attr.begin(); itr!= attr.end();itr++){
        cout<<*itr<<" | ";
    }
    cout<<endl;


    Tuple t;
    Result res;
    Row r;
    
    //搜索所有条目
    while(block){
        for(int i=0;i<rcdPerBlock;i++){
            if(block[i*length]!=1) continue;
            readTuple(block,i*length,table.attri_types,t);
            if(validCheck(conditions,t)){
                r=t.fetchRow(table.attri_names,attr);
                res.row.push_back(r);
            }
        }
        blockID++;
        block=NULL;
       //B=get_block(table.tablename,blockID,0);
       //b=B.fetch_begin();
    }
    print(res);
    return res.row.size();
}

//查找记录，返回条数，有索引, 暂时不太能写
int  RecordManager::selectRecord_index(const Table &table, const vector<string> &attr, const vector<condition> conditions, const condition indexcon)
{   
    return false;
}

//插入记录
bool RecordManager::insertRecord(const Table &table, const Tuple &record)
{
   // Block *B=get_block(table.tablename,blockID,1);
   // char* block=B.fetch_begin();

    char *block=blockBuffer;
    int length = 1;
    //计算每条记录的length
    for(auto itr= table.attri_types.begin(); itr!= table.attri_types.end();itr++){
        if(itr->type==AType::String) length+=itr->char_sz+1;
        else if(itr->type==AType::Integer) length+=sizeof(int);
        else if(itr->type==AType::Float)   length+=sizeof(float);
    }
    int rcdPerBlock= (4096)/length;  //一块最多多少个
    int i=0;
    for(;i<rcdPerBlock;i++){
        if(block[i*length]==1) continue;  //该条已经有了
        else{
            block+=i*length;    //把block指针调到插入位置
            break;
        }
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
                cout<<"Invalid error";
                throw "Unknow running error";

        }
    }
    //if(4096-(block-B.fetch_begin())<length) {
    //    B.set_full();
    //}
    //ret_block(B);
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
    int rcdPerBlock= (4096)/length;  //一块最多多少个
    int blockID=0;
    
    //Block *B=get_block(table.tablename,blockID,)
    char *block=blockBuffer;
    
    Tuple t;
    //搜索所有条目
    while(block){
        for(int i=0;i<rcdPerBlock;i++){
            if(block[i*length]!=1) continue;
            readTuple(block,i*length,table.attri_types,t);
            if(validCheck(conditions,t)){    //此条记录应该被删除
                block[i*length]=0;
                /*delete index on bplus tree*/
            }
        }
        blockID++;
        block=NULL;
       //ret_block(B);
       //B=get_block(table.tablename,blockID,0);
       //b=B.fetch_begin();
    }
    return true;
}
