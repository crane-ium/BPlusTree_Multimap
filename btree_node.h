#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
/** Part of the BPlusTree library
  * @file bplustree/btree_node.h
  */
#include "btree_functions.h"

static const int DEBUG = 0;

using namespace std;

template<typename T>
struct btree_node{
    enum class node_type { root, parent, child};
    enum Leaf {notleaf=1, isleaf=0}; //for certain flags
    template<typename U> friend class BPlusTree; //give BTree access

    //base ctor
    btree_node(size_t min=1, bool dupes=false);
    //BIG3
    ~btree_node();
    btree_node(const btree_node<T>& copy);
    btree_node<T>& operator =(const btree_node<T>& copy);
    //MEMBER FUNCTIONS
    bool insert(const T& input, bool force=false);
    bool remove(const T& input); //Removes a found input

    T *exists(const T& input); //Returns ptr/nullptr of found/notfound data
    btree_node<T>* find(const T& input); //Returns node containing the input, or nullptr if dne
//    T& get_var(const T& input); //Returns reference of data; Assumes exists

    size_t size() const;
    void print() const; //prints a tree
    void print_tree(size_t level=0) const; //prints the whole tree from the node
    bool verify(node_type nt=node_type::root) const;
    template<typename U>
    friend ostream& operator <<(ostream& outs, const btree_node<U>& node);
    template<typename U>
    friend void fix_excess(btree_node<U>*& node);
private:
    size_t _min, __d_s/*, __c_s*/; //__d_s: datasize, __c_s: childsize
    bool __dupes;
    T* __d; //data
    btree_node<T>* __n; //BPLUSTREE: If leaf data, point to right sibling
    btree_node<T>** __c; //children
    bool excess() const; //Returns t/f if _data length > 2*_min
    bool is_leaf() const; //Checks if it is a leaf
    void insert_child(btree_node<T> *node); //Inserts a node
    /**/bool rotate_left(size_t i); //rotates from a leaf sibling with spare data
    /**/bool rotate_right(size_t i); //rotates a data, and a child if not leaf
    //Checks all children to see if anyone can lend the empty sibling data
    /**/bool rotate_check(size_t i);
    /**/void merge(btree_node<T>* left, btree_node<T>* &right); //merges two nodes
    /**/bool take_greatest(T& input); //Takes largest data from leaf in subtree
    /**/bool take_smallest(T& input); //Takes smallest data from leaf in subtree
    T& get_smallest();
    /**/bool fix_empty_child(size_t i); //Checks child for emptiness and fixes
    //At __c[c_i], check whether to merge left or right
    /**/bool merge_data_child(size_t c_i);
};

