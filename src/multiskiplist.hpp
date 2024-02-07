
#include <functional>
#include <vector>
#include <random>


template<class T, class Compare = std::less<class T>, 
            typename TRandom = std::default_random_engine, int maxLevel=10>
class multiskiplist {
    std::vector<Node*> levels;
    const double prob;
    size_t nb;

public:
    multiskiplist(double p=0.5);
    size_t size() const;

    void sketch() const;
    void print() const;
    bool empty() const;

    const T& front() const;
    const T& back() const;

    T& front();
    T& back();

    void assign();
    void assign(int n, T e);

    void insert();
    void find() const;
/*
    insert();
    find();
    erase();
    begin();
    end();
*/
};