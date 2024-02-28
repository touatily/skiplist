#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <chrono>
#include "slnode.hpp"

enum orientation {
    VERTICAL = 0,
    HORIZONTAL
};

template<class T, class Compare = std::less<T>, typename TRandom = std::default_random_engine, int MaxLevel=10>
class skiplist {
    std::vector<SLNode<T>*> levels;
    SLNode<T>* last;
    const double prob;
    size_t nb;
    TRandom generator;

public:
    skiplist(double p=0.5);
    template <typename Iterator> skiplist(const Iterator& first_element, const Iterator& last_element, double p=0.5);
    size_t size() const;
    bool exists(const T& e) const;
    void insert(const T& e);
    void sketch(orientation orient=VERTICAL) const;
    void erase(const T& e);
    void print() const;
    bool empty() const;
    unsigned int count(const T& e) const;
    const T& front() const;
    const T& back() const;

    class iterator 
    {
    public:
        iterator(SLNode<T>* c=nullptr): current(c) {}

        const T& operator*() const { return current->get_val(); }
        //pointer operator->() { return m_ptr; }

        iterator& operator++() { current = current->get_next(); return *this; }  
        iterator operator++(int) { iterator tmp = *this; current = current->get_next(); return tmp; }
        iterator& operator--() { current = current->get_prev(); return *this; }  
        iterator operator--(int) { iterator tmp = *this; current = current->get_prev(); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b)  { return a.current == b.current; }
        friend bool operator!= (const iterator& a, const iterator& b)  { return a.current != b.current; }
        friend void skiplist<T, Compare, TRandom, MaxLevel>::erase(iterator it);
    private:
        SLNode<T>* current;
    };

    iterator begin() { return iterator(levels.front()); }
    iterator end() { return iterator(nullptr); }
    iterator rbegin() { return iterator(nullptr); }
    iterator rend() { return iterator(last); }

    class const_iterator 
    {
    public:
        const_iterator(SLNode<T>* c=nullptr): current(c) {}
        const SLNode<T>& operator*() const { return *current; }
        //pointer operator->() { return m_ptr; }

        const_iterator& operator++() { current = current->get_next(); return *this; }  
        const_iterator operator++(int) { const_iterator tmp = *this; current = current->get_next(); return tmp; }
        const_iterator& operator--() { current = current->get_prev(); return *this; }  
        const_iterator operator--(int) { const_iterator tmp = *this; current = current->get_prev(); return tmp; }

        friend bool operator== (const const_iterator& a, const const_iterator& b)  { return a.current == b.current; };
        friend bool operator!= (const const_iterator& a, const const_iterator& b)  { return a.current != b.current; };  
    private:
        SLNode<T>* current;
    };

