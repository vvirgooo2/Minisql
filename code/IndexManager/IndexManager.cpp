#include"IndexManager.h"
//BNode
template <typename T>
BTNode<T>::BTNode(int n)
:n(n), size(0), isleaf(true), min(T(NULL)), lastnode(NULL), nextnode(NULL), parent(NULL)
{
    keys = new T[n]();
    p = new Position[n]();
    child = new BTNode<T>*[n+1];
}
template <typename T>
BTNode<T>::~BTNode()
{
    delete keys;
    delete p;
    delete child;
}
template <typename T>
bool BTNode<T>::isroot()
{
    return bool(this->parent == NULL);
}
template <typename T>
bool BTNode<T>::islegal()
{
    if (this->isleaf) {
        return bool(this->size >= (this->n) / 2) || this->isroot();
    }
    else if (this->isroot()) {
        return bool(this->size >= 2);
    }
    else
        return bool(this->size >= (this->n + 1) / 2);
}
template <typename T>
BTNode<T>* BTNode<T>::Split()
{
    BTNode<T> *node;
    node = new BTNode<T>(n);
    int i;
    if (this->isleaf)
    {
        for (i = (this->size + 1) / 2; i < this->size;i++) {
            node->p[node->size] = this->p[i];
            node->keys[node->size++] = this->keys[i];
            this->p[i].clear();
            this->keys[i] = T(NULL);
        }
        node->min = keys[0];
    }
    else {
        for (i = (this->size+1)/2; i < this->size;i++) {
            node->child[node->size] = this->child[i];
            node->child[node->size++]->parent = node;
        }
        for (i = 0; i < node->size;i++) {
            node->keys[i] = node->child[i + 1]->min;
        }
        node->min = node->child[0]->min;
    }
    //update of other infos
    this->size = (this->size + 1) / 2;
    node->isleaf = this->isleaf;
    node->parent = this->parent;
    node->nextnode = this->nextnode;
    this->nextnode = node;
    node->lastnode = this;
    return node;
}
//insert into the leaf
template <typename T>
BTNode<T>* BTNode<T>::Insert(T key, const Position &p)
{
    int i, j;
    //for nonleaf node, size if the number of branches
    if (!this->isleaf) {
        return GetBranch(key);
    }
    //for leaf nodes, size is the number of keys
    else {
        //insert the key into it
        for (i = 0; i < this->size;i++) {
            if (key < this->keys[i])
                break;
        }
        for (j = this->size-1; j >= i;j--) {
            this->p[j + 1] = this->p[j];
            this->keys[j + 1] = this->keys[j];
        }
        this->keys[i] = key;
        this->p[i] = p;
        this->size++;
        if (this->size == 1 || key < this->min)
            this->min = key;

        if (this->size > this->n - 1) {
            return this->Split();
        }
        return NULL;
    }
}
//insert into the internal node
template <typename T>
BTNode<T>* BTNode<T>::Insert(BTNode<T>* node)
{
    if (node == NULL || this->isleaf)
        return NULL;
    int i, j;
    //insert the branch node
    for (i = 0; i < this->size;i++) {
        if (node->min < this->child[i]->min)
            break;
    }
    if (i == 0) {
        node->lastnode = this->child[0]->lastnode;
    }
    if (i == this->size) {
        node->nextnode = this->child[this->size - 1]->nextnode;
    }
    for (j = this->size - 1; j >= i;j--){
        this->child[j+1] = this->child[j];
    }
    this->child[i] = node;
    this->size++;
    //update the keys of the child
    for (i = 0; i < this->size - 1;i++) {
        this->keys[i] = this->child[i + 1]->min;
        this->child[i]->nextnode = this->child[i + 1];
        this->child[i + 1]->lastnode = this->child[i];
    }
    //update other infos
    if (this->size == 1 || node->min < this->min)
        this->min = node->min;
    node->parent = this;
    if (this->size > this->n) {
        return this->Split();
    }
    return NULL;
}
template <typename T>
BTNode<T>* BTNode<T>::GetBranch(T key)
{
    if (isleaf)
        return NULL;
    int i;
    for (i = 0; i < size - 1;i++) {
        if (key < keys[i])
            break;
    }
    return child[i];
}
//this merge method assume that b is the bigger one, and the nodes in the 2 nodes are already sorted
template <typename T>
bool BTNode<T>::Merge(BTNode *b)
{
    //return true for merging successfully
    //false for failure, but the elements has been banlanced
    if (this->size == 0 || b->size == 0)
        return true;//if one of the nodes is empty, then return, no need to merge
    if (this->size >= (this->n+1)/2 && b->size >= (b->n+1)/2)
        return false;//if both are legal, return false, already balanced, no need to merge
    int i, j;
    if (isleaf) {
        //if can't merge, the balance
        if (this->size + b->size > n - 1) {
            //if b is illegal
            if (!b->islegal()){
                for (i = b->size - 1; i >= 0; i--) {
                    b->keys[i + 1] = b->keys[i];
                    b->p[i + 1] = b->p[i];
                }
                b->keys[0] = this->keys[this->size - 1];
                b->p[0] = this->p[this->size - 1];
                this->p[this->size - 1].clear();
                this->keys[this->size - 1] = T(NULL);
                this->size--;
                b->size++;
                b->min = b->keys[0];
            }
            //if this node is illgal
            else {
                this->keys[this->size] = b->keys[0];
                this->p[this->size] = b->p[0];
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->keys[i + 1];
                    b->p[i] = b->p[i + 1];
                }
                b->keys[b->size - 1] = T(NULL);
                b->p[b->size - 1].clear();
                this->size++;
                b->size--;
                b->min = b->keys[0];
            }
            return false;
        }
        //if can merge
        else {
            for (i = this->size, j = 0;j < b->size;i++,j++) {
                this->keys[i] = b->keys[j];
                this->p[i] = b->p[j];
            }
            this->size += b->size;
            this->nextnode = b->nextnode;
            if (b->nextnode != NULL) {
                b->nextnode->lastnode = this;
            }
            b->size = 0;
            return true;
        }
    }
    //for nonleaf nodes
    else {
        //if can't merge, then banlance
        if (this->size + b->size > n) {
            //if b is the illegal node
            if (!b->islegal()) {
                for (i = b->size - 1; i >= 0;i--) {
                    b->child[i + 1] = b->child[i];
                }
                b->child[0] = this->child[this->size - 1];
                b->child[0]->parent = b;
                b->min = b->child[0]->min;
                this->child[this->size - 1] = NULL;
                this->keys[this->size - 2] = T(NULL);
                this->size--;
                b->size++;
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->child[i + 1]->min;
                }
            }
            //if this node is the illegal one
            else {
                this->child[this->size] = b->child[0];
                this->keys[this->size - 1] = this->child[this->size]->min;
                for (i = 0; i < b->size - 1;i++) {
                    b->child[i] = b->child[i + 1];
                }
                b->min = b->child[0]->min;
                b->child[b->size - 1] = NULL;
                if (b->size > 1)
                    b->keys[b->size - 2] = T(NULL);
                this->size++;
                b->size--;
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->child[i + 1]->min;
                }
            }
            return false;
        }
        //if can merge
        else {
            for (i = this->size, j = 0; j < b->size;i++, j++) {
                this->child[i] = b->child[j];
            }
            this->size += b->size;
            b->size = 0;
            this->nextnode = b->nextnode;
            if (b->nextnode != NULL) {
                b->nextnode->lastnode = this;
            }
            for (i = 0; i < this->size - 1;i++) {
                this->keys[i] = this->child[i + 1]->min;
            }
        }
        return true;
    }
}
//Implentation of BTree class
template <typename T>
BTree<T>::BTree(IndexInfo<T> &indexinfo)
:type(indexinfo.type), attr_name(indexinfo.attr_name)
{
    switch(indexinfo.type)
    {
        case AType::Integer:
            n = 4096 / (indexinfo.key_maxsize + sizeof(BTNode<int> *) + sizeof(Position));
            break;
        case AType::Float:
            n = 4096 / (indexinfo.key_maxsize + sizeof(BTNode<float> *) + sizeof(Position));
            break;
        case AType::String:
            n = 4096 / (indexinfo.key_maxsize + sizeof(BTNode<string> *) + sizeof(Position));
            break;
        default:
            n = 100;
    }
    first = root = new BTNode<T>(n);
    root->isleaf = true;
    build(indexinfo);
}
template <typename T>
BTree<T>::~BTree()
{
    delete root;
}
template <typename T>
bool BTree<T>::Insert(const T key, const Position & p)
{
    BTNode<T>* temp;
    BTNode<T>* new_root;
    //find the right position
    temp = root->Insert(key, p);
    //if root is the leaf
    if (temp == NULL)
        return true;
    while (!temp->isleaf) {
        temp = temp->Insert(key, p);
    }
    //insert into the leaf nodes, temp is the new node if splitted
    temp = temp->Insert(key, p);
    //insert the new node into its parent
    while (temp != NULL) {
        //if temp is splitted by the root, then create a new root
        if (temp->isroot()) {
            new_root = new BTNode<T>(n);
            new_root->Insert(root);
            new_root->Insert(temp);
            root = new_root;
            break;
        }
        temp = temp->parent->Insert(temp);
    }
    return true;
}
template <typename T>
void BTree<T>::build(IndexInfo<T> &indexinfo)
{
    first = root = new BTNode<T>(n);
    int i;
    for (i = 0; i < indexinfo.size;i++) {
        Insert(indexinfo.keys[i], indexinfo.p[i]);
    }
    return;
}
template <typename T>
BTNode<T>* BTree<T>::Find(T key)
{
    BTNode<T> *temp;
    temp = root;
    while (!temp->isleaf) {
        temp = temp->GetBranch(key);
    }
    return temp;
}
template <typename T>
bool BTree<T>::DeleteKey(T key)
{
    int i;
    BTNode<T> *temp, *parent;
    temp = this->Find(key);
    for (i = 0; i < temp->size;i++) {
        if (key == temp->keys[i])
            break;
    }
    for (; i < temp->size - 1;i++) {
        temp->keys[i] = temp->keys[i + 1];
        temp->p[i] = temp->p[i + 1];
    }
    temp->size--;
    while (!temp->islegal()) {
        if (temp->isroot()) {
            root = temp->child[0];
            root->parent = NULL;
            delete temp;
            temp = root;
            continue;
        }
        //if noe legal, then we need to merge the 2 nodes
        if (temp->lastnode != NULL) {
            //if can merge, then temp is deleted
            if (temp->lastnode->Merge(temp)) {
                parent = temp->parent;
                for (i = 0; i < parent->size;i++) {
                    if (parent->child[i] == temp)
                        break;
                }
                for (; i < parent->size - 1;i++) {
                    parent->child[i] = parent->child[i + 1];
                }
                parent->size--;
                for (i = 0; i < parent->size;i++) {
                    parent->keys[i] = parent->child[i + 1]->min;
                }
                delete temp;
                temp = parent;
            }
            //if can't, then the 2 nodes has already been balanced, temp should be lagal
        }
        else if (temp->nextnode != NULL) {
            //don't care about the warning, temp should be initialized by find
            if (temp->Merge(temp->nextnode)) {
                parent = temp->nextnode->parent;
                for (i = 0; i < parent->size;i++) {
                    if (parent->child[i] == temp->nextnode)
                        break;
                }
                for (; i < parent->size - 1;i++) {
                    parent->child[i] = parent->child[i + 1];
                }
                parent->size--;
                for (i = 0; i < parent->size;i++) {
                    parent->keys[i] = parent->child[i + 1]->min;
                }
                delete temp->nextnode;
                temp = parent;
            }
            //if can't, then the 2 nodes has already been balanced, temp should be legal
        }
        else {
            throw runtime_error("The B+ Tree root is not valid");
        }
        temp = parent;
    }
    return true;
}
template <typename T>
vector<Position> BTree<T>::GetPosition(T val, int op)
{
    //0= 1<> 2> 3>= 4< 5<=
    BTNode<T> *temp;
    vector<Position> pv;
    int i;
    if (op == 0 || op == 2 || op == 3) {
        temp = root;
        while (!temp->isleaf) {
            temp = temp->GetBranch(val);
        }
        for (i = 0; i < temp->size;i++) {
            if (val <= temp->keys[i])
                break;
        }
        switch(op) {
            case 0: //=
                if (val != temp->keys[i])
                    pv.clear();
                else
                    pv.push_back(temp->p[i]);
                break;
            case 2: //>
                if (val == temp->keys[i]) {
                    i++;
                }
                for (; i < temp->size;i++) {
                    pv.push_back(temp->p[i]);
                }
                temp = temp->nextnode;
                while (temp != NULL)
                {
                    for (i = 0; i < temp->size;i++) {
                        pv.push_back(temp->p[i]);
                    }
                    temp = temp->nextnode;
                }
                break;
            case 3: //>=
                for (; i < temp->size;i++) {
                    pv.push_back(temp->p[i]);
                }
                temp = temp->nextnode;
                while (temp != NULL)
                {
                    for (i = 0; i < temp->size;i++) {
                        pv.push_back(temp->p[i]);
                    }
                    temp = temp->nextnode;
                }
                break;
        }
    }
    else {
        temp = first;
        //find the position of the first key that bigger or equal to the condition
        while (temp != NULL) {
            for (i = 0; i < temp->size;i++) {
                if (val <= temp->keys[i]) {
                    break;
                }
                pv.push_back(temp->p[i]);
            }
        }
        switch(op) {
            case 1:// <>
                for (i++; i < temp->size;i++) {
                    pv.push_back(temp->p[i]);
                }
                temp = temp->nextnode;
                while (temp != NULL) {
                    for (i = 0; i < temp->size;i++) {
                        pv.push_back(temp->p[i]);
                    }
                }
                break;
            case 4: //<
                break;
            case 5: //<=
                if (val == temp->keys[i]) {
                    pv.push_back(temp->p[i]);
                }
                break;
        }
    }
    return pv;
}
//
TableIndex::TableIndex()
:tablename(""), n(0), i_n(0), f_n(0), s_n(0)
{
}
TableIndex::TableIndex(const string &tablename)
:tablename(tablename), n(0), i_n(0), f_n(0), s_n(0)
{
}
TableIndex::~TableIndex()
{
    //release the memory space of the vector
    int_index.clear();
    float_index.clear();
    str_index.clear();
    int_index.resize(0);
    float_index.resize(0);
    str_index.resize(0);
}
bool TableIndex::CreateIndex(IndexInfo<int> &indexinfo)
{
    if (indexinfo.type != AType::Integer)
        return false;
    BTree<int> *BT = new BTree<int>(indexinfo);
    int_index.push_back(*BT);
    return true;
}
bool TableIndex::CreateIndex(IndexInfo<float> &indexinfo)
{
    if (indexinfo.type != AType::Float)
        return false;
    BTree<float> *BT = new BTree<float>(indexinfo);
    float_index.push_back(*BT);
    return true;
}
bool TableIndex::CreateIndex(IndexInfo<string> &indexinfo)
{
    if (indexinfo.type != AType::String)
        return false;
    BTree<string> *BT = new BTree<string>(indexinfo);
    str_index.push_back(*BT);
    return true;
}
bool TableIndex::DeleteIndex(const string& attr_name)
{
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    for (it_i = int_index.begin(); it_i != int_index.end();++it_i) {
        if (it_i->attr_name == attr_name) {
            int_index.erase(it_i);
            return true;
        }
    }
    for (it_f = float_index.begin(); it_f != float_index.end();++it_f) {
        if (it_f->attr_name == attr_name) {
            float_index.erase(it_f);
            return true;
        }
    }
    for (it_s = str_index.begin(); it_s != str_index.end();++it_s) {
        if (it_s->attr_name == attr_name) {
            str_index.erase(it_s);
            return true;
        }
    }
    throw runtime_error("[Error] Can't find the index to be deleted!");
    return false;//
}
//given the index_name and the corresponding value, when insert
bool TableIndex::InsertKey(vector<string> index_name, vector<sqlvalue> v, const Position& p)
{
    std::vector<string>::iterator it_index_name;
    std::vector<sqlvalue>::iterator it_v;
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    for (it_index_name = index_name.begin(), it_v = v.begin(); it_index_name != index_name.end() && it_v != v.end();it_index_name++, it_v++) {
        switch (it_v->type.type) {
            case AType::Integer:{
                for (it_i = int_index.begin(); it_i != int_index.end();it_i++) {
                    if (it_i->attr_name == *it_index_name) {
                        it_i->Insert(it_v->i, p);
                        continue;
                    }
                }
                break;
            }
            case AType::Float: {
                for (it_f = float_index.begin(); it_f != float_index.end();it_f++) {
                    if (it_f->attr_name == *it_index_name) {
                        it_f->Insert(it_v->f, p);
                        continue;
                    }
                }
                break;
            }
            case AType::String: {
                for (it_s = str_index.begin(); it_s != str_index.end();it_s++) {
                    if (it_s->attr_name == *it_index_name) {
                        it_s->Insert(it_v->str, p);
                        continue;
                    }
                }
                break;
            }
        }
        throw runtime_error("[Error] Can't find the index " + *it_index_name + " to be updated for table " + tablename);
    }
    return true;
}
bool TableIndex::DeleteKey(vector<string> index_name, vector<sqlvalue> v)
{
    std::vector<string>::iterator it_index_name;
    std::vector<sqlvalue>::iterator it_v;
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    for (it_index_name = index_name.begin(), it_v = v.begin(); it_index_name != index_name.end() && it_v != v.end();it_index_name++, it_v++) {
        switch (it_v->type.type) {
            case AType::Integer:{
                for (it_i = int_index.begin(); it_i != int_index.end();it_i++) {
                    if (it_i->attr_name == *it_index_name) {
                        it_i->DeleteKey(it_v->i);
                        continue;
                    }
                }
                break;
            }
            case AType::Float: {
                for (it_f = float_index.begin(); it_f != float_index.end();it_f++) {
                    if (it_f->attr_name == *it_index_name) {
                        it_f->DeleteKey(it_v->f);
                        continue;
                    }
                }
                break;
            }
            case AType::String: {
                for (it_s = str_index.begin(); it_s != str_index.end();it_s++) {
                    if (it_s->attr_name == *it_index_name) {
                        it_s->DeleteKey(it_v->str);
                        continue;
                    }
                }
                break;
            }
        }
        throw runtime_error("[Error] Can't find the index " + *it_index_name + " to be updated for table " + tablename);
    }
    return true;
}
vector<Position> TableIndex::GetPosition(const condition& c)
{
    switch (c.val.type.type) {
        case AType::Integer:{
            std::vector<BTree<int>>::iterator it;
            for (it = int_index.begin(); it != int_index.end();it++)
                if (it->attr_name == c.name)
                    return it->GetPosition(c.val.i, c.op);
        }
        case AType::Float:{
            std::vector<BTree<float>>::iterator it;
            for (it = float_index.begin(); it != float_index.end();it++)
                if (it->attr_name == c.name)
                    return it->GetPosition(c.val.f, c.op);
        }
        case AType::String:{
            std::vector<BTree<string>>::iterator it;
            for (it = str_index.begin(); it != str_index.end();it++)
                if (it->attr_name == c.name)
                    return it->GetPosition(c.val.str, c.op);
        }
    }
    throw runtime_error("[Error] the attribute " + c.name + " in the where condition is not a index");
}
IndexManager::IndexManager()
:n(0)
{
}
IndexManager::~IndexManager()
{
    TI.clear();
    TI.resize(0);
}

