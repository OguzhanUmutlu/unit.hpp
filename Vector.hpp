#pragma once
#include <array>
#include <cmath>
#include <iostream>

#include "Unit.hpp"

template <size_t N, typename T>
struct Vector {
    std::array<T, N> components;

    constexpr Vector() : components{} {
    }

    template <typename... Args>
    constexpr Vector(Args... args) : components{static_cast<T>(args)...} {
        static_assert(sizeof...(args) == N, "Number of arguments must match Vector size.");
    }

    constexpr explicit Vector(const std::array<T, N>& arr) : components(arr) {
    }

    constexpr T& operator[](size_t index) {
        return components[index];
    }

    constexpr const T& operator[](size_t index) const {
        return components[index];
    }

    constexpr T& x() requires (N >= 1) {
        return components[0];
    }

    constexpr const T& x() const requires (N >= 1) {
        return components[0];
    }

    constexpr T& y() requires (N >= 2) {
        return components[1];
    }

    constexpr const T& y() const requires (N >= 2) {
        return components[1];
    }

    constexpr T& z() requires (N >= 3) {
        return components[2];
    }

    constexpr const T& z() const requires (N >= 3) {
        return components[2];
    }

    constexpr T& w() requires (N >= 4) {
        return components[3];
    }

    constexpr const T& w() const requires (N >= 4) {
        return components[3];
    }

    constexpr auto operator+(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] + other[i];
        return result;
    }

    constexpr auto operator-(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] - other[i];
        return result;
    }

    constexpr auto operator*(auto scalar) const {
        using ResultT = decltype(components[0] * scalar);
        Vector<N, ResultT> result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] * scalar;
        return result;
    }

    constexpr auto operator/(auto scalar) const {
        using ResultT = decltype(components[0] / scalar);
        Vector<N, ResultT> result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] / scalar;
        return result;
    }

    constexpr auto operator+() const {
        return *this;
    }

    constexpr auto operator-() const {
        Vector<N, T> result;
        for (size_t i = 0; i < N; ++i) result[i] = -components[i];
        return result;
    }

    constexpr auto& operator+=(const Vector& other) {
        for (size_t i = 0; i < N; ++i) components[i] += other[i];
        return *this;
    }

    constexpr auto& operator-=(const Vector& other) {
        for (size_t i = 0; i < N; ++i) components[i] -= other[i];
        return *this;
    }

    constexpr auto& operator*=(auto scalar) {
        for (size_t i = 0; i < N; ++i) components[i] *= scalar;
        return *this;
    }

    constexpr auto& operator/=(auto scalar) {
        for (size_t i = 0; i < N; ++i) components[i] /= scalar;
        return *this;
    }

    template <typename U>
    constexpr explicit operator Vector<N, U>() const {
        Vector<N, U> result;
        for (size_t i = 0; i < N; ++i) result[i] = static_cast<U>(components[i]);
        return result;
    }

    constexpr auto dot(const Vector& other) const {
        T sum = T{0};
        for (size_t i = 0; i < N; ++i) sum += components[i] * other[i];
        return sum;
    }

    constexpr auto componentWiseMul(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] * other[i];
        return result;
    }

    constexpr auto componentWiseDiv(const Vector& other) const {
        Vector result;
        for (size_t i = 0; i < N; ++i) result[i] = components[i] / other[i];
        return result;
    }

    constexpr auto lengthSquared() const {
        return dot(*this);
    }

    constexpr auto length() const {
        if constexpr (requires { std::sqrt(lengthSquared()); }) {
            return std::sqrt(lengthSquared());
        } else {
            return T{std::sqrt(static_cast<double>(lengthSquared()))};
        }
    }

    constexpr auto normalized() const {
        return *this / length();
    }

    constexpr auto angleTo(const Vector& other) const {
        auto den = std::sqrt(lengthSquared() * other.lengthSquared());
        if (den < 1e-9) return Unit::defaults::rad{0};

        auto val = dot(other) / den;
        if (val > 1.0) val = 1.0;
        if (val < -1.0) val = -1.0;

        return Unit::defaults::rad{std::acos(val)};
    }

    constexpr auto projectedOnto(const Vector& axis) const {
        auto axis_len_sq = axis.lengthSquared();
        if (axis_len_sq == 0) return Vector{};
        return (dot(axis) / axis_len_sq) * axis;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec) {
        os << "{";
        for (size_t i = 0; i < N; ++i) {
            os << vec[i] << (i < N - 1 ? ", " : "");
        }
        return os << "}";
    }

    auto operator<=>(const Vector&) const = default;

    friend auto operator*(auto scalar, const Vector& vec) {
        return vec * scalar;
    }
};
