#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include "btree_node.h"

template<typename T>
/**
 * @brief The BPlusTree class
 * Create a tree data structure that can handle an
 * iterator while maintaining a complete tree
 */
class BPlusTree{
public:
    BPlusTree(size_t min=1, bool dupes=false);
    //BIG3
    ~BPlusTree();
    BPlusTree(const BPlusTree<T>& copy);
    BPlusTree<T>& operator =(const BPlusTree<T>& copy);
    //MEMBER FUNCTIONS
    bool insert(const T& input);
    bool remove(const T& input);

    bool exists(const T& input);
    T* find(const T& input);
    T& get(const T& input);

    void cleartree(); //Clears all but the root
    void print() const; //Prints the linked-list of bplustree
    void print_tree() const; //Prints the btree in tree format
    bool verify() const;
    bool empty() const;
    size_t size() const;
    void operator ()(const size_t& min, const bool& dupes){
        if(__head->is_leaf()){
            if(min > _min){
                //simple allowing of resizing
                _min = min;
                __head->_min = min;
            }
            if(__dupes == false){ //Allow from false->true only (for simple feature)
                __dupes = dupes;
                __head->__dupes = dupes;
            }
        }
    }
    template<typename U>
    friend std::ostream& operator <<(std::ostream& outs, const BPlusTree<U>& bt);
private:
    size_t _min;
    bool __dupes;
    btree_node<T>* __head;
};

template<typename T>
BPlusTree<T>::BPlusTree(size_t min, bool dupes): _min(min), __dupes(dupes){
    __head = new btree_node<T>(min, dupes);
}
template<typename T>
BPlusTree<T>::~BPlusTree(){
    delete __head;
}
template<typename T>
BPlusTree<T>::BPlusTree(const BPlusTree<T>& copy){
    /*this->*/
    _min = copy._min;
    __dupes = copy.__dupes;
    __head = new btree_node<T>((*copy.__head));
}
template<typename T>
BPlusTree<T>& BPlusTree<T>::operator =(const BPlusTree<T>& copy){
    if(this == (&copy))
        return (*this);
    BPlusTree temp(copy);
    swap(_min, temp._min);
    swap(__dupes, temp.__dupes);
    swap(__head, temp.__head);
    return (*this);
}
template<typename T>
bool BPlusTree<T>::insert(const T &input){
    bool check = __head->insert(input);
    fix_excess(__head);
//    assert(__head->verify());
    return check;
}
template<typename T>
bool BPlusTree<T>::remove(const T &input){
    //Tries to remove a desired input;
    //Returns false if no found input;
    bool check = __head->remove(input);
    //Check if __head needs reorganizing too
    fix_excess(__head);
    return check;
}
template<typename T>
void BPlusTree<T>::print() const{
    __head->print();
}
template<typename T>
void BPlusTree<T>::print_tree() const{
    __head->print_tree();
}
template<typename T>
bool BPlusTree<T>::verify() const{
    bool check = __head->verify();
//    if(check)
//        cout << "----VERIFIED----\n";
//    else
//        cout << "!!!!FAILED VERIFICATION!!!!\n";
    return check;
}
template<typename T>
std::ostream& operator <<(std::ostream& outs, const BPlusTree<T>& bt){
    bt.print();
    return outs;
}
template<typename T>
bool BPlusTree<T>::exists(const T &input){
    return (__head->exists(input) != nullptr);
}
template<typename T>
T* BPlusTree<T>::find(const T &input){
    return __head->exists(input);
}
template<typename T>
T& BPlusTree<T>::get(const T &input){
    T* val = __head->exists(input);
    assert(val!=nullptr);
    return (*val);
}
template<typename T>
size_t BPlusTree<T>::size() const{
    return __head->size();
}
template<typename T>
bool BPlusTree<T>::empty() const{
    return !(bool)__head->__d_s;
}

template<typename T>
void BPlusTree<T>::cleartree(){
    __head->__d_s = 0; //delete all children
    for(size_t i = 0; i < __head->_min*2+2; i++){
        delete __head->__c[i];
        __head->__c[i] = nullptr;
    }
}

#endif // BPLUSTREE_H
