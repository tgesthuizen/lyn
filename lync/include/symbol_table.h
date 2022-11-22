#ifndef LYN_SYMBOL_TABLE_H
#define LYN_SYMBOL_TABLE_H

namespace lyn {

struct symbol_table {
  int register_name(std::string_view name);

  std::unordered_map<std::string_view, int> name_to_id = {};
  int next_id = 1;
  int first_global_id = 0;
  int first_local_id = 0;
};

inline int symbol_table::register_name(std::string_view name) {
  const int id = next_id++;
  name_to_id[name] = id;
  return id;
}

} // namespace lyn

#endif
