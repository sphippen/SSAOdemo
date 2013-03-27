/**
 * File: Vec3.cpp
 * Author: Spencer Phippen
 *
 * Contains function definitions for the Vec3 class.
 */

#include "Vec3.h"

#include <cmath>

Vec3::Vec3() : x(0.0f), y(0.0f), z(0.0f) { }

Vec3::Vec3(float scalar) : x(scalar), y(scalar), z(scalar) { }

 Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) { }

float Vec3::norm() const
{
  return sqrtf(x*x + y*y + z*z);
}

void Vec3::normalize()
{
  float length = this->norm();
  x /= length;
  y /= length;
  z /= length;
}

Vec3 Vec3::normalized() const
{
  Vec3 toRet(*this);
  toRet.normalize();
  return toRet;
}
  
Vec3 Vec3::add(const Vec3& v) const
{
  return Vec3(x + v.x, y + v.y, z + v.z);
}

Vec3 Vec3::subtract(const Vec3& v) const
{
  return Vec3(x - v.x, y - v.y, z - v.z);
}

float Vec3::dot(const Vec3& v) const
{
  return x*v.x + y*v.y + z*v.z;
}

Vec3 Vec3::cross(const Vec3& v) const
{
  return Vec3(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}

Vec3 Vec3::scale(float sx, float sy, float sz) const
{
  return Vec3(x * sx, y * sy, z * sz);
}

Vec3 Vec3::scale(float scalar) const
{
  return Vec3(x * scalar, y * scalar, z * scalar);
}

Vec3 Vec3::operator+(const Vec3& v) const
{
  return add(v);
}

Vec3 Vec3::operator-(const Vec3& v) const
{
  return subtract(v);
}