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
    bool insert(const K& k, const V& v);
    bool insert(const Pair<K,V>& pair);
    bool erase(const K& k);
    void clear();

    size_t size() const;
    bool is_valid() const {return (map.verify());}

    void print() const{ map.print();}
    friend ostream& operator<<(ostream& outs, const simple_map<K,V>& m){
        outs << "------------ MAP -------------" << endl;
        outs << m.map << endl;
        return outs;
    }

private:
    BTree<Pair<K,V> > map; //By default stores integers as keys
    size_t keycount; //I have size functions, this reduces workload though
};

template<typename K, typename V>
simple_map<K,V>::simple_map(){
//    map(2, false);
    keycount = 0;
}

template<typename K, typename V>
bool simple_map<K,V>::insert(const K &k, const V &v){
    bool check = map.insert(Pair<K,V>(k, v));
    if(check)
        keycount++;
    return check;
}
template<typename K, typename V>
bool simple_map<K,V>::insert(const Pair<K,V>& pair){
    bool check = map.insert(pair);
    if(check)
        keycount++;
    return check;
}
template<typename K, typename V>
bool simple_map<K,V>::erase(const K &k){
    //Search for Pair<K,V>(K,V())
    Pair<K,V>* ptr = map.find(Pair<K,V>(k, V()));
    if(ptr == nullptr)
        return false;
    map.remove((*ptr));
    keycount--;
    return true;
}
template<typename K, typename V>
size_t simple_map<K,V>::size() const{
    return keycount;
}
template<typename K, typename V>
V& simple_map<K,V>::operator [](const K& k){
    return at(k);
}
template<typename K, typename V>
V& simple_map<K,V>::at(const K &k){
    Pair<K,V>* ptr = map.find(k);
    if(ptr == nullptr){
        map.insert(Pair<K,V>(k, V()));
        //ptr = new Pair<K,V>(k,V()) then insert?
        ptr = map.find(k);
    }
    return (*ptr).val;
}

#endif // MAP_H
