#include <iostream>
#include "btree"
#include "pair.h"
#include "map.h"
#include <random>
#include <ctime>
void test_BTree_auto(int tree_size=5000, int how_many=500, bool report=false);
bool test_BTree_auto(int how_many, bool report=true);
int Random(int lo, int hi)
{
    int r = rand()%(hi-lo+1)+lo;

    return r;
}
struct tester{
    int val;
    tester(int v=0):val(v){}
};
void old_tests();
void test_pair_v1();
using namespace std;

int main()
{
    using pair = Pair<string, long long>;
    simple_map<string, long long> map;
    map.insert("foo", 111);
    map.insert("bar",222);
    map.insert("test",333);
    cout << map << endl;
    map.erase("bar");
    map.erase("bar");
    map.print();
    cout << map.size() << endl;
    map["bar"] = 1;
    map.at("foo") = -111;
    map["googly"] = 1235;
    map.at("new") = 321;
    map.insert(pair("pairinsert",99));
    map.print();
    return 0;
}

void test_BTree_auto(int tree_size, int how_many, bool report){
    bool verified = true;
    for (int i = 0; i< how_many; i++){
        if (report){
            cout<<"*********************************************************"<<endl;
            cout<<" T E S T:    "<<i<<endl;
            cout<<"*********************************************************"<<endl;
        }
        if (!test_BTree_auto(tree_size, report)){
            cout<<"T E S T :   ["<<i<<"]    F A I L E D ! ! !"<<endl;
            verified = false;
            return;
        }
    }
    cout<<"**************************************************************************"<<endl;
    cout<<"**************************************************************************"<<endl;
    cout<<"             E N D     T E S T: "<<how_many<<" tests of "<<tree_size<<" items: ";
    cout<<(verified?"VERIFIED": "VERIFICATION FAILED")<<endl;
    cout<<"**************************************************************************"<<endl;
    cout<<"**************************************************************************"<<endl;

}

bool test_BTree_auto(int how_many, bool report){
    const int MAX = 10000;
    assert(how_many < MAX);
    BTree<int> bpt;
    int a[MAX];
    int original[MAX];
    int deleted_list[MAX];

    int original_size;
    int size;
    int deleted_size = 0;

    //fill a[ ]
    for (int i= 0; i< how_many; i++){
        a[i] = i;
    }
    //shuffle a[ ]: Put this in a function!
    for (int i = 0; i< how_many; i++){
        int from = Random(0, how_many-1);
        int to = Random(0, how_many -1);
        int temp = a[to];
        a[to] = a[from];
        a [from] = temp;
    }
    //copy  a[ ] -> original[ ]:
    copy_array(original, a, how_many);
    size = how_many;
    original_size = how_many;
    for (int i=0; i<size; i++){
        bpt.insert(a[i]);

    }
//    bpt.print();
    if (report){
        cout<<"========================================================"<<endl;
        cout<<"  "<<endl;
        cout<<"========================================================"<<endl;
        cout<<bpt<<endl<<endl;
    }
    for (int i= 0; i<how_many; i++){
        int r = Random(0, how_many - i - 1);
        if (report){
            cout<<"========================================================"<<endl;
            cout<<bpt<<endl;
            cout<<". . . . . . . . . . . . . . . . . . . . . . . . . . . . "<<endl;
            cout<<"deleted: "; print_array(deleted_list, deleted_size);
            cout<<"   from: "; print_array(original, original_size);
            cout<<endl;
            cout<<"  REMOVING ["<<a[r]<<"]"<<endl;
            cout<<"========================================================"<<endl;
        }
        bpt.remove(a[r]);


        delete_item(a, r, size, deleted_list[deleted_size++]);
        if (!bpt.verify()){
            cout<<setw(6)<<i<<" I N V A L I D   T R E E"<<endl;
            cout<<"Original Array: "; print_array(original, original_size);
            cout<<"Deleted Items : "; print_array(deleted_list, deleted_size);
            cout<<endl<<endl<<bpt<<endl<<endl;
            return false;
        }
    }
    if (report) cout<<" V A L I D    T R E E"<<endl;
    cout << bpt << endl;

    return true;
}

void old_tests(){

    srand(time(NULL));
//    test_BTree_auto(1000, 10, false);
    BTree<int> bt;
    for(size_t i = 0; i < 10; i++)
        bt.insert(i);
    bt.get(5) = 10;
    bt.get(10) = 33;
    bt.print();
    cout << bt.size() << endl;
    cout << bt.verify() << endl;
    bt.remove(33);
    bt.print();
    cout << bt.verify() << endl;
    cout << bt.size() << endl;
    vector<int> v;
    v.push_back(1);
    v.push_back(5);
    v.push_back(10);
    vector<int> v2;
    v2.push_back(33);
    v2.push_back(44);
    v += v2;
    cout << v << endl;
    size_t st = 5;
    int in = -5;
    cout << (int(st) > in) << endl;
    BTree<int> bt2(bt);
    bt2.insert(33);
    bt2.print();
    BTree<int> bt3;
    bt3 = bt2;
    bt3.insert(111);
    bt3.print();
}

void test_pair_v1(){

    typedef Pair<string, int> siPair;
    Pair<string, int> pair("str",123);
    Pair<string, int> pair2("str",321);
    BTree<Pair<string, int> > btpair;
    btpair.insert(siPair("new1", 111));
    btpair.insert(siPair("random", 222));
    btpair.insert(siPair("extra", 333));
    btpair.insert(siPair("extr", 444));
    btpair.get(siPair("extra", int())) = 555;

    int ii = siPair("123", 666);
    cout << ii << endl;
    cout << btpair << endl;
}
