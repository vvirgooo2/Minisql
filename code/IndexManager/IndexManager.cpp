#include"IndexManager.h"

bool clearkey(int &key)
{
    key = 0;
    return true;
}
bool clearkey(float &key)
{
    key = 0;
    return true;
}
bool clearkey(string &key)
{
    key.clear();
    return true;
}
//BNode
template <typename T>
BTNode<T>::BTNode(int n)
:n(n), size(0), isleaf(false), lastnode(NULL), nextnode(NULL), parent(NULL), id_in_parent(0)
{
    clearkey(min);
    keys = new T[n]();
    p = new Position[n]();
    child = new BTNode<T>*[n+1];
}
template <typename T>
BTNode<T>::~BTNode()
{
    delete []keys;
    delete []p;
    delete []child;
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
            clearkey(this->keys[i]);
        }
        node->min = node->keys[0];
    }
    else {
        for (i = (this->size+1)/2; i < this->size;i++) {
            node->child[node->size] = this->child[i];
            node->child[node->size]->parent = node;
            node->size++;
            this->child[i] = NULL;
            clearkey(this->keys[i - 1]);
        }
        for (i = 0; i < node->size - 1;i++) {
            node->keys[i] = node->child[i + 1]->min;
            node->child[i]->id_in_parent = i;
        }
        node->child[i]->id_in_parent = i;
        node->min = node->child[0]->min;
    }
    //update of other infos
    this->size = (this->size + 1) / 2;
    node->isleaf = this->isleaf;
    node->parent = this->parent;
    if (this->nextnode != NULL) {
        this->nextnode->lastnode = node;
    }
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
    if (this->size == 0) {
        this->child[0] = node;
        this->size++;
        this->min = node->min;
        node->parent = this;
        node->id_in_parent = 0;
        node->nextnode = node->lastnode = NULL;
        return NULL;
    }
    for (j = this->size - 1; j >= i;j--){
        this->child[j+1] = this->child[j];
    }
    this->child[i] = node;
    this->size++;
    node->parent = this;
    node->id_in_parent = i;
    //update the keys of the child
    for (i = 0; i < this->size - 1;i++) {
        this->keys[i] = this->child[i + 1]->min;
        this->child[i]->nextnode = this->child[i + 1];
        this->child[i + 1]->lastnode = this->child[i];
        this->child[i]->id_in_parent = i;
    }
    this->child[i]->id_in_parent = i;
    //update other infos
    if (node->min < this->min)
        this->min = node->min;
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
//the merge also update the data in the front,
template <typename T>
bool BTNode<T>::Merge(BTNode *b)
{
    //return true for merging successfully
    //false for failure, but the elements has been banlanced
    BTNode<T> *temp;
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
                clearkey(this->keys[this->size - 1]);
                this->size--;
                b->size++;
                b->min = b->keys[0];
                temp = b;
                while (!temp->isroot()) {
                    if (temp->id_in_parent != 0) {
                        temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                    }
                    temp->parent->min = temp->parent->child[0]->min;
                    temp = temp->parent;
                }
            }
            //if this node is illgal
            else {
                this->keys[this->size] = b->keys[0];
                this->p[this->size] = b->p[0];
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->keys[i + 1];
                    b->p[i] = b->p[i + 1];
                }
                clearkey(b->keys[b->size - 1]);
                b->p[b->size - 1].clear();
                this->size++;
                b->size--;
                b->min = b->keys[0];
                temp = b;
                while (!temp->isroot()) {
                    if (temp->id_in_parent != 0) {
                        temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                    }
                    temp->parent->min = temp->parent->child[0]->min;
                    temp = temp->parent;
                }
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
            //update of the parent node
            temp = b->parent;
            for (i = b->id_in_parent; i < temp->size - 1;i++) {
                temp->child[i] = temp->child[i + 1];
                temp->child[i]->id_in_parent = i;
            }
            temp->child[temp->size - 1] = NULL;
            if (temp->size > 1)
                clearkey(temp->keys[temp->size - 2]);
            temp->size--;
            for (i = 0; i < temp->size - 1;i++) {
                temp->keys[i] = temp->child[i + 1]->min;
            }
            temp->min = temp->child[0]->min;
            while (!temp->isroot()) {
                if (temp->id_in_parent != 0) {
                    temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                }
                temp->parent->min = temp->parent->child[0]->min;
                temp = temp->parent;
            }
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
                b->child[0]->id_in_parent = 0;
                b->min = b->child[0]->min;
                this->child[this->size - 1] = NULL;
                if (this->size > 1)
                    clearkey(this->keys[this->size - 2]);
                this->size--;
                b->size++;
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->child[i + 1]->min;
                    b->child[i]->id_in_parent = i;
                }
                b->child[i]->id_in_parent = i;
                temp = b;
                while (!temp->isroot()) {
                    if (temp->id_in_parent != 0) {
                        temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                    }
                    temp->parent->min = temp->parent->child[0]->min;
                    temp = temp->parent;
                }
            }
            //if this node is the illegal one
            else {
                this->child[this->size] = b->child[0];
                this->keys[this->size - 1] = this->child[this->size]->min;
                this->child[this->size]->parent = this;
                this->child[this->size]->id_in_parent = this->size;
                for (i = 0; i < b->size - 1;i++) {
                    b->child[i] = b->child[i + 1];
                    b->child[i]->id_in_parent = i;
                }
                b->min = b->child[0]->min;
                b->child[b->size - 1] = NULL;
                if (b->size > 1)
                    clearkey(b->keys[b->size - 2]);
                this->size++;
                b->size--;
                for (i = 0; i < b->size - 1;i++) {
                    b->keys[i] = b->child[i + 1]->min;
                    b->child[i]->id_in_parent = i;
                }
                b->child[i]->id_in_parent = i;
                temp = b;
                while (!temp->isroot()) {
                    if (temp->id_in_parent != 0) {
                        temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                    }
                    temp->parent->min = temp->parent->child[0]->min;
                    temp = temp->parent;
                }
            }
            return false;
        }
        //if can merge
        else {
            for (i = this->size, j = 0; j < b->size;i++, j++) {
                this->child[i] = b->child[j];
                this->child[i]->id_in_parent = i;
                this->child[i]->parent = this;
            }
            this->size += b->size;
            b->size = 0;
            this->nextnode = b->nextnode;
            if (b->nextnode != NULL) {
                b->nextnode->lastnode = this;
            }
            for (i = 0; i < this->size - 1;i++) {
                this->keys[i] = this->child[i + 1]->min;
                this->child[i]->id_in_parent = i;
            }
            this->child[i]->id_in_parent = i;
            //update of of the parent node
            temp = b->parent;
            for (i = b->id_in_parent; i < temp->size - 1;i++) {
                temp->child[i] = temp->child[i + 1];
                temp->child[i]->id_in_parent = i;
            }
            temp->child[temp->size - 1] = NULL;
            if (temp->size > 1)
                clearkey(temp->keys[temp->size - 2]);
            temp->size--;
            //updates of keys
            for (i = 0; i < temp->size - 1;i++) {
                temp->keys[i] = temp->child[i + 1]->min;
                temp->child[i]->id_in_parent = i;
            }
            temp->child[i]->id_in_parent = i;
            temp->min = temp->child[0]->min;
            while (!temp->isroot()) {
                if (temp->id_in_parent != 0) {
                    temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                }
                temp->parent->min = temp->parent->child[0]->min;
                temp = temp->parent;
            }
            return true;
        }
    }
}
//Implentation of BTree class
template <typename T>
BTree<T>::BTree(IndexInfo<T> &indexinfo)
:type(indexinfo.type), attr_name(indexinfo.attr_name), num_keys(0), key_maxsize(indexinfo.key_maxsize)
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
    BTNode<T>* temp, *temp_split;
    BTNode<T>* new_root;
    //find the right position,
    temp = this->Find(key);
    temp_split = temp->Insert(key, p);
    //if root is the leaf, and no split
    while (!temp->isroot()) {
        if (temp->id_in_parent != 0) {
            temp->parent->keys[temp->id_in_parent - 1] = temp->min;
        }
        if (key < temp->parent->min) {
            temp->parent->min = key;
        }
        temp = temp->parent;
    }
    if (temp_split == NULL) {
        num_keys++;
        return true;
    }
    temp = temp_split;
    //now temp points to the NULL, which means no split, or not NULL, which means split occurs, for latter, we insert the new node to it's parent
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
    num_keys++;
    return true;
}
template <typename T>
void BTree<T>::build(IndexInfo<T> &indexinfo)
{
    //first = root = new BTNode<T>(n);
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
    BTNode<T> *temp, *parent, *latch, *next_node;
    temp = this->Find(key);
    //find the right place of key
    for (i = 0; i < temp->size;i++) {
        if (key == temp->keys[i])
            break;
    }
    //if can't find the key, then throw error
    if (i == temp->size) {
        throw runtime_error("The key to be deleted can't be found in the index Tree");
    }
    //move the key in the leaf
    for (; i < temp->size - 1;i++) {
        temp->keys[i] = temp->keys[i + 1];
        temp->p[i] = temp->p[i + 1];
    }
    clearkey(temp->keys[i]);
    temp->p[i].clear();
    temp->size--;
    temp->min = temp->keys[0];
    //update the min of the parent
    latch = temp;
    while (!temp->isroot()) {
        if (temp->id_in_parent != 0) {
            temp->parent->keys[temp->id_in_parent - 1] = temp->min;
        }
        temp->parent->min = temp->parent->child[0]->min;
        temp = temp->parent;
    }
    temp = latch;

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
                delete temp;
                temp = parent;
            }
            //if can't, then the 2 nodes has already been balanced, we should update the min of its parent
            else {
                latch = temp;
                //update the min of the parent
                while (!temp->isroot()) {
                    if (temp->id_in_parent != 0) {
                        temp->parent->keys[temp->id_in_parent - 1] = temp->min;
                    }
                    temp->parent->min = temp->parent->child[0]->min;
                    temp = temp->parent;
                }
                temp = latch;
            }
        }
        else if (temp->nextnode != NULL) {
            //don't care about the warning, temp should be initialized by find
            next_node = temp->nextnode;
            if (temp->Merge(next_node)) {
                parent = next_node->parent;
                delete next_node;
                temp = parent;
            }
            //if can't, then the 2 nodes has already been balanced, temp should be legal
        }
        else {
            throw runtime_error("The B+ Tree root is not valid");
        }
    }
    num_keys--;
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
            bool is_break = false;
            for (i = 0; i < temp->size;i++) {
                if (val <= temp->keys[i]) {
                    is_break = true;
                    break;
                }
                pv.push_back(temp->p[i]);
            }
            if (is_break)
                break;
            temp = temp->nextnode;
        }
        if (temp == NULL)
            return pv;
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
                    temp = temp->nextnode;
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
//the interface
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
    i_n++;
    n++;
    return true;
}
bool TableIndex::CreateIndex(IndexInfo<float> &indexinfo)
{
    if (indexinfo.type != AType::Float)
        return false;
    BTree<float> *BT = new BTree<float>(indexinfo);
    float_index.push_back(*BT);
    f_n++;
    n++;
    return true;
}
bool TableIndex::CreateIndex(IndexInfo<string> &indexinfo)
{
    if (indexinfo.type != AType::String)
        return false;
    BTree<string> *BT = new BTree<string>(indexinfo);
    str_index.push_back(*BT);
    s_n++;
    n++;
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
            i_n--;
            n--;
            return true;
        }
    }
    for (it_f = float_index.begin(); it_f != float_index.end();++it_f) {
        if (it_f->attr_name == attr_name) {
            float_index.erase(it_f);
            f_n--;
            n--;
            return true;
        }
    }
    for (it_s = str_index.begin(); it_s != str_index.end();++it_s) {
        if (it_s->attr_name == attr_name) {
            str_index.erase(it_s);
            s_n--;
            n--;
            return true;
        }
    }
    throw runtime_error("[Error] Can't find the index to be deleted!");
    return false;//
}
//given the index_name and the corresponding value, when insert
bool TableIndex::InsertKey(vector<sqlvalue> index_value, const Position& p)
{
    std::vector<sqlvalue>::iterator it_v;
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    bool is_find;
    for (it_v = index_value.begin(); it_v != index_value.end();it_v++) {
        is_find = false;
        switch (it_v->type.type) {
            case AType::Integer:{
                for (it_i = int_index.begin(); it_i != int_index.end();it_i++) {
                    if (it_i->attr_name == it_v->type.attri_name) {
                        it_i->Insert(it_v->i, p);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
            case AType::Float: {
                for (it_f = float_index.begin(); it_f != float_index.end();it_f++) {
                    if (it_f->attr_name == it_v->type.attri_name) {
                        it_f->Insert(it_v->f, p);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
            case AType::String: {
                for (it_s = str_index.begin(); it_s != str_index.end();it_s++) {
                    if (it_s->attr_name == it_v->type.attri_name) {
                        it_s->Insert(it_v->str, p);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
        }
        if (!is_find)
            throw runtime_error("[Error] Can't find the index " + it_v->type.attri_name + " to be updated for table " + tablename);
    }
    return true;
}
bool TableIndex::DeleteKey(vector<sqlvalue> index_value)
{
    std::vector<sqlvalue>::iterator it_v;
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    bool is_find;
    for (it_v = index_value.begin();it_v != index_value.end();it_v++) {
        is_find = false;
        switch (it_v->type.type) {
            case AType::Integer:{
                for (it_i = int_index.begin(); it_i != int_index.end();it_i++) {
                    if (it_i->attr_name == it_v->type.attri_name) {
                        it_i->DeleteKey(it_v->i);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
            case AType::Float: {
                for (it_f = float_index.begin(); it_f != float_index.end();it_f++) {
                    if (it_f->attr_name == it_v->type.attri_name) {
                        it_f->DeleteKey(it_v->f);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
            case AType::String: {
                for (it_s = str_index.begin(); it_s != str_index.end();it_s++) {
                    if (it_s->attr_name == it_v->type.attri_name) {
                        it_s->DeleteKey(it_v->str);
                        is_find = true;
                        break;
                    }
                }
                break;
            }
        }
        if (!is_find)
            throw runtime_error("[Error] Can't find the index " + it_v->type.attri_name + " to be updated for table " + tablename);
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
    Read();
}
IndexManager::~IndexManager()
{
    Save();
    TI.clear();
    TI.resize(0);
}

TableIndex* IndexManager::FindTable(const string &tablename)
{
    std::vector<TableIndex>::iterator it_table;
    for (it_table = TI.begin(); it_table != TI.end();it_table++) {
        if (it_table->tablename == tablename) {
            return &(*it_table);
        }
    }
    return NULL;
}
void IndexManager::Error()
{

}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<int> &indexinfo)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->CreateIndex(indexinfo);
    }
    else {
        temp = new TableIndex(tablename);
        bool is_succe = temp->CreateIndex(indexinfo);
        TI.push_back(*temp);
        if (is_succe)
            n++;
        return is_succe;
    }
}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<float> &indexinfo)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->CreateIndex(indexinfo);
    }
    else {
        temp = new TableIndex(tablename);
        bool is_succe = temp->CreateIndex(indexinfo);
        TI.push_back(*temp);
        if (is_succe)
            n++;
        return is_succe;
    }
}
bool IndexManager::CreateIndex(const string &tablename, IndexInfo<string> &indexinfo)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->CreateIndex(indexinfo);
    }
    else {
        temp = new TableIndex(tablename);
        bool is_succe = temp->CreateIndex(indexinfo);
        TI.push_back(*temp);
        if (is_succe)
            n++;
        return is_succe;
    }
}
bool IndexManager::DeleteIndex(const string &tablename, const string &index_name)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        if (temp->DeleteIndex(index_name)){
            n--;
            return true;
        }
        return false;
    }
    else {
        throw runtime_error("Can't find table " + tablename + " in index manager");
    }
}
bool IndexManager::InsertKey(const string &tablename, vector<sqlvalue> index_value, const Position& p)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->InsertKey(index_value, p);
    }
    else {
        throw runtime_error("Can't find table " + tablename + " in index manager");
    }
}
bool IndexManager::DeleteKey(const string &tablename, vector<sqlvalue> index_value)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->DeleteKey(index_value);
    }
    else {
        throw runtime_error("Can't find table " + tablename + " in index manager");
    }
}
vector<Position> IndexManager::GetPosition(const string &tablename, const condition &c)
{
    TableIndex *temp;
    temp = this->FindTable(tablename);
    if (temp != NULL) {
        return temp->GetPosition(c);
    }
    else {
        throw runtime_error("Can't find table " + tablename + " in index manager");
    }
}
//save the file into the index.
bool IndexManager::Save()
{
    fstream output;
    std::vector<TableIndex>::iterator it_table;
    std::vector<BTree<int>>::iterator it_i;
    std::vector<BTree<float>>::iterator it_f;
    std::vector<BTree<string>>::iterator it_s;
    int i;
    output.open("index.dat", ios::out | ios::binary);
    //output the number of tables that has index
    output << this->n << endl;
    //for each table
    for (it_table = TI.begin(); it_table != TI.end();it_table++) {
        output << it_table->tablename << endl;
        output << it_table->n << ' ' << it_table->i_n << ' ' << it_table->f_n << ' ' << it_table->s_n << endl;
        for (it_i = it_table->int_index.begin(); it_i != it_table->int_index.end();it_i++) {
            output << it_i->attr_name << endl;
            output << 0 << ' ' << it_i->key_maxsize << ' ' << it_i->num_keys << endl;
            BTNode<int> *temp;
            temp = it_i->first;
            while (temp != NULL) {
                for (i = 0; i < temp->size;i++) {
                    output << temp->keys[i] << ' ' << temp->p[i].blockID << ' ' << temp->p[i].offset << endl;
                }
                temp = temp->nextnode;
            }
        }
        for (it_f = it_table->float_index.begin(); it_f != it_table->float_index.end();it_f++) {
            output << it_f->attr_name << endl;
            output << 1 << ' ' << it_f->key_maxsize << ' ' << it_f->num_keys << endl;
            BTNode<float> *temp;
            temp = it_f->first;
            while (temp != NULL) {
                for (i = 0; i < temp->size;i++) {
                    output << temp->keys[i] << ' ' << temp->p[i].blockID << ' ' << temp->p[i].offset << endl;
                }
                temp = temp->nextnode;
            }
        }
        for (it_s = it_table->str_index.begin(); it_s != it_table->str_index.end();it_s++) {
            output << it_s->attr_name << endl;
            output << 2 << ' ' << it_s->key_maxsize << ' ' << it_s->num_keys << endl;
            BTNode<string> *temp;
            temp = it_s->first;
            while (temp != NULL) {
                for (i = 0; i < temp->size;i++) {
                    output << temp->keys[i] << endl
                           << temp->p[i].blockID << ' ' << temp->p[i].offset << endl;
                }
                temp = temp->nextnode;
            }
        }
    }
    output.close();
    return true;
}
bool IndexManager::Read()
{
    fstream input;
    int i, j, k;
    int t_n, t_i_n, t_f_n, t_s_n, num_table, type, key_maxsize, num_keys;
    string tablename, attr_name;
    IndexInfo<int> *int_indexinfo;
    IndexInfo<float> *float_indexinfo;
    IndexInfo<string> *str_indexinfo;
    Position p;
    input.open("index.dat", ios::in | ios::binary);
    if (!input.is_open()) {
        cout << "file open error" << endl;
        return false;
    }
    input >> num_table;
    //for each table
    for (i = 0; i < num_table;i++) {
        getline(input, tablename);
        getline(input, tablename);
        input >> t_n >> t_i_n >> t_f_n >> t_s_n;
        //for each integer index in the table
        {
            int key;
            for (j = 0; j < t_i_n;j++) {
                getline(input, attr_name);
                getline(input, attr_name);
                input >> type >> key_maxsize >> num_keys;
                int_indexinfo = new IndexInfo<int>(tablename, num_keys, attr_name, AType::Integer, key_maxsize);
                if (type != 0)
                    throw runtime_error("Read Error");
                for (k = 0; k < num_keys;k++) {
                    input >> key >> p.blockID >> p.offset;
                    int_indexinfo->AddKey(key, p.blockID, p.offset);
                }
                this->CreateIndex(tablename, *int_indexinfo);
                delete int_indexinfo;
            }
        }
        //for each float index in the table
        {
            float key;
            for (j = 0; j < t_f_n;j++) {
                getline(input, attr_name);
                getline(input, attr_name);
                input >> type >> key_maxsize >> num_keys;
                float_indexinfo = new IndexInfo<float>(tablename, num_keys, attr_name, AType::Float, key_maxsize);
                if (type != 1)
                    throw runtime_error("Read Error");
                for (k = 0; k < num_keys;k++) {
                    input >> key >> p.blockID >> p.offset;
                    float_indexinfo->AddKey(key, p.blockID, p.offset);
                }
                this->CreateIndex(tablename, *float_indexinfo);
                delete float_indexinfo;
            }
        }
        //for each string index in the table
        {
            string key;
            for (j = 0; j < t_s_n;j++) {
                getline(input, attr_name);
                getline(input, attr_name);
                input >> type >> key_maxsize >> num_keys;
                str_indexinfo = new IndexInfo<string>(tablename, num_keys, attr_name, AType::String, key_maxsize);
                if (type != 2)
                    throw runtime_error("Read Error");
                for (k = 0; k < num_keys;k++) {
                    getline(input, key);
                    getline(input, key);
                    input >> p.blockID >> p.offset;
                    str_indexinfo->AddKey(key, p.blockID, p.offset);
                }
                this->CreateIndex(tablename, *str_indexinfo);
                delete str_indexinfo;
            }
        }
    }
    return true;
}

/*
//the following code is used to test the B+Tree, the user could comment them if you don't want to use
template<typename T>
bool compare(T a, T b)
{
    return a < b;
}
#define MAXN 100000
int a[MAXN];
int main(void)
{
    IndexInfo<int> indexinfo("my test table", MAXN, "grade", AType::Integer, 4);
    int i;
    for (i = MAXN - 1; i >= 0; i--)
    {
        a[i] = rand();
        indexinfo.AddKey(a[i], i * 4 / 4096, i * 4 % 4096);
    }
    IndexManager IM;
    //IM.Read();
    IM.CreateIndex(indexinfo.tablename, indexinfo);
    // //test for deleting
    sqlvalue v;
    v.type.type = AType::Integer;
    v.type.attri_name = "grade";
    vector<sqlvalue> vv;
    for (i = MAXN-1; i >= MAXN/2+1;i--) {
        v.i = a[i];
        vv.push_back(v);
        IM.DeleteKey("my test table", vv);
        vv.pop_back();
    }
    // sort(a, a + MAXN/2+1, compare<int>);
    // cout << "hello" << endl;
    // //test the result after the delete
    // BTNode<int> *node;
    // node = IM.TI.front().int_index.front().first;
    // int top = 0;
    // while (node != NULL) {
    //     for (i = 0; i < node->size;i++) {
    //         if (i % 15 == 0) {
    //             cout << endl;
    //         }
    //         cout << node->keys[i] << ' ';
    //     }
    //     for (i = 0; i < node->size;i++) {
    //         if (i % 15 == 0) {
    //             cout << endl;
    //         }
    //         cout << a[top++] << ' ';
    //     }
    //     cout << a[0] << endl;
    //     cout << endl;
    //     node = node->nextnode;
    // }
    //test for select
    condition c;
    c.name = "grade";
    c.op = 1; //>=1000
    c.val.i = 1000;
    c.val.type.type = AType::Integer;
    vector<Position> p;
    p = IM.GetPosition("my test table", c);
    cout << "hello" << endl;
    //IM.DeleteIndex("my test table", "grade");
    //cout << "Index Deleted" << endl;

    IM.Save();

    return 0;
}
*/
