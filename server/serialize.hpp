#ifndef SERIALIZE_HPP
#define SERIALIZE_HPP

#include <cassert>
#include <cstdint>
#include <cstring>
#include <limits>
#include <ostream>
#include <vector>

template <typename T>
struct serialize {
    // empty
};

template <>
struct serialize<std::uint8_t> {
    void operator()(std::ostream & ostream, std::uint8_t value) const {
        ostream.write(reinterpret_cast<char *>(&value), 1);
    }
};

template <>
struct serialize<std::uint16_t> {
    void operator()(std::ostream & ostream, std::uint16_t value) const {
        for (int i{0}; i < 2; ++i) {
            serialize<std::uint8_t>{}(ostream, static_cast<std::uint8_t>((value >> i * 8) & 0xff));
        }
    }
};

template <>
struct serialize<std::uint32_t> {
    void operator()(std::ostream & ostream, std::uint32_t value) const {
        for (int i{0}; i < 4; ++i) {
            serialize<std::uint8_t>{}(ostream, static_cast<std::uint8_t>((value >> i * 8) & 0xff));
        }
    }
};

template <>
struct serialize<std::uint64_t> {
    void operator()(std::ostream & ostream, std::uint64_t value) const {
        for (int i{0}; i < 8; ++i) {
            serialize<std::uint8_t>{}(ostream, static_cast<std::uint8_t>((value >> i * 8) & 0xff));
        }
    }
};

template <>
struct serialize<std::int8_t> {
    void operator()(std::ostream & ostream, std::int8_t value) const {
        std::uint8_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint8_t>{}(ostream, buffer);
    }
};

template <>
struct serialize<std::int16_t> {
    void operator()(std::ostream & ostream, std::int16_t value) const {
        std::uint16_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint16_t>{}(ostream, buffer);
    }
};

template <>
struct serialize<std::int32_t> {
    void operator()(std::ostream & ostream, std::int32_t value) const {
        std::uint32_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint32_t>{}(ostream, buffer);
    }
};

template <>
struct serialize<std::int64_t> {
    void operator()(std::ostream & ostream, std::int64_t value) const {
        std::uint64_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint64_t>{}(ostream, buffer);
    }
};

template <>
struct serialize<float> {
    void operator()(std::ostream & ostream, float value) const {
        std::uint32_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint32_t>{}(ostream, buffer);
    }
};

template <>
struct serialize<double> {
    void operator()(std::ostream & ostream, double value) const {
        std::uint64_t buffer;
        std::memcpy(&buffer, &value, sizeof(buffer));

        serialize<std::uint64_t>{}(ostream, buffer);
    }
};

template <typename T>
struct serialize<std::vector<T>> {
    void operator()(std::ostream & ostream, std::vector<T> const & vector) const {
        assert(vector.size() <= std::numeric_limits<std::uint32_t>::max());

        serialize<std::uint32_t>{}(ostream, static_cast<std::uint32_t>(vector.size()));

        for (std::uint32_t i{0}; i < static_cast<std::uint32_t>(vector.size()); ++i) {
            serialize<T>{}(ostream, vector[i]);
        }
    }
};

#endif  // SERIALIZE_HPP
