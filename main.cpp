


#include <iostream>
#include "src/slnode.hpp"
#include "src/skiplist.hpp"


using namespace std;


int main() {
    skiplist<double> sk(0.6);

    sk.insert(9);
    sk.insert(7);
    sk.insert(6);
    sk.insert(1);
    sk.insert(3);
    

    sk.sketch();

    sk.print();

    cout << "size = " << sk.size() << endl;

    return 0;
}