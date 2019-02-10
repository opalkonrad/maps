#ifndef AISDI_MAPS_TREEMAP_H
#define AISDI_MAPS_TREEMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class TreeMap
{
public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  struct Node
  {
    value_type value;
    int height;
    Node *lChild, *rChild, *parent;

    Node(value_type value)
        : value(value), height(1), lChild(nullptr), rChild(nullptr), parent(nullptr) {}
  };

  int size = 0;
  Node *root = nullptr;
  Node *pBegin = nullptr;
  Node *pEnd = nullptr;

  int height(Node *n)
  {
    return n ? n->height : 0;
  }

  int heightDifference(Node *n)
  {
    return n ? (height(n->lChild) - height(n->rChild)) : 0;
  }

  key_type getKey(Node *n) const
  {
    return n->value.first;
  }

  Node *smallestNode(Node *n) const
  {
    if (n)
    {
      Node *current = n;

      while (current->lChild)
      {
        current = current->lChild;
      }

      return current;
    }

    return nullptr;
  }

  Node *highestNode(Node *n) const
  {
    if (n)
    {
      Node *current = n;

      while (current->rChild)
      {
        current = current->rChild;
      }

      return current;
    }

    return nullptr;
  }

  Node *findNode(const key_type &key) const
  {
    Node *tmp = this->root;

    while (tmp != nullptr)
    {
      if (getKey(tmp) == key)
      {
        break;
      }
      else if (getKey(tmp) < key)
      {
        tmp = tmp->rChild;
      }
      else
      {
        tmp = tmp->lChild;
      }
    }

    return tmp;
  }

  Node *LL(Node *n)
  {
    Node *m = n->lChild;
    Node *o = m->rChild;

    m->parent = n->parent; // set parent of new highest node

    // perform rotation
    m->rChild = n;
    n->lChild = o;

    // set new parents
    n->parent = m;

    if (o)
    {
      o->parent = n;
    }

    // update heights
    n->height = std::max(height(n->lChild), height(n->rChild)) + 1;
    m->height = std::max(height(m->lChild), height(m->rChild)) + 1;

    return m;
  }

  Node *RR(Node *n)
  {
    Node *m = n->rChild;
    Node *o = m->lChild;

    m->parent = n->parent; // set parent of new highest node

    // perform rotation
    m->lChild = n;
    n->rChild = o;

    // set new parents
    n->parent = m;

    if (o)
    {
      o->parent = n;
    }

    // update heights
    n->height = std::max(height(n->lChild), height(n->rChild)) + 1;
    m->height = std::max(height(m->lChild), height(m->rChild)) + 1;

    return m;
  }

  Node *insert(value_type value, Node *current)
  {
    if (current == nullptr)
    {
      ++size;
      Node *newNode = new Node(value);

      // set new pBegin and pEnd if necessary
      if (size == 1)
      {
        pBegin = pEnd = newNode;
      }
      else if (getKey(newNode) < getKey(pBegin))
      {
        pBegin = newNode;
      }
      else if (getKey(newNode) > getKey(pEnd))
      {
        pEnd = newNode;
      }

      return newNode;
    }

    if (value.first < getKey(current))
    {
      current->lChild = insert(value, current->lChild);

      if (current->lChild)
      {
        current->lChild->parent = current;
      }
    }
    else if (value.first > getKey(current))
    {
      current->rChild = insert(value, current->rChild);

      if (current->rChild)
      {
        current->rChild->parent = current;
      }
    }
    else
    {
      return current;
    }

    // update height
    current->height = std::max(height(current->lChild), height(current->rChild)) + 1;

    // check if it is still balanced
    int balance = heightDifference(current);

    // balance tree
    if (balance > 1 && value.first < getKey(current->lChild))
    {
      return LL(current);
    }

    if (balance < -1 && value.first > getKey(current->rChild))
    {
      return RR(current);
    }

    if (balance > 1 && value.first > getKey(current->lChild))
    {
      current->lChild = RR(current->lChild);
      return LL(current);
    }

    if (balance < -1 && value.first < getKey(current->rChild))
    {
      current->rChild = LL(current->rChild);
      return RR(current);
    }

    return current;
  }

  Node *remove(key_type key, Node *current)
  {
    if (current == nullptr)
    {
      return current;
    }

    if (key < getKey(current))
    {
      current->lChild = remove(key, current->lChild);

      if (current->lChild)
      {
        current->lChild->parent = current;
      }
    }
    else if (key > getKey(current))
    {
      current->rChild = remove(key, current->rChild);

      if (current->rChild)
      {
        current->rChild->parent = current;
      }
    }
    else
    { // node to be deleted
      --size;

      // node with 0/1 child
      if ((current->lChild == nullptr) || (current->rChild == nullptr))
      {
        Node *tmp = current->lChild ? current->lChild : current->rChild;

        // 0 child case (leaf)
        if (tmp == nullptr)
        {

          // set new pBegin if necessary
          if (current == pBegin)
          {
            pBegin = current->parent;
          }

          //set new pEnd if necessary
          if (current == pEnd)
          {
            pEnd = current->parent;
          }

          tmp = current;
          current = nullptr;
        }
        else
        { // 1 child case

          // set new pBegin if necessary
          if (current->lChild == pBegin)
          {
            pBegin = current;
          }

          //set new pEnd if necessary
          if (current->rChild == pEnd)
          {
            pEnd = current;
          }

          current->value = tmp->value;
          current->lChild = current->rChild = nullptr;
        }

        delete tmp;
      }
      else
      { // node with two children
        Node *tmp = smallestNode(current->rChild);

        current->value = tmp->value; // copy data

        current->rChild = remove(getKey(tmp), current->rChild); // delete inorder successor
      }
    }

    // tree had one node
    if (current == nullptr)
    {
      return current;
    }

    // update height
    current->height = std::max(height(current->lChild), height(current->rChild)) + 1;

    // check if it is still balanced
    int balance = heightDifference(current);

    // balance tree
    if (balance > 1 && heightDifference(current->lChild) >= 0)
    {
      return LL(current);
    }

    if (balance > 1 && heightDifference(current->lChild) < 0)
    {
      current->lChild = RR(current->lChild);
      return LL(current);
    }

    if (balance < -1 && heightDifference(current->rChild) <= 0)
    {
      return RR(current);
    }

    if (balance < -1 && heightDifference(current->rChild) > 0)
    {
      current->rChild = LL(current->rChild);
      return RR(current);
    }

    return current;
  }

  void removeTree(Node *n)
  {
    if (n)
    {
      removeTree(n->lChild);
      removeTree(n->rChild);
      delete n;
    }
  }

  Node *makeCopy(Node *node, Node *parent)
  {
    if (node)
    {
      Node *newNode = new Node(node->value);
      newNode->height = node->height;
      newNode->parent = parent;

      if (node->lChild)
      {
        newNode->lChild = makeCopy(node->lChild, newNode);
      }

      if (node->rChild)
      {
        newNode->rChild = makeCopy(node->rChild, newNode);
      }

      return newNode;
    }

    return nullptr;
  }

public:
  Node *pEndReturn() const
  {
    return pEnd;
  }

  TreeMap() = default;

  TreeMap(std::initializer_list<value_type> list)
  {
    for (auto [key, value] : list)
    {
      (*this)[key] = value;
    }
  }

  TreeMap(const TreeMap &other)
      : size(other.size)
  {
    root = makeCopy(other.root, nullptr);
    pBegin = smallestNode(root);
    pEnd = highestNode(root);
  }

  TreeMap(TreeMap &&other)
      : size(other.size), root(std::exchange(other.root, nullptr)), pBegin(std::exchange(other.pBegin, nullptr)), pEnd(std::exchange(other.pEnd, nullptr)) {}

  ~TreeMap()
  {
    removeTree(root);
  }

  TreeMap &operator=(const TreeMap &other)
  {
    if (this == &other)
    {
      return *this;
    }

    removeTree(root);

    size = other.size;

    root = makeCopy(other.root, nullptr);
    pBegin = smallestNode(root);
    pEnd = highestNode(root);

    return *this;
  }

  TreeMap &operator=(TreeMap &&other)
  {
    if (this == &other)
    {
      return *this;
    }

    removeTree(root);

    size = other.size;

    root = std::exchange(other.root, nullptr);
    pBegin = std::exchange(other.pBegin, nullptr);
    pEnd = std::exchange(other.pEnd, nullptr);

    return *this;
  }

  bool isEmpty() const
  {
    return !size;
  }

  mapped_type &operator[](const key_type &key)
  {
    auto result = find(key);

    if (result != end())
    {
      return result->second;
    }

    root = insert({key, mapped_type()}, root);

    return find(key)->second;
  }

  const mapped_type &valueOf(const key_type &key) const
  {
    auto result = find(key);

    if (result == end())
    {
      throw std::out_of_range("Value not found!");
    }

    return result->second;
  }

  mapped_type &valueOf(const key_type &key)
  {
    Node *result = findNode(key);

    if (result == nullptr)
    {
      throw std::out_of_range("Trying to get value of non-existing element!");
    }

    return result->value.second;
  }

  const_iterator find(const key_type &key) const
  {
    return const_iterator(this, findNode(key));
  }

  iterator find(const key_type &key)
  {
    return iterator(const_iterator(this, findNode(key)));
  }

  void remove(const key_type &key)
  {
    if (findNode(key))
    {
      root = remove(key, root);
      return;
    }

    throw std::out_of_range("Removing from empty map!");
  }

  void remove(const const_iterator &it)
  {
    remove(it->first);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const TreeMap &other) const
  {
    if (size != other.size)
    {
      return 0;
    }

    const_iterator it(this, pBegin);

    for (auto itOther = other.begin(); itOther != other.end(); ++itOther, ++it)
    {
      if (*it != *itOther)
      {
        return 0;
      }
    }

    return 1;
  }

  bool operator!=(const TreeMap &other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return iterator(cbegin());
  }

  iterator end()
  {
    return iterator(cend());
  }

  const_iterator cbegin() const
  {
    return const_iterator(this, pBegin);
  }

  const_iterator cend() const
  {
    return const_iterator(this, nullptr);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type = typename TreeMap::value_type;
  using pointer = const typename TreeMap::value_type *;
  using key_type = typename TreeMap::key_type;
  using mapped_type = typename TreeMap::mapped_type;

private:
  const TreeMap<KeyType, ValueType> *map;
  Node *actualNode;

public:
  explicit ConstIterator(const TreeMap<KeyType, ValueType> *map, Node *actualNode)
      : map(map), actualNode(actualNode) {}

  ConstIterator(const ConstIterator &other)
      : map(other.map), actualNode(other.actualNode) {}

  ConstIterator &operator++()
  {
    if (actualNode == nullptr)
    {
      throw std::out_of_range("Incrementing end()");
    }

    if (actualNode->rChild)
    {
      actualNode = actualNode->rChild;

      while (actualNode->lChild)
      {
        actualNode = actualNode->lChild;
      }
      return *this;
    }

    while (actualNode->parent && actualNode != actualNode->parent->lChild)
    {
      actualNode = actualNode->parent;
    }

    if (actualNode->parent)
    {
      actualNode = actualNode->parent;
    }
    else
    {
      actualNode = nullptr;
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    auto tmp = *this;
    operator++();
    return tmp;
  }

  ConstIterator &operator--()
  {
    if (map->pEndReturn() == nullptr)
    {
      throw std::out_of_range("Decrementing iterator of an empty tree!");
    }

    if (actualNode == nullptr)
    {
      actualNode = map->pEndReturn();
      return *this;
    }

    if (actualNode->lChild)
    {
      actualNode = actualNode->lChild;

      while (actualNode->rChild)
      {
        actualNode = actualNode->rChild;
      }
      return *this;
    }

    while (actualNode->parent && actualNode != actualNode->parent->rChild)
    {
      actualNode = actualNode->parent;
    }

    if (actualNode->parent)
    {
      actualNode = actualNode->parent;
    }
    else
    {
      actualNode = nullptr;
    }

    return *this;
  }

  ConstIterator operator--(int)
  {
    auto tmp = *this;
    operator--();
    return tmp;
  }

  reference operator*() const
  {
    if (actualNode == nullptr)
    {
      throw std::out_of_range("Dereferencing nullptr!");
    }

    return actualNode->value;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator &other) const
  {
    return map == other.map && actualNode == other.actualNode;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class TreeMap<KeyType, ValueType>::Iterator : public TreeMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename TreeMap::reference;
  using pointer = typename TreeMap::value_type *;

  explicit Iterator()
  {
  }

  Iterator(const ConstIterator &other)
      : ConstIterator(other)
  {
  }

  Iterator &operator++()
  {
    ConstIterator::operator++();
    return *this;
  }

  Iterator operator++(int)
  {
    auto result = *this;
    ConstIterator::operator++();
    return result;
  }

  Iterator &operator--()
  {
    ConstIterator::operator--();
    return *this;
  }

  Iterator operator--(int)
  {
    auto result = *this;
    ConstIterator::operator--();
    return result;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  reference operator*() const
  {
    // ugly cast, yet reduces code duplication.
    return const_cast<reference>(ConstIterator::operator*());
  }
};

} // namespace aisdi

#endif /* AISDI_MAPS_MAP_H */