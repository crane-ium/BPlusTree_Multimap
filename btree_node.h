#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "btree_functions.h"

static const int DEBUG = 0;

using namespace std;

template<typename T>
struct btree_node{
    template<typename U> friend class BTree; //give BTree access
    //base ctor
    btree_node(size_t min=1, bool dupes=false);
    //BIG3
    ~btree_node();
    //MEMBER FUNCTIONS
    bool insert(const T& input, bool force=false);
    bool remove(const T& input); //Removes a found input

    T *exists(const T& input); //Returns t/f if exists
    T& get_var(const T& input); //Returns address of data; Assumes exists

    size_t size() const;
    void print(size_t level=0); //prints the whole tree from the node
    bool verify() const;
    template<typename U>
    friend ostream& operator <<(ostream& outs, btree_node<U>& node);
    template<typename U>
    friend void fix_excess(btree_node<U>*& node);
private:
    size_t _min, __d_s/*, __c_s*/; //__d_s: datasize, __c_s: childsize
    bool __dupes;
    T* __d; //data
    btree_node<T>** __c; //children
    bool excess() const; //Returns t/f if _data length > 2*_min
    bool is_leaf() const; //Checks if it is a leaf
    void insert_child(btree_node<T> *node); //Inserts a node
    bool rotate_left(size_t i); //rotates from a leaf sibling with spare data
    bool rotate_right(size_t i); //rotates a data, and a child if not leaf
    //Checks all children to see if anyone can lend the empty sibling data
    bool rotate_check(size_t i);
    void merge(btree_node<T>* left, btree_node<T>* &right); //merges two nodes
    bool take_greatest(T& input); //Takes largest data from leaf in subtree
    bool take_smallest(T& input); //Takes smallest data from leaf in subtree
    bool fix_empty_child(size_t i); //Checks child for emptiness and fixes
    //At __c[c_i], check whether to merge left or right
    bool merge_data_child(size_t c_i);
};

