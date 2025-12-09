/*
* Unit.hpp
 * A header-only C++20 library for compile-time dimensional analysis and unit conversion.
 *
 * Version: 0.16
 * Author:  OguzhanUmutlu
 * GitHub:  https://github.com/OguzhanUmutlu/unit.hpp
 *
 * Licensed under the MIT License.
 */

#pragma once
#include <array>
#include <iostream>

#include "Vector.hpp"

template <size_t Rows, size_t Cols, typename T>
struct Matrix {
    std::array<T, Rows * Cols> elements;

    constexpr Matrix() : elements{} {
    }

    template <typename... Args>
    constexpr Matrix(Args... args) : elements{static_cast<T>(args)...} {
        static_assert(sizeof...(args) == Rows * Cols, "Number of arguments must match Matrix size.");
    }

    constexpr explicit Matrix(const std::array<T, Rows * Cols>& arr) : elements(arr) {
    }

    static constexpr Matrix Identity() requires (Rows == Cols) {
        Matrix m;
        for (size_t i = 0; i < Rows; ++i) {
            m(i, i) = T{1};
        }
        return m;
    }

    static constexpr Matrix Zero() {
        return Matrix{};
    }

    constexpr T& operator()(size_t r, size_t c) {
        return elements[r * Cols + c];
    }

    constexpr const T& operator()(size_t r, size_t c) const {
        return elements[r * Cols + c];
    }

    constexpr T& operator[](size_t index) {
        return elements[index];
    }

    constexpr const T& operator[](size_t index) const {
        return elements[index];
    }

    constexpr Vector<Cols, T> row(size_t r) const {
        Vector<Cols, T> result;
        for (size_t c = 0; c < Cols; ++c) {
            result[c] = (*this)(r, c);
        }
        return result;
    }

    constexpr Vector<Rows, T> col(size_t c) const {
        Vector<Rows, T> result;
        for (size_t r = 0; r < Rows; ++r) {
            result[r] = (*this)(r, c);
        }
        return result;
    }

    constexpr auto operator+(const Matrix& other) const {
        Matrix result;
        for (size_t i = 0; i < elements.size(); ++i) result[i] = elements[i] + other[i];
        return result;
    }

    constexpr auto operator-(const Matrix& other) const {
        Matrix result;
        for (size_t i = 0; i < elements.size(); ++i) result[i] = elements[i] - other[i];
        return result;
    }

    constexpr auto operator*(auto scalar) const {
        using ResultT = decltype(elements[0] * scalar);
        Matrix<Rows, Cols, ResultT> result;
        for (size_t i = 0; i < elements.size(); ++i) result[i] = elements[i] * scalar;
        return result;
    }

    constexpr auto operator/(auto scalar) const {
        using ResultT = decltype(elements[0] / scalar);
        Matrix<Rows, Cols, ResultT> result;
        for (size_t i = 0; i < elements.size(); ++i) result[i] = elements[i] / scalar;
        return result;
    }

    constexpr auto& operator+=(const Matrix& other) {
        for (size_t i = 0; i < elements.size(); ++i) elements[i] += other[i];
        return *this;
    }

    constexpr auto& operator-=(const Matrix& other) {
        for (size_t i = 0; i < elements.size(); ++i) elements[i] -= other[i];
        return *this;
    }

    constexpr auto& operator*=(auto scalar) {
        for (size_t i = 0; i < elements.size(); ++i) elements[i] *= scalar;
        return *this;
    }

    template <size_t OtherCols>
    constexpr auto operator*(const Matrix<Cols, OtherCols, T>& other) const {
        Matrix<Rows, OtherCols, T> result;
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < OtherCols; ++c) {
                T sum = T{0};
                for (size_t k = 0; k < Cols; ++k) {
                    sum += (*this)(r, k) * other(k, c);
                }
                result(r, c) = sum;
            }
        }
        return result;
    }

    constexpr Vector<Rows, T> operator*(const Vector<Cols, T>& vec) const {
        Vector<Rows, T> result;
        for (size_t r = 0; r < Rows; ++r) {
            T sum = T{0};
            for (size_t k = 0; k < Cols; ++k) {
                sum += (*this)(r, k) * vec[k];
            }
            result[r] = sum;
        }
        return result;
    }

    constexpr Matrix<Cols, Rows, T> transposed() const {
        Matrix<Cols, Rows, T> result;
        for (size_t r = 0; r < Rows; ++r) {
            for (size_t c = 0; c < Cols; ++c) {
                result(c, r) = (*this)(r, c);
            }
        }
        return result;
    }

    constexpr auto componentWiseMul(const Matrix& other) const {
        Matrix<Rows, Cols, T> result;
        for (size_t i = 0; i < elements.size(); ++i) result[i] = elements[i] * other[i];
        return result;
    }

    constexpr auto determinant() const requires (Rows == 2 && Cols == 2) {
        const auto& m = *this;
        return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    }

    constexpr auto determinant() const requires (Rows == 3 && Cols == 3) {
        const auto& m = *this;
        return m(0, 0) * (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) -
            m(0, 1) * (m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0)) +
            m(0, 2) * (m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0));
    }

    friend std::ostream& operator<<(std::ostream& os, const Matrix& mat) {
        os << "{\n";
        for (size_t r = 0; r < Rows; ++r) {
            os << "  {";
            for (size_t c = 0; c < Cols; ++c) {
                os << mat(r, c) << (c < Cols - 1 ? ", " : "");
            }
            os << "}" << (r < Rows - 1 ? ",\n" : "\n");
        }
        return os << "}";
    }

    auto operator<=>(const Matrix&) const = default;

    friend auto operator*(auto scalar, const Matrix& mat) {
        return mat * scalar;
    }
};

template <typename T> using Matrix2x2 = Matrix<2, 2, T>;
template <typename T> using Matrix3x3 = Matrix<3, 3, T>;
template <typename T> using Matrix4x4 = Matrix<4, 4, T>;

template <size_t N, typename T> using SquareMatrix = Matrix<N, N, T>;
