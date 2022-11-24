#ifndef LYN_STRING_TABLE_H
#define LYN_STRING_TABLE_H

#include <cstring>
#include <memory_resource>
#include <string_view>

namespace lyn {

class string_table {
public:
  std::string_view store(std::string_view target) {
    return std::string_view(
        static_cast<char *>(std::memcpy(alloc.allocate(std::size(target), 1u),
                                        target.data(), std::size(target))),
        std::size(target));
  }

private:
  std::pmr::monotonic_buffer_resource alloc;
};

} // namespace lyn

#endif