template<typename T>
btree_node<T>::btree_node(size_t min, bool dupes)
    : _min(min), __dupes(dupes), __n(nullptr){
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
btree_node<T>::btree_node(const btree_node<T>& copy){
    if(DEBUG) cout << "CALLING COPY CTOR on " << copy << endl;;
    __dupes = copy.__dupes;
    _min = copy._min;
    __d_s = copy.__d_s;
    //allocate space first
    __d = new T[2*_min+1]; //2*min is limit; give it space to hold an extra past threshold
    __c = new btree_node<T>*[2*_min+2]; //2*min+1 is limit, extra to hold past threshold
    //fill the space with copy's data
    for(size_t i = 0; i < 2*_min+1; i++){
        __d[i] = copy.__d[i];
    }
    if(!copy.is_leaf())
        for(size_t i = 0; i < 2*_min+2; i++){
            if(copy.__c[i]){ //check that it is not nullptr
                __c[i] = new btree_node<T>((*copy.__c[i])); //recursive call
            }else
                __c[i] = nullptr;
        }
    else
        for(size_t i = 0; i < 2*_min+2; i++)
            __c[i] = nullptr;
    if(DEBUG) cout << "COPY CTOR DONE FOR " << (*this) << endl;
}
template<typename T>
btree_node<T>& btree_node<T>::operator =(const btree_node<T>& copy){
    if(this == &copy)
        return (*this);
    btree_node<T> temp(copy);
    swap(__dupes, temp.__dupes);
    swap(_min, temp._min);
    swap(__d_s, temp.__d_s);
    swap(__d, temp.__d);
    swap(__c, temp.__c);
    return (*this);
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
        //Check the data at child if dupes aren't allowed
        if(child < __d_s && !__dupes && __d[child] == input)
            return false;
        bool check = __c[child]->insert(input);
        if(__c[child]->excess()){
            //The child is overburdened. Take action
            if(DEBUG) cout << "[INSERT]: Overburdened after inserting " << input << endl;
            //Handle children by splitting them up and taking one
            btree_node<T>* new_child = new btree_node<T>(_min, __dupes);
            //give new_child right half:
            //Give _min data elements, _min+1 children
            //adjust: bplusttree must adjust differently if leaf or not
            size_t adjust = (__c[child]->is_leaf())?isleaf:notleaf; //0:1 respectively
            T data_k; //the data that is duplicated for a bplustree
            if(adjust==isleaf){
                //Duplicate data to be held in the parent
                data_k = T(__c[child]->__d[_min]);
                //Check if need set ptr of new_child to right sibling
                new_child->__n = __c[child]->__n;
            }
            for(size_t i = _min+adjust; i < __c[child]->__d_s; i++){ //give data
                swap(__c[child]->__d[i], new_child->__d[i-_min-adjust]);
            }
            for(size_t i = _min+1; i < __c[child]->__d_s+1; i++){ //give data
                swap(__c[child]->__c[i], new_child->__c[i-_min-1]);
            }
            //Change in size
            int change = __c[child]->__d_s - (_min+adjust);
            new_child->__d_s += change; //increase new_child values
            __c[child]->__d_s-=change; //reduce old_child's values
            if(adjust == notleaf){
                T d=T(); //take the child's middle value and move it into parent
                swap(__c[child]->__d[_min], d);
                __c[child]->__d_s--;
                insert_sorted(__d, __d_s, d, __dupes);
            }else{ //BPLUSTREE WORK HERE
                //make a copy (and if it's a mpair/pair,
                // do not copy the value/vector)
                insert_sorted(__d, __d_s, T(data_k), __dupes);
                //Link the right to the left, since they are leafs
                __c[child]->__n = new_child;
            }
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
        return check;
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
void btree_node<T>::print() const{
    //Recurses down to the furthest left leaf, then prints only that
    //That will print the whole tree
    if(!is_leaf())
        __c[0]->print();
    else{
        cout << (*this);
        if(__n != nullptr){
            cout << "-->";
            __n->print();
        }else
            cout << endl;
    }
}
template<typename T>
void btree_node<T>::print_tree(size_t level) const{
    //PRINT BACKWARDS
    cout << setfill(' ');
    if(!is_leaf()){
        //Print the first half of children
        for(size_t i = (__d_s); i > (__d_s)/2; i--)
            __c[i]->print_tree(15+level);
    }
    cout << setiosflags(ios_base::fixed) << setw(level)
         << (*this) << endl;
    if(!is_leaf()){
        //Print the first half of children
        for(size_t i = 0; (__d_s)/2 - i + 1 > 0; i++)
            __c[(__d_s)/2 - i]->print_tree(15+level);
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
ostream& operator <<(ostream& outs, const btree_node<T>& node){
    outs << "[";
    //Also, can display the node it's connected to!
    for(size_t i = 0; i < node.__d_s; i++){
        outs << node.__d[i];
        if(i+1 < node.__d_s)
            outs<< " ";
    }
    outs << "]";
//    if(node.__n != nullptr)
//        outs << "-->" << (*node.__n);
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
        // reorganize normally if there are children, else
        // duplicate the node passed to the right
        int adjust = (node->is_leaf())? node->isleaf : node->notleaf;
        T data_k;
        if(adjust == node->isleaf){
            data_k = T(node->__d[node->_min]); //duplicate the data min
        }
        for(size_t i = node->_min+adjust; i < node->__d_s+1; i++){ //give data
            if(i < node->__d_s)
                swap(node->__d[i], new_child->__d[i-node->_min-adjust]);
            swap(node->__c[i], new_child->__c[i-node->_min-adjust]);
        }
        size_t change = node->__d_s - (node->_min + adjust);
        new_child->__d_s = change; //increase new_child values
        node->__d_s-= change; //reduce old_child's values
        new_root->insert_child(node);
        new_root->insert_child(new_child);
        if(adjust==node->notleaf){
            T d=T(); //take the child's middle value and move it into parent
            swap(node->__d[node->_min], d);
            node->__d_s--;
            insert_sorted(new_root->__d, new_root->__d_s, d, node->__dupes);
        }else{
            //The middle value has already been moved into the sibling
            // Set left child __n to point to right sibling
            // Set duplicate data_k as data into the parent
            node->__n = new_child;
            insert_sorted(new_root->__d, new_root->__d_s, data_k, node->__dupes);
        }
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
bool btree_node<T>::verify(node_type nt) const{
    //Check children and data align
    bool flag_leaf = true;
    size_t child_count = 0;
    if(nt == node_type::root && __d_s == 0) //empty root is valid
        return true;
    if(nt != node_type::root && __d_s == 0){
        if(DEBUG)
            cout << "Verify: empty node\n";
        return false;
    }
    if(DEBUG) cout << (*this) << endl;
    //Scout for extra non-nullptrs where they shouldn't be
    // and check that children count = data_size + 1
    for(size_t i = 0; i < 2*_min+2; i++){
        if(i < __d_s+1){ //only count if it's within the range
            if(__c[i]==nullptr && flag_leaf)
                flag_leaf = false;
            else if(__c[i] != nullptr && !flag_leaf){
                if(DEBUG)
                    cout << "not-null child in leaf\n";
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
    //Check if leaf
    if(child_count == 0 && !flag_leaf){
        //Check that data is in-order
        for(size_t i = 0; i < __d_s-1; i++){
            if(__d[i] > __d[i+1] || __d[i] == __d[i+1]){
                if(DEBUG) cout << "invalid __d: " << __d[i] << ">=" << __d[i+1] << endl;
                return false;
            }
            //Verify not greater equal to next sibling
            if(__n && (__d[__d_s-1] > __n->__d[0] || __d[__d_s-1] == __n->__d[0])){
                if(DEBUG) cout << "Invalid __d with __n->__d: "
                               << __d[__d_s-1] << " >= " << __n->__d[0] << endl;
                return false;
            }
        }
        return true;
    }

    //Check each data is > all data in it's child
    for(size_t i = 0; i < __d_s; i++){
        for(size_t ii = 0; ii < __c[i]->__d_s; ii++)
            if(!(__d[i] > __c[i]->__d[ii])){
                if(DEBUG) cout << "Invalid: __d <= __c.__d: "
                               << __d[i] << "<=" << __c[i]->__d[ii] << endl;
                return false;
            }
    }
    //BPlusTree data is equal to __c[i+1]->smallest
    for(size_t i = 0; i < __d_s; i++){
        if(__d[i] != __c[i+1]->get_smallest()){
            if(DEBUG) cout << "Invalid: data not equal to smallest: "
                           << __d[i] << "!=" << __c[i+1]->get_smallest() << endl;
            return false;
        }
    }
    bool checker;
    for(size_t i = 0; i < __d_s+1; i++){
        checker = __c[i]->verify();
        if(!checker)
            return false;
    }

//    for(size_t i = 0; i < __d_s+1; i++){
//        //Check that the children are placed accordingly to the data
//        if(i < __d_s){
//            if(__d[i] < __c[i]->__d[0] || __d[i] > __c[i+1]->__d[0]){
//                cout << "Index's child is greater than data\n";
//                return false;
//            }
//        }else{ //check last child
//            if(__d[i-1] > __c[i]->__d[0]){
//                cout << "Last child is smaller than data\n";
//                return false;
//            }
//        }
//        try{
//            if(!__c[i]->verify(node_type::child)) //if there is a nullptr in __c, then it will crash
//                return false;
//        }catch(...){
//            cout << "Nullptr child\n";
//            return false;
//        }

//    }
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
    for(int j = i; j < int(__d_s); j++){
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
        child->take_greatest(input); //recursive call
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
        child->take_smallest(input); //recursive call
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
    if(is_leaf()){ //BPTree only considers leaf data as real data
        size_t index = index_of(__d, __d_s, input);
        if(DEBUG) cout << (*this) << endl;
        if(index < __d_s){
            return &__d[index];
        }else
            return nullptr;
    }else{ //not leaf, then keep diving
        size_t i = first_greater(__d, __d_s, input);
        return __c[i]->exists(input);
    }
}
template<typename T>
size_t btree_node<T>::size() const{
    return this->__d_s;
}
template<typename T>
T& btree_node<T>::get_smallest(){
    //Returns smallest data for this tree
    if(!is_leaf())
        return __c[0]->get_smallest();
    else
        return __d[0];
}
template<typename T>
btree_node<T>* btree_node<T>::find(const T &input){
    if(!is_leaf()){
        size_t i = first_greater(__d, __d_s, input);
        return __c[i]->find(input);
    }else{
        size_t i = index_of(__d, __d_s, input);
        if(i == __d_s)
            return nullptr;
        return (this);
    }
}
//template<typename T>
//T& get_var(const T& input){
//    T* val = exists(input);
//    assert(val!=nullptr);
//    return (*val);
//}
#endif // BTREE_NODE_H
