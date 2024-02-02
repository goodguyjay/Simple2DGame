//
// Created by jay on 1/22/24.
//

#include "Vec2.hpp"
#include <cmath>

Vec2::Vec2() {}

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin) {

}

Vec2 Vec2::operator + (const Vec2 &rhs) const {
    return {x + rhs.x, y + rhs.y};
}

Vec2 Vec2::operator - (const Vec2&rhs) const {
    return {x - rhs.x, y - rhs.y};
}


Vec2 Vec2::operator / (const float val) const {
    return {x / val, y / val};
}

Vec2 Vec2::operator * (const float val) const {
    return {x * val, y * val};
}

bool Vec2::operator == (const Vec2 &rhs) const {
    return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator != (const Vec2 &rhs) const {
    return (x != rhs.x && y != rhs.y);
}

void Vec2::operator += (const Vec2 &rhs) {
    this->x += rhs.x;
    this->y += rhs.y;
}

void Vec2::operator -= (const Vec2 &rhs) {
    this->x -= rhs.x;
    this->y -= rhs.y;
}

void Vec2::operator *= (const float val) {
    this->x *= val;
    this->y *= val;
}

void Vec2::operator /= (const float val) {
    this->x /= val;
    this->y /= val;
}

float Vec2::dist(const Vec2 &rhs) const {
    return sqrtf((powf(x - rhs.x, 2)) + powf(y - rhs.y, 2));
}

float Vec2::length() const {
    return sqrtf(x * x + y * y);
}
