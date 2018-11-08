#ifndef PAIR_H
#define PAIR_H

#include <iostream>
#include <vector>

using namespace std;

template<typename K, typename V>
struct Pair{
    K key;
    V val;
    //CTOR
    Pair(const K& k=K(), const V& v=V());
    //MODIFIERS
    Pair<K,V>& operator =(const V& right);
    //COMPARISON OPERATORS

    operator V() const;
    template<typename M, typename N>
    friend bool operator ==(const Pair<M,N>& lhs, const Pair<M,N>& rhs);
    template<typename M, typename N>
    friend bool operator < (const Pair<M,N>& lhs, const Pair<M,N>& rhs);
    template<typename M, typename N>
    friend bool operator > (const Pair<M,N>& lhs, const Pair<M,N>& rhs);
    template<typename M, typename N>
    friend bool operator <= (const Pair<M,N>& lhs, const Pair<M,N>& rhs);
    //Addition operator
    template<typename M, typename N>
    friend Pair<M,N> operator + (const Pair<M,N>& lhs, const Pair<M,N>& rhs);
    template<typename M, typename N>
    friend ostream& operator <<(ostream& , const Pair<M,N>& );
};

template<typename K, typename V>
Pair<K,V>::Pair(const K &k, const V &v)
    : key(k), val(v){

}
template<typename K, typename V>
bool operator ==(const Pair<K, V>& lhs, const Pair<K, V>& rhs){
    //Compares if the keys are equivalent
    return lhs.key == rhs.key;
}
template<typename K, typename V>
bool operator < (const Pair<K, V>& lhs, const Pair<K, V>& rhs){
    return lhs.key < rhs.key;
}
template<typename K, typename V>
bool operator > (const Pair<K, V>& lhs, const Pair<K, V>& rhs){
    return lhs.key > rhs.key;
}
template<typename K, typename V>
bool operator <= (const Pair<K, V>& lhs, const Pair<K, V>& rhs){
    return lhs.key <= rhs.key;
}
template<typename K, typename V>
Pair<K, V> operator + (const Pair<K, V>& lhs, const Pair<K, V>& rhs){
    //Adds the lhs/rhs values
//    Pair<K,V>* pair = new Pair<K,V>;
//    pair->key = lhs.key;
//    pair->val = lhs.val;
//    pair->val += rhs.val;
    return Pair<K,V>(lhs.key, (lhs.val + rhs.val));
}
template<typename K, typename V>
std::ostream& operator <<(std::ostream& outs, const Pair<K,V>& print){
    outs << "(" << print.key << ": " << print.val << ")";
    return outs;
}
template<typename K, typename V>
Pair<K,V>& Pair<K,V>::operator =(const V& right){
    this->val = right;
    return (*this);
}
template<typename K, typename V>
Pair<K,V>::operator V() const{
    return this->val;
}

#endif // PAIR_H
