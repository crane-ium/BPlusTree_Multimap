#ifndef BTREE_FUNCTIONS_H
#define BTREE_FUNCTIONS_H

#include <cstdlib>

using namespace std;

template<typename T>
bool is_there(T* data, const size_t s, const T& target){
    for(size_t i = 0; i < s; i++)
        if(data[i]==target)
            return true;
    return false;
}
template<typename T>
size_t first_ge(T* data, const size_t s, const T& target){
    //Return the first index that data[index] is >= target
    for(size_t i = 0; i < s; i++)
        if(data[i] >= target)
            return i;
    return s;
}
template<typename T>
bool insert_sorted(T* data, size_t& s, const T& target, bool dupes=false){
    if(!dupes && is_there(data, s, target))
        return false;
    data[s] = target;
    for(size_t i = 0; s - i > 0; i++){ //insertion sort it
        if(data[s-i] < data[s-i-1])
            swap(data[s-i], data[s-i-1]);
        else
            break;
    }
    s++;
    return true;
}

#endif // BTREE_FUNCTIONS_H
