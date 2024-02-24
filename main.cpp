


#include <iostream>
#include "src/slnode.hpp"
#include "src/skiplist.hpp"


using namespace std;


int main() {
    skiplist<double> sk(0.9);

    sk.insert(5);

    cout << "first = " << sk.back() << endl;
    sk.insert(6);
    cout << "first = " << sk.back() << endl;
    sk.insert(7);
    cout << "first = " << sk.back() << endl;
    sk.insert(8);
    cout << "first = " << sk.back() << endl;
    sk.insert(1);
    sk.insert(3);
    sk.insert(1);
    cout << "first = " << sk.back() << endl;
    sk.insert(0);
    sk.insert(9);
    
    cout << "first = " << sk.back() << endl;

    sk.sketch();

    sk.print();

    cout << "size = " << sk.size() << endl;

    return 0;
}