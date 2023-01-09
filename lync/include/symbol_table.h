#ifndef LYN_SYMBOL_TABLE_H
#define LYN_SYMBOL_TABLE_H

#include <string_view>
#include <unordered_map>
#include <vector>

#include <constants.h>

namespace lyn {

class scope {
private:
  friend class symbol_table;
  std::vector<std::unordered_map<std::string_view, int>::node_type>
      shadowed_symbols;
};

class symbol_table {
public:
  int register_global(std::string_view name);
  int register_global(std::string_view name, scope &current_scope);
  int register_local(std::string_view name, scope &current_scope);
  int reify_local(std::string_view name, int id, scope &current_scope);

  void pop_scope(scope &s);

  int operator[](std::string_view name) const {
    const auto iter = name_to_id.find(name);
    return iter != std::end(name_to_id) ? iter->second : 0;
  }

  int get_next_global_id() const { return next_global_id; }
  int get_next_local_id() const { return next_local_id; }

private:
  std::unordered_map<std::string_view, int> name_to_id = {};
  int next_global_id = 1;
  int next_local_id = first_local_id;
};

inline int symbol_table::register_global(std::string_view name) {
  const int id = next_global_id++;
  name_to_id[name] = id;
  return id;
}

inline int symbol_table::register_global(std::string_view name,
                                         scope &current_scope) {
  return reify_local(name, next_global_id++, current_scope);
}

inline int symbol_table::register_local(std::string_view name,
                                        scope &current_scope) {
  return reify_local(name, next_local_id++, current_scope);
}

inline int symbol_table::reify_local(std::string_view name, int id,
                                     scope &current_scope) {
  if (auto node = name_to_id.extract(name)) {
    current_scope.shadowed_symbols.emplace_back(std::move(node));
  }
  name_to_id[name] = id;
  return id;
}

inline void symbol_table::pop_scope(scope &s) {
  for (auto &&node : s.shadowed_symbols) {
    name_to_id.erase(node.key());
    name_to_id.insert(std::move(node));
  }
}

} // namespace lyn

#endif
