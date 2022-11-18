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

} // namespace lyn

#endif
