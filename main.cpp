#include <iostream>
#include "btree"

using namespace std;

int main()
{

    BTree<int> btr(1);
    btr.insert(1);
    btr.insert(5);
    btr.insert(10);
    btr.insert(15);
    btr.insert(20);
    btr.insert(25);
    btr.insert(30);
    btr.insert(35);
    btr.print();
//    btree_node<int>* node = new btree_node<int>(1);
//    node->insert(1);
//    node->insert(-5);
//    node->insert(10);
//    fix_excess(node);
//    node->insert(15);
//    node->insert(8);
//    node->print();
//    node->insert(7);
//    node->insert(6);
//    node->insert(4);
//    cout << (*node) << endl;
//    node->print();
    return 0;
}
