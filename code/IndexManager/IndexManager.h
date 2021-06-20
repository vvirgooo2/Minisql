#ifndef _INDEXMANAGER_H_
#define _INDEXMANAGER_H_
#include"../SqlDataType.h"
//#include"../API/API.h"
#include<iostream>
#include<map>
#include<string>
#include<vector>
#include<algorithm>
#include<iomanip>
#include<stdexcept>
#include<vector>

template <typename T>
class BTNode
{
    public:
        int n, size;
        bool isleaf;
        T *keys, min; //n-1 keys and the minimal value of this branch
        Position *p; //n-1 position for n-1 keys
        BTNode<T> **child;//n child node
        BTNode<T> *lastnode, *nextnode, *parent;
        BTNode(int n);
        ~BTNode();
        BTNode<T>* Insert(T key, const Position &p);
        BTNode<T>* Insert(BTNode<T> *node);
        BTNode<T>* Split();
        BTNode<T>* GetBranch(T key);
        bool Merge(BTNode *b);
        bool isroot();
        bool islegal();
};
//the class of the B+ Tree
template <typename T>
class BTree
{
    public:
        AType type;
        string attr_name;
        int n;
        BTNode<T> *root, *first;
        BTree() {};
        BTree(IndexInfo<T> &indexinfo);
        ~BTree();
        void build(IndexInfo<T> &indexinfo);
        bool Insert(const T key, const Position &p);
        bool DeleteKey(T key);
        BTNode<T> *Find(T key);
        vector<Position> GetPosition(T val, int op);

};
//the indices for one table
class TableIndex
{
    public:
        string tablename;
        int n, i_n, f_n, s_n; // the total number of indices, int indices, float indices, string indices respectively
        std::vector<BTree<int>> int_index;
        std::vector<BTree<float>> float_index;
        std::vector<BTree<string>> str_index;
        TableIndex();
        TableIndex(const string&tablename);
        ~TableIndex();
        bool CreateIndex(IndexInfo<int> &indexinfo);
        bool CreateIndex(IndexInfo<float> &indexinfo);
        bool CreateIndex(IndexInfo<string> &indexinfo);
        bool DeleteIndex(const string& attr_name);
        bool InsertKey(vector<string> index_name, vector<sqlvalue> v, const Position& p);//update when insert or delete a tuple, vector is the attributes of the tuples
        bool DeleteKey(vector<string> index_name, vector<sqlvalue> v);
        vector<Position> GetPosition(const condition& c);//return a vector of positions of the points that satisfy the consition
};
//this class is the interface of the API, the highest level to control B+Tree
class IndexManager
{
    public:
        int n;//the number of table that has an index
        vector<TableIndex> TI;
        IndexManager();
        ~IndexManager();
        bool CreateIndex(const string &tablename, IndexInfo<int> &indexinfo);
        bool CreateIndex(const string &tablename, IndexInfo<float> &indexinfo);
        bool CreateIndex(const string &tablename, IndexInfo<string> &indexinfo);
        bool DeleteIndex(const string &tablename, const string &index_name);
        //note that index_name, v, p should all be indices, selected according to catalog manager
        bool InsertKey(const string &tablename, vector<string> index_name, vector<sqlvalue> v, const Position& p);
        //note that index_name, v should all be indices, selected according to catalog manager
        bool DeleteKey(const string &tablename, vector<string> index_name, vector<sqlvalue> v);
        vector<Position> GetPosition(const string &tablename, const condition &c);
        bool Save();
        bool Read();

    private:
        TableIndex &FindTable(const string &tablename);
        void Error();
};
#endif
