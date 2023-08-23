#pragma once
#include <iostream>
#include <cstdlib>

template <typename T, typename Allocator = std::allocator<T>>
class List {
  private:

  struct BaseNode {
    BaseNode* previous = this;
    BaseNode* next = this;
  };

  struct Node : public BaseNode {
    T value;
  };

  BaseNode fake_node_ = BaseNode();
  size_t size_ = 0;

  using allocator_node = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
  using alloc_traits = std::allocator_traits<allocator_node>;

  allocator_node alloc_ = allocator_node();

  void swap(List& other);

  public:

  template <bool IsConst>
  class CommonIterator {
    private:
    Node* ptr_;
    std::ptrdiff_t distance_to_first_node_;

    public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = std::conditional_t<IsConst, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference	= value_type&;

    CommonIterator() noexcept = default;
    CommonIterator(Node* ptr, size_t distance_to_first_node) noexcept : ptr_(ptr), distance_to_first_node_(distance_to_first_node) {}
    CommonIterator(const CommonIterator& other) noexcept = default;
    ~CommonIterator() noexcept = default;
    CommonIterator& operator=(const CommonIterator& other) noexcept = default;

    CommonIterator& operator++() noexcept {
      ++distance_to_first_node_;
      ptr_ = static_cast<Node*>(ptr_ -> next);
      return *this;
    }

    CommonIterator& operator--() noexcept {
      --distance_to_first_node_;
      ptr_ = static_cast<Node*>(ptr_ -> previous);
      return *this;
    }

    CommonIterator operator++(int) noexcept {
      CommonIterator tmp = *this;
      ++distance_to_first_node_;
      ptr_ = static_cast<Node*>(ptr_ -> next);
      return tmp;
    }

    CommonIterator operator--(int) noexcept {
      CommonIterator tmp = *this;
      --distance_to_first_node_;
      ptr_ = static_cast<Node*>(ptr_ -> previous);
      return tmp;
    }

    reference operator*() noexcept {
      return ptr_ -> value;
    }

    pointer operator->() noexcept {
      return &(ptr_ -> value);
    }

    difference_type get_distance() const noexcept {
      return distance_to_first_node_;
    }

    template <bool Bool>
    difference_type operator-(const CommonIterator<Bool>& iter) const noexcept {
      return distance_to_first_node_ - iter.get_distance();
    }

    template <bool Bool>
    bool operator!=(const CommonIterator<Bool>& iter) const noexcept {
      return  (*this - iter) != 0;
    }

    template <bool Bool>
    bool operator==(const CommonIterator<Bool>& iter) const noexcept {
      return (*this - iter) == 0;
    }

    template <bool Bool>
    bool operator<(const CommonIterator<Bool>& iter) const noexcept {
      return (*this - iter) < 0;
    }

    template <bool Bool>
    bool operator>(const CommonIterator<Bool>& iter) const noexcept {
      return iter < *this;
    }

    template <bool Bool>
    bool operator<=(const CommonIterator<Bool>& iter) const noexcept {
      return !(*this > iter);
    }

    template <bool Bool>
    bool operator>=(const CommonIterator<Bool>& iter) const noexcept {
      return !(*this < iter);
    }

    operator CommonIterator<true>() {
      return CommonIterator<true>(ptr_, distance_to_first_node_);
    }
  };

  using value_type = T;
  using allocator_type = Allocator;
  using reference = T&;
  using const_reference = const T&;

  using reverse_iterator = std::reverse_iterator<CommonIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<CommonIterator<true>>;
  using const_iterator = CommonIterator<true>;
  using iterator = CommonIterator<false>;

  List();
  List(size_t count, const T& value, const Allocator& alloc = Allocator());
  explicit List(size_t count, const Allocator& alloc = Allocator());
  List(const List& other);
  List(std::initializer_list<T> init, const Allocator& alloc = Allocator());

  List& operator=(const List& other);
  ~List() noexcept;

  T& front() noexcept;
  const T& front() const noexcept;
  T& back() noexcept;
  const T& back() const noexcept;

  void push_back(const T& value);
  void pop_back();
  void push_front(const T& value);
  void pop_front();


  bool empty() const noexcept;
  size_t size() const noexcept;

  iterator begin() noexcept;
  const_iterator begin() const;

  iterator end() noexcept;
  const_iterator end() const;

  const_iterator cbegin() const;
  const_iterator cend() const;


  reverse_iterator rbegin() noexcept;
  const_reverse_iterator rbegin() const;

  reverse_iterator rend() noexcept;
  const_reverse_iterator rend() const;

  const_reverse_iterator rcbegin() const;
  const_reverse_iterator rcend() const;

  Allocator get_allocator() const noexcept;
};

