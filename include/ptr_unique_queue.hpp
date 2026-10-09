// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2026, Ivan Pizhenko. All rights reserved.

#pragma once

#if __cplusplus < 202002L
#error "This header file required at least C++ 20"
#endif

// STL
#include <memory>
#include <type_traits>

// Boost
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace stdx {

template<typename T, typename PtrT = T*>
struct ptr_unique_queue_default_key_extractor {
  using result_type = T;

  T& operator()(PtrT& v) const noexcept
  {
    return *v;
  }

  const T& operator()(const PtrT& v) const noexcept
  {
    return *v;
  }
};

template<typename T, typename PtrT = T*>
struct ptr_unique_queue_item {
  std::size_t id;
  PtrT payload;
};

template <
    typename T,
    typename PtrT = T*,
    typename KeyExtractor = ptr_unique_queue_default_key_extractor<T, PtrT>,
    typename KeyHash = std::hash<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename KeyPred = std::equal_to<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename Allocator = std::allocator<ptr_unique_queue_item<T, PtrT>>>
class ptr_unique_queue {
public:
  using value_type = ptr_unique_queue_item<T, PtrT>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  explicit ptr_unique_queue(size_type allowed_duplicate_count = 1) :
      m_allowed_duplicate_count(allowed_duplicate_count),
      m_last_id(0)
  {
  }

  ptr_unique_queue(const ptr_unique_queue& src) = default;

  ptr_unique_queue(ptr_unique_queue&& src) noexcept = default;

  // Assignment

  ptr_unique_queue& operator=(const ptr_unique_queue& src) = default;

  ptr_unique_queue& operator=(ptr_unique_queue&& src) = default;

  // Capacity

  [[nodiscard]]
  size_type size() const noexcept
  {
    return m_items.size();
  }

  [[nodiscard]]
  size_type max_size() const noexcept
  {
    return m_items.max_size();
  }

  [[nodiscard]]
  bool empty() const noexcept
  {
    return m_items.empty();
  }

  // Element access

  reference front() noexcept
  {
    return sequential().front();
  }

  const_reference front() const noexcept
  {
    return sequential().front();
  }

  reference back() noexcept
  {
    return sequential().back();
  }

  const_reference back() const noexcept
  {
    return sequential().back();
  }

  // Operations

  template <typename P = PtrT, typename = std::enable_if_t<std::is_copy_constructible_v<P>>>
  void push_back(const PtrT& v)
  {
    check_and_remove_duplicates(v);
    m_items.push_back(ptr_unique_queue_item<T, PtrT> {++m_last_id, v });
  }

  void push_back(PtrT&& v)
  {
    check_and_remove_duplicates(v);
    m_items.push_back(ptr_unique_queue_item<T, PtrT> {++m_last_id, std::move(v) });
  }

  void pop_front() noexcept
  {
    sequential().pop_front();
  }

  void clear() noexcept
  {
    m_items.clear();
    m_last_id = 0;
  }

  void swap(ptr_unique_queue& other) noexcept
  {
    m_items.swap(other.m_items);
    std::swap(m_last_id, other.m_last_id);
    std::swap(m_allowed_duplicate_count, other.m_allowed_duplicate_count);
  }

  // Iterators

  [[nodiscard]]
  auto begin() noexcept
  {
    return sequential().begin();
  }

  [[nodiscard]]
  auto begin() const noexcept
  {
    return sequential().begin();
  }

  [[nodiscard]]
  auto cbegin() const noexcept
  {
    return begin();
  }

  [[nodiscard]]
  auto end() noexcept
  {
    return sequential().end();
  }

  [[nodiscard]]
  auto end() const noexcept
  {
    return sequential().end();
  }

  [[nodiscard]]
  auto cend() const noexcept
  {
    return end();
  }

   // Reverse iterators

   [[nodiscard]]
   auto rbegin() noexcept
   {
     return sequential().rbegin();
   }

  [[nodiscard]]
  auto rbegin() const noexcept
  {
    return sequential().rbegin();
  }

  [[nodiscard]]
  auto crbegin() const noexcept
  {
    return rbegin();
  }

  [[nodiscard]]
  auto rend() noexcept
  {
    return sequential().rend();
  }

  [[nodiscard]]
  auto rend() const noexcept
  {
    return sequential().rend();
  }

  [[nodiscard]]
  auto crend() const noexcept
  {
    return rend();
  }

  template <typename T1, typename PtrT1, typename KeyExtractor1, typename KeyHash1, typename KeyPred1, typename Allocator1>
  friend bool operator==(
      const ptr_unique_queue<T1, PtrT1, KeyExtractor1, KeyHash1, KeyPred1, Allocator1>& lhs,
      const ptr_unique_queue<T1, PtrT1, KeyExtractor1, KeyHash1, KeyPred1, Allocator1>& rhs) noexcept;

  template <typename T1, typename PtrT1, typename KeyExtractor1, typename KeyHash1, typename KeyPred1, typename Allocator1>
  friend bool operator!=(
      const ptr_unique_queue<T1, PtrT1, KeyExtractor1, KeyHash1, KeyPred1, Allocator1>& lhs,
      const ptr_unique_queue<T1, PtrT1, KeyExtractor1, KeyHash1, KeyPred1, Allocator1>& rhs) noexcept;

private:
  struct key_extractor {
    using result_type = KeyExtractor::result_type;

    result_type operator()(const ptr_unique_queue_item<T, PtrT>& v) const
    {
      return KeyExtractor()(v.payload);
    }
  };

  struct key_hash {
    std::size_t operator()(const std::remove_cv_t<typename key_extractor::result_type>& v) const
    {
      return KeyHash()(v);
    }
  };

  struct key_pred {
    bool operator()(
        const std::remove_cv_t<typename key_extractor::result_type>& lhs,
        const std::remove_cv_t<typename key_extractor::result_type>& rhs) const
    {
      return KeyPred()(lhs, rhs);
    }
  };

  struct sequential_tag {};
  struct hashed_tag {};

  using container_type = boost::multi_index_container<
      value_type,
      boost::multi_index::indexed_by<
          boost::multi_index::sequenced<boost::multi_index::tag<sequential_tag>>,
          boost::multi_index::hashed_non_unique<boost::multi_index::tag<hashed_tag>, key_extractor, key_hash, key_pred>
      >,
      Allocator
  >;

  // Indexes

  [[nodiscard]]
  auto& sequential() noexcept
  {
    return m_items.template get<0>();
  }

  [[nodiscard]]
  auto& sequential() const noexcept
  {
    return m_items.template get<0>();
  }

  [[nodiscard]]
  auto& hashed() noexcept
  {
    return m_items.template get<1>();
  }

  [[nodiscard]]
  auto& hashed() const noexcept
  {
    return m_items.template get<1>();
  }

  void check_and_remove_duplicates(const PtrT& v)
  {
    auto& idx = hashed();
    auto it = idx.find(KeyExtractor()(v));
    if (it != idx.end()) {
      auto max_it = it;
      ++it;
      size_type count = 1;
      for (; it != idx.end() && KeyPred()(KeyExtractor()(it->payload), KeyExtractor()(v)); ++it, ++count) {
        if (it->id > max_it->id) max_it = it;
      }
      if (count == m_allowed_duplicate_count) {
        idx.erase(max_it);
      }
    }
  }

  container_type m_items;
  size_type m_allowed_duplicate_count;
  size_type m_last_id;
};

template <typename T, typename PtrT, typename KeyExtractor, typename KeyHash, typename KeyPred, typename Allocator>
inline void swap(
    ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& a,
    ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& b) noexcept
{
  a.swap(b);
}

template <typename T, typename PtrT, typename KeyExtractor, typename KeyHash, typename KeyPred, typename Allocator>
[[nodiscard]]
inline bool operator==(
    const ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& lhs,
    const ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& rhs) noexcept
{
  return lhs.m_allowed_duplicate_count == rhs.m_allowed_duplicate_count && lhs.m_items == rhs.m_items;
}

template <typename T, typename PtrT, typename KeyExtractor, typename KeyHash, typename KeyPred, typename Allocator>
[[nodiscard]]
inline bool operator!=(
    const ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& lhs,
    const ptr_unique_queue<T, PtrT, KeyExtractor, KeyHash, KeyPred, Allocator>& rhs) noexcept
{
  return !(lhs == rhs);
}

template <
    typename T,
    typename KeyExtractor = ptr_unique_queue_default_key_extractor<T, std::unique_ptr<T>>,
    typename KeyHash = std::hash<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename KeyPred = std::equal_to<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename Allocator = std::allocator<ptr_unique_queue_item<T, std::unique_ptr<T>>>>
using unique_ptr_unique_queue = ptr_unique_queue<T, std::unique_ptr<T>, KeyExtractor, KeyHash, KeyPred, Allocator>;

template <
    typename T,
    typename KeyExtractor = ptr_unique_queue_default_key_extractor<T, std::shared_ptr<T>>,
    typename KeyHash = std::hash<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename KeyPred = std::equal_to<std::remove_cv_t<typename KeyExtractor::result_type>>,
    typename Allocator = std::allocator<ptr_unique_queue_item<T, std::shared_ptr<T>>>>
using shared_ptr_unique_queue = ptr_unique_queue<T, std::shared_ptr<T>, KeyExtractor, KeyHash, KeyPred, Allocator>;

} // namespace stdx
