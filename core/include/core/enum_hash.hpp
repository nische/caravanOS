#pragma once
#include <type_traits>
#include <cstddef>
#include <functional>

namespace core::detail {
template <typename E>
struct enum_hash {
  static_assert(std::is_enum<E>::value, "enum_hash only for enum types");
  using U = std::underlying_type_t<E>;
  std::size_t operator()(E e) const noexcept { return std::hash<U>{}(static_cast<U>(e)); }
};
} // namespace caravanos::detail
