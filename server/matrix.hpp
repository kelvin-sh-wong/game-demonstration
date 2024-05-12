#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <initializer_list>

template <std::size_t M, std::size_t N>
class Matrix {
public:
    Matrix() : data_{} {
        // empty
    }

    Matrix(std::initializer_list<double> data) {
        assert(data.size() == M * N);

        std::copy(data.begin(), data.end(), data_.begin());
    }

    static Matrix<M, N> zero() {
        return Matrix<M, N>{};
    }

    double * operator[](std::size_t i) {
        return &data_[i * N];
    }

    double const * operator[](std::size_t i) const {
        return &data_[i * N];
    }

private:
    std::array<double, M * N> data_;
};

template <std::size_t M1, std::size_t N1, std::size_t M2, std::size_t N2>
Matrix<M1, N1> operator+(Matrix<M1, N1> const & matrix_1, Matrix<M2, N2> const & matrix_2) {
    static_assert(M1 == M2 && N1 == N2);

    Matrix<M1, N1> result;

    for (std::size_t i{0}; i < M1; ++i) {
        for (std::size_t j{0}; j < N1; ++j) {
            result[i][j] = matrix_1[i][j] + matrix_2[i][j];
        }
    }

    return result;
}

template <std::size_t M1, std::size_t N1, std::size_t M2, std::size_t N2>
Matrix<M1, N1> operator-(Matrix<M1, N1> const & matrix_1, Matrix<M2, N2> const & matrix_2) {
    static_assert(M1 == M2 && N1 == N2);

    Matrix<M1, N1> result;

    for (std::size_t i{0}; i < M1; ++i) {
        for (std::size_t j{0}; j < N1; ++j) {
            result[i][j] = matrix_1[i][j] - matrix_2[i][j];
        }
    }

    return result;
}

template <std::size_t M, std::size_t N>
Matrix<M, N> operator-(Matrix<M, N> const & matrix) {
    return -1.0 * matrix;
}

template <std::size_t M1, std::size_t N1, std::size_t M2, std::size_t N2>
Matrix<M1, N2> operator*(Matrix<M1, N1> const & matrix_1, Matrix<M2, N2> const & matrix_2) {
    static_assert(N1 == M2);

    Matrix<M1, N2> result;

    for (std::size_t i{0}; i < M1; ++i) {
        for (std::size_t j{0}; j < N2; ++j) {
            for (std::size_t k{0}; k < N1; ++k) {
                result[i][j] += matrix_1[i][k] * matrix_2[k][j];
            }
        }
    }

    return result;
}

template <std::size_t M, std::size_t N>
Matrix<M, N> operator*(double scalar, Matrix<M, N> const & matrix) {
    Matrix<M, N> result;

    for (std::size_t i{0}; i < M; ++i) {
        for (std::size_t j{0}; j < N; ++j) {
            result[i][j] = scalar * matrix[i][j];
        }
    }

    return result;
}

template <std::size_t M, std::size_t N>
Matrix<M, N> operator*(Matrix<M, N> const & matrix, double scalar) {
    return scalar * matrix;
}

template <std::size_t M, std::size_t N>
Matrix<N, M> transpose(Matrix<M, N> const & matrix) {
    Matrix<N, M> result;

    for (std::size_t i{0}; i < N; ++i) {
        for (std::size_t j{0}; j < M; ++j) {
            result[i][j] = matrix[j][i];
        }
    }

    return result;
}

#endif  // MATRIX_HPP
