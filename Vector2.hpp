#pragma once
#include <iostream>

#include "Unit.hpp"

template <typename T>
struct Vector2 {
    T x;
    T y;

    constexpr Vector2() = default;

    constexpr Vector2(T x, T y) : x(x), y(y) {
    }

    constexpr Vector2(const Vector2& other) = default;

    constexpr auto operator+(const Vector2& other) const {
        return Vector2{x + other.x, y + other.y};
    }

    constexpr auto operator-(const Vector2& other) const {
        return Vector2{x - other.x, y - other.y};
    }

    constexpr auto operator*(auto scalar) const {
        return Vector2<decltype(x * scalar)>{x * scalar, y * scalar};
    }

    constexpr auto operator/(auto scalar) const {
        return Vector2<decltype(x / scalar)>{x / scalar, y / scalar};
    }

    constexpr auto operator+() const {
        return *this;
    }

    constexpr auto operator-() const {
        return Vector2{-x, -y};
    }

    constexpr auto& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    constexpr auto& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    constexpr auto& operator*=(auto scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    constexpr auto& operator/=(auto scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    template <typename U>
    constexpr explicit operator Vector2<U>() const {
        return Vector2<U>{static_cast<U>(x), static_cast<U>(y)};
    }

    constexpr auto perpendicular() const {
        return Vector2{-y, x};
    }

    constexpr auto dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    constexpr auto cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }

    constexpr auto componentWiseMul(const Vector2& other) const {
        return Vector2{x * other.x, y * other.y};
    }

    constexpr auto componentWiseDiv(const Vector2& other) const {
        return Vector2{x / other.x, y / other.y};
    }

    constexpr auto length() const {
        if constexpr (requires { sqrt(lengthSquared()); }) {
            return sqrt(lengthSquared());
        } else {
            return T{sqrt(static_cast<double>(lengthSquared()))};
        }
    }

    constexpr auto lengthSquared() const {
        return x * x + y * y;
    }

    constexpr auto normalized() const {
        return *this / length();
    }

    constexpr auto angleTo(const Vector2& other) const {
        return Unit::defaults::rad{atan2(other.y - y, other.x - x)};
    }

    constexpr auto angle() const {
        return Unit::defaults::rad{atan2(y, x)};
    }

    constexpr Vector2 rotatedBy(auto phi) const {
        auto cos_phi = Unit::defaults::cos(phi);
        auto sin_phi = Unit::defaults::sin(phi);
        return {
            x * cos_phi - y * sin_phi,
            x * sin_phi + y * cos_phi
        };
    }

    constexpr auto projectedOnto(const Vector2& axis) const {
        auto axis_length_squared = axis.lengthSquared();
        return (dot(axis) / axis_length_squared) * axis;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
        return os << "{" << vec.x << ", " << vec.y << "}";
    }

    auto operator<=>(const Vector2&) const = default;

    friend auto operator*(auto scalar, const Vector2& vec) {
        return vec * scalar;
    }
};
