#include <gtest/gtest.h>
#include <meta.h>
#include <type_traits>

namespace {

struct A {};
struct B {};
struct C {};

template <class...> struct test_trait : std::false_type {};
template <> struct test_trait<A, B, C> : std::true_type {};

TEST(derive_pack, propagates_pack) {
  EXPECT_TRUE((lyn::derive_pack_t<test_trait, lyn::type_list<A, B, C>>::value));
}

template <class...> struct empty_trait : std::false_type {};
template <> struct empty_trait<> : std::true_type {};

TEST(derive_pack, works_with_empty_pack) {
  EXPECT_TRUE((lyn::derive_pack_t<empty_trait, lyn::type_list<>>::value));
}

} // namespace
