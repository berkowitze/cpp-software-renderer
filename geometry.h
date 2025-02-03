// Copied from https://github.com/ssloy/tinyrenderer/tree/f6fecb7ad493264ecd15e230411bfb1cca539a12
// (I've already written vector classes before so...)
#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <ostream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t>
struct Vec2
{
  union
  {
    struct
    {
      t u, v;
    };
    struct
    {
      t x, y;
    };
    t raw[2];
  };
  Vec2() : u(0), v(0) {}
  Vec2(t _u, t _v) : u(_u), v(_v) {}
  inline Vec2<t> operator+(const Vec2<t> &V) const { return Vec2<t>(u + V.u, v + V.v); }
  inline Vec2<t> operator-(const Vec2<t> &V) const { return Vec2<t>(u - V.u, v - V.v); }
  inline Vec2<t> operator*(float f) const { return Vec2<t>(u * f, v * f); }
  template <class>
  friend std::ostream &operator<<(std::ostream &s, Vec2<t> &v);
};

template <class t>
struct Vec3
{
  union
  {
    struct
    {
      t x, y, z;
    };
    struct
    {
      t ivert, iuv, inorm;
    };
    t raw[3];
  };
  Vec3() : x(0), y(0), z(0) {}
  Vec3(t _x, t _y, t _z) : x(_x), y(_y), z(_z) {}
  inline Vec3<t> operator^(const Vec3<t> &v) const { return Vec3<t>(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x); }
  inline Vec3<t> operator+(const Vec3<t> &v) const { return Vec3<t>(x + v.x, y + v.y, z + v.z); }
  inline Vec3<t> operator-(const Vec3<t> &v) const { return Vec3<t>(x - v.x, y - v.y, z - v.z); }
  inline Vec3<t> operator*(float f) const { return Vec3<t>(x * f, y * f, z * f); }
  inline t operator*(const Vec3<t> &v) const { return x * v.x + y * v.y + z * v.z; }
  float norm() const { return std::sqrt(x * x + y * y + z * z); }
  Vec3<t> &normalize(t l = 1)
  {
    *this = (*this) * (l / norm());
    return *this;
  }
  template <class>
  friend std::ostream &operator<<(std::ostream &s, Vec3<t> &v);
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int> Vec3i;

template <class t>
std::ostream &operator<<(std::ostream &s, Vec2<t> &v)
{
  s << "(" << v.x << ", " << v.y << ")\n";
  return s;
}

template <class t>
std::ostream &operator<<(std::ostream &s, Vec3<t> &v)
{
  s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
  return s;
}

class Matrix
{
  std::vector<std::vector<float> > m;

public:
  int rows, cols;
  Matrix(int r = 4, int c = 4)
  {
    for (int i = 0; i < r; i++)
    {
      std::vector<float> row;
      for (int j = 0; j < c; j++)
      {
        row.push_back(0.0f);
      }
      m.push_back(row);
    }
    rows = r;
    cols = c;
  };

  std::vector<float> &operator[](const int i)
  {
    return m[i];
  }

  static Matrix identity(int dim)
  {
    Matrix identity_matrix(dim, dim);
    for (int i = 0; i < dim; i++)
    {
      identity_matrix[i][i] = 1.0f;
    }
    return identity_matrix;
  }

  Matrix operator*(const Matrix &A)
  {
    Matrix result(rows, A.cols);
    for (int i = 0; i < A.cols; i++)
    {
      for (int j = 0; j < rows; j++)
      {
        float value = 0;
        for (int k = 0; k < A.rows; k++)
        {
          value += A.m[k][i] * m[j][k];
        }
        result.m[j][i] = value;
      }
    }
    return result;
  }

  Matrix transpose()
  {
    Matrix result(cols, rows);
    for (int i = 0; i < rows; i++)
    {
      for (int j = 0; j < cols; j++)
      {
        result.m[j][i] = m[i][j];
      }
    }
    return result;
  }

  friend std::ostream &operator<<(std::ostream &s, Matrix &m);
};

#endif //__GEOMETRY_H__