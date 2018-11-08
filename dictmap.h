#ifndef DICTMAP_H
#define DICTMAP_H

#include "map.h"

template<typename K=long long, typename V=long long, typename K2=string>
class Dict : simple_map<K,V>, simple_map<K2,V>{
public:
    Dict();

    friend ostream& operator <<(ostream& outs, Dict<K,V,K2>& dict){
        outs << "------------ MAP ONE -------------" << endl;
        outs << dict.simple_map<K,V>::map << endl;
        outs << "------------ MAP TWO -------------" << endl;
        outs << dict.simple_map<K2,V>::map << endl;
        return outs;
    }
private:
    size_t __totalkeys;
};

template<typename K, typename V, typename K2>
Dict<K,V,K2>::Dict()
    : simple_map<K,V>(), simple_map<K2,V>(){
    __totalkeys = 0;
}
#endif // DICTMAP_H
