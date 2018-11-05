#include <iostream>
#include "btree"

using namespace std;

int main()
{
    btree_node<int>* node = new btree_node<int>();
    node->insert(1);
    node->insert(-5);
    node->insert(10);
    fix_excess(node);
    cout << (*node) << endl;
    node->print();
    return 0;
}
