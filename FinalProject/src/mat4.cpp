/**
 * File: Mat4.cpp
 * Author: Spencer Phippen
 *
 * Contains function definitions for Mat4.
 */

#include "Mat4.h"

#include <cmath>
#include "Vec3.h"

Mat4::Mat4() { }

// Note the order of the initializations: this is intentional
Mat4::Mat4(float scalar) : m11(scalar), m12(scalar), m13(scalar), m14(scalar),
                            m21(scalar), m22(scalar), m23(scalar), m24(scalar),
                            m31(scalar), m32(scalar), m33(scalar), m34(scalar),
                            m41(scalar), m42(scalar), m43(scalar), m44(scalar) { }

Mat4::Mat4(float m11, float m21, float m31, float m41,
            float m12, float m22, float m32, float m42,
            float m13, float m23, float m33, float m43,
            float m14, float m24, float m34, float m44) :
            m11(m11), m12(m12), m13(m13), m14(m14),
            m21(m21), m22(m22), m23(m23), m24(m24),
            m31(m31), m32(m32), m33(m33), m34(m34),
            m41(m41), m42(m42), m43(m43), m44(m44) { }

Mat4 Mat4::add(const Mat4& m) const
{
  return Mat4(m11 + m.m11, m21 + m.m21, m31 + m.m31, m41 + m.m41,
              m12 + m.m12, m22 + m.m22, m32 + m.m32, m42 + m.m42,
              m13 + m.m13, m23 + m.m23, m33 + m.m33, m43 + m.m43,
              m14 + m.m14, m24 + m.m24, m34 + m.m34, m44 + m.m44);
}

Mat4 Mat4::multiply(const Mat4& m) const
{
  Mat4 toReturn;
  
  toReturn.m11 = m11 * m.m11 + m21 * m.m12 + m31 * m.m13 + m41 * m.m14;
  toReturn.m21 = m11 * m.m21 + m21 * m.m22 + m31 * m.m23 + m41 * m.m24;
  toReturn.m31 = m11 * m.m31 + m21 * m.m32 + m31 * m.m33 + m41 * m.m34;
  toReturn.m41 = m11 * m.m41 + m21 * m.m42 + m31 * m.m43 + m41 * m.m44;
    
  toReturn.m12 = m12 * m.m11 + m22 * m.m12 + m32 * m.m13 + m42 * m.m14;
  toReturn.m22 = m12 * m.m21 + m22 * m.m22 + m32 * m.m23 + m42 * m.m24;
  toReturn.m32 = m12 * m.m31 + m22 * m.m32 + m32 * m.m33 + m42 * m.m34;
  toReturn.m42 = m12 * m.m41 + m22 * m.m42 + m32 * m.m43 + m42 * m.m44;
    
  toReturn.m13 = m13 * m.m11 + m23 * m.m12 + m33 * m.m13 + m43 * m.m14;
  toReturn.m23 = m13 * m.m21 + m23 * m.m22 + m33 * m.m23 + m43 * m.m24;
  toReturn.m33 = m13 * m.m31 + m23 * m.m32 + m33 * m.m33 + m43 * m.m34;
  toReturn.m43 = m13 * m.m41 + m23 * m.m42 + m33 * m.m43 + m43 * m.m44;
    
  toReturn.m14 = m14 * m.m11 + m24 * m.m12 + m34 * m.m13 + m44 * m.m14;
  toReturn.m24 = m14 * m.m21 + m24 * m.m22 + m34 * m.m23 + m44 * m.m24;
  toReturn.m34 = m14 * m.m31 + m24 * m.m32 + m34 * m.m33 + m44 * m.m34;
  toReturn.m44 = m14 * m.m41 + m24 * m.m42 + m34 * m.m43 + m44 * m.m44;
    
  return toReturn;
}

Vec3 Mat4::multiply(const Vec3& v) const
{
  Vec3 toReturn;
    
  toReturn.x = m11 * v.x + m21 * v.y + m31 * v.z;
  toReturn.y = m12 * v.x + m22 * v.y + m32 * v.z;
  toReturn.z = m13 * v.x + m23 * v.y + m33 * v.z;
    
  return toReturn;
}

Mat4 Mat4::operator+(const Mat4& m) const
{
  return add(m);
}

Mat4 Mat4::operator*(const Mat4& m) const
{
  return multiply(m);
}

Vec3 Mat4::operator*(const Vec3& v) const
{
  return multiply(v);
}

Mat4 Mat4::rotationAboutXMatrix(float theta)
{
  return Mat4(1.0f,        0.0f,         0.0f, 0.0f,
              0.0f, cosf(theta), -sinf(theta), 0.0f,
              0.0f, sinf(theta),  cosf(theta), 0.0f,
              0.0f,        0.0f,         0.0f, 1.0f);
}