template <typename T, typename Allocator>
List<T, Allocator>::List() = default;

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const T& value, const Allocator& alloc) : size_(count) , alloc_(alloc) {
  BaseNode* p_previous = &fake_node_;
  Node* p_now = nullptr;
  for (size_t i = 0; i < size_; ++i)  {
    p_now = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, p_now, nullptr, nullptr, value);
    } catch (...) {
      alloc_traits::deallocate(alloc_, p_now, 1);
      Node* tmp = static_cast<Node*>(fake_node_.next);
      for (size_t j = 0; j < i; ++j) {
        if (i == j + 1) {
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp), 1);
        } else {
          tmp = static_cast<Node*>(tmp -> next);
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp -> previous));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp -> previous), 1);
        }
      }
      throw;
    }
    (p_previous -> next) = p_now;
    (p_now -> previous) = p_previous;
    p_previous = p_now;
  }
  fake_node_.previous = (size_ != 0) ? p_now : &fake_node_;
  if (size_ != 0) {
    (p_now -> next) = &fake_node_;
  } else {
    fake_node_.next = &fake_node_;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t count, const Allocator& alloc) : size_(count), alloc_(alloc) {
  BaseNode* p_previous = &fake_node_;
  Node* p_now = nullptr;
  for (size_t i = 0; i < size_; ++i)  {
    p_now = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, p_now, nullptr, nullptr);
    } catch (...) {
      alloc_traits::deallocate(alloc_, p_now, 1);
      Node* tmp = static_cast<Node*>(fake_node_.next);
      for (size_t j = 0; j < i; ++j) {
        if (i == j + 1) {
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp), 1);
        } else {
          tmp = static_cast<Node*>(tmp -> next);
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp -> previous));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp -> previous), 1);
        }
      }
      throw;
    }
    (p_previous -> next) = p_now;
    (p_now -> previous) = p_previous;
    p_previous = p_now;
  }
  fake_node_.previous = (size_ != 0) ? p_now : &fake_node_;
  if (size_ != 0) {
    (p_now -> next) = &fake_node_;
  } else {
    fake_node_.next = &fake_node_;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc) : size_(init.size()), alloc_(alloc) {
  BaseNode* p_previous = &fake_node_;
  Node* p_now;
  typename std::initializer_list<T>::const_iterator iter_creat = init.begin();
  while (iter_creat != init.end()) {
    p_now = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, p_now, nullptr, nullptr, *iter_creat);
    } catch (...) {
      alloc_traits::deallocate(alloc_, p_now, 1);
      Node* tmp = static_cast<Node*>(fake_node_.next);
      typename std::initializer_list<T>::const_iterator iter_destroy = init.begin();
      while (iter_creat != iter_destroy) {
        if (--iter_creat != iter_destroy) {
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp), 1);
        } else {
          tmp = static_cast<Node*>(tmp -> next);
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp -> previous));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp -> previous), 1);
        }
        ++iter_destroy;
      }
      throw;
    }
    (p_previous -> next) = p_now;
    (p_now -> previous) = p_previous;
    p_previous = p_now;
    ++iter_creat;
  }
  fake_node_.previous = (size_ != 0) ? p_now : &fake_node_;
  if (size_ != 0) {
    (p_now -> next) = &fake_node_;
  } else {
    fake_node_.next = &fake_node_;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() noexcept {
  if (size_ != 0) {
    Node* tmp = static_cast<Node*>(fake_node_.next);
    while (tmp != &fake_node_) {
      tmp = static_cast<Node*>(tmp -> next);
      alloc_traits::destroy(alloc_, static_cast<Node*>(tmp -> previous));
      alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp -> previous), 1);
    }
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List<T, Allocator>& other) {
  alloc_ = alloc_traits::select_on_container_copy_construction(other.get_allocator());
  size_ = other.size_;
  BaseNode* p_previous = &fake_node_;
  Node* p_now;
  Node* p_node_other = static_cast<Node*>(other.fake_node_.next);
  for (size_t i = 0; i < size_; ++i)  {
    p_now = alloc_traits::allocate(alloc_, 1);
    try {
      alloc_traits::construct(alloc_, p_now, nullptr, nullptr, p_node_other -> value);
    } catch (...) {
      alloc_traits::deallocate(alloc_, p_now, 1);
      Node* tmp = static_cast<Node*>(fake_node_.next);
      for (size_t j = 0; j < i; ++j) {
        if (i == j + 1) {
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp), 1); 
        } else {
          tmp = static_cast<Node*>(tmp -> next);
          alloc_traits::destroy(alloc_, static_cast<Node*>(tmp -> previous));
          alloc_traits::deallocate(alloc_, static_cast<Node*>(tmp -> previous), 1);
        }
      }
      throw;
    }
    p_node_other = static_cast<Node*>(p_node_other -> next);

    (p_previous -> next) = p_now;
    (p_now -> previous) = p_previous;
    p_previous = p_now;
  }
  fake_node_.previous = (other.size_ != 0) ? p_now : &fake_node_;
  if (other.size_ != 0) {
    (p_now -> next) = &fake_node_;
  } else {
    fake_node_.next = &fake_node_;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::swap(List<T, Allocator>& other) {
  if (other.size_ != 0) {
    (other.fake_node_.previous) -> next = &fake_node_;
    (other.fake_node_.next) -> previous = &fake_node_;
  }
  if (size_ != 0) {
    (fake_node_.next) -> previous = &other.fake_node_;
    (fake_node_.previous) -> next = &other.fake_node_;
  }
  BaseNode* tmp1 = (size_ != 0) ? fake_node_.next : &other.fake_node_;
  BaseNode* tmp2 = (size_ != 0) ? fake_node_.previous : &other.fake_node_;
  fake_node_.next = (other.size_ != 0) ? other.fake_node_.next : &fake_node_;
  fake_node_.previous = (other.size_ != 0) ? other.fake_node_.previous : &fake_node_;
  other.fake_node_.next = tmp1;
  other.fake_node_.previous = tmp2;
  std::swap(size_, other.size_);
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List<T, Allocator>& other) {
  List<T, Allocator> tmp = other;
  tmp.alloc_ = alloc_;
  if constexpr (alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = other.get_allocator();
  }
  swap(tmp);
  return *this;
}

template <typename T, typename Allocator>
Allocator List<T, Allocator>::get_allocator() const noexcept {
  return alloc_;
}

template <typename T, typename Allocator>
bool List<T, Allocator>::empty() const noexcept { return size_ == 0; }
template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const noexcept { return size_; }


template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() noexcept {
  Node* tmp = static_cast<Node*>(fake_node_.next);
  return iterator(tmp, 0);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
  Node* tmp = static_cast<Node*>(fake_node_.next);
  return const_iterator(tmp, 0);
}


template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() noexcept {
  Node* tmp = static_cast<Node*>(&fake_node_);
  return iterator(tmp, size_);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
  Node* tmp = static_cast<Node*>((const_cast<BaseNode*>(&fake_node_)));
  return const_iterator(tmp, size_);
}


template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
  Node* tmp = static_cast<Node*>(fake_node_.next);
  return const_iterator(tmp, 0);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
  Node* tmp = static_cast<Node*>((const_cast<BaseNode*>(&fake_node_)));
  return const_iterator(tmp, size_);
}


