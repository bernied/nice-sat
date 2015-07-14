#ifndef NICESAT_ADTS_LIST_HPP_
#define NICESAT_ADTS_LIST_HPP_

#include <cassert>
#include <stddef.h>
#include <cstdio>

namespace nicesat {
  template<class T>
  struct ListNode {
    ListNode<T>* next;
    T            value;
  };

  template <class T, bool destruct = false>
  class List {
  public:
    List() : _size(0), _head(NULL), _tail(NULL) { }
    
    explicit List(const T& elem) : _size(1), _head(getNewNode()) {
      _tail = _head;
      _head->value = elem;
      _head->next = NULL;
    }
    
    List(const List<T, destruct>& other) : _size(0), _head(NULL), _tail(NULL) {
      append(other);
    }
    
    ~List() {
      clear();
    }
    
    const List& operator=(const List<T, destruct>& rhs) {
      clear();
      append(rhs);
      return *this;
    }
    
    void clear() {
      if (_tail != NULL) {
        if (destruct) {
          iterator i = _head;
          while (i != NULL) {
            i->value.~T();
            i = i->next;
          }
        }
        _tail->next = _freeList;
        _freeList = _head;
        _head = _tail = NULL;
        _size = 0;
      }
    }

    bool operator==(const List& other) const {
      if (_size != other._size) {
        return false;
      }
      
      ListNode<T>* iter1 = _head;
      ListNode<T>* iter2 = other._head;
      
      while (iter1 != NULL && iter2 != NULL) {
        if (iter1->value != iter2->value) {
          return false;
        }
        iter1 = iter1->next; iter2 = iter2->next;
      };
      
      return (iter1 == NULL && iter2 == NULL);
    }

    bool operator !=(const List& other) const {
      return !(*this == other);
    }
    
    int size() const {
      return _size;
    }
    
    bool isEmpty() const {
      return _head == NULL;
    }

    void insertFront(const T& elem) {    
      ListNode<T>* node = getNewNode();
      node->value = elem;
      node->next = _head;
      if (_head == NULL) {
        _tail = node;
      }
      _head = node;
      ++_size;
    }
    
    void insertBack(const T& elem) {
      ListNode<T>* node = getNewNode();
      node->value = elem;
      node->next = NULL;
      if (_tail != NULL) {
        _tail->next = node;
      } else {
        _head = node;
      }
      _tail = node;
      ++_size;
    }

    typedef ListNode<T>* iterator;
    
    iterator first() const { return _head; }
    iterator end()   const { return NULL;  }

    void insertAfter(iterator iter, const T& elem) {
      if (iter == NULL) {
        insertFront(elem);
      } else {
        ListNode<T>* node = getNewNode();
        node->value = elem;
        node->next  = iter->next;
        iter->next  = node;
        ++_size;
      }
    }
    
    void next(iterator& iter) const {
      assert(iter);
      iter = iter->next;
    }

    const T& operator[](iterator iter) const {
      assert(iter);
      return iter->value;
    }
    
    T& operator[](iterator iter) {
      assert(iter);
      return iter->value;
    }
    
    void removeHead() {
      remove(_head);
    }
    
    void remove(iterator& iter, iterator prev) {
      assert(iter);
      assert(prev == NULL || prev->next == iter);

      --_size;
      if (iter == _head) {
        assert(prev == NULL);
        _head = _head->next;
        if (_head == NULL) {
          _tail = NULL;
        }
        freeNode(iter);
        iter = _head;
      } else {
        assert(prev != NULL);
        prev->next = iter->next;
        if (iter == _tail) {
          _tail = prev;
        }
        freeNode(iter);
        iter = prev->next;
      }
    }
    
    void append(iterator iter) {
      if (iter == NULL) return;
      
      if (_tail == NULL) {
        _head = _tail = getNewNode();
        _head->value = iter->value;
        iter = iter->next;
        _size = 1;
      }
      
      // First, we 'allocate' nodes from the freeList
      _tail->next = _freeList;
      while (_tail->next != NULL && iter != NULL) {
        _tail = _tail->next;
        _tail->value = iter->value;
        iter = iter->next;
        ++_size;
      }
      _freeList = _tail->next;
      
      // Then, if we still have more to copy, we use the allocator
      while (iter != NULL) {
        _tail->next = new ListNode<T>;
        _tail = _tail->next;
        _tail->value = iter->value;
        iter = iter->next;
        ++_size;
      }
      
      _tail->next = NULL;
    }

    // Non-destructive append
    void append(const List<T, destruct>& other) {
      if (other._head != NULL) {
        append(other._head);
      }
    }
    
    void appendDestroy(List<T, destruct>& other) {
      if (other._head != NULL) {
        if (_tail != NULL) {
          _tail->next = other._head;
        } else {
          _head = other._head;
        }
        _tail = other._tail;
        _size += other._size;
      }
      other._size = 0;
      other._head = NULL;
      other._tail = NULL;
    }
    
  private:
    int _size;
    ListNode<T>* _head;
    ListNode<T>* _tail;

  private:
    // A custom memory pool based on a linked list of nodes
    static ListNode<T>* _freeList;
    static ListNode<T>* getNewNode() {
      if (_freeList) {
        ListNode<T>* ans = _freeList;
        _freeList = _freeList->next;
        return ans;
      } else {
        return new ListNode<T>;
      }
    }
    
    static inline void freeNode(ListNode<T>* node) {
      if (destruct) node->value.~T();
      node->next = _freeList;
      _freeList = node;
    }
  };
  
  template <class T, bool destruct>
  ListNode<T>* List<T, destruct>::_freeList = NULL;
}

#endif//NICESAT_ADTS_LIST_HPP_
