/*
* Unit.hpp
 * A header-only C++20 library for compile-time dimensional analysis and unit conversion.
 *
 * Version: 0.14
 * Author:  OguzhanUmutlu
 * GitHub:  https://github.com/OguzhanUmutlu/unit.hpp
 *
 * Licensed under the MIT License.
 */

#pragma once
#include <iostream>
#include <cmath>

#include "Unit.hpp"

template <typename T>
struct Vector3 {
    T x;
    T y;
    T z;

    constexpr Vector3() = default;

    constexpr Vector3(T x, T y, T z) : x(x), y(y), z(z) {
    }

    constexpr Vector3(const Vector3& other) = default;

    constexpr auto operator+(const Vector3& other) const {
        return Vector3{x + other.x, y + other.y, z + other.z};
    }

    constexpr auto operator-(const Vector3& other) const {
        return Vector3{x - other.x, y - other.y, z - other.z};
    }

    constexpr auto operator*(auto scalar) const {
        return Vector3<decltype(x * scalar)>{x * scalar, y * scalar, z * scalar};
    }

    constexpr auto operator/(auto scalar) const {
        return Vector3<decltype(x / scalar)>{x / scalar, y / scalar, z / scalar};
    }

    constexpr auto operator+() const {
        return *this;
    }

    constexpr auto operator-() const {
        return Vector3{-x, -y, -z};
    }

    constexpr auto& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    constexpr auto& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    constexpr auto& operator*=(auto scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    constexpr auto& operator/=(auto scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    template <typename U>
    constexpr explicit operator Vector3<U>() const {
        return Vector3<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
    }

    constexpr auto dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    constexpr auto cross(const Vector3& other) const {
        return Vector3{
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    constexpr auto componentWiseMul(const Vector3& other) const {
        return Vector3{x * other.x, y * other.y, z * other.z};
    }

    constexpr auto componentWiseDiv(const Vector3& other) const {
        return Vector3{x / other.x, y / other.y, z / other.z};
    }

    constexpr auto length() const {
        if constexpr (requires { sqrt(lengthSquared()); }) {
            return sqrt(lengthSquared());
        } else {
            return T{sqrt(static_cast<double>(lengthSquared()))};
        }
    }

    constexpr auto lengthSquared() const {
        return x * x + y * y + z * z;
    }

    constexpr auto normalized() const {
        return *this / length();
    }

    constexpr auto angleTo(const Vector3& other) const {
        auto den = length() * other.length();
        if (den == 0) return Unit::defaults::rad{0};

        auto val = dot(other) / den;
        if (val > 1.0) val = 1.0;
        if (val < -1.0) val = -1.0;

        return Unit::defaults::rad{acos(val)};
    }

    constexpr Vector3 rotatedBy(auto angle, const Vector3& axis) const {
        auto cos_phi = Unit::defaults::cos(angle);
        auto sin_phi = Unit::defaults::sin(angle);

        auto k = axis;
        auto v = *this;

        return v * cos_phi + k.cross(v) * sin_phi + k * (k.dot(v)) * (1.0 - cos_phi);
    }

    constexpr auto projectedOnto(const Vector3& axis) const {
        auto axis_length_squared = axis.lengthSquared();
        return (dot(axis) / axis_length_squared) * axis;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
        return os << "{" << vec.x << ", " << vec.y << ", " << vec.z << "}";
    }

    auto operator<=>(const Vector3&) const = default;

    friend auto operator*(auto scalar, const Vector3& vec) {
        return vec * scalar;
    }
};
