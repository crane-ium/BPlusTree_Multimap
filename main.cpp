#include <iostream>
#include "bplustree.h"
#include "pair.h"
#include "map.h"
#include "dictmap.h"
#include "mpair.h"
#include <random>
#include <ctime>
#include "multimap.h"

int DEBUG = 0;

void test_BPlusTree_auto(int tree_size=5000, int how_many=500, bool report=false);
bool test_BPlusTree_auto(int how_many, bool report=true);
int Random(int lo, int hi)
{
    int r = rand()%(hi-lo+1)+lo;

    return r;
}
void old_tests();
void test_pair_v1();
void test_pair_v2();
template<typename maptype=simple_map<int, string> >
void multimap_test1(); //tests map/multimap
void bplustree_test1();

using namespace std;

int main()
{
//bplustree_test1();
//    cout << "-------- SIMPLEMAP ---------" << endl;
//    multimap_test1<>();
//    cout << "-------- MULTIMAP ---------" << endl;
    multimap_test1<multimap<int, string> >();
//    multimap<string, int> mm;

//    vector<int> vv;
//    cout << vv << endl;
//    mm.insert("key", 1);
//    mm.insert("bar", 22);
//    mm["key"] += 10;
//    mm["foo"] += -5;
//    cout << mm << endl;
//    cout << mm.size() << endl;
//    mm["foo"] -= -5;
//    mm[""] -= 100;
//    mm[""] += 100;
//    cout << mm.size() << endl << "Valid: " << mm.is_valid() << endl;
//    mm["bar"] += 33;
//    mm["bar"] -= 99;
//    mm["foo"] -= 66;
//    mm.erase("");
//    mm.erase("foo");
//    cout << mm.size() << endl << "Valid: " << mm.is_valid() << endl;
//    mm.clear();
//    cout << mm.size() << endl << "Valid: " << mm.is_valid() << endl;
//    mm.print();

    
//    MPair<string, int> mp1("key");
//    vector<int> v;
//    v.push_back(22);
//    v.push_back(55);
//    v.push_back(88);
//    mp1.vec += 25;
//    mp1.vec += v;
//    cout << mp1 << endl;

//    B b;
//    b.display();
//    b.assign("foo");
//    b.assign(123);
//    b.display();

//    Dict<int, int, string> dictionary;
//    dictionary.insert("Test", 123);
//    dictionary.insert(987, 555);
    return 0;
}

template<typename maptype>
void multimap_test1(){
    //Run a comprehensive test on simple_map and multimap
    //First let's test map
    maptype m;
    static const size_t KEYS = 100;
    static const int RMIN=-100, RMAX=100;
    size_t unique_keys = 0;
    int* map_keys = new int[KEYS]; //keep track of what we want to add
    size_t unq_keys[RMAX-RMIN] = {0};
    for(size_t i = 0; i < KEYS; i++){
        map_keys[i] = rand() % (RMAX - RMIN) + RMIN;
    }
    //count unique keys
    for(size_t i = 0; i < KEYS; i++){
        if(!unq_keys[map_keys[i]-RMIN]){
            unique_keys++;
        }
        unq_keys[map_keys[i]-RMIN]++;
    }
    //INSERT INTO MAP!
    for(size_t i = 0; i < KEYS; i++){
        m[map_keys[i]] += (string)("0");
    }
    cout << "~Inserted into map~" << endl;
    m.print();
    cout << "-- AFTER insert data --\n";
    cout << "Unique keys: " << unique_keys
         << (unique_keys==m.size()?" Valid size":" Invalid size") << endl;
    m.print_data();
//TODO
    //NOW DELETE FROM THE MAP!
    //let's delete the first half of keys, regardless of uniqueness
//    for(size_t i = 0; i < KEYS/2; i++){
//        m.erase(map_keys[i]);
//        if(unq_keys[map_keys[i]-RMIN]){
//            unq_keys[map_keys[i]-RMIN] = 0;
//            unique_keys--;
//        }
//    }
//    cout << "~Deleted from map~" << endl;
//    m.print();
//    cout << "-- AFTER delete data --\n";
//    cout << "Unique keys AFTER DELETE: " << unique_keys
//         << (unique_keys==m.size()?" Valid size":" Invalid size") << endl;
//    m.print_data();
    //Test other various things
    m.clear();
    cout << ">Cleared the tree\n";
    m.print();
    m.print_data();
    cout << ">Insert 5/5 (unique/duplicates) test\n";
    for(size_t i = 0; i < 10; i++){
        m.insert(i/2, to_string(i)); //insert half the keys
//        m.print();
    }
    m.print();
    m.print_data();
    cout << ">Copy ctor test: copy first map + insert\n";
    maptype m2(m);
    for(size_t i = 0; i < 10; i++)
        m2.insert(i, to_string(i+100));
    m2.print();
    m2.print_data();
    cout << ">Copy operator test: copy second map + insert\n";
    maptype m3;
    cout << ">Printing final m3 data\n";
    m3 = m2;
    for(size_t i = 5; i < 15; i++)
        m3[i] += to_string(i);
    m3.print();
    m3.print_data();
//    Using it directly into cout
    cout << "map3[3]: " <<  m3[3] << endl;
    cout << "--Complete Tests--\n\n";
}

