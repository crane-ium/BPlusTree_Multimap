#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "btree_node.h"

template<typename T>
class BTree{
public:
    BTree(size_t min=1, bool dupes=false);
    //BIG3

    //MEMBER FUNCTIONS
    bool insert(const T& input);
    bool remove(const T& input);
    void print();
    bool verify() const;
private:
    size_t _min;
    bool __dupes;
    btree_node<T>* __head;
};

template<typename T>
BTree<T>::BTree(size_t min, bool dupes): _min(min), __dupes(dupes){
    __head = new btree_node<T>(min, dupes);
}
template<typename T>
bool BTree<T>::insert(const T &input){
    bool check = __head->insert(input);
    fix_excess(__head);
//    assert(__head->verify());
    return check;
}
template<typename T>
bool BTree<T>::remove(const T &input){
    //Tries to remove a desired input;
    //Returns false if no found input;
    bool check = __head->remove(input);
    //Check if __head needs reorganizing too

    return check;
}
template<typename T>
void BTree<T>::print(){
    __head->print();
}
template<typename T>
bool BTree<T>::verify() const{
    bool check = __head->verify();
    if(check)
        cout << "----VERIFIED----\n";
    else
        cout << "!!!!FAILED VERIFICATION!!!!\n";
}

#endif // BTREE_H
