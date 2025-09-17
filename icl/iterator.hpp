#pragma once

#include <iterator>

namespace icl {

/** \brief Performs an addition using a container's member function add, when
 * operator= is called. */
template <typename ContainerT> class add_iterator {
public:
  /// The container's type.
  using container_type = ContainerT;
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = void;
  using pointer = void;
  using reference = void;

  /** An add_iterator is constructed with a container and a position
      that has to be maintained. */
  add_iterator(ContainerT& cont, ContainerT::iterator iter)
      : _cont(&cont), _iter(iter) {}

  /** This assignment operator adds the \c value before the current position.
      It maintains it's position by incrementing after addition.    */
  add_iterator& operator=(ContainerT::const_reference value) {
    _iter = icl::add(*_cont, _iter, value);
    if (_iter != _cont->end())
      ++_iter;
    return *this;
  }

  add_iterator& operator*() { return *this; }
  add_iterator& operator++() { return *this; }
  add_iterator& operator++(int) { return *this; }

private:
  ContainerT* _cont;
  ContainerT::iterator _iter;
};

/** Function adder creates and initializes an add_iterator */
template <typename ContainerT, typename IteratorT>
add_iterator<ContainerT> adder(ContainerT& cont, IteratorT iter_) {
  return add_iterator<ContainerT>(cont, typename ContainerT::iterator(iter_));
}

/** \brief Performs an insertion using a container's member function add, when
 * operator= is called. */
template <typename ContainerT> class insert_iterator {
public:
  /// The container's type.
  using container_type = ContainerT;
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = void;
  using pointer = void;
  using reference = void;

  /** An insert_iterator is constructed with a container and a position
      that has to be maintained. */
  insert_iterator(ContainerT& cont, ContainerT::iterator iter)
      : _cont(&cont), _iter(iter) {}

  /** This assignment operator adds the \c value before the current position.
      It maintains it's position by incrementing after addition.    */
  insert_iterator& operator=(ContainerT::const_reference value) {
    _iter = _cont->insert(_iter, value);
    if (_iter != _cont->end())
      ++_iter;
    return *this;
  }

  insert_iterator& operator*() { return *this; }
  insert_iterator& operator++() { return *this; }
  insert_iterator& operator++(int) { return *this; }

private:
  ContainerT* _cont;
  ContainerT::iterator _iter;
};

/** Function inserter creates and initializes an insert_iterator */
template <typename ContainerT, typename IteratorT>
insert_iterator<ContainerT> inserter(ContainerT& cont, IteratorT iter_) {
  return insert_iterator<ContainerT>(cont,
                                     typename ContainerT::iterator(iter_));
}

} // namespace icl
