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
#include <algorithm>
#include "Vector2.hpp"

template <typename T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    constexpr Rect() : x(0), y(0), width(0), height(0) {
    }

    constexpr Rect(T x, T y, T width, T height)
        : x(x), y(y), width(width), height(height) {
    }

    constexpr Rect(const Vector2<T>& position, const Vector2<T>& size)
        : x(position.x()), y(position.y()), width(size.x()), height(size.y()) {
    }

    constexpr Vector2<T> position() const {
        return Vector2<T>{x, y};
    }

    constexpr Vector2<T> size() const {
        return Vector2<T>{width, height};
    }

    constexpr Vector2<T> center() const {
        return Vector2<T>{x + width / 2, y + height / 2};
    }

    constexpr T left() const {
        return x;
    }

    constexpr T right() const {
        return x + width;
    }

    constexpr T yMin() const {
        return y;
    }

    constexpr T yMax() const {
        return y + height;
    }

    constexpr void setPosition(const Vector2<T>& pos) {
        x = pos.x();
        y = pos.y();
    }

    constexpr void setSize(const Vector2<T>& size) {
        width  = size.x();
        height = size.y();
    }

    constexpr bool contains(const Vector2<T>& point) const {
        return point.x() >= x && point.x() < (x + width) &&
            point.y() >= y && point.y() < (y + height);
    }

    constexpr bool intersects(const Rect& other) const {
        return x < other.x + other.width &&
            x + width > other.x &&
            y < other.y + other.height &&
            y + height > other.y;
    }

    constexpr Rect intersection(const Rect& other) const {
        T newX      = std::max(x, other.x);
        T newY      = std::max(y, other.y);
        T newRight  = std::min(x + width, other.x + other.width);
        T newBottom = std::min(y + height, other.y + other.height);

        if (newRight > newX && newBottom > newY) {
            return Rect(newX, newY, newRight - newX, newBottom - newY);
        }
        return Rect(0, 0, 0, 0);
    }

    constexpr bool operator==(const Rect&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const Rect& rect) {
        return os << "{x: " << rect.x << ", y: " << rect.y
            << ", w: " << rect.width << ", h: " << rect.height << "}";
    }
};
