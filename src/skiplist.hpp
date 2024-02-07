#ifndef SKIPLIST_H
#define SKIPLIST_H


#include <functional>
#include <iostream>
#include <vector>
#include <random>
#include <cassert>
#include <chrono>
#include "slnode.hpp"


template<class T, class Compare = std::less<T>, typename TRandom = std::default_random_engine, int MaxLevel=10>
class skiplist {
    std::vector<SLNode<T>*> levels;
    const double prob;
    size_t nb;
    TRandom generator;

public:
    skiplist(double p=0.5);
    //skiplist(const InputIterator& first, const InputIterator& last, double p=0.5);
    size_t size() const;
    bool exists(const T& e) const;
    void insert(const T& e);
    void sketch() const;

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

        const T& operator*() const { return *(current->val); }
        //pointer operator->() { return m_ptr; }

        iterator& operator++() { current = current->next; return *this; }  
        iterator operator++(int) { iterator tmp = *this; current = current->next; return tmp; }

        iterator& operator--() { current = current->prev; return *this; }  
        iterator operator--(int) { iterator tmp = *this; current = current->prev; return tmp; }

        friend bool operator== (const iterator& a, const iterator& b)  { return a.current == b.current; };
        friend bool operator!= (const iterator& a, const iterator& b)  { return a.current != b.current; };
   

    private:
        SLNode<T>* current;
    };


    class const_iterator 
    {
    public:
        const_iterator(SLNode<T>* c=nullptr): current(c) {}

        const SLNode<T>& operator*() const { return *current; }
        //pointer operator->() { return m_ptr; }

        const_iterator& operator++() { current = current->next; return *this; }  
        const_iterator operator++(int) { const_iterator tmp = *this; current = current->next; return tmp; }

        const_iterator& operator--() { current = current->prev; return *this; }  
        const_iterator operator--(int) { const_iterator tmp = *this; current = current->prev; return tmp; }

        friend bool operator== (const const_iterator& a, const const_iterator& b)  { return a.current == b.current; };
        friend bool operator!= (const const_iterator& a, const const_iterator& b)  { return a.current != b.current; };

        /*
        friend bool operator< (const const_iterator& a, const const_iterator& b)  { return *(a.current->val) < *(b.current->val); };     
        friend bool operator> (const const_iterator& a, const const_iterator& b)  { return *(a.current->val) > *(b.current->val); };  
        friend bool operator<= (const const_iterator& a, const const_iterator& b)  { return *(a.current->val) <= *(b.current->val); };
        friend bool operator>= (const const_iterator& a, const const_iterator& b)  { return *(a.current->val) >= *(b.current->val); };         
        */    

    private:
        SLNode<T>* current;
    };

    const_iterator begin() const {
        return const_iterator(levels[0]);
    }
    const_iterator end() const {
        return const_iterator(nullptr);
    }

    const_iterator rbegin() const {
        return const_iterator(nullptr);
    }
    const_iterator rend() const {
        return const_iterator(nullptr);
    }

    void erase(iterator& it);
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
void skiplist<T, Compare, TRandom, MaxLevel>::sketch() const {
    for(int i=0; i < levels.size(); i++) {
        auto p = levels[i];

        while(p) {
            std::cout << *(p->val) << " ";
            p = p->next;
        }
        std::cout << std::endl;
    }
}


template<class T, class Compare, typename TRandom, int MaxLevel>
skiplist<T, Compare, TRandom, MaxLevel>::skiplist(double p): levels(MaxLevel, nullptr), prob(p), nb(0),
        generator(std::chrono::system_clock::now().time_since_epoch().count()) {}


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

    while(p) {
        while(p->next != nullptr && *(p->next->val) < e) {
            p = p->next;
        }
        if(p->next && *(p->next->val) == e) return true;
        p = p->down;
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
            levels[i]->down = levels[i-1];
            levels[i-1]->up = levels[i];
        }
        nb++;
    } else {
        if(e < *(levels.front()->val)) {
            const T* tmp = levels.front()->val;
            T* p = new T(e);

            for(int i=0; i < levels.size(); i++) {
                levels[i]->val = p; 
            }

            insert(*tmp);
            delete tmp;
        } else {
            SLNode<T>* p = levels.back();
            bool found = false;

            std::vector<SLNode<T>*> previous = levels;
            int i = MaxLevel - 1;
            while(!found && i>0) {
                while(previous[i]->next && *(previous[i]->next->val) < e) {
                    previous[i] = previous[i]->next;
                }
                if(previous[i]->next && *(previous[i]->next->val) == e) {
                    found = true;
                }
                previous[i-1] = previous[i]->down;
                i--;
            }
            
            if(found) return;

            assert(previous[0] != nullptr);

            while(previous[0]->next && *(previous[0]->next->val) < e) {
                previous[0] = previous[0]->next;
            }
            if(previous[0]->next && *(previous[0]->next->val) == e) return;

            // add to level 0
            T* element = new T(e);
            auto node = new SLNode<T>(element);
            node->next = previous[0]->next;
            node->prev = previous[0];
            node->down = nullptr;
            node->up = nullptr;
            if(node->next) node->next->prev = node;
            previous[0]->next = node;

            i = 1;
            SLNode<T>* nDown = node;
            while(i < MaxLevel && generator() < (generator.max() + generator.min()) * this->prob) {
                auto node = new SLNode<T>(element);

                node->next = previous[i]->next;
                node->prev = previous[i];
                node->down = nDown;
                node->up = nullptr;

                nDown->up = node;
                if(node->next) node->next->prev = node;
                previous[i]->next = node;

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

#endif // SKIPLIST_H