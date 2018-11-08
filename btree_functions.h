#ifndef BTREE_FUNCTIONS_H
#define BTREE_FUNCTIONS_H

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

template<typename T>
bool is_there(T* data, const size_t s, const T& target){
    for(size_t i = 0; i < s; i++)
        if(data[i]==target)
            return true;
    return false;
}
template<typename T>
size_t index_of(T* data, const size_t s, const T& target){
    //Returns the index of the found target, else returns s
    for(size_t i = 0; i < s; i++)
        if(data[i] == target)
            return i;
    return s;
}
template<typename T>
size_t first_ge(T* data, const size_t s, const T& target){
    //Return the first index that data[index] is >= target
    for(size_t i = 0; i < s; i++)
        if(target <= data[i])
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
template<typename T>
void move_to_end(T* data, size_t& s, size_t i){
    for(size_t j = i; j < s; j++){
        swap(data[j], data[j+1]);
    }
    s--;
}
template<typename T>
void copy_array(T a_new[], const T a_copy[], const size_t& s){
    for(size_t i = 0; i < s; i++)
        a_new[i] = a_copy[i];
}
template<typename T>
void print_array(T arr[], const T& s){
    for(size_t i = 0; int(i) < s; i++){
        cout <<  "[" << setw(4)  << setfill('0') << arr[i] << "] ";
    }
    cout << endl;
}
template<typename T>
void delete_item(T data[], int& i, int& s, T& entry){
    swap(entry, data[i]);
    s--;
    for(int ii = i; i < s; i++)
        swap(data[ii], data[ii+1]);
}
template<typename T>
ostream& operator <<(ostream& outs, const vector<T>& data){
    outs << "{";
    for(size_t i = 0; i < data.size(); i++){
        outs << data[i];
        if(i < data.size()-1)
            outs << ", ";
    }
    outs << "}";
    return outs;
}
template<typename T>
vector<T>& operator +=(vector<T>& left, vector<T>& right){
    for(size_t i = 0; i < right.size(); i++){
        left.push_back(right[i]);
    }
    return left;
}
#endif // BTREE_FUNCTIONS_H