Mat4 Mat4::rotationAboutYMatrix(float theta)
{
  return Mat4( cosf(theta), 0.0f, sinf(theta), 0.0f,
                      0.0f, 1.0f,        0.0f, 0.0f,
              -sinf(theta), 0.0f, cosf(theta), 0.0f,
                      0.0f, 0.0f,        0.0f, 1.0f);
}

Mat4 Mat4::rotationAboutZMatrix(float theta)
{
  return Mat4(cosf(theta), -sinf(theta), 0.0f, 0.0f,
              sinf(theta),  cosf(theta), 0.0f, 0.0f,
                      0.0f,         0.0f, 1.0f, 0.0f,
                      0.0f,         0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::rotationAboutAxisMatrix(float theta, const Vec3& axis)
{
  float alpha = atan2(axis.x, axis.y);
  float beta = atan2(sqrt(axis.x * axis.x + axis.y + axis.y), axis.z);
    
  if (axis.x == 0.0f && axis.y == 0.0f)
  {
    alpha = 0.0f;
    if (axis.z == 0.0f)
    {
      beta = 0.0f;
      theta = 0.0f;
    }
  }
    
  Mat4 m = identityMatrix();
    
  m = m.multiply(rotationAboutZMatrix(-alpha));
  m = m.multiply(rotationAboutXMatrix(-beta));
  m = m.multiply(rotationAboutZMatrix(theta));
  m = m.multiply(rotationAboutXMatrix(beta));
  m = m.multiply(rotationAboutZMatrix(alpha));
    
  return m;
}

Mat4 Mat4::scalingMatrix(float sx, float sy, float sz)
{
  return Mat4(  sx, 0.0f, 0.0f, 0.0f,
              0.0f,   sy, 0.0f, 0.0f,
              0.0f, 0.0f,   sz, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::scalingInvtMatrix(float sx, float sy, float sz)
{
  return Mat4(1/sx, 0.0f, 0.0f, 0.0f,
              0.0f, 1/sy, 0.0f, 0.0f,
              0.0f, 0.0f, 1/sz, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::translationMatrix(float tx, float ty, float tz)
{
  return Mat4(1.0f, 0.0f, 0.0f,   tx,
              0.0f, 1.0f, 0.0f,   ty,
              0.0f, 0.0f, 1.0f,   tz,
              0.0f, 0.0f, 0.0f, 1.0f);
}

Mat4 Mat4::translationInvtMatrix(float tx, float ty, float tz)
{
  return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
                -tx,  -ty,  -tz, 1.0f);
}
  
void Mat4::lookAtMatrix(const Vec3& eye, const Vec3& spot, const Vec3& up, Mat4& posTransform, Mat4& normalTransform)
{
  Vec3 look = spot - eye;
  Vec3 right = look.cross(up);
  Vec3 nUp = right.cross(look);
    
  look.normalize();
  right.normalize();
  nUp.normalize();
    
  Mat4 toRet(right.x, right.y, right.z, 0.0f,
                nUp.x,   nUp.y,   nUp.z, 0.0f,
              -look.x, -look.y, -look.z, 0.0f,
                0.0f,    0.0f,    0.0f, 1.0f);
    
  posTransform = toRet * translationMatrix(-eye.x, -eye.y, -eye.z);
  normalTransform = toRet * translationInvtMatrix(-eye.x, -eye.y, -eye.z);
}
  
Mat4 Mat4::perspectiveMatrix(float fovY, float aspect, float near, float far)
{
  float y = 1 / tanf(fovY * 0.5f);
  float x = y / aspect;
    
  float z1 = -(far + near) / (far - near);
  float z2 =  (-2.0f * near * far) / (far - near);
    
  return Mat4(x, 0,     0,  0,
              0, y,     0,  0,
              0, 0,    z1, z2,
              0, 0, -1.0f,  0);
}

Mat4 Mat4::perspectiveInvMatrix(float fovY, float aspect, float near, float far)
{
  float y = 1 / tanf(fovY * 0.5f);
  float x = y / aspect;

  float z1 = -(far + near) / (far - near);
  float z2 = (-2.0f * near * far) / (far - near);

  return Mat4(1/x, 0, 0, 0,
              0, 1/y, 0, 0,
              0, 0, 0, -1,
              0, 0, 1/z2, (far + near) / (2 * far * near));
}

Mat4 Mat4::viewportMatrix(float x, float y, float width, float height)
{
  Mat4 toRet = identityMatrix();
    
  toRet = toRet.multiply(Mat4::translationMatrix(x, y, 0.0f));
  toRet = toRet.multiply(Mat4::scalingMatrix(0.5f * width, 0.5f * height, 0.5f));
  toRet = toRet.multiply(Mat4::translationMatrix(1.0f, 1.0f, 1.0f));
    
  return toRet;
}

Mat4 Mat4::identityMatrix()
{
  return Mat4(1.0f, 0.0f, 0.0f, 0.0f,
              0.0f, 1.0f, 0.0f, 0.0f,
              0.0f, 0.0f, 1.0f, 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f);
}
