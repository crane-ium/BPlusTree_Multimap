#include <iostream>
#include "btree"
#include <random>

using namespace std;

int main()
{

    BTree<int> btr(2);
    for(size_t i = 0; i < 10; i++)
        btr.insert(i);
    btr.remove(7);
    btr.remove(1);
    btr.remove(0);
    btr.remove(2);
    btr.remove(5);
    btr.print();
    btr.remove(3);
    btr.print();
    btr.verify();
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
