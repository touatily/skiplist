#ifndef SLNODE_H
#define SLNODE_H

#include <random>
#include <ostream>

template<class U, class Compare, typename TRandom, int MaxLevel>
class skiplist;

template<class T> 
class SLNode {
    const T* val;
    SLNode* next;
    SLNode* prev;

    SLNode* up;
    SLNode* down;
    

    //template<class U, class Compare, typename TRandom, int MaxLevel>
    //friend class skiplist;

public:
    SLNode(T* val, SLNode* next=nullptr, SLNode* prev=nullptr, SLNode* up=nullptr, SLNode* down=nullptr): 
            val(val), next(next), prev(prev), up(up), down(down) {}
    
    SLNode* get_up() const { return up; }
    SLNode* get_down() const { return down; }
    SLNode* get_next() const { return next; }
    SLNode* get_prev() const { return prev; }

    void set_up(SLNode* up) { this->up=up; }
    void set_down(SLNode* down) { this->down=down; }
    void set_next(SLNode* next) { this-> next=next; }
    void set_prev(SLNode* prev) { this->prev=prev; }

    const T& get_val() const { return *val; }
    
    template<class U, class Compare, typename TRandom, int MaxLevel>
    friend void skiplist<T, Compare, TRandom, MaxLevel>::erase(typename skiplist<T, Compare, TRandom, MaxLevel>::iterator it);

    friend std::ostream& operator<< (std::ostream& out, const SLNode<T>& n) {
        out << *(n.val);
        return out;
    }
};


#endif // SLNODE_H