#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <chrono>
#include <utility>
#include "slnode.hpp"
#include "skiplist_exceptions.hpp"

enum orientation {
    VERTICAL = 0,
    HORIZONTAL
};



template<class K, class V, class Compare=std::less<K>, typename TRandom=std::default_random_engine, int MaxLevel=10>
class skiplist {
    std::vector<SLNode<K, V>*> levels;
    SLNode<K, V>* last;
    double prob;
    size_t nb;
    TRandom generator;

public:
    typedef std::pair<const K* const, V* const> value_type;
    class iterator;
    class const_iterator;

    skiplist(double p=0.5);
    template <class Iterator> skiplist(const Iterator& first_element, const Iterator& last_element, double p=0.5);
    skiplist(const skiplist<K, V, Compare, TRandom, MaxLevel>& sk);
    skiplist<K, V, Compare, TRandom, MaxLevel>& operator=(const skiplist<K, V, Compare, TRandom, MaxLevel>& sk);

    ~skiplist();
    size_t size() const;
    void clear();
    double get_prob() { return prob; }
    bool exists(const K& e) const;

    std::pair<iterator, bool> insert(K k, V v) {
        value_type kv = make_pair(&k, &v);
        return insert(kv);
    }
    std::pair<iterator, bool> insert(const value_type& p);
    std::pair<iterator, bool> insert(iterator& it, const value_type& p);
    template <class InputIterator> void insert (InputIterator first, InputIterator last);

    void sketch(orientation orient=VERTICAL) const;
    
    void print() const;
    bool empty() const;
    unsigned int count(const K& e) const;
    const K& front() const;
    const K& back() const;

    V& operator[](const K& k);
    const V& at(const K& k) const;
    V& at(const K& k);

    iterator begin() { return iterator(levels.front()); }
    iterator end() { return iterator(nullptr); }
    iterator rbegin() { return iterator(nullptr); }
    iterator rend() { return iterator(last); }

    const_iterator cbegin() const { return const_iterator(levels.front()); }
    const_iterator cend() const { return const_iterator(nullptr); }
    const_iterator crbegin() const { return const_iterator(nullptr); }
    const_iterator crend() const { return const_iterator(last); }

    size_t erase(const K& e);
    void erase(iterator it);
    void erase (iterator first_element, iterator last_element);
    iterator find(const K& e);
    const_iterator find(const K& e) const;

    iterator lower_bound(const K& e);
    const_iterator lower_bound(const K& e) const;
    iterator upper_bound(const K& e);
    const_iterator upper_bound(const K& e) const;
    

    class iterator : public std::iterator< std::bidirectional_iterator_tag, value_type>
    {
    public:
        iterator(SLNode<K, V>* c=nullptr): current(c) {}

        const value_type& operator*() const { return current->get_key_value(); }
        const value_type* operator->() const { return &(current->get_key_value()); }

        iterator& operator++() { current = current->get_next(); return *this; }  
        iterator operator++(int) { iterator tmp = *this; current = current->get_next(); return tmp; }
        iterator& operator--() { current = current->get_prev(); return *this; }  
        iterator operator--(int) { iterator tmp = *this; current = current->get_prev(); return tmp; }
        iterator operator+(int n) {
            auto it = *this;
            while(it.current && n > 0) {
                it.current = it.current->get_next();
                n--;
            }
            return it;
        }
        iterator operator-(int n) {
            auto it = *this;
            while(it.current && n > 0) {
                it.current = it.current->get_prev();
                n--;
            }
            return it;
        } 

        friend bool operator== (const iterator& a, const iterator& b)  { return a.current == b.current; }
        friend bool operator!= (const iterator& a, const iterator& b)  { return a.current != b.current; }
        friend void skiplist<K, V, Compare, TRandom, MaxLevel>::erase(iterator it);
    private:
        SLNode<K, V>* current;

        friend const_iterator::const_iterator(const iterator& it);
    };

    class const_iterator : public std::iterator< std::bidirectional_iterator_tag, value_type>
    {
    public:
        const_iterator(SLNode<K, V>* c=nullptr): current(c) {}
        const_iterator(const iterator& it): current(it.current) {}
        const value_type& operator*() const { return current->get_key_value(); }
        const value_type* const operator->() const { return &(current->get_key_value()); }

        const_iterator& operator++() { current = current->get_next(); return *this; }  
        const_iterator operator++(int) { const_iterator tmp = *this; current = current->get_next(); return tmp; }
        const_iterator& operator--() { current = current->get_prev(); return *this; }  
        const_iterator operator--(int) { const_iterator tmp = *this; current = current->get_prev(); return tmp; }
        const_iterator operator+(int n) {
            auto it = *this;
            while(it.current && n > 0) {
                it.current = it.current->get_next();
                n--;
            }
            return it;
        }
        const_iterator operator-(int n) {
            auto it = *this;
            while(it.current && n > 0) {
                it.current = it.current->get_prev();
                n--;
            }
            return it;
        } 

