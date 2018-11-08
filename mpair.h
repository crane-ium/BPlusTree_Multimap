#ifndef MPAIR_H
#define MPAIR_H

#include <vector>
#include "pair.h"

using namespace std;

template<typename K, typename V>
struct MPair : public Pair<K,V>{
    vector<V> vec;
    MPair(const K& k=K());
    MPair(const K &k, const V& v);
    MPair(const K& k, const vector<V>& v);
    MPair<K,V>& operator =(const vector<V>& v){
        vec.clear();
        copy(v.begin(), v.end(), back_inserter(vec));
        return (*this);
    }
    template<typename M, typename N>
    friend ostream& operator <<(ostream& outs, const MPair<M,N>& mp){
        outs << "(" << mp.key << ": " << mp.vec << ")";
        return outs;
    }
};
template<typename K, typename V>
MPair<K,V>::MPair(const K &k): Pair<K,V>(k){

}
template<typename K, typename V>
MPair<K,V>::MPair(const K &k, const V &v): Pair<K,V>(k){
    vec.push_back(v);
}
template<typename K, typename V>
MPair<K,V>::MPair(const K &k, const vector<V> &v)
    : Pair<K,V>(), Pair<K,V>::key(k), vec(v.copy()){
}

#endif // MPAIR_H
