#ifndef SLNODE_H
#define SLNODE_H

#include <random>
#include <ostream>
#include "skiplist_exceptions.hpp"

template<class T, class U, class Compare, typename TRandom, int MaxLevel>
class skiplist;



template<class K, class V> 
class SLNode {
    typedef std::pair<const K* const, V* const> value_type;

    value_type kv;
    SLNode<K, V>* next;
    SLNode<K, V>* prev;
    SLNode<K, V>* up;
    SLNode<K, V>* down;

public:
    SLNode(const K* key, V* value, SLNode<K, V>* next=nullptr, SLNode<K, V>* prev=nullptr, SLNode<K, V>* up=nullptr, SLNode<K, V>* down=nullptr): 
            kv(std::make_pair(key, value)), next(next), prev(prev), up(up), down(down) {
                if(! key) throw SLNodeException("Impossible to set key to nullptr");
            }
    
    SLNode<K, V>* get_up() const { return up; }
    SLNode<K, V>* get_down() const { return down; }
    SLNode<K, V>* get_next() const { return next; }
    SLNode<K, V>* get_prev() const { return prev; }
    const K& get_key() const { 
        if(! kv.first) throw SLNodeException("Impossible to get key from empty SLNode");
        return *(kv.first); 
    }
    const V& get_value() const { 
        if(! kv.second) throw SLNodeException("Impossible to get value from empty SLNode");
        return *(kv.second); 
    }
    const value_type& get_key_value() const { return kv; };

    void set_up(SLNode<K, V>* up) { this->up=up; }
    void set_down(SLNode<K, V>* down) { this->down=down; }
    void set_next(SLNode<K, V>* next) { this-> next=next; }
    void set_prev(SLNode<K, V>* prev) { this->prev=prev; }
    
    template<class T, class U, class Compare, typename TRandom, int MaxLevel>
    friend void skiplist<T, U, Compare, TRandom, MaxLevel>::erase(typename skiplist<T, U, Compare, TRandom, MaxLevel>::iterator it);

    template<class T, class U, class Compare, typename TRandom, int MaxLevel>
    friend void skiplist<T, U, Compare, TRandom, MaxLevel>::clear();
};

template<class K, class V> 
std::ostream& operator<< (std::ostream& out, const SLNode<K, V>& n) {
    out << "<" << n.get_key() << ", " << n.get_value() << ">";
    return out;
}

#endif // SLNODE_H