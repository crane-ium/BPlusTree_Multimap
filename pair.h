#ifndef PAIR_H
#define PAIR_H

#include <iostream>

template<typename K, typename V>
struct Pair{
    K key;
    V val;

    Pair(const K& k=K(), const V& v=V());
    friend std::ostream& operator <<(std::ostream& outs, Pair<K,V>& pair);
};

template<typename K, typename V>
Pair<K,V>::Pair(const K &k, const V &v)
    : key(k), val(v){

}
template<typename K, typename V>
std::ostream& operator <<(std::ostream& outs, Pair<K,V>& pair){
    outs << "(" << key << ": " << val << ")";
    return outs;
}


#endif // PAIR_H