        friend bool operator== (const const_iterator& a, const const_iterator& b)  { return a.current == b.current; };
        friend bool operator!= (const const_iterator& a, const const_iterator& b)  { return a.current != b.current; };  
    private:
        SLNode<K, V>* current;
    };
};


template<class K, class V, class Compare, typename TRandom, int MaxLevel>
void skiplist<K, V, Compare, TRandom, MaxLevel>::print() const {
    std::cout << "skiplist: ";
    for(auto it=cbegin(); it != cend(); ++it) {
        std::cout << *(it->first) << " ";
    }
    std::cout << std::endl;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
void skiplist<K, V, Compare, TRandom, MaxLevel>::sketch(orientation orient) const {
    if(empty()) {
        std::cout << "{{ skiplit empty }}" << std::endl;
        return;
    }
    switch(orient) {
    case VERTICAL:
        for(int i=0; i < MaxLevel; i++) {
            auto p = levels[i];
            while(p) {
                std::cout << p->get_key() << " ";
                p = p->get_next();
            }
            std::cout << std::endl;
        }
        break;
    case HORIZONTAL:
        auto p = levels.front();
        while(p) {
            auto q = p;
            while(q) {
                std::cout << q->get_key() << " ";
                q = q->get_up();
            }
            std::cout << std::endl;

            p = p->get_next();
        }
        break;
    }
}


template<class K, class V, class Compare, typename TRandom, int MaxLevel>
skiplist<K, V, Compare, TRandom, MaxLevel>::skiplist(double p): levels(MaxLevel, nullptr), prob(p), nb(0),
        generator(std::chrono::system_clock::now().time_since_epoch().count()), last(nullptr) {}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
template <typename Iterator>
skiplist<K, V, Compare, TRandom, MaxLevel>::skiplist(const Iterator& first_element, const Iterator& last_element, double p): levels(MaxLevel, nullptr), prob(p),
        generator(std::chrono::system_clock::now().time_since_epoch().count()), nb(0) {
    insert(first_element, last_element);
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
skiplist<K, V, Compare, TRandom, MaxLevel>::skiplist(const skiplist<K, V, Compare, TRandom, MaxLevel>& sk): skiplist(sk.cbegin(), sk.cend(), sk.prob) {}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
skiplist<K, V, Compare, TRandom, MaxLevel>& skiplist<K, V, Compare, TRandom, MaxLevel>::operator=(const skiplist<K, V, Compare, TRandom, MaxLevel>& sk) {
    if(this != &sk) {
        clear();
        prob = sk.prob;
        insert(sk.cbegin(), sk.cend());
    }
    return *this;
}


template<class K, class V, class Compare, typename TRandom, int MaxLevel>
void skiplist<K, V, Compare, TRandom, MaxLevel>::clear() {
    SLNode<K, V>* p = levels.front();
    const K* k; V* v;
    while(p) {
        SLNode<K, V>* q = p;
        p = p->get_next();
        std::tie(k, v) = q->get_key_value();
        while(q) {
            SLNode<K, V>* tmp = q->get_up();
            delete q;
            q = tmp;
        }
        delete k;
        delete v;
    }
    for(int i=0; i < levels.size(); i++) {
        levels[i] = nullptr;
    }
    last = nullptr;
    nb = 0;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
skiplist<K, V, Compare, TRandom, MaxLevel>::~skiplist() {
    clear();
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
size_t skiplist<K, V, Compare, TRandom, MaxLevel>::size() const {
    return nb;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
bool skiplist<K, V, Compare, TRandom, MaxLevel>::empty() const {
    return nb == 0;
    // return levels.front() == nullptr;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
bool skiplist<K, V, Compare, TRandom, MaxLevel>::exists(const K& e) const {
    if(empty()) return false;
    SLNode<K, V>* p = levels.back();
    if(Compare()(e, p->get_val())) return false;
    if(p->get_val() == e) return true;

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_val(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_val() == e) return true;
        p = p->get_down();
    }
    return false;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
std::pair<typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator, bool> skiplist<K, V, Compare, TRandom, MaxLevel>::insert(const value_type& p) {
    if(empty()) {
        const K* const k = new K(*p.first);
        V* v = new V(*p.second);

        levels[0] = new SLNode<K, V>(k, v);
        for(int i=1; i < levels.size(); i++) {
            levels[i] = new SLNode<K, V>(k, v);
            levels[i]->set_down(levels[i-1]);
            levels[i-1]->set_up(levels[i]);
        }
        nb++;
        last = levels.front();
        return {begin(), true};
    } else if(*p.first == levels.front()->get_key()) {
        return {begin(), false};
    } else if(Compare()(*p.first, levels.front()->get_key())) {
        const K* const k = new K(*p.first);
        V* v = new V(*p.second);

        levels[0] = new SLNode<K, V>(k, v, levels[0]);
        levels[0]->get_next()->set_prev(levels[0]);
        for(int i=1; i < levels.size(); i++) {
            levels[i] = new SLNode<K, V>(k, v, levels[i]);
            levels[i]->get_next()->set_prev(levels[i]);
            levels[i]->set_down(levels[i-1]);
            levels[i-1]->set_up(levels[i]);
        }

        SLNode<K, V>* next = levels.front()->get_next(), *tmp = nullptr;
        while(next->get_up() && generator() < (generator.max() + generator.min()) * this->prob) {
            next = next->get_up();
        }

        tmp = next->get_up();
        next->set_up(nullptr);
        next = tmp;
        while(next) {
            tmp = next->get_up();
            next->get_prev()->set_next(next->get_next());
            if(next->get_next()) next->get_next()->set_prev(next->get_prev());
            delete next;
            next = tmp;
        }
        nb++;
        return {begin(), true};
    } else {
        std::vector<SLNode<K, V>*> previous = levels;
        int i = MaxLevel - 1;
        while(i>0) {
            while(previous[i]->get_next() && Compare()(previous[i]->get_next()->get_key(), *p.first)) {
                previous[i] = previous[i]->get_next();
            }
            if(previous[i]->get_next() && previous[i]->get_next()->get_key() == *p.first) {
                return {previous[i]->get_next(), false};
            }
            previous[i-1] = previous[i]->get_down();
            i--;
        }
        
        while(previous[0]->get_next() && Compare()(previous[0]->get_next()->get_key(), *p.first)) {
            previous[0] = previous[0]->get_next();
        }
        if(previous[0]->get_next() && previous[0]->get_next()->get_key() == *p.first) return {previous[0]->get_next(), false};

        // add to level 0
        const K* const k = new K(*p.first);
        V* v = new V(*p.second);
        auto node = new SLNode<K, V>(k, v, previous[0]->get_next(), previous[0]);
        if(node->get_next()) node->get_next()->set_prev(node);
        previous[0]->set_next(node);

        if(node->get_next() == nullptr) {
            last = node;
        }

        i = 1;
        SLNode<K, V>* nDown = node;
        while(i < MaxLevel && generator() < (generator.max() + generator.min()) * this->prob) {
            auto node = new SLNode<K, V>(k, v, previous[i]->get_next(), previous[i], nullptr, nDown);
            nDown->set_up(node);
            if(node->get_next()) node->get_next()->set_prev(node);
            previous[i]->set_next(node);

            nDown = node;
            i++;
        }
        nb++;
        return {iterator(node), true};
    }
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
std::pair<typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator, bool> skiplist<K, V, Compare, TRandom, MaxLevel>::insert(typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator& it, const value_type& p) {
    // temporary version
    return insert(p);
}


template<class K, class V, class Compare, typename TRandom, int MaxLevel>
template <class InputIterator> 
void skiplist<K, V, Compare, TRandom, MaxLevel>::insert (InputIterator first_element, InputIterator last_element) {
    for(auto it=first_element; it != last_element; ++it) {
        insert({it->first, it->second});      
    }
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
unsigned int skiplist<K, V, Compare, TRandom, MaxLevel>::count(const K& e) const {
    return (exists(e))? 1 : 0;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
const K& skiplist<K, V, Compare, TRandom, MaxLevel>::front() const {
    if(empty()) throw SkiplistException("Calling front method on an empty skiplist");
    return levels.front()->get_key();
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
const K& skiplist<K, V, Compare, TRandom, MaxLevel>::back() const {
    if(empty()) throw SkiplistException("Calling back method on an empty skiplist");
    return last->get_key();
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel> 
typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator skiplist<K, V, Compare, TRandom, MaxLevel>::find(const K& e) {
    if(empty()) return end();
    SLNode<K, V>* p = levels.back();
    if(Compare()(e, p->get_key())) return end();
    if(p->get_key() == e) return begin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_key(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_key() == e) {
            p = p->get_next();
            while(p->get_down()) {
                p = p->get_down();
            }
            return iterator(p);
        }
        p = p->get_down();
    }
    return end();
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
typename skiplist<K, V, Compare, TRandom, MaxLevel>::const_iterator skiplist<K, V, Compare, TRandom, MaxLevel>::find(const K& e) const {
    if(empty()) return cend();
    SLNode<K, V>* p = levels.back();
    if(Compare()(e, p->get_key())) return cend();
    if(p->get_key() == e) return cbegin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_key(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_key() == e) {
            p = p->get_next();
            while(p->get_down()) {
                p = p->get_down();
            }
            return const_iterator(p);
        }
        p = p->get_down();
    }
    return cend();
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
void skiplist<K, V, Compare, TRandom, MaxLevel>::erase(typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator it) {
    if(it != end()) {
        SLNode<K, V>* p = it.current;
        if(p == last) last = last->get_prev();
        const K* k = it->first;
        V* v = it->second;

        if(it == begin()) {
            SLNode<K, V>* q = p->get_next();
            if(q == nullptr) {
                for(int i=0; i < MaxLevel; i++) {
                    delete levels[i];
                    levels[i] = nullptr;
                }
            } else {
                delete levels[0];
                levels[0] = q;
                q->set_prev(nullptr);
                for(int i=1; i < MaxLevel; i++) {
                    if(! q->get_up()) {
                        q->set_up(new SLNode<K, V>(q->kv.first, q->kv.second, levels[i]->get_next(), nullptr, nullptr, q));
                        if(levels[i]->get_next()) {
                            levels[i]->get_next()->set_prev(q->get_up());
                        }
                    } 
                    q = q->get_up();
                    q->set_prev(nullptr);
                    delete levels[i];
                    levels[i] = q; 
                }
            }
        } else {
            while(p) {
                SLNode<K, V>* tmp = p->get_up();
                p->get_prev()->set_next(p->get_next());
                if(p->get_next()) p->get_next()->set_prev(p->get_prev());
                delete p;
                p = tmp;
            }
        }
        nb--;
        delete k;
        delete v;
    }
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
size_t skiplist<K, V, Compare, TRandom, MaxLevel>::erase(const K& e) {
    auto it = find(e);
    size_t ans = (it == end())? 0:1;
    erase(it);
    return ans;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
void skiplist<K, V, Compare, TRandom, MaxLevel>::erase(typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator first_element, typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator last_element){
    //for(auto it = first_element; it != last_element; ++it) {
    //    erase(it);
    //}
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator skiplist<K, V, Compare, TRandom, MaxLevel>::lower_bound(const K& e) {
    if(empty()) return end();
    SLNode<K, V>* p = levels.back(), *q = nullptr;
    if(Compare()(e, p->get_key())) return begin();
    if(p->get_key() == e) return begin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_key(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_key() == e) {
            p = p->get_next();
            while(p->get_down()) {
                p = p->get_down();
            }
            return iterator(p);
        }
        q = p;
        p = p->get_down();
    }
    return iterator(q->get_next());
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
typename skiplist<K, V, Compare, TRandom, MaxLevel>::const_iterator skiplist<K, V, Compare, TRandom, MaxLevel>::lower_bound(const K& e) const {
    if(empty()) return cend();
    SLNode<K, V>* p = levels.back(), *q = nullptr;
    if(Compare()(e, p->get_key())) return cbegin();
    if(p->get_key() == e) return cbegin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_key(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_key() == e) {
            p = p->get_next();
            while(p->get_down()) {
                p = p->get_down();
            }
            return const_iterator(p);
        }
        q = p;
        p = p->get_down();
    }
    return const_iterator(q->get_next());
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
typename skiplist<K, V, Compare, TRandom, MaxLevel>::iterator skiplist<K, V, Compare, TRandom, MaxLevel>::upper_bound(const K& e) {
    skiplist<K, V, Compare, TRandom, MaxLevel>::iterator it = lower_bound(e);
    if(it != end() && *it->first == e) {
        ++it;
    }
    return it;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
typename skiplist<K, V, Compare, TRandom, MaxLevel>::const_iterator skiplist<K, V, Compare, TRandom, MaxLevel>::upper_bound(const K& e) const {
    skiplist<K, V, Compare, TRandom, MaxLevel>::const_iterator it = lower_bound(e);
    if(it != cend() && *it->first == e) {
        ++it;
    }
    return it;
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
V& skiplist<K, V, Compare, TRandom, MaxLevel>::operator[](const K& k) {
    return *((insert(k, V())).first->second);
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
const V& skiplist<K, V, Compare, TRandom, MaxLevel>::at(const K& k) const {
    auto it = find(k);
    if(it == end()) {
        throw SLNodeException("Key doesn't exist in skiplist");
    } else {
        return *(it->second);
    }
}

template<class K, class V, class Compare, typename TRandom, int MaxLevel>
V& skiplist<K, V, Compare, TRandom, MaxLevel>::at(const K& k) {
    auto it = find(k);
    if(it == end()) {
        throw SLNodeException("Key doesn't exist in skiplist");
    } else {
        return *(it->second);
    }
}

#endif // SKIPLIST_H