TableIndex& IndexManager::FindTable(const string &tablename)
{
    std::vector<TableIndex>::iterator it_table;
    for (it_table = TI.begin(); it_table != TI.end();it_table++) {
        if (it_table->tablename == tablename) {
            return *it_table;
        }
    }
    throw runtime_error("Can't find table " + tablename + " in index manager");
}
void IndexManager::Error()
{

}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<int> &indexinfo)
{
    return this->FindTable(tablename).CreateIndex(indexinfo);
}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<float> &indexinfo)
{
    return this->FindTable(tablename).CreateIndex(indexinfo);
}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<string> &indexinfo)
{
    return this->FindTable(tablename).CreateIndex(indexinfo);
}
bool IndexManager::DeleteIndex(const string &tablename, const string &index_name)
{
    return this->FindTable(tablename).DeleteIndex(index_name);
}
bool IndexManager::InsertKey(const string &tablename, vector<string> index_name, vector<sqlvalue> v, const Position& p)
{
    return this->FindTable(tablename).InsertKey(index_name, v, p);
}
bool IndexManager::DeleteKey(const string &tablename, vector<string> index_name, vector<sqlvalue> v)
{
    return this->FindTable(tablename).DeleteKey(index_name, v);
}
vector<Position> IndexManager::GetPosition(const string &tablename, const condition &c)
{
    return this->FindTable(tablename).GetPosition(c);
}
bool IndexManager::Save()
{
    return true;
}
bool IndexManager::Read()
{
    return true;
}


