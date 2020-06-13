#pragma once

#include "Types.h"
#include "vkr_Assert.h"

namespace vkr
{

//------------------------------------------------------------------------------
inline uint Max(uint a, uint b)
{
    return a > b ? a : b;
}

//------------------------------------------------------------------------------
inline uint Align(uint x, uint align)
{
    vkr_assert(align);
    return ((x + align - 1) / align) * align;
}

//------------------------------------------------------------------------------
struct Vec4
{
    union
    {
        float v[4];
        struct
        {
            float x, y, z, w;
        };
    };

    Vec4() = default;
    constexpr Vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

//------------------------------------------------------------------------------
struct Vec2
{
    union
    {
        float v[2];
        struct
        {
            float x, y;
        };
    };

    Vec2() = default;
    Vec2(float x, float y) : x(x), y(y) {}
};

//------------------------------------------------------------------------------
inline Vec2 operator*(float f, Vec2 v)
{
    return Vec2(v.x * f, v.y * f);
}

//------------------------------------------------------------------------------
inline Vec2 operator*(Vec2 v, float f)
{
    return f * v;
}

//------------------------------------------------------------------------------
inline Vec2 operator+(Vec2 a, Vec2 b)
{
    return Vec2(a.x + b.x, a.y + b.y);
}

//------------------------------------------------------------------------------
inline Vec2 operator-(Vec2 a)
{
    return Vec2(-a.x , -a.y);
}

//------------------------------------------------------------------------------
inline Vec2 operator-(Vec2 a, Vec2 b)
{
    return a + (-b);
}

//------------------------------------------------------------------------------
// Matrix is saved as column major, meaning that the vector at m[0] is the first
// column of the matrix, m[1] is the second column etc.
// The first row is given by Vec4(m[0][0], m[1][0], m[2][0], m[3][0])
// This means that the internal indexing is reversed compared to mathematical
// convention where mat[i][j] gives ith row and jth column. For that we have
// operator()
//------------------------------------------------------------------------------
struct Mat44
{
    static Mat44 Identity()
    {
        return Mat44
        (
            Vec4(1, 0, 0, 0),
            Vec4(0, 1, 0, 0),
            Vec4(0, 0, 1, 0),
            Vec4(0, 0, 0, 1)
        );
    }

    float m[4][4];

    //------------------------------------------------------------------------------
    Mat44() = default;
    //------------------------------------------------------------------------------
    Mat44(const Vec4& a, const Vec4& b, const Vec4& c, const Vec4& d)
    {
        m[0][0] = a.x; m[0][1] = a.y; m[0][2] = a.z; m[0][3] = a.w;
        m[1][0] = b.x; m[1][1] = b.y; m[1][2] = b.z; m[1][3] = b.w;
        m[2][0] = c.x; m[2][1] = c.y; m[2][2] = c.z; m[2][3] = c.w;
        m[3][0] = d.x; m[3][1] = d.y; m[3][2] = d.z; m[3][3] = d.w;
    }

    //------------------------------------------------------------------------------
    const Vec4& GetPosition() const
    {
        return *reinterpret_cast<const Vec4*>(m[3]);
    }

    //------------------------------------------------------------------------------
    constexpr float operator()(int i, int j) const
    {
        return m[j][i];
    }

    //------------------------------------------------------------------------------
    constexpr float& operator()(int i, int j)
    {
        return m[j][i];
    }
};

//------------------------------------------------------------------------------
inline Mat44 operator*(const Mat44& a, const Mat44& b)
{
    Mat44 out;

    out(0, 0) = a(0, 0) * b(0, 0) + a(0, 1) * b(1, 0) + a(0, 2) * b(2, 0) + a(0, 3) * b(3, 0);
    out(1, 0) = a(1, 0) * b(0, 0) + a(1, 1) * b(1, 0) + a(1, 2) * b(2, 0) + a(1, 3) * b(3, 0);
    out(2, 0) = a(2, 0) * b(0, 0) + a(2, 1) * b(1, 0) + a(2, 2) * b(2, 0) + a(2, 3) * b(3, 0);
    out(3, 0) = a(3, 0) * b(0, 0) + a(3, 1) * b(1, 0) + a(3, 2) * b(2, 0) + a(3, 3) * b(3, 0);

    out(0, 1) = a(0, 0) * b(0, 1) + a(0, 1) * b(1, 1) + a(0, 2) * b(2, 1) + a(0, 3) * b(3, 1);
    out(1, 1) = a(1, 0) * b(0, 1) + a(1, 1) * b(1, 1) + a(1, 2) * b(2, 1) + a(1, 3) * b(3, 1);
    out(2, 1) = a(2, 0) * b(0, 1) + a(2, 1) * b(1, 1) + a(2, 2) * b(2, 1) + a(2, 3) * b(3, 1);
    out(3, 1) = a(3, 0) * b(0, 1) + a(3, 1) * b(1, 1) + a(3, 2) * b(2, 1) + a(3, 3) * b(3, 1);

    out(0, 2) = a(0, 0) * b(0, 2) + a(0, 1) * b(1, 2) + a(0, 2) * b(2, 2) + a(0, 3) * b(3, 2);
    out(1, 2) = a(1, 0) * b(0, 2) + a(1, 1) * b(1, 2) + a(1, 2) * b(2, 2) + a(1, 3) * b(3, 2);
    out(2, 2) = a(2, 0) * b(0, 2) + a(2, 1) * b(1, 2) + a(2, 2) * b(2, 2) + a(2, 3) * b(3, 2);
    out(3, 2) = a(3, 0) * b(0, 2) + a(3, 1) * b(1, 2) + a(3, 2) * b(2, 2) + a(3, 3) * b(3, 2);

    out(0, 3) = a(0, 0) * b(0, 3) + a(0, 1) * b(1, 3) + a(0, 2) * b(2, 3) + a(0, 3) * b(3, 3);
    out(1, 3) = a(1, 0) * b(0, 3) + a(1, 1) * b(1, 3) + a(1, 2) * b(2, 3) + a(1, 3) * b(3, 3);
    out(2, 3) = a(2, 0) * b(0, 3) + a(2, 1) * b(1, 3) + a(2, 2) * b(2, 3) + a(2, 3) * b(3, 3);
    out(3, 3) = a(3, 0) * b(0, 3) + a(3, 1) * b(1, 3) + a(3, 2) * b(2, 3) + a(3, 3) * b(3, 3);

    return out;
}

//------------------------------------------------------------------------------
inline Vec4 operator*(const Mat44& m, const Vec4& v)
{
    Vec4 out;

    out.x = v.x * m(0, 0) + v.y * m(0, 1) + v.z * m(0, 2) + v.w * m(0, 3);
    out.y = v.x * m(1, 0) + v.y * m(1, 1) + v.z * m(1, 2) + v.w * m(1, 3);
    out.z = v.x * m(2, 0) + v.y * m(2, 1) + v.z * m(2, 2) + v.w * m(2, 3);
    out.w = v.x * m(3, 0) + v.y * m(3, 1) + v.z * m(3, 2) + v.w * m(3, 3);

    return out;
}

}
