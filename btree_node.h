#ifndef BTREE_NODE_H
#define BTREE_NODE_H

#include <cstdlib>
#include <iostream>
#include <cassert>
#include <iomanip>
#include "btree_functions.h"

static const int DEBUG = 6;

using namespace std;

template<typename T>
struct btree_node{
    template<typename U> friend class BTree; //give BTree access
    //base ctor
    btree_node(size_t min=1, bool dupes=false);
    //BIG3

    //MEMBER FUNCTIONS
    bool insert(const T& input, bool force=false);
    bool remove(const T& input); //Removes a found input
    void print(size_t level=0); //prints the whole tree from the node
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
    bool excess() const;
    bool is_leaf() const;
    void insert_child(btree_node<T> *node);
    bool rotate_left(size_t i); //rotates from a leaf sibling with spare data
    bool rotate_right(size_t i);
    //Checks all children to see if anyone can lend the empty sibling data
    bool rotate_check(size_t i);
    void merge(btree_node<T>& node); //merges node into this
    bool take_greatest(T& input); //Takes largest data from leaf in subtree
    bool fix_empty_child(size_t i); //Checks child for emptiness and fixes
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
    //RETURNS FALSE IF NO WORK LEFT TO DO!!!
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
        if(!check) //it wasn't found or nothing to do. Return to exit
            return false;
        //else we check if anything needs management
        //Scenario 1: Child is leaf with spares
        if(__c[child]->is_leaf() && __c[child]->__d_s > 0)
            return false; //It's done, we're good
        else if(__c[child]->is_leaf()){
            //Scenario 2: Empty leaf
            bool r_check = rotate_check(child);
            if(r_check)
                return false;
            //Scenario 3: No siblings
            //Sub scenario 1: Give + merge with data of parent
            if(__d_s > 1){
                T d;
                btree_node<T>* ptr;
                bool flag = true;
                if(child < __d_s){
                    swap(d, __d[child]); //take the data
                    ptr = __c[child+1];
                    delete __c[child];
                    __c[child] = nullptr;
                }else{
                    swap(d, __d[child-1]);
                    ptr = __c[child-1];
                    flag = false; //Don't have to reshuffle data since it's at end
                    delete __c[child];
                }
                move_to_end(__d, __d_s, child); //reshuffle data
                ptr->insert(d); //insert data into pointer
                for(size_t i = child; i < __d_s+1; i++){
                    swap(__c[i], __c[i+1]);
                } //organize the children to move the nullptrs out of bounds
                return false; //escape sequence
            }
            //sub scenario 2: Give the lone data of the parent
            //and then get a loan from parents
            if(__d_s == 1){
                T d;
                swap(__d[0], d);
                __d_s--;
                __c[child]->insert(d);
                return true;
            }
            if(DEBUG) cout << "UHHHH, shouldn't be here in code...\n";
        }else{ //SCENARIO 4: Child was not a leaf

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
template<typename T>
bool btree_node<T>::rotate_left(size_t i){
    if(__c[i+1]->__d_s < 2) //cannot take from a child with less than 2
        return false;
    //Move the smallest element from the right child to the left
    T d; //temporary data
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
    T d; //temporary data
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
        return true;
    }else{
        if(__c[__d_s]->take_greatest(input)){
            //Since it took from this child leaf, then check it if it needs rotations
            if(!rotate_check(__d_s)){
                //Check if rotation check could not fix
                //Then solution is to merge parent data into child
            }
        }
    }
}
template<typename T>
bool btree_node<T>::fix_empty_child(size_t i){
    //Checks if a child __d_s is 0.
    // If it is, then move the parent data down and merge
    if(__c[i]->__d_s == 0){

    }else{
        return false;
    }
}
#endif // BTREE_NODE_H
