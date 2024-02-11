//
// Created by jay on 1/22/24.
//

#ifndef VEC2_HPP
#define VEC2_HPP



class Vec2 {
public:
    float x = 0;
    float y = 0;

    Vec2();
    Vec2(float xin, float yin);

    bool operator == (const Vec2 &rhs) const;
    bool operator != (const Vec2 &rhs) const;

    Vec2 operator + (const Vec2 &rhs) const;
    Vec2 operator - (const Vec2 &rhs) const;
    Vec2 operator / (float val) const;
    Vec2 operator * (float val) const;

    void operator += (const Vec2 &rhs);
    void operator -= (const Vec2 &rhs);
    void operator *= (const float val);
    void operator /= (const float val);

    [[nodiscard]] float dist(const Vec2 &rhs) const;
    [[nodiscard]] float length() const;
};



#endif //VEC2_HPP
