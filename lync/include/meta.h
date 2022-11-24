#ifndef LYN_META_H
#define LYN_META_H

namespace lyn {

template <class...> struct type_list {};
template <template <class...> class Target, class Base> struct derive_pack;
template <template <class...> class Target, class... Pack>
struct derive_pack<Target, type_list<Pack...>> {
  using type = Target<Pack...>;
};
template <template <class...> class Target, class Base>
using derive_pack_t = typename derive_pack<Target, Base>::type;

// Taken verbatim from cppreference.com:
// https://en.cppreference.com/w/cpp/utility/unreachable
[[noreturn]] inline void unreachable() {
  // Uses compiler specific extensions if possible.
  // Even if no extension is used, undefined behavior is still raised by
  // an empty function body and the noreturn attribute.
#ifdef __GNUC__ // GCC, Clang, ICC
  __builtin_unreachable();
#elifdef _MSC_VER // MSVC
  __assume(false);
#endif
}

} // namespace lyn

#endif
