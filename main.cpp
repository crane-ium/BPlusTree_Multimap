#include <iostream>
#include "btree"

using namespace std;

int main()
{
    btree_node<int> node;
    node.insert(1);
    node.insert(-5);
    node.insert(10);
    cout << node << endl;
    return 0;
}
