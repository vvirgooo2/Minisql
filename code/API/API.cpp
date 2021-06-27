#include "API.h"
#include"../SqlDataType.h"
#include"../interpreter/interpreter.h"
#include"../RecordManager/RecordManager.h"
#include"../IndexManager/IndexManager.h"
#include"../BufferManager/BufferManager.h"
#include"../CatalogManager/CatalogManager.h"
#include<vector>
#include<string>
using namespace std;
RecordManager *rm;
BufferManage bm;
IndexManager im;
CatalogManager cm;
Table get_test_table(){
 //属性
    vector<attri_type> attris;
    attri_type attr;
    attr.type=AType::Integer;
    attr.attri_name="id";
    attris.push_back(attr);


    attr.type=AType::String;
    attr.attri_name="name";
    attr.char_sz=12;
    attris.push_back(attr);

    attr.type=AType::Float;
    attr.attri_name="score";
    attris.push_back(attr);

    //Fetch
    vector<string> Fetch;
    Fetch.push_back("id");
    Fetch.push_back("name");
    Fetch.push_back("score");

    Table table;
    table.tablename="student2";
    table.attri_names=Fetch;
    table.attri_types=attris;
    table.attri_count=3;
    table.attri_types[0].primary=true;
    table.attri_types[1].unique=true;
    return table;

}

//API层建表
//功能：
//先判断有无表名冲突
//判断有无主键
//判断属性是否符合条件
//传给Catalog建表，传给Record建文件，传给Index建主键索引
void API_create_table(string tablename,vector<attri_type>attris){
    //先判断有无表名冲突
#ifdef CMDEBUG
    auto &test=cm.GetTable(tablename);
    for(int i=0;i<test.attri_names.size();i++){
        cout<<test.attri_names[i]<<endl;
    }
    for(auto itr=test.index.begin(); itr!= test.index.end();itr++){
        cout<<itr->first<<" "<<itr->second<<endl;
    }
#endif
    if (cm.ExistTable(tablename)) throw std::runtime_error("Table already exists!");
    //判断有无主键
    int primaryflag=0;
    for(int i=0;i<attris.size();i++){
        if(attris[i].primary==true) primaryflag++;
        if(attris[i].type==AType::String&&attris[i].char_sz>255) 
            throw std::runtime_error("Length of char is bigger than 255");
        if(attris[i].type==AType::String&&attris[i].char_sz<=0)
            throw std::runtime_error("Length of char is smaller than 0");
    }
    if(primaryflag==0) throw std::runtime_error("No primary key");
    else if(primaryflag>1) throw std::runtime_error("More than one primary key");
    //传给Catalog建表
    //cout<<attris[1].char_sz<<endl;
    cm.CreateTable(tablename, attris);
    cm.WriteToFile();
    //传给Index建主键索引，在创建表的时候完成初始化一个空的b+tree
    
}

//删除表
//判断存在
//Catalog删除信息，Record删除表文件，Index删除有关索引
void API_drop_table(string tablename){
    //判断存在
    if (!cm.ExistTable(tablename)) {
       throw std::runtime_error("Not exist this table");
        //return false;
    }
    auto table = cm.GetTable(tablename);
    //Catalog删除信息
    cm.RemoveTable(table);
    cm.WriteToFile();
    std::cout << "Table " << tablename << " dropped." << std::endl;
    //Record删除表文件
    rm->dropTable(tablename);
    //Index删除有关索引
    for(int i=0;i<table.index.size();i++){
        im.DeleteIndex(table.tablename,table.index[i].first);
        cout<<table.index[i].second<<" dropped."<<endl;
    }
}

