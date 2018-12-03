#ifndef MAP_H
#define MAP_H

#include <iostream>
#include <typeinfo>
#include "bplustree.h"
#include "pair.h"

//The map can hold keys of string or int
template<typename K=long long, typename V=int>
class simple_map{
public:
    class Iterator{
    public:
        friend class simple_map;
        Iterator(bool unassigned=true, bool end=false):__unassigned(unassigned), __end(end){}
        Iterator operator++(int __empty){
            __iter++;
            return *this;
        }
        Iterator operator++(){__iter++; return *this;}
        Pair<K,V>& operator *(){
            return (*__iter);
        }
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs){
            return lhs.__iter == rhs.__iter;
        }
        friend bool operator !=(const Iterator& lhs, const Iterator& rhs){
            return !(lhs.__iter == rhs.__iter);
        }
        Iterator& operator =(const typename BPlusTree<Pair<K,V> >::Iterator& it){
            __iter = it;
            return *this;
        }
    private:
        typename BPlusTree<Pair<K,V> >::Iterator __iter;
        bool __end, __unassigned;
    };
    //CTORS
    simple_map();
    simple_map(const simple_map<K,V>& copy);
    simple_map<K,V>& operator =(const simple_map<K,V>& copy);

    //Element access controls
    V& operator [](const K& k);
    V& at(const K& k);
    const V& at(const K& k) const;
    //MEMBER MODIFIERS
    virtual bool insert(const K& k, const V& v=V());
    virtual bool insert(const Pair<K,V>& pair);
    virtual bool erase(const K& k);
    virtual void clear();
    //ITERATOR STUFF
    Iterator begin(){Iterator it; it = __map.begin(); return it;}
    Iterator end(){Iterator it; it = __map.end(); return it;}
    Iterator lower_bound(const K& k){Iterator it; it = __map.find(Pair<K,V>(k));
                                    return it;}
    Iterator upper_bound(const K& k){Iterator it; it = __map.find(Pair<K,V>(k));
                                     return it;}

    size_t size() const;
    virtual bool is_valid() const {return (__map.verify());}
    virtual void print_data(ostream& outs=cout) const; //prints some stats about current map

    virtual void print() const{ __map.print(); cout << endl;}
    friend ostream& operator<<(ostream& outs, const simple_map<K,V>& m){
//        outs << "------------ MAP -------------" << endl;
        outs << m.__map;
//        outs << endl;
        return outs;
    }

protected:
    BPlusTree<Pair<K,V> > __map; //By default stores integers as keys
    size_t __keys; //I have size functions, this reduces workload though
};

template<typename K, typename V>
simple_map<K,V>::simple_map(){
//    map(2, false);
    __keys = 0;
}
template<typename K, typename V>
simple_map<K,V>::simple_map(const simple_map<K,V>& copy){
    if(DEBUG) cout << "[SIMPLEMAP] copy ctor\n";
    __map = copy.__map; //call bpt copy operator
    __keys = copy.__keys;
    cout << "Keys: " << __keys << endl;
    if(DEBUG) cout << "[SIMPLEMAP] copy ctor DONE\n";
}
template<typename K, typename V>
simple_map<K,V>& simple_map<K,V>::operator =(const simple_map<K,V>& copy){
    if(this == &copy)
        return (*this);
    if(DEBUG) cout << "[SIMPLEMAP] copy operator\n";
//    simple_map<K,V> temp(copy);
//    swap(__keys, temp.__keys);
    __keys = copy.__keys;
    __map = copy.__map;
    if(DEBUG) cout << "[SIMPLEMAP] copy operator DONE\n";
//    swap(__map, temp.__map);
    //temp becomes out of scope and is deleted
    return (*this);
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
    Pair<K,V>* ptr = &__map.get(Pair<K,V>(k, V()));
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
void simple_map<K,V>::print_data(ostream& outs) const{
    outs << "Size: " << size() << " | Valid Tree: "
         << (is_valid()?"True":"False") << endl;
}
template<typename K, typename V>
V& simple_map<K,V>::at(const K &k){
    Pair<K,V>* ptr = &__map.get(k);
    if(ptr == nullptr){
        __map.insert(Pair<K,V>(k, V()));
        //ptr = new Pair<K,V>(k,V()) then insert?
        ptr = &__map.get(k);
        __keys++;
    }
    return (*ptr).val;
}

#endif // MAP_H
