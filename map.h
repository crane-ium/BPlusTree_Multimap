#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <typeinfo>
#include "btree"
#include "pair.h"

//The map can hold keys of string or int
template<typename K=long long, typename V=int>
class simple_map{
public:
    //CTORS
    simple_map();

    //Element access controls
    V& operator [](const K& k);
    V& at(const K& k);
    const V& at(const K& k) const;
    //MEMBER MODIFIERS
    virtual bool insert(const K& k, const V& v);
    virtual bool insert(const Pair<K,V>& pair);
    virtual bool erase(const K& k);
    virtual void clear();

    size_t size() const;
    virtual bool is_valid() const {return (__map.verify());}

    virtual void print() const{ __map.print();}
    friend ostream& operator<<(ostream& outs, const simple_map<K,V>& m){
        outs << "------------ MAP -------------" << endl;
        outs << m.__map << endl;
        return outs;
    }

protected:
    BTree<Pair<K,V> > __map; //By default stores integers as keys
    size_t __keys; //I have size functions, this reduces workload though
};

template<typename K, typename V>
simple_map<K,V>::simple_map(){
//    map(2, false);
    __keys = 0;
}

template<typename K, typename V>
bool simple_map<K,V>::insert(const K &k, const V &v){
    bool check = __map.insert(Pair<K,V>(k, v));
    if(check)
        __keys++;
    return check;
}
template<typename K, typename V>
bool simple_map<K,V>::insert(const Pair<K,V>& pair){
    bool check = __map.insert(pair);
    if(check)
        __keys++;
    return check;
}
template<typename K, typename V>
bool simple_map<K,V>::erase(const K &k){
    //Search for Pair<K,V>(K,V())
    Pair<K,V>* ptr = __map.find(Pair<K,V>(k, V()));
    if(ptr == nullptr)
        return false;
    __map.remove((*ptr));
    __keys--;
    return true;
}
template<typename K, typename V>
size_t simple_map<K,V>::size() const{
    return __keys;
}
template<typename K, typename V>
V& simple_map<K,V>::operator [](const K& k){
    return at(k);
}
template<typename K, typename V>
void simple_map<K,V>::clear(){
    __map.cleartree();
    __keys = 0;
}
template<typename K, typename V>
V& simple_map<K,V>::at(const K &k){
    Pair<K,V>* ptr = __map.find(k);
    if(ptr == nullptr){
        __map.insert(Pair<K,V>(k, V()));
        //ptr = new Pair<K,V>(k,V()) then insert?
        ptr = __map.find(k);
    }
    return (*ptr).val;
}

#endif // MAP_H
