#ifndef PAIR_H
#define PAIR_H

template<typename K, typename V>
struct Pair{
    K key;
    V val;

    Pair(const K& k=K(), const V& v=V());
};

template<typename K, typename V>
Pair<K,V>::Pair(const K &k, const V &v)
    : key(k), val(v){

}

#endif // PAIR_H
