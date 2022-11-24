#ifndef LYN_SPAN_H
#define LYN_SPAN_H

#include <cstddef>

namespace lyn {

template <class T> class span {
public:
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

  T &front() { return *begin(); }
  T &back() { return *(end() - 1); }

private:
  T *m_data = nullptr;
  std::size_t m_size = 0u;
};

} // namespace lyn

#endif
