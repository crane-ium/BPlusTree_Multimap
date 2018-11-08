#ifndef MULTIMAP_H
#define MULTIMAP_H

#include "map.h"

template<typename K=string, typename V=int>
class multimap : simple_map<K,vector<V>{
public:
    multimap();
private:
    BTree<MPair<K, V> > __map;
};

#endif // MULTIMAP_H