//创建索引：
//判断表，属性
//传给Index建表，Catalog更新信息
void API_create_index(string tablename,string indexname,string at_name){
    // check if table exists
    if (!cm.ExistTable(tablename)) {
        throw std::runtime_error("Table not exist.");
    }
    auto &table = cm.GetTable(tablename);
    int attrFetch=-1;
    for(int i=0;i<table.attri_names.size();i++){
        if(at_name==table.attri_names[i]){
            attrFetch=i;
            if(table.attri_types[i].primary) throw std::runtime_error("This primary key has auto Index");
            else if(!table.attri_types[i].unique) throw std::runtime_error("Can't create index on a key not unique");
        }
    }
    if(attrFetch==-1) throw std::runtime_error("Attribute not exists in this table");

    // check if there already has an index
    for (auto &index: table.index) {
        if (index.first == at_name) {
            throw std::runtime_error("This attribute already has a index");
        }
    }

    //record a the catalog
    table.index.emplace_back(std::make_pair(at_name,indexname));
    cm.WriteToFile();

    // call managers to create empty bplus tree
    if(table.attri_types[attrFetch].type==AType::String){
        IndexInfo<string> info(tablename,0,table.attri_types[attrFetch].attri_name,AType::String,table.attri_types[attrFetch].char_sz+1);
        im.CreateIndex(tablename,info);
    }
    else if(table.attri_types[attrFetch].type==AType::Integer){
        IndexInfo<int> info(tablename,0,table.attri_types[attrFetch].attri_name,AType::Integer,sizeof(int));
        im.CreateIndex(tablename,info);
    }
    else if(table.attri_types[attrFetch].type==AType::Float){
        IndexInfo<float> info(tablename,0,table.attri_types[attrFetch].attri_name,AType::Float,sizeof(float));
        im.CreateIndex(tablename,info);
    }
    
    //call record manager to add nodes
    rm->CreateIndex(table,table.attri_types[attrFetch]);
}

//删除索引
//传给index删除索引，Catalog更新信息
void API_drop_index(string indexname){
    bool index = cm.ExistIndex(indexname);
    if(!index){
        throw std::runtime_error("Index not exists");
    }
    auto &table = cm.GetIndex(indexname);
    //catalogshanchu 
    string att;
    for (auto &idx: table.index) {
        if (idx.second == indexname) {
            //传给index删除索引
            table.index.erase(std::find_if(table.index.begin(), table.index.end(),
                                           [&indexname](const std::pair<std::string, std::string> &item) {
                                               return item.second == indexname;
                                           }));
            att=idx.first;
            std::cout << "Index " << indexname << " dropped." << std::endl;
        }
    }
    cm.WriteToFile();
    //indexmanager delete
    im.DeleteIndex(table.tablename,att);
}

//选择（全选）
//判断表名，判断条件是否合理
//传给record
void API_select(string tablename, vector<condition> conditions){
    if(!cm.ExistTable(tablename)) throw std::runtime_error("No such table!");
    Table table=cm.GetTable(tablename);
    vector<string> attris=table.attri_names;
    API_selectpart(attris,tablename,conditions);
}

//选择（部分）
//判断属性名，条件的合理性
//注意属性名参数只有name成员被赋值了
void API_selectpart(vector<string> attris, string tablename, vector<condition> conditions){
    if(!cm.ExistTable(tablename)) throw std::runtime_error("No such table!");
    auto table=cm.GetTable(tablename);
    //check the table
    //检查属性是否存在
    bool attrExist=false;
    for(int i=0;i<attris.size();i++){
        for(int j=0;j<table.attri_names.size();j++){
            if(attris[i]==table.attri_names[j]){
                attrExist=true;
                break;
            }
        }
        if(!attrExist) throw std::runtime_error("Attributes is not existed.");
        attrExist=false;
    }

    bool attrFetch=false;
    //处理条件，int与float,主要是类型冲突
    for(int i=0;i<conditions.size();i++){
        for(int j=0;j<table.attri_names.size();j++){
            if(conditions[i].name==table.attri_names[j]){
                if(conditions[i].val.type.type==AType::Integer&&
                    table.attri_types[j].type==AType::Float){
                        conditions[i].val.type.type=AType::Float;
                        conditions[i].val.f=(float)conditions[i].val.i;
                    }
                else if(conditions[i].val.type.type!=table.attri_types[j].type) throw std::runtime_error("Type conflict!");
                if(conditions[i].val.type.type==AType::String&&
                    conditions[i].val.str.size()>table.attri_types[j].char_sz) throw runtime_error("Require string is too long!");
                
                attrFetch=true;
                break;
            }
        }
        if(!attrFetch) throw std::runtime_error("SYNTAX ERROR: Unknown attributes in conditions!");
        attrFetch=false;
    }
    //检查能否利用索引,查到的第一个
    condition indexcon;
    for(int i=0;i<conditions.size();i++){
        for(int j=0;j<table.index.size();j++){
            if(conditions[i].name==table.index[j].first){
                indexcon=conditions[i];
                rm->selectRecord_index(table,attris,conditions,indexcon,true);
                return;
            }
        }
    }
    rm->selectRecord(table,attris,conditions,true);
}