template<typename T>
btree_node<T>::btree_node(size_t min, bool dupes)
    : _min(min), __dupes(dupes){
    __d_s = 0;
//    __c_s = 0;
    __d = new T[_min*2+1];
    __c = new btree_node<T>*[_min*2+2];
    for(size_t i = 0; i < _min*2+2; i++)
        __c[i] = nullptr;
}
template<typename T>
btree_node<T>::~btree_node(){
    if(DEBUG) cout << "-----------------Deleting " << __d[0] << endl;
    delete[] __d;
    for(size_t i = 0; i < _min*2+2; i++){
        if(__c[i] != nullptr && i > __d_s)
            if(DEBUG) cout << "There was a non-null child out of place\n";
        delete __c[i]; //delete all children
    }
}
template<typename T>
bool btree_node<T>::insert(const T &input, bool force){
    //force: Place it into this node directly
    if(force || is_leaf()){ //check if there are children
        //Then insert into this
        return insert_sorted(__d, __d_s, input, __dupes);
    }else{
        //get the child the input will be inserted into
        size_t child = first_ge(__d, __d_s, input);
        bool check = __c[child]->insert(input);
        if(__c[child]->excess()){
            //The child is overburdened. Take action
            if(DEBUG) cout << "[INSERT]: Overburdened after inserting " << input << endl;
            //Handle children by splitting them up and taking one
            btree_node<T>* new_child = new btree_node<T>(_min, __dupes);
            //give new_child right half:
            //Give _min data elements, _min+1 children
            for(size_t i = _min+1; i < __c[child]->__d_s; i++){ //give data
                swap(__c[child]->__d[i], new_child->__d[i-_min-1]);
            }
            for(size_t i = _min+1; i < __c[child]->__d_s+1; i++){ //give data
                swap(__c[child]->__c[i], new_child->__c[i-_min-1]);
            }
            new_child->__d_s = _min; //increase new_child values
            __c[child]->__d_s-=_min; //reduce old_child's values
            T d=T(); //take the child's middle value and move it into parent
            swap(__c[child]->__d[_min], d);
            __c[child]->__d_s--;
            insert_sorted(__d, __d_s, d, __dupes);
            //Now move the new child into the parent (this)
            //Child will go into spot child+1
            insert_child(new_child);
        }
        return check;
    }
}
template<typename T>
bool btree_node<T>::remove(const T& input){
    //RETURNS FALSE IF NO WORK LEFT TO DO!!!
    //Removes a data element if it is found
    if(is_there(__d, __d_s, input)){
        //Found it, remove it. Then deal with consequences
        size_t d_i = index_of(__d, __d_s, input);
        if(DEBUG) cout << "Remove: Found " << input << " at " << d_i << endl;
        if(!is_leaf()){
            //Then take from it's greatest subtree
            if(__c[d_i+1]->__d_s > __c[d_i]->__d_s){
                __c[d_i+1]->take_smallest(__d[d_i]);
                    if(__c[d_i+1]->__d_s == 0)
                        fix_empty_child(d_i+1);
            }else{
                __c[d_i]->take_greatest(__d[d_i]);
                if(__c[d_i]->__d_s == 0)
                    fix_empty_child(d_i);
            }
        }else{
            for(size_t i = d_i; i < __d_s; i++){ //sort it out of bounds
                swap(__d[i], __d[i+1]);
            }
            __d_s--;
        }
//        if(DEBUG) cout << "Remove: returning\n";
        return true;
    }else if(is_leaf()){
        //Didn't find, exit
        return false;
    }else{
//        //Get child to search, then recursive remove on child
        size_t child = first_ge(__d, __d_s, input);
        bool check = __c[child]->remove(input); //check checks if remove happened
        if(__c[child]->__d_s > 0)
            return check;
        else{
            fix_empty_child(child);
        }
    }
}
template<typename T>
bool btree_node<T>::is_leaf() const{
    if(__c[0]==nullptr)
        return true;
    else
        return false;
}
template<typename T>
void btree_node<T>::print(size_t level){
    //PRINT BACKWARDS
    cout << setfill(' ');
    if(!is_leaf()){
        //Print the first half of children
        for(size_t i = (__d_s); i > (__d_s)/2; i--)
            __c[i]->print(15+level);
    }
    cout << setiosflags(ios_base::fixed) << setw(level)
         << (*this) << endl;
    if(!is_leaf()){
        //Print the first half of children
        for(size_t i = 0; (__d_s)/2 - i + 1 > 0; i++)
            __c[(__d_s)/2 - i]->print(15+level);
    }
}
template<typename T>
bool btree_node<T>::excess() const{
    if(__d_s > 2*_min)
        return true;
    else
        return false;
}
template<typename T>
ostream& operator <<(ostream& outs, btree_node<T>& node){
    outs << "[";
    for(size_t i = 0; i < node.__d_s; i++){
        outs << node.__d[i];
        if(i+1 < node.__d_s)
            outs<< " ";
    }
    outs << "]";
    return outs;
}
template<typename T>
void fix_excess(btree_node<T>*& node){
    //Also check that the root isn't empty
    if(node->__d_s == 0){
        //if root is empty, it needs to be filled
        //The root MUST've given it's data to a child, since
        // if it were removed, it would've taken the greatest_data
        //Therefore one child has __d_s == 1 AND only one child
        btree_node<T>* temp = node;
        btree_node<T>* child = node->__c[0];
        if(!node->is_leaf()){
            node = child;
            temp->__c[0] = nullptr;
            delete temp;
        }
    }
    //Used for fixing the root if it is too large
    if(node->__d_s > node->_min*2){
        //SPLIT
        btree_node<T>* new_root = new btree_node<T>(node->_min, node->__dupes);
        btree_node<T>* new_child = new btree_node<T>(node->_min, node->__dupes);
        //Give _min data elements, _min+1 children
        for(size_t i = node->_min+1; i < node->__d_s+1; i++){ //give data
            if(i < node->__d_s)
                swap(node->__d[i], new_child->__d[i-node->_min-1]);
            swap(node->__c[i], new_child->__c[i-node->_min-1]);
        }
        new_child->__d_s = node->_min; //increase new_child values
        node->__d_s-=node->_min; //reduce old_child's values
        new_root->insert_child(node);
        new_root->insert_child(new_child);\
        T d=T(); //take the child's middle value and move it into parent
        swap(node->__d[node->_min], d);
        node->__d_s--;
        insert_sorted(new_root->__d, new_root->__d_s, d, node->__dupes);
        node = new_root;
    }
}
template<typename T>
void btree_node<T>::insert_child(btree_node<T>* node){
    //place the child at the end and then swap it until it's in place
//    swap(__c[_min*2+1], node); //set the other to nullptr
    __c[_min*2+1] = node;
    for(size_t i = _min*2+1; i > 0; i--){
        if(__c[i-1] == nullptr || __c[i]->__d[0] < __c[i-1]->__d[0])
            swap(__c[i], __c[i-1]);
    }
}
template<typename T>
bool btree_node<T>::verify() const{
    //Check children and data align
    bool flag_leaf = true;
    size_t child_count = 0;
    if(__d_s == 0){
        if(DEBUG)
            cout << "Verify: empty node\n";
        return false;
    }
    for(size_t i = 0; i < 2*_min+2; i++){
        if(i < __d_s+1){ //only count if it's within the range
            if(__c[i]==nullptr && flag_leaf)
                flag_leaf = false;
            else if(__c[i] != nullptr && !flag_leaf){
                if(DEBUG)
                    cout << "not null child in leaf\n";
                return false;
            }
            if(__c[i]!=nullptr)
                child_count++;
        }else{
            if(__c[i] != nullptr){
                if(DEBUG)
                    cout << "Verify: Child out of range\n";
                return false;
            }
        }
    }
    if(child_count == 0 && !flag_leaf)
        return true;
    for(size_t i = 0; i < __d_s+1; i++){
        //Check that the children are placed accordingly to the data
        if(i < __d_s){
            if(__d[i] < __c[i]->__d[0] || __d[i] > __c[i+1]->__d[0]){
                cout << "Index's child is greater than data\n";
                return false;
            }
        }else{ //check last child
            if(__d[i-1] > __c[i]->__d[0]){
                cout << "Last child is smaller than data\n";
                return false;
            }
        }
        try{
            if(!__c[i]->verify()) //if there is a nullptr in __c, then it will crash
                return false;
        }catch(...){
            cout << "Nullptr child\n";
            return false;
        }

    }
    return true;
}
template<typename T>
bool btree_node<T>::rotate_left(size_t i){
    if(__c[i+1]->__d_s < 2) //cannot take from a child with less than 2
        return false;
    //Move the smallest element from the right child to the left
    T d=T(); //temporary data
    btree_node<T>* left = nullptr;
    btree_node<T>* child = __c[i+1]; //PTR keep track of child
    bool flag_leaf = child->is_leaf(); //Check if child is leaf
    //Take from the right child, then sort it and reduce size
    //Move a child over, too, if there are children
    swap(child->__d[0], d);
    if(!flag_leaf)
        swap(child->__c[0], left); //set the old child to nullptr too
    if(DEBUG>5) cout << "Rotate left\n";
    for(size_t j = 0; j < child->__d_s+1; j++){
        if(j < child->__d_s)
            swap(child->__d[j], child->__d[j+1]);
        if(!flag_leaf)
            swap(child->__c[j], child->__c[j+1]);
    }
    child->__d_s--;
    //Next, swap that into current i in this
    swap(__d[i], d);
    //Next, insert  the data into the index's child
    __c[i]->insert(d, true);
    __c[i]->insert_child(left);
    if(i>0)
        rotate_left(i-1);
    return true;
}
template<typename T>
bool btree_node<T>::rotate_right(size_t i){ //cannot take from a child with less than 2
    if(__c[i]->__d_s < 2)
        return false;
    //Move the smallest element from the right child to the left
    T d=T(); //temporary data
    btree_node<T>* right = nullptr;
    btree_node<T>* child = __c[i]; //PTR keep track of child
    size_t child_s = child->__d_s;
    bool flag_leaf = child->is_leaf(); //Check if child is leaf
    //Take from the right child, then sort it and reduce size
    //Move a child over, too, if there are children
    swap(child->__d[child_s-1], d);
    if(!flag_leaf)
        swap(child->__c[child_s], right); //set the old child to nullptr too
    if(DEBUG>5) cout << "Rotate right\n";
    child->__d_s--;
    //Next, swap that into current i in this
    swap(__d[i], d);
    //Next, insert  the data into the index's child
    __c[i+1]->insert(d, true);
    if(!flag_leaf)
        __c[i+1]->insert_child(right);
    if(i<__d_s-1)
        rotate_right(i+1);
    return true;
}
template<typename T>
bool btree_node<T>::rotate_check(size_t i){
    //Checks all children for possible spares
    bool flag_complete = false;
    for(int j = i-1; j >= 0; j--){
        //Keep trying to rotate right
        if(rotate_right(j)){
            flag_complete = true;
            break;
        }
    }
    for(int j = i; j < __d_s; j++){
        if(rotate_left(j)){
            flag_complete = true;
            break;
        }
    }

    return flag_complete;
}
template<typename T>
bool btree_node<T>::take_greatest(T &input){
    if(is_leaf()){
        //take the greatest from here!
        swap(__d[__d_s-1], input);
        __d_s--;
        if(DEBUG) cout << "Took greatest " << input << endl;
        return true;
    }else{
        btree_node<T>* child = __c[__d_s];
        bool check = child->take_greatest(input); //recursive call
        if(child->__d_s == 0){
            bool check = fix_empty_child(__d_s);
            if(DEBUG) cout << "takegreatest: fix_empty_child = " << check << endl;
        }
        return true;
    }
}
template<typename T>
bool btree_node<T>::take_smallest(T &input){
    if(is_leaf()){
        //take the smallest from here!
        swap(__d[0], input);
        move_to_end(__d, __d_s, 0);
        if(DEBUG) cout << "Took smallest " << input << endl;
        return true;
    }else{
        btree_node<T>* child = __c[0];
        bool check = child->take_smallest(input); //recursive call
        if(child->__d_s == 0){
            bool check = fix_empty_child(0);
            if(DEBUG) cout << "take_smallest: fix_empty_child = " << check << endl;
        }
        return true;
    }
}
template<typename T>
bool btree_node<T>::fix_empty_child(size_t i){
    //Checks if a child __d_s is 0.
    // If it is, then move the parent data down and merge
    btree_node<T>* child = __c[i];
    if(child->__d_s == 0){
        bool check = rotate_check(i);
        if(check)
            return true; //return it's fixed
        check = merge_data_child(i);
        return true;
    }else
        return false;
    //should never get here
    cout << "Hit end of " << __FUNCTION__<< endl;
    assert(false);
}
template<typename T>
bool btree_node<T>::merge_data_child(size_t c_i){
    //Moves _data[d_i] into a child and deletes __c[c_i]
    if(c_i < __d_s){
        if(DEBUG) cout << "merge_data_child: merge right\n";
        T d=T();
        swap(d, __d[c_i]);
        __c[c_i]->insert(d, true);
        merge(__c[c_i+1], __c[c_i]); // merge into the right child
        for(size_t i = c_i; i < __d_s; i++){ //Swap empty data to the right
            if(i < __d_s - 1)
                swap(__d[i], __d[i+1]);
            swap(__c[i], __c[i+1]);
        }
        __d_s--;
        return true;
    }else if(c_i == __d_s){
        if(DEBUG) cout << "merge_data_child: merge left\n";
        //means it's far right child
        //Special case: Instead of merging data into the right
        // merge into the left
        T d=T();
        swap(d, __d[__d_s-1]);
        __d_s--;
        __c[c_i]->insert(d, true);
        merge(__c[c_i-1], __c[c_i]);
        return true;
    }
    if(DEBUG) cout << "Something failed in merge_data_child\n";
    return false;
}
template<typename T>
void btree_node<T>::merge(btree_node<T>* left, btree_node<T>* &right){
    //Merges the right into the left
    //Assumes the right has same # of data as children
    if(DEBUG) cout << "merge()\n";
    T d=T();
    btree_node<T>* temp = nullptr;
    for(size_t i = 0; i < right->__d_s; i++){
        temp = right->__c[i];
        right->__c[i] = nullptr;
        left->insert_child(temp);
        if(i<right->__d_s){
            swap(d, right->__d[i]);
            left->insert(d, true);
        }
    }
    delete right;
    right=nullptr;
}
template<typename T>
T* btree_node<T>::exists(const T& input){
    size_t i = first_ge(__d, __d_s, input);
    if(__d[i] == input && i < __d_s)
        return &__d[i];
    else if(!is_leaf()){
        return __c[i]->exists(input);
    }else
        return nullptr;
}
template<typename T>
size_t btree_node<T>::size() const{
    size_t count = 0;
    if(!is_leaf()){
        for(size_t i = 0; i < __d_s+1; i++){
            count += __c[i]->size();
        }
    }
    count += __d_s;
    return count;
}
//template<typename T>
//T& get_var(const T& input){
//    T* val = exists(input);
//    assert(val!=nullptr);
//    return (*val);
//}
#endif // BTREE_NODE_H
