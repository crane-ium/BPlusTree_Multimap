#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "btree_functions.h"

static const int DEBUG = 1;

using namespace std;

template<typename T>
struct btree_node{
    template<typename U> friend class BTree; //give BTree access
    //base ctor
    btree_node(size_t min=1, bool dupes=false);
    //BIG3

    //MEMBER FUNCTIONS
    bool insert(const T& input);
    bool remove(const T& input); //Removes a found input
    void print(size_t level=0); //prints the whole tree from the node
    bool excess() const;
    bool verify() const;
    template<typename U>
    friend ostream& operator <<(ostream& outs, btree_node<U>& node);
    template<typename U>
    friend void fix_excess(btree_node<U>*& node);
//private:
    size_t _min, __d_s/*, __c_s*/; //__d_s: datacount, __c_s: childrencount
    bool __dupes;
    T* __d; //data
    btree_node<T>** __c; //children
    bool is_leaf() const;
    void insert_child(btree_node<T> *node);
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
bool btree_node<T>::insert(const T &input){
    if(is_leaf()){ //check if there are children
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
            T d; //take the child's middle value and move it into parent
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
    //Removes a data element if it is found
    if(is_there(__d, __d_s, input)){
        //Found it, remove it. Then deal with consequences
        size_t d_i = index_of(__d, __d_s, input);
        for(size_t i = d_i; i < __d_s; i++){ //sort it out of bounds
            swap(__d[i], __d[i+1]);
        }
        __d_s--;
        return true;
    }else if(is_leaf()){
        return false;
    }else{
        size_t child = first_ge(__d, __d_s, input);
        bool check = __c[child]->remove(input);
        if(!check) //it wasn't found. Return to exit
            return false;
        //else we check if anything needs management
        //Scenario 1: Child is leaf with spares
        if(__c[child]->is_leaf() && __c[child]->__d_s > 0)
            return true; //It's done, we're good
        else if(__c[child]->is_leaf()){
            //Scenario 2: Empty leaf

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
    //Used for fixing the root if it is too large
    if(node->__d_s > node->_min*2){
        //Fix it!
        btree_node<T>* new_root = new btree_node<T>(node->_min, node->__dupes);
        btree_node<T>* new_child = new btree_node<T>(node->_min, node->__dupes);
        //Give _min data elements, _min+1 children
        for(size_t i = node->_min+1; i < node->__d_s; i++){ //give data
            swap(node->__d[i], new_child->__d[i-node->_min-1]);
        }
        for(size_t i = node->_min+1; i < node->__d_s+1; i++){ //give data
            swap(node->__c[i], new_child->__c[i-node->_min-1]);
        }
        new_child->__d_s = node->_min; //increase new_child values
        node->__d_s-=node->_min; //reduce old_child's values
        new_root->insert_child(node);
        new_root->insert_child(new_child);
        T d; //take the child's middle value and move it into parent
        swap(node->__d[node->_min], d);
        node->__d_s--;
        insert_sorted(new_root->__d, new_root->__d_s, d, node->__dupes);
        node = new_root;
    }
}
template<typename T>
void btree_node<T>::insert_child(btree_node<T>* node){
    //place the child at the end and then swap it until it's in place
    __c[_min*2+1] = node;
    for(size_t i = _min*2+1; i > 0; i--){
        if(__c[i-1] == nullptr || __c[i]->__d[0] < __c[i-1]->__d[0])
            swap(__c[i], __c[i-1]);
    }
}
template<typename T>
bool btree_node<T>::verify() const{
    //Check children and data align
    if(is_leaf()){
        //check that there are no children
        for(size_t i = 0; i < 2*_min+2; i++){
            if(__c[i] != nullptr){
                cout << "There is a child in a leaf\n";
                return false;
            }
        }
        return true;
    }else{
        for(size_t i = 0; i < __d_s+1; i++){
            //Check that the children are placed accordingly to the data
            if(i < __d_s){
                if(__d[i] < __c[i]->__d[0]){
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
            return true;
        }
    }
}
#endif // BTREE_NODE_H
