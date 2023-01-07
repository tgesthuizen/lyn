#ifndef LYN_SYMBOL_TABLE_H
#define LYN_SYMBOL_TABLE_H

#include <cassert>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace lyn {

class scope {
private:
  friend class symbol_table;
  std::vector<std::unordered_map<std::string_view, int>::node_type>
      shadowed_symbols;
};

class symbol_table {
public:
  int register_primitive(std::string_view name);
  int register_global(std::string_view name);
  int register_local(std::string_view name, scope &current_scope);
  int gen_id() { return next_id++; }

  void start_global_registering() { first_global_id = next_id; }
  void start_local_registering() { first_local_id = next_id; }

  void pop_scope(scope &s);

  int operator[](std::string_view name) const {
    const auto iter = name_to_id.find(name);
    return iter != std::end(name_to_id) ? iter->second : 0;
  }

  int get_next_id() const { return next_id; }
  int get_first_global_id() const { return first_global_id; }
  int get_first_local_id() const { return first_local_id; }

private:
  std::unordered_map<std::string_view, int> name_to_id = {};
  int next_id = 1;
  int first_global_id = 0;
  int first_local_id = 0;
};

inline int symbol_table::register_primitive(std::string_view name) {
  assert(first_global_id == 0);
  return register_global(name);
}

inline int symbol_table::register_global(std::string_view name) {
  assert(first_local_id == 0);
  const int id = gen_id();
  name_to_id[name] = id;
  return id;
}

inline int symbol_table::register_local(std::string_view name,
                                        scope &current_scope) {
  if (auto node = name_to_id.extract(name)) {
    current_scope.shadowed_symbols.emplace_back(std::move(node));
  }
  const int id = gen_id();
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
