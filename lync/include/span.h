#ifndef LYN_SPAN_H
#define LYN_SPAN_H

#include <cstddef>
#include <cstring>
#include <type_traits>
#include <utility>
#include <vector>

namespace lyn {

template <class T> class span {
public:
  using value_type = T;

  span() = default;
  span(const span &other) = default;
  span(span &&other) = default;
  span &operator=(const span &other) = default;
  span &operator=(span &&other) = default;
  ~span() noexcept = default;
  span(T *data, std::size_t size) : m_data{data}, m_size{size} {}

  T *data() const { return m_data; }
  std::size_t size() const { return m_size; }
  bool empty() const { return !m_size; }
  T *begin() const { return m_data; }
  T *end() const { return m_data + m_size; }

  T &front() const { return *begin(); }
  T &back() const { return *(end() - 1); }
  T &operator[](std::size_t idx) const { return m_data[idx]; }

private:
  T *m_data = nullptr;
  std::size_t m_size = 0u;
};

template <class Alloc, class Container>
span<typename Container::value_type> spanify(Alloc &alloc,
                                             const Container &vec) {
  using T = typename Container::value_type;
  static_assert(std::is_trivially_destructible<T>::value);
  return {static_cast<T *>(std::memcpy(
              alloc.allocate(sizeof(T) * std::size(vec), alignof(T)),
              std::data(vec), sizeof(T) * std::size(vec))),
          std::size(vec)};
}

} // namespace lyn

#endif
