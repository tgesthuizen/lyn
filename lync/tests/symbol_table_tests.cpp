#include <gtest/gtest.h>
#include <symbol_table.h>

namespace {

TEST(symbol_table, can_access_registered_primitive) {

  lyn::symbol_table symtab;
  const std::string primitive_name = "prim";
  const auto id = symtab.register_primitive(primitive_name);
  EXPECT_EQ(id, symtab[primitive_name]);
}

TEST(symbol_table, registered_names_get_different_ids) {
  lyn::symbol_table symtab;
  const std::string prim1 = "prim1";
  const std::string prim2 = "prim2";
  const auto id1 = symtab.register_primitive(prim1);
  const auto id2 = symtab.register_primitive(prim2);
  EXPECT_NE(id1, id2);
}

TEST(symbol_table, can_shadow_global) {
  lyn::symbol_table symtab;
  const std::string name = "name";
  const auto id1 = symtab.register_primitive(name);
  symtab.start_global_registering();
  symtab.start_local_registering();
  lyn::scope scope;
  const auto id2 = symtab.register_local(name, scope);
  ASSERT_NE(id1, id2);
  EXPECT_EQ(id2, symtab[name]);
  symtab.pop_scope(scope);
  EXPECT_EQ(id1, symtab[name]);
}

} // namespace
