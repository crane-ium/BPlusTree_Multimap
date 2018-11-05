#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "btree_functions.h"

using namespace std;

template<typename T>
struct btree_node{
    template<typename U> friend class BTree; //give BTree access
    //base ctor
    btree_node(size_t min=1, bool dupes=false);
    //BIG3

    //MEMBER FUNCTIONS
    bool insert(const T& input);
    void print(size_t level=0) const;
    template<typename U>
    friend ostream& operator <<(ostream& outs, btree_node<U>& node);
private:
    size_t _min, __d_c, __c_c; //__d_c: datacount, __c_c: childrencount
    bool __dupes;
    T* __d; //data
    btree_node<T>** __c; //children
    bool is_leaf() const;
};

template<typename T>
btree_node<T>::btree_node(size_t min, bool dupes)
    : _min(min), __dupes(dupes){
    __d_c = 0;
    __c_c = 0;
    __d = new T[_min*2+1];
    __c = new btree_node<T>*[_min*2+2];
    for(size_t i = 0; i < _min*2+2; i++)
        __c[i] = nullptr;
}
template<typename T>
bool btree_node<T>::insert(const T &input){
    if(is_leaf()){ //check if there are children
        //Then insert into this
        insert_sorted(__d, __d_c, input, __dupes);
    }
}
template<typename T>
bool btree_node<T>::is_leaf() const{
    if(__c_c == 0)
        return true;
    else
        return false;
}
template<typename T>
void btree_node<T>::print(size_t level) const{
    if(is_leaf()){
        cout << setiosflags(ios_base::fixed) << setw(level)
             << (*this);
    }
}
template<typename T>
ostream& operator <<(ostream& outs, btree_node<T>& node){
    outs << "[";
    for(size_t i = 0; i < node.__d_c; i++){
        outs << node.__d[i];
        if(i+1 < node.__d_c)
            outs<< " ";
    }
    outs << "]";
    return outs;
}

#endif // BTREE_NODE_H
