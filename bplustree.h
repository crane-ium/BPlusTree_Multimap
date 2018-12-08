#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <exception>
#include "btree_node.h"

//Exception handling for a non-default dict. Prevent index access/creation
//  if not defaultdict
struct BPTIndexException : public std::exception{
    const char* what() const throw(){
        return "[BPT]: Index in non-defaultdict does not exist.";
    }
};

template<typename T>
/**
 * @brief The BPlusTree class
 * Create a tree data structure that can handle an
 * iterator while maintaining a complete tree
 */
class BPlusTree{
public:
    BPlusTree(size_t min=1, bool dupes=false);

    class Iterator{ //Node-Pointer encapsulating class
    public:
        friend class BPlusTree;
        Iterator(btree_node<T>* _btn=nullptr, size_t _key=0)
            : __it(_btn), __key(_key){}
        T& operator *(){ //Don't give access to change
            return __it->__d[__key];
        }
        Iterator operator++(int __empty){
            if(!__it)
                return nullptr;
            if(__key < __it->__d_s-1){
                inc();
            }else{
                if(__it->__n != nullptr){
                    __it = __it->__n;
                    __key = 0;
                }else{
                    if (__key < __it->__d_s){
                        __it = nullptr;
                        __key = 0;
                    }
//                        inc(); //equivalent to Iterator end()
                }
            }
            return (*this);
        }
        Iterator operator++(){
            (*this)++;
            return (*this);
        }
        bool is_null() const{
            return (__it ? false : true);
        }
        friend bool operator == (const Iterator& lhs, const Iterator& rhs){
            return (lhs.__it == rhs.__it && lhs.__key == rhs.__key);
        }
        friend bool operator != (const Iterator& lhs, const Iterator& rhs){
            return (!(lhs==rhs));
        }
        void print() const{
            cout << __it->__d[__key] << endl;
        }
    private:
        btree_node<T>* __it;
        size_t __key;
        size_t __total_keys;
        void inc(){__key++;__total_keys++;}
    };

    //BIG3
    ~BPlusTree();
    BPlusTree(const BPlusTree<T>& copy);
    BPlusTree<T>& operator =(const BPlusTree<T>& copy);
    //MEMBER FUNCTIONS
    bool insert(const T& input);
    bool remove(const T& input);

    bool exists(const T& input) const;
//    T* find(const T& input);
    T& get(const T& input);

    //ITERATOR FUNCTIONS
    Iterator find(const T& input);
    Iterator begin() const;
    Iterator end() const;

    void cleartree(); //Clears all but the root
    void print() const; //Prints the linked-list of bplustree
    void print_tree() const; //Prints the btree in tree format
    bool verify(bool output=false) const;
    bool empty() const;
    size_t size();
    void set_defaultdict(){__defaultdict=true;}
    void set_not_defaultdict(){__defaultdict=false;}
    bool is_defaultdict(){return __defaultdict;}
//    void operator ()(const size_t& min, const bool& dupes){
//        if(__head->is_leaf()){
//            if(min > _min){
//                //simple allowing of resizing
//                _min = min;
//                __head->_min = min;
//            }
//            if(__dupes == false){ //Allow from false->true only (for simple feature)
//                __dupes = dupes;
//                __head->__dupes = dupes;
//            }
//        }
//    }
    template<typename U>
    friend std::ostream& operator <<(std::ostream& outs, const BPlusTree<U>& bt);
private:
    size_t _min;
    bool __dupes;
    btree_node<T>* __head;
    size_t __size; //Track the size;
    bool __defaultdict=true;
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
    __defaultdict = copy.__defaultdict;
    if(DEBUG) cout << "[BPT] copy ctor\n";
//    __head = new btree_node<T>((*copy.__head));
    typename BPlusTree::Iterator iter;
    for(iter = copy.begin(); iter != copy.end(); iter++){
        insert(*iter);
    }
    if(DEBUG) cout << "[BPT] copy ctor DONE\n";
}
template<typename T>
BPlusTree<T>& BPlusTree<T>::operator =(const BPlusTree<T>& copy){
    if(this == (&copy))
        return (*this);
    if(DEBUG) cout << "[BPT] copy operator\n";
//    BPlusTree temp(copy);
//    swap(_min, temp._min);
//    swap(__dupes, temp.__dupes);
//    swap(__head, temp.__head);
    _min = copy._min;
    __dupes = copy.__dupes;
    __defaultdict = copy.__defaultdict;
//    __head = new btree_node<T>(*copy.__head);
    typename BPlusTree::Iterator iter;
    for(iter = copy.begin(); iter != copy.end(); iter++){
        insert(*iter);
    }
    if(DEBUG) cout << "[BPT] copy operator DONE\n";
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
bool BPlusTree<T>::verify(bool output) const{
    bool check = __head->verify();
    if(output){
        if(check)
            cout << "----VERIFIED----\n";
        else
            cout << "!!!!FAILED VERIFICATION!!!!\n";
    }
    return check;
}
template<typename T>
std::ostream& operator <<(std::ostream& outs, const BPlusTree<T>& bt){
    bt.print();
    return outs;
}
template<typename T>
bool BPlusTree<T>::exists(const T &input) const{
    return (__head->exists(input) != nullptr);
}
//----------ITERATOR FUNCTIONS----------
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::find(const T &input){
    btree_node<T>* found = __head->find(input);
    size_t index = 0;
    if(found)
        index = index_of(found->__d, found->__d_s, input);
    return BPlusTree<T>::Iterator(found, index);
}
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::begin() const{
    if(DEBUG) cout << "[BPT].it begin()\n";
    btree_node<T>* walker = __head;
    while(!walker->is_leaf()) //Travel to the first leaf
        walker = walker->__c[0];
    if(walker == __head && __head->__d_s == 0){
        if(DEBUG) cout << "[BPT].it begin(): head empty (NOBIGGY)\n";
        return BPlusTree<T>::Iterator(nullptr);
    }
    if(DEBUG) cout << "[BPT].it begin done\n";
    return BPlusTree<T>::Iterator(walker);
}
template<typename T>
typename BPlusTree<T>::Iterator BPlusTree<T>::end() const{
//    btree_node<T>* walker = __head;
//    while(!walker->is_leaf())
//        walker = walker->__c[walker->__d_s];
    return BPlusTree<T>::Iterator(nullptr);
}
template<typename T>
T& BPlusTree<T>::get(const T &input){
    T* val = __head->exists(input);
    if(val==nullptr && !__defaultdict)
        throw BPTIndexException();
//    assert(val!=nullptr);
    return (*val);
}
template<typename T>
size_t BPlusTree<T>::size(){
    typename BPlusTree<T>::Iterator it;
    size_t count = 0;
    for(it=begin(); it != end(); it++){
        count++;
    }
    __size = count; //update size. Just in case we want to utilize this
    return __size;
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
