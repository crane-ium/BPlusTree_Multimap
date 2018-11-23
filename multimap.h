#ifndef MULTIMAP_H
#define MULTIMAP_H

#include "map.h"
#include "mpair.h"

template<typename K=string, typename V=int>
class multimap : private simple_map<K,vector<V> >{
public:
    //CTOR
    multimap(); //Calls ctor of base class
    multimap(const multimap<K,V>& copy);
    multimap<K,V>& operator =(const multimap<K,V>& copy);
    
    vector<V>& operator [](const K& k); //Accesses the vec at key; creates vec if no key exists yet
    bool insert(const K& k);
    bool insert(const K& k, const V& v); //inserts a key: vec
    bool insert(const MPair<K,V>& pair);
    bool erase(const K &k);

    void clear() {__map.cleartree();this->__keys=0;}
    bool is_valid() const{return __map.verify();}

    using simple_map<K, vector<V> >::size; //make inherted private class's function public
//    size_t size() const{return this->__keys;}
    void print() const{__map.print();cout << endl;}
    void print_data(ostream &outs=cout) const;

    friend ostream& operator <<(ostream& outs, const multimap<K,V>& m){
//        outs << "------------ MAP -------------" << endl;
        outs << m.__map;
//        outs << endl;
        return outs;
    }
private:
    BPlusTree<MPair<K, V> > __map;
};

template<typename K, typename V>
multimap<K,V>::multimap()
    : simple_map<K,vector<V> >(){

}
template<typename K, typename V>
multimap<K,V>::multimap(const multimap<K,V>& copy)
    : simple_map<K, vector<V> >(){
    __map = copy.__map;
    this->__keys = copy.__keys;
}
template<typename K, typename V>
multimap<K,V>& multimap<K,V>::operator =(const multimap<K,V>& copy){
    if(this == &copy)
        return (*this);
    //copy-swap idiom
    multimap<K,V> temp(copy); //copy ctor
    swap(this->__keys, temp.__keys);
    swap(__map, temp.__map);
    return(*this);
}
template<typename K, typename V>
vector<V>& multimap<K,V>::operator [](const K& k){
//    MPair<K,V>* ptr = __map.find(k);
    typename BPlusTree<MPair<K, V> >::Iterator iter = __map.find(k);

    if(iter.is_null()){
        this->__keys++;
        __map.insert(MPair<K,V>(k));
        iter = __map.find(k);
    }
    return (*iter).vec;
}
template<typename K, typename V>
bool multimap<K,V>::insert(const K& k, const V& v){
    //Inserts a key + value
    bool check = __map.insert(MPair<K,V>(k));
    if(check){
        this->__keys++;
    }
    MPair<K,V>* ptr = &__map.get(MPair<K,V>(k));
    ptr->vec += v;
    return check;
}
template<typename K, typename V>
bool multimap<K,V>::insert(const K& k){
    //Inserts a key + value
    bool check = __map.insert(MPair<K,V>(k));
    if(check)
        this->__keys++;
    return check;
}
template<typename K, typename V>
bool multimap<K,V>::insert(const MPair<K,V>& pair){
    //Inserts a key + value
    bool check = __map.insert(pair);
    if(check)
        this->__keys++;
    return check;
}
template<typename K, typename V>
void multimap<K,V>::print_data(ostream &outs) const{
    outs << "Size: " << size() << " | Valid Tree: "
         << (is_valid()?"True":"False") << endl;
}
template<typename K, typename V>
bool multimap<K,V>::erase(const K &k){
    MPair<K,V>* ptr = &__map.get(MPair<K,V>(k));
    if(!ptr)
        return false;
    __map.remove((*ptr));
    this->__keys--;
    return true;
}
#endif // MULTIMAP_H
