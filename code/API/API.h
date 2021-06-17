#include"../SqlDataType.h"
#include"../interpreter/interpreter.h"
#include<vector>
#include<string>

//interpreter只做了语义层面的解释
//API中再检查一遍条件和存储中是否一致
//然后调用各模块来做
void API_create_table(string tablename,vector<attri_type>attris);
void API_drop_table(string tablename);
void API_create_index(string tablename,string indexname,string at_name);
void API_drop_index(string indexname);
void API_select(string tablename, vector<condition> conditions);
void API_selectpart(vector<string> attris, string tablename, vector<condition> conditions);
void API_insert(string tablename,vector<sqlvalue> value_list);
void API_delete(string tablename,vector<condition> conditions);   