//插入
//判断表名
//预处理值表（判断和表是否对应）,一定要把string的长度填充上！！！！！！
//调用record的select或索引判断主键或unique键的冲突
//无问题给record插入，顺便同步给index
void API_insert(string tablename,vector<sqlvalue> value_list){
    if(value_list[0].i%1000==0) cout<<value_list[0].i<<endl;
    
    //check the table
    if(!cm.ExistTable(tablename)) throw std::runtime_error("No such table!");
    Table table=cm.GetTable(tablename);
    //prepare the tuple
    if(value_list.size()!=table.attri_count) throw std::runtime_error("The number of values is not equal to the number of attributes!");
    for(int i=0;i<value_list.size();i++){
        //float2int
        if(value_list[i].type.type==AType::Integer&&table.attri_types[i].type==AType::Float){
            value_list[i].type.type=AType::Float;
            value_list[i].f=value_list[i].i;
        }
        if(value_list[i].type.type!=table.attri_types[i].type){
            throw std::runtime_error("Type of value is violated.");
        }
        if(value_list[i].type.type==AType::String&&value_list[i].str.size()>table.attri_types[i].char_sz){
            throw std::runtime_error("You insert a too long string to "+table.attri_names[i]);
        }
        if(value_list[i].type.type==AType::String){
            value_list[i].type.char_sz=table.attri_types[i].char_sz;
        }
        value_list[i].type.attri_name=table.attri_names[i];
    } 
    //check duplicate

    for(int i=0;i<value_list.size();i++){
        bool flagin=false;
        if(table.attri_types[i].primary||table.attri_types[i].unique){
            vector<condition> conditions;
            condition indexcon;
            indexcon.name=table.attri_names[i];
            indexcon.op=0;
            indexcon.val=value_list[i];
            conditions.push_back(indexcon);
            //check index
            for(int j=0;j<table.index.size();j++){
                if(table.index[j].first==table.attri_names[i]){
                    flagin=true;
                    vector<Position> pos;
                    pos=im.GetPosition(table.tablename,indexcon);
                    if(!pos.empty()) throw std::runtime_error("Duplicate keys in "+table.attri_names[i]);
                }
            }
            if(flagin) continue;
            //noindex
            if(rm->selectRecord(table,table.attri_names,conditions,false)){
                throw std::runtime_error("Duplicate keys in "+table.attri_names[i]);
            }
        }
    }
    Tuple t;
    t.element=value_list;
    rm->insertRecord(table,t);
}

//删除
//判断表名
//预处理条件
//给record删除，同步index
void API_delete(string tablename,vector<condition> conditions){
    if(!cm.ExistTable(tablename)) throw std::runtime_error("No such table!");
    //check the table
    Table table=cm.GetTable(tablename);
    //check the conditions
    bool attrFetch=false;
    //处理条件，int与float,主要是类型冲突
    for(int i=0;i<conditions.size();i++){
        for(int j=0;j<table.attri_names.size();j++){
            if(conditions[i].name==table.attri_names[j]){
                if(conditions[i].val.type.type==AType::Integer&&
                    table.attri_types[j].type==AType::Float){
                        conditions[i].val.type.type=AType::Float;
                        conditions[i].val.f=(float)conditions[i].val.i;
                    }
                else if(conditions[i].val.type.type!=table.attri_types[j].type) throw std::runtime_error("Type conflict!");
                if(conditions[i].val.type.type==AType::String&&
                    conditions[i].val.str.size()>table.attri_types[j].char_sz) throw runtime_error("Require string is too long!");
                
                attrFetch=true;
                break;
            }
        }
        if(!attrFetch) throw std::runtime_error("SYNTAX ERROR: Unknown attributes in conditions!");
        attrFetch=false;
    }
    rm->deleteRecord(table,conditions);
}