    const_iterator begin() const { return const_iterator(levels[0]); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator rbegin() const { return const_iterator(nullptr); }
    const_iterator rend() const { return const_iterator(last); }

    void erase(iterator it);
    iterator find(const T& e);
    const_iterator find(const T& e) const;
};


template<class T, class Compare, typename TRandom, int MaxLevel>
void skiplist<T, Compare, TRandom, MaxLevel>::print() const {
    std::cout << "skiplist: ";
    for(auto it=skiplist<T>::begin(); it != skiplist<T>::end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

template<class T, class Compare, typename TRandom, int MaxLevel>
void skiplist<T, Compare, TRandom, MaxLevel>::sketch(orientation orient) const {
    if(empty()) {
        std::cout << "{{ skiplit empty }}" << std::endl;
        return;
    }
    switch(orient) {
    case VERTICAL:
        for(int i=0; i < MaxLevel; i++) {
            auto p = levels[i];
            while(p) {
                std::cout << p->get_val() << " ";
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
                std::cout << q->get_val() << " ";
                q = q->get_up();
            }
            std::cout << std::endl;

            p = p->get_next();
        }
        break;
    }
}


template<class T, class Compare, typename TRandom, int MaxLevel>
skiplist<T, Compare, TRandom, MaxLevel>::skiplist(double p): levels(MaxLevel, nullptr), prob(p), nb(0),
        generator(std::chrono::system_clock::now().time_since_epoch().count()), last(nullptr) {}

template<class T, class Compare, typename TRandom, int MaxLevel>
template <typename Iterator>
skiplist<T, Compare, TRandom, MaxLevel>::skiplist(const Iterator& first_element, const Iterator& last_element, double p): levels(MaxLevel, nullptr), prob(p),
        generator(std::chrono::system_clock::now().time_since_epoch().count()), nb(0) {
    
    std::vector<T*> elements;
    for(auto it=first_element; it != last_element; ++it) {
        elements.push_back(&(*it));
    }

    if(elements.size() == 0) return;
    sort(elements.begin(), elements.end(), [&](T* a, T* b) { return Compare()(*a,  *b); });

    T* e = new T(*elements[0]);
    levels[0] = new SLNode<T>(e);
    nb++;
    for(int i=1; i < MaxLevel; i++) {
        levels[i] = new SLNode<T>(e, nullptr, nullptr, nullptr, levels[i-1]);
        levels[i-1]->set_up(levels[i]);
    }
    std::vector<SLNode<T>*> previous = levels;
    for(int i=1; i < elements.size(); i++) {
        if(*elements[i] != previous[0]->get_val()) {
            e = new T(*elements[i]);
            previous[0] = new SLNode<T>(e, nullptr, previous[0]);
            previous[0]->get_prev()->set_next(previous[0]);
            
            nb++;
            int j=1;
            while(j < MaxLevel && generator() < (generator.max() + generator.min()) * this->prob) {
                previous[j]->set_next(new SLNode<T>(e, nullptr, previous[j], nullptr, previous[j-1]));
                previous[j-1]->set_up(previous[j]->get_next());
                previous[j] = previous[j]->get_next();

                j++;
            }
        }
    }
    last = previous[0];
}

template<class T, class Compare, typename TRandom, int MaxLevel>
size_t skiplist<T, Compare, TRandom, MaxLevel>::size() const {
    return nb;
}

template<class T, class Compare, typename TRandom, int MaxLevel>
bool skiplist<T, Compare, TRandom, MaxLevel>::empty() const {
    return nb == 0;
    // return levels.front() == nullptr;
}

template<class T, class Compare, typename TRandom, int MaxLevel>
bool skiplist<T, Compare, TRandom, MaxLevel>::exists(const T& e) const {
    SLNode<T>* p = levels.back();
    if(p == nullptr) return false;
    if(*(p->val) > e) return false;
    if(*(p->val) == e) return true;

    while(p) {
        while(p->next != nullptr && Compare()(p->get_next()->get_val(), e)) {
            p = p->get_next();
        }
        if(p->next && p->get_next()->get_val() == e) return true;
        p = p->get_down();
    }
    return false;
}

template<class T, class Compare, typename TRandom, int MaxLevel>
void skiplist<T, Compare, TRandom, MaxLevel>::insert(const T& e) {
    if(empty()) {
        T* p = new T(e);

        for(int i=0; i < levels.size(); i++) {
            levels[i] = new SLNode(p);
        }
        for(int i=1; i < levels.size(); i++) {
            levels[i]->set_down(levels[i-1]);
            levels[i-1]->set_up(levels[i]);
        }
        nb++;
        last = levels[0];
    } else if(e == levels.front()->get_val()) {
        return;
    } else {
        if(Compare()(e, levels.front()->get_val())) {
            T* element = new T(e);
            for(int i=0; i < levels.size(); i++) {
                levels[i] = new SLNode(element, levels[i]);
                levels[i]->get_next()->set_prev(levels[i]);
            }
            for(int i=1; i < levels.size(); i++) {
                levels[i]->set_down(levels[i-1]);
                levels[i-1]->set_up(levels[i]);
            }

            SLNode<T>* p = levels.front()->get_next();
            int i = 1;
            while(i < MaxLevel && generator() < (generator.max() + generator.min()) * this->prob) {
                i++;
                p = p->get_up();
            }

            SLNode<T>* tmp = p->get_up();
            p->set_up(nullptr);
            p = tmp;
            while(p) {
                tmp = p->get_up();
                p->get_prev()->set_next(p->get_next());
                if(p->get_next()) p->get_next()->set_prev(p->get_prev());
                delete p;
                p = tmp;
            }
            nb++;
        } else {
            SLNode<T>* p = levels.back();
            bool found = false;

            std::vector<SLNode<T>*> previous = levels;
            int i = MaxLevel - 1;
            while(!found && i>0) {
                while(previous[i]->get_next() && Compare()(previous[i]->get_next()->get_val(), e)) {
                    previous[i] = previous[i]->get_next();
                }
                if(previous[i]->get_next() && previous[i]->get_next()->get_val() == e) {
                    found = true;
                }
                previous[i-1] = previous[i]->get_down();
                i--;
            }
            
            if(found) return;
            assert(previous[0] != nullptr);
            while(previous[0]->get_next() && Compare()(previous[0]->get_next()->get_val(), e)) {
                previous[0] = previous[0]->get_next();
            }
            if(previous[0]->get_next() && previous[0]->get_next()->get_val() == e) return;

            // add to level 0
            T* element = new T(e);
            auto node = new SLNode<T>(element);
            node->set_next(previous[0]->get_next());
            node->set_prev(previous[0]);
            node->set_down(nullptr);
            node->set_up(nullptr);
            if(node->get_next()) node->get_next()->set_prev(node);
            previous[0]->set_next(node);

            if(node->get_next() == nullptr) {
                last = node;
            }

            i = 1;
            SLNode<T>* nDown = node;
            while(i < MaxLevel && generator() < (generator.max() + generator.min()) * this->prob) {
                auto node = new SLNode<T>(element);

                node->set_next(previous[i]->get_next());
                node->set_prev(previous[i]);
                node->set_down(nDown);
                node->set_up(nullptr);
                nDown->set_up(node);
                if(node->get_next()) node->get_next()->set_prev(node);
                previous[i]->set_next(node);

                nDown = node;
                i++;
            }
            nb++;
        }
    }
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
unsigned int skiplist<T, Compare, TRandom, MaxLevel>::count(const T& e) const {
    return (exists(e))? 1 : 0;
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
const T& skiplist<T, Compare, TRandom, MaxLevel>::front() const {
    return levels.front()->get_val();
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
const T& skiplist<T, Compare, TRandom, MaxLevel>::back() const {
    return last->get_val();
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
typename skiplist<T, Compare, TRandom, MaxLevel>::iterator skiplist<T, Compare, TRandom, MaxLevel>::find(const T& e) {
    SLNode<T>* p = levels.back();
    if(p == nullptr) return end();
    if(Compare()(e, p->get_val())) return end();
    if(p->get_val() == e) return begin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_val(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_val() == e) {
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

template<class T, class Compare, typename TRandom, int MaxLevel> 
typename skiplist<T, Compare, TRandom, MaxLevel>::const_iterator skiplist<T, Compare, TRandom, MaxLevel>::find(const T& e) const {
    SLNode<T>* p = levels.back();
    if(p == nullptr) return end();
    if(Compare()(e, p->get_val())) return end();
    if(p->get_val() == e) return begin();

    while(p) {
        while(p->get_next() && Compare()(p->get_next()->get_val(), e)) {
            p = p->get_next();
        }
        if(p->get_next() && p->get_next()->get_val() == e) {
            p = p->get_next();
            while(p->get_down()) {
                p = p->get_down();
            }
            return const_iterator(p);
        }
        p = p->get_down();
    }
    return end();
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
void skiplist<T, Compare, TRandom, MaxLevel>::erase(typename skiplist<T, Compare, TRandom, MaxLevel>::iterator it) {

    if(it != end()) {
        SLNode<T>* p = it.current;
        if(p == last) last = last->get_prev();
        const T*  e = it.current->val;

        if(it == begin()) {
            if(p->get_next() == nullptr) {
                for(int i=0; i < MaxLevel; i++) {
                    delete levels[i];
                    levels[i] = nullptr;
                }
            } else {
                auto q = p->get_next();
                delete levels[0];
                levels[0] = q;
                q->set_prev(nullptr);
                for(int i=1; i < MaxLevel; i++) {
                    if(! q->get_up()) {
                        q->set_up(new SLNode<T>(*q));
                        q->get_up()->set_down(q); 
                        q->get_up()->set_next(levels[i]->get_next());
                    } 
                    q = q->get_up();
                    q->set_prev(nullptr);
                    delete levels[i];
                    levels[i] = q; 
                }
            }
        } else {
            
            while(p) {
                auto tmp = p->get_up();
                p->get_prev()->set_next(p->get_next());
                if(p->get_next()) p->get_next()->set_prev(p->get_prev());
                delete p;
                p = tmp;
            }
        }
        nb--;
        delete e;
    }
}

template<class T, class Compare, typename TRandom, int MaxLevel> 
void skiplist<T, Compare, TRandom, MaxLevel>::erase(const T& e) {
    auto it = find(e);
    erase(it);
}

#endif // SKIPLIST_H