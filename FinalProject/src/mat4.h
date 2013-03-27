/**
 * File: Mat4.h
 * Author: Spencer Phippen
 *
 * Contains class definition for Mat4, a 4 by 4 matrix, and related functions.
 */


#ifndef _SPH_MAT4_H_
#define _SPH_MAT4_H_

class Vec3;

/**
  * Represents a 4 by 4 matrix.
  * This representation is column-major,
  * i.e. "m13" is the value in the first column and third row.
  */
class Mat4
{
public:
  
  /**
    * Creates a matrix without initializing any elements.
    */
  Mat4();
    
  /**
    * Creates a matrix with every element set to "scalar".
    */
  explicit Mat4(float scalar);
    
  /**
    * Creates a matrix with m11 set to "m11", m21 set to "m21", etc.
    */
  Mat4(float m11, float m21, float m31, float m41,
        float m12, float m22, float m32, float m42,
        float m13, float m23, float m33, float m43,
        float m14, float m24, float m34, float m44);
    
  /**
    * Returns the matrix created by adding this and m.
    */
  Mat4 add(const Mat4& m) const;
    
  /**
    * Returns the matrix created by multiplying this with m,
    * i.e. this * m (notice the order).
    */
  Mat4 multiply(const Mat4& m) const;

  /**
    * Returns the vector created by multiplying this with v,
    * i.e. this * v.
    */
  Vec3 multiply(const Vec3& p) const;
    
  /**
    * See add(Mat4 m).
    */
  Mat4 operator+(const Mat4& m) const;
    
  /**
    * See multiply(Mat4 m).
    */
  Mat4 operator*(const Mat4& m) const;

  /**
    * See multiply(const Vec3& v).
    */
  Vec3 operator*(const Vec3& v) const;
    
  /**
    * The 16 components of a 4 by 4 matrix,
    * column major indexing.
    * Order is important for memory layout.
    */
  float m11;
  float m12;
  float m13;
  float m14;
  float m21;
  float m22;
  float m23;
  float m24;
  float m31;
  float m32;
  float m33;
  float m34;
  float m41;
  float m42;
  float m43;
  float m44;

  /**
    * Returns a transformation matrix that rotates
    * "theta" radians about the axis given in the function
    * name, or "axis" in "rotationAboutAxis".
    */
  static Mat4 rotationAboutXMatrix(float theta);
  static Mat4 rotationAboutYMatrix(float theta);
  static Mat4 rotationAboutZMatrix(float theta);
  static Mat4 rotationAboutAxisMatrix(float theta, const Vec3& axis);
    
  /**
    * Returns a transformation matrix that scales
    * by "sx" in the x direction, "sy" in the y direction,
    * and "sz" in the z direction.
    */
  static Mat4 scalingMatrix(float sx, float sy, float sz);
  static Mat4 scalingInvtMatrix(float sx, float sy, float sz);
    
  /**
    * Returns a transformation matrix that translates
    * by "tx" in the x direction, "ty" in the y direction,
    * and "tz" in the z direction.
    */
  static Mat4 translationMatrix(float tx, float ty, float tz);
  static Mat4 translationInvtMatrix(float tx, float ty, float tz);
    
  /**
    * Returns a transformation matrix that orients the
    * coordinate system with the specified viewer ending
    * up at the origin, looking down the -z axis, with
    * the y-axis as the up vector.
    */
  static void lookAtMatrix(const Vec3& eye, const Vec3& spot, const Vec3& up, Mat4& posTransform, Mat4& normalTransform);
    
  /**
    * Returns a transformation matrix that applies a perspective
    * projection with the specified field of view in Y, aspect ratio,
    * and near and far planes.
    */
  static Mat4 perspectiveMatrix(float fovY, float aspect, float near, float far);
  static Mat4 perspectiveInvMatrix(float fovY, float aspect, float near, float far);
  /**
    * Returns a transformation matrix that transforms points in canonical coordinates
    * to the specified width and height, with the old origin now at (x, y).
    */
  static Mat4 viewportMatrix(float x, float y, float width, float height);

  /**
    * Returns the identity matrix.
    */
  static Mat4 identityMatrix();
};

#endif