template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() noexcept {
  List<T, Allocator>::iterator iter = this -> end();
  return reverse_iterator(iter);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin() const {
  List<T, Allocator>::const_iterator iter = this -> end();
  return const_reverse_iterator(iter);
}


template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() noexcept {
  List<T, Allocator>::iterator iter = this -> begin();
  return reverse_iterator(iter);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend() const {
  List<T, Allocator>::const_iterator iter = this -> begin();
  return const_reverse_iterator(iter);
}


template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rcbegin() const {
  List<T, Allocator>::const_iterator iter = this -> end();
  return const_reverse_iterator(iter);
}
template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rcend() const {
  List<T, Allocator>::const_iterator iter = this -> begin();
  return const_reverse_iterator(iter);
}


template <typename T, typename Allocator>
T& List<T, Allocator>::front() noexcept {
  return *(begin());
}
template <typename T, typename Allocator>
const T& List<T, Allocator>::front() const noexcept {
  return *(cbegin());
}

template <typename T, typename Allocator>
T& List<T, Allocator>::back() noexcept {
  return *(--end());
}
template <typename T, typename Allocator>
const T& List<T, Allocator>::back() const noexcept {
  return *(--cend());
}


template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  Node* new_node = alloc_traits::allocate(alloc_, 1);
  try {
    alloc_traits::construct(alloc_, new_node, nullptr, nullptr, value);
  } catch(...) {
    alloc_traits::deallocate(alloc_, new_node, 1);
    throw;
  }
  Node* last_element = static_cast<Node*>(fake_node_.previous);
  new_node -> next = static_cast<Node*>(&fake_node_);
  new_node -> previous = last_element;
  last_element -> next = new_node;
  fake_node_.previous = new_node;
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  Node* last_element = static_cast<Node*>(fake_node_.previous);
  Node* tmp = static_cast<Node*>(last_element -> previous);
  tmp -> next = static_cast<Node*>(&fake_node_);
  fake_node_.previous = tmp;
  alloc_traits::destroy(alloc_, last_element);
  alloc_traits::deallocate(alloc_, last_element, 1);
  --size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  Node* new_node = alloc_traits::allocate(alloc_, 1);
  try {
    alloc_traits::construct(alloc_, new_node,  nullptr, nullptr, value);
  } catch(...) {
    alloc_traits::deallocate(alloc_, new_node, 1);
    throw;
  }
  Node* first_element = static_cast<Node*>(fake_node_.next);
  first_element -> previous = new_node;
  fake_node_.next = new_node;
  new_node -> previous = static_cast<Node*>(&fake_node_);
  new_node -> next = first_element;
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  Node* first_element = static_cast<Node*>(fake_node_.next);
  Node* tmp = static_cast<Node*>(first_element -> next);
  tmp -> previous = static_cast<Node*>(&fake_node_);
  fake_node_.next = tmp;
  alloc_traits::destroy(alloc_, first_element);
  alloc_traits::deallocate(alloc_, first_element, 1);
  --size_;
}
