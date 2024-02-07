#ifndef SLNODE_H
#define SLNODE_H

#include <random>
#include <ostream>

template<class T> 
class SLNode {
    const T* val;
    SLNode* next;
    SLNode* prev;

    SLNode* up;
    SLNode* down;
    SLNode(T* val): val(val), next(nullptr), prev(nullptr), up(nullptr), down(nullptr) {}

    template<class U, class Compare, typename TRandom, int MaxLevel>
    friend class skiplist;

public:
    SLNode* get_up() { return up; }
    SLNode* get_down() { return down; }
    SLNode* get_next() { return next; }
    SLNode* get_prev() { return prev; }

    friend std::ostream& operator<< (std::ostream& out, const SLNode<T>& n) {
        out << *(n.val);
        return out;
    }
};


#endif // SLNODE_H