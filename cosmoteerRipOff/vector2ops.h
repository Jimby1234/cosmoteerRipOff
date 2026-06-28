#pragma once
#include "raylib.h"
#include <functional>

inline Vector2 operator+(const Vector2& a, const Vector2& b) { return { a.x + b.x, a.y + b.y }; }
inline Vector2 operator-(const Vector2& a, const Vector2& b) { return { a.x - b.x, a.y - b.y }; }
inline Vector2 operator*(const Vector2& a, float s) { return { a.x * s,   a.y * s }; }
inline Vector2 operator*(float s, const Vector2& a) { return { a.x * s,   a.y * s }; }
inline Vector2 operator/(const Vector2& a, float s) { return { a.x / s,   a.y / s }; }

inline Vector2& operator+=(Vector2& a, const Vector2& b) { a.x += b.x; a.y += b.y; return a; }
inline Vector2& operator-=(Vector2& a, const Vector2& b) { a.x -= b.x; a.y -= b.y; return a; }
inline Vector2& operator*=(Vector2& a, float s) { a.x *= s;   a.y *= s;   return a; }
inline Vector2& operator/=(Vector2& a, float s) { a.x /= s;   a.y /= s;   return a; }
inline bool operator==(const Vector2& a, const Vector2& b)
{
    return (int)a.x == (int)b.x && (int)a.y == (int)b.y;
}

namespace std {
    template<>
    struct hash<Vector2> {
        size_t operator()(const Vector2& v) const {
            size_t hx = std::hash<float>{}(v.x);
            size_t hy = std::hash<float>{}(v.y);
            return hx ^ (hy << 1); // combine the two hashes
        }
    };
}