void test_BPlusTree_auto(int tree_size, int how_many, bool report){
    bool verified = true;
    for (int i = 0; i< how_many; i++){
        if (report){
            cout<<"*********************************************************"<<endl;
            cout<<" T E S T:    "<<i<<endl;
            cout<<"*********************************************************"<<endl;
        }
        if (!test_BPlusTree_auto(tree_size, report)){
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

bool test_BPlusTree_auto(int how_many, bool report){
    const int MAX = 10000;
    assert(how_many < MAX);
    BPlusTree<int> bpt;
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
//    test_BPlusTree_auto(1000, 10, false);
    BPlusTree<int> bt;
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
    BPlusTree<int> bt2(bt);
    bt2.insert(33);
    bt2.print();
    BPlusTree<int> bt3;
    bt3 = bt2;
    bt3.insert(111);
    bt3.print();
}

void test_pair_v1(){

    typedef Pair<string, int> siPair;
    Pair<string, int> pair("str",123);
    Pair<string, int> pair2("str",321);
    BPlusTree<Pair<string, int> > btpair;
    btpair.insert(siPair("new1", 111));
    btpair.insert(siPair("random", 222));
    btpair.insert(siPair("extra", 333));
    btpair.insert(siPair("extr", 444));
    btpair.get(siPair("extra", int())) = 555;

    int ii = siPair("123", 666);
    cout << ii << endl;
    cout << btpair << endl;
}

void test_pair_v2(){
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
    map.clear();
    map.print();
}

void bplustree_test1(){
    //bpt<mpair<string, int> > tree;
    //the T is mpair<string, int>
    //if we want to copy an mpair with only the key
    //we need be able to construct a new one
    //without taking the values (ints)
    //so then we should take
    using BPT = BPlusTree<int>;
    BPT bpt;
    bool check;
    size_t count = 0;
    for(size_t i = 0; i < 100; i++){
        check = bpt.insert(rand() % 100);
        if(check)
            count++;
    }
    bpt.print();
    bpt.print_tree();
    for(size_t i = 0; i < 10; i++){

//        cout << "Finding " << i << ": " << (*ptr) << endl;
    }
//    (*bpt.find(8)) = 11;
    bpt.print_tree();
    bpt.verify(true);
    cout << "Size: " << bpt.size() << " vs Size count: " << count << endl;

    typename BPT::Iterator iter;
    iter = bpt.begin();
    for(iter=bpt.begin(); iter != bpt.end(); ++iter){
        cout << (*iter) << " : ";
        iter.print();
    }
    bpt.cleartree();
    assert(bpt.empty());
    bpt.insert(50);
    bpt.print();
    for(iter = bpt.begin(); iter != bpt.end(); iter++)
        iter.print();
    cout << "Size: " << bpt.size() << endl;
    bpt.get(50) = 25;
    assert(bpt.verify());
    cout << bpt << endl;
}
