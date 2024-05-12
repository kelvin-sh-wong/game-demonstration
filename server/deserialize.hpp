#ifndef DESERIALIZE_HPP
#define DESERIALIZE_HPP

#include <cstdint>
#include <cstring>
#include <istream>
#include <vector>

template <typename T>
struct deserialize {
    // empty
};

template <>
struct deserialize<std::uint8_t> {
    std::uint8_t operator()(std::istream & istream) const {
        std::uint8_t value{0};
        istream.read(reinterpret_cast<char *>(&value), 1);

        return value;
    }
};

template <>
struct deserialize<std::uint16_t> {
    std::uint16_t operator()(std::istream & istream) const {
        std::uint16_t value{0};
        for (int i{0}; i < 2; ++i) {
            value |= static_cast<std::uint16_t>(deserialize<std::uint8_t>{}(istream)) << i * 8;
        }

        return value;
    }
};

template <>
struct deserialize<std::uint32_t> {
    std::uint32_t operator()(std::istream & istream) const {
        std::uint32_t value{0};
        for (int i{0}; i < 4; ++i) {
            value |= static_cast<std::uint32_t>(deserialize<std::uint8_t>{}(istream)) << i * 8;
        }

        return value;
    }
};

template <>
struct deserialize<std::uint64_t> {
    std::uint64_t operator()(std::istream & istream) const {
        std::uint64_t value{0};
        for (int i{0}; i < 8; ++i) {
            value |= static_cast<std::uint64_t>(deserialize<std::uint8_t>{}(istream)) << i * 8;
        }

        return value;
    }
};

template <>
struct deserialize<std::int8_t> {
    std::int8_t operator()(std::istream & istream) const {
        std::uint8_t buffer{deserialize<std::uint8_t>{}(istream)};

        std::int8_t value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <>
struct deserialize<std::int16_t> {
    std::int16_t operator()(std::istream & istream) const {
        std::uint16_t buffer{deserialize<std::uint16_t>{}(istream)};

        std::int16_t value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <>
struct deserialize<std::int32_t> {
    std::int32_t operator()(std::istream & istream) const {
        std::uint32_t buffer{deserialize<std::uint32_t>{}(istream)};

        std::int32_t value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <>
struct deserialize<std::int64_t> {
    std::int64_t operator()(std::istream & istream) const {
        std::uint64_t buffer{deserialize<std::uint64_t>{}(istream)};

        std::int64_t value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <>
struct deserialize<float> {
    float operator()(std::istream & istream) const {
        std::uint32_t buffer{deserialize<std::uint32_t>{}(istream)};

        float value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <>
struct deserialize<double> {
    double operator()(std::istream & istream) const {
        std::uint64_t buffer{deserialize<std::uint64_t>{}(istream)};

        double value;
        std::memcpy(&value, &buffer, sizeof(value));

        return value;
    }
};

template <typename T>
struct deserialize<std::vector<T>> {
    std::vector<T> operator()(std::istream & istream) const {
        std::vector<T> vector;

        std::uint32_t size{deserialize<std::uint32_t>{}(istream)};

        for (std::uint32_t i{0}; i < size; ++i) {
            vector.push_back(deserialize<T>{}(istream));
        }

        return vector;
    }
};

#endif  // DESERIALIZE_HPP
