#ifndef AISDI_MAPS_HASHMAP_H
#define AISDI_MAPS_HASHMAP_H

#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <utility>
#include <list>

namespace aisdi
{

template <typename KeyType, typename ValueType>
class HashMap
{

public:
  using key_type = KeyType;
  using mapped_type = ValueType;
  using value_type = std::pair<const key_type, mapped_type>;
  using size_type = std::size_t;
  using reference = value_type &;
  using const_reference = const value_type &;

  using entry_list = std::list<value_type>;

  class ConstIterator;
  class Iterator;
  using iterator = Iterator;
  using const_iterator = ConstIterator;

private:
  entry_list *entries;
  size_type size;
  size_type buckets;

  static constexpr float threshold = .75;

public:
  HashMap()
      : entries(new entry_list[17]), size(0), buckets(16) {}

  HashMap(std::initializer_list<value_type> list)
      : entries(new entry_list[list.size() * 2 + 1]), size(0), buckets(list.size() * 2)
  {
    for (auto [key, value] : list)
    {
      (*this)[key] = value;
    }
  }

  HashMap(const HashMap &other)
      : entries(new entry_list[other.buckets + 1]), size(0), buckets(other.buckets)
  {
    for (auto [key, value] : other)
    {
      (*this)[key] = value;
    }
  }

  HashMap(HashMap &&other)
      : entries(std::exchange(other.entries, nullptr)), size(other.size), buckets(other.buckets) {}

  ~HashMap()
  {
    delete[] entries;
  }

  HashMap &operator=(const HashMap &other)
  {
    if (this == &other)
    {
      return *this;
    }

    delete[] entries;

    buckets = other.buckets;
    size = 0;
    entries = new entry_list[buckets + 1];

    for (auto [key, value] : other)
    {
      (*this)[key] = value;
    }

    return *this;
  }

  HashMap &operator=(HashMap &&other)
  {
    delete[] entries;

    entries = std::exchange(other.entries, nullptr);
    buckets = other.buckets;
    size = other.size;

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
    else
    {
      if (static_cast<float>(size) / static_cast<float>(buckets) >= threshold)
      {
        resize();
      }

      auto keyHash = hash(key);
      ++size;
      return entries[keyHash].emplace_back(key, ValueType()).second;
    }
  }

  const mapped_type &valueOf(const key_type &key) const
  {
    if (auto it = find(key); it == cend())
    {
      throw std::out_of_range("Value not found!");
    }
    else
    {
      return it->second;
    }
  }

  mapped_type &valueOf(const key_type &key)
  {
    if (auto it = find(key); it == end())
    {
      throw std::out_of_range("Value not found!");
    }
    else
    {
      return it->second;
    }
  }

  const_iterator find(const key_type &key) const
  {
    auto keyHash = hash(key);

    for (auto it = entries[keyHash].begin(); it != entries[keyHash].end(); ++it)
    {
      if (it->first == key)
      {
        return ConstIterator(this, it, keyHash);
      }
    }

    return cend();
  }

  iterator find(const key_type &key)
  {
    auto keyHash = hash(key);

    for (auto it = entries[keyHash].begin(); it != entries[keyHash].end(); ++it)
    {
      if (it->first == key)
      {
        return Iterator(ConstIterator(this, it, keyHash));
      }
    }

    return end();
  }

  void remove(const key_type &key)
  {
    auto keyHash = hash(key);

    for (auto it = entries[keyHash].begin(); it != entries[keyHash].end(); ++it)
    {
      if (it->first == key)
      {
        entries[keyHash].erase(it);
        --size;
        return;
      }
    }

    throw std::out_of_range("Attempt to remove not existing element!");
  }

  void remove(const const_iterator &it)
  {
    remove(it->first);
  }

  size_type getSize() const
  {
    return size;
  }

  bool operator==(const HashMap &other) const
  {
    if (getSize() != other.getSize())
    {
      return 0;
    }

    for (const auto &item : other)
    {
      const auto it = find(item.first);

      if (it == cend())
      {
        return 0;
      }

      if (it->second != item.second)
      {
        return 0;
      }
    }

    return 1;
  }

  bool operator!=(const HashMap &other) const
  {
    return !(*this == other);
  }

  iterator begin()
  {
    return Iterator(cbegin());
  }

  iterator end()
  {
    return Iterator(cend());
  }

  const_iterator cbegin() const
  {
    if (size == 0)
    {
      return cend();
    }

    size_type index = 0;

    while (entries[index].empty())
    {
      ++index;
    }

    return ConstIterator(this, entries[index].begin(), index);
  }

  const_iterator cend() const
  {
    return ConstIterator(this, entries[buckets].begin(), buckets);
  }

  const_iterator begin() const
  {
    return cbegin();
  }

  const_iterator end() const
  {
    return cend();
  }

private:
  size_t hash(const KeyType &key) const
  {
    return key % buckets;
  }

  size_t reHash(const KeyType &key) const
  {
    return key % (buckets * 2);
  }

  void resize()
  {
    entry_list *newEntries = new entry_list[buckets * 2 + 1];

    for (auto [key, value] : *this)
    {
      newEntries[reHash(key)].emplace_back(key, value);
    }

    buckets *= 2;
    delete[] entries;
    entries = newEntries;
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::const_reference;
  using iterator_category = std::bidirectional_iterator_tag;
  using key_type = KeyType;
  using value_type = typename HashMap::value_type;
  using pointer = const typename HashMap::value_type *;
  using size_type = std::size_t;

  using list_it = typename std::list<value_type>::const_iterator;

private:
  const HashMap<KeyType, ValueType> *map;
  list_it it;
  size_t index;

public:
  explicit ConstIterator(const HashMap<KeyType, ValueType> *map, list_it it, size_t index)
      : map(map), it(it), index(index) {}

  ConstIterator(const ConstIterator &other) : map(other.map), it(other.it), index(other.index) {}

  ConstIterator &operator++()
  {
    if (*this == map->cend())
    {
      throw std::out_of_range("Incrementing end()!");
    }

    ++it;

    while (it == map->entries[index].cend())
    {
      ++index;

      if (index == map->buckets)
      {
        *this = map->cend();
        return *this;
      }

      it = map->entries[index].cbegin();
    }

    return *this;
  }

  ConstIterator operator++(int)
  {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  ConstIterator &operator--()
  {
    if (*this == map->cbegin())
    {
      throw std::out_of_range("Decrementing begin()!");
    }

    if (*this == map->cend())
    {
      index = map->buckets - 1;
      it = map->entries[index].cend();
    }

    while (it == map->entries[index].cbegin())
    {
      --index;
      it = map->entries[index].cend();
    }

    --it;

    return *this;
  }

  ConstIterator operator--(int)
  {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  reference operator*() const
  {
    if (*this == map->end())
    {
      throw std::out_of_range("Dereferencing end()!");
    }
    return *it;
  }

  pointer operator->() const
  {
    return &this->operator*();
  }

  bool operator==(const ConstIterator &other) const
  {
    return map == other.map && index == other.index && it == other.it;
  }

  bool operator!=(const ConstIterator &other) const
  {
    return !(*this == other);
  }
};

template <typename KeyType, typename ValueType>
class HashMap<KeyType, ValueType>::Iterator : public HashMap<KeyType, ValueType>::ConstIterator
{
public:
  using reference = typename HashMap::reference;
  using pointer = typename HashMap::value_type *;

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

#endif /* AISDI_MAPS_HASHMAP_H */