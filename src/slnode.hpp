#ifndef SLNODE_H
#define SLNODE_H

#include <random>
#include <ostream>
#include "skiplist_exceptions.hpp"

template<class U, class Compare, typename TRandom, int MaxLevel>
class skiplist;

template<class T> 
class SLNode {
    const T* val;
    SLNode* next;
    SLNode* prev;
    SLNode* up;
    SLNode* down;

public:
    SLNode(const T* val, SLNode* next=nullptr, SLNode* prev=nullptr, SLNode* up=nullptr, SLNode* down=nullptr): 
            val(val), next(next), prev(prev), up(up), down(down) {}
    
    SLNode* get_up() const { return up; }
    SLNode* get_down() const { return down; }
    SLNode* get_next() const { return next; }
    SLNode* get_prev() const { return prev; }

    void set_up(SLNode* up) { this->up=up; }
    void set_down(SLNode* down) { this->down=down; }
    void set_next(SLNode* next) { this-> next=next; }
    void set_prev(SLNode* prev) { this->prev=prev; }

    const T& get_val() const { 
        if(! val) throw SLNodeException("Impossible to get value from empty SLNode");
        return *val; 
    }
    
    template<class U, class Compare, typename TRandom, int MaxLevel>
    friend void skiplist<U, Compare, TRandom, MaxLevel>::erase(typename skiplist<U, Compare, TRandom, MaxLevel>::iterator it);

    template<class U, class Compare, typename TRandom, int MaxLevel>
    friend skiplist<U, Compare, TRandom, MaxLevel>::~skiplist();

    friend std::ostream& operator<< (std::ostream& out, const SLNode<T>& n) {
        out << *(n.val);
        return out;
    }
};


#endif // SLNODE_H