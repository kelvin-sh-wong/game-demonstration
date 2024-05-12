#ifndef HASH_COMBINE_HPP
#define HASH_COMBINE_HPP

#include <cstddef>
#include <functional>

template <typename T>
void hash_combine(std::size_t & seed, T const & value) {
    seed ^= std::hash<T>{}(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

#endif  // HASH_COMBINE_HPP
