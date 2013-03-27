/**
 * File: Vec3.h
 * Author: Spencer Phippen
 *
 * Contains class definition for Vec3, a 3-component vector.
 */

#ifndef _SPH_VEC3_H_
#define _SPH_VEC3_H_

/**
  * Represents a vector with 3 components.
  */
class Vec3
{
public:
  
  /**
    * Creates a vector initializing all components to 0.
    */
  Vec3();
    
  /**
    * Creates a vector with all three components set to "scalar".
    */
  explicit Vec3(float scalar);
    
  /**
    * Creates a vector with the specified components.
    */
  Vec3(float x, float y, float z);
    
  /**
    * Returns the 2-norm of this vector.
    */
  float norm() const;
    
  /**
    * Scales x, y, z such that this->norm() == 1.0f.
    * Behavior undefined when x == y == z == 0.
    */
  void normalize();

  /**
    * Returns a noramalized version of this vector.
    * Behavior undefined when x == y == z == 0.
    */
  Vec3 normalized() const;

  /**
    * Returns the vector created by adding this and v.
    */
  Vec3 add(const Vec3& v) const;

  /**
    * Returns the vector created by subtracting v from this.
    */
  Vec3 subtract(const Vec3& v) const;

  /**
    * Returns the dot product of this vector with v.
    */
  float dot(const Vec3& v) const;
    
  /**
    * Returns the cross product of this vector with v.
    */
  Vec3 cross(const Vec3& v) const;
    
  /**
    * Returns a version of this vector with the x component
    * scaled by "sx", the y component scaled by "sy", and the z component
    * scaled by "sz".
    */
  Vec3 scale(float sx, float sy, float sz) const;
    
  /**
    * Returns a version of this vector with every component scaled
    * by "scalar".
    */
  Vec3 scale(float scalar) const;

  /**
    * See add(const Vec3& v).
    */
  Vec3 operator+(const Vec3& v) const;
  
  /**
    * See subtract(const Vec3& v).
    */
  Vec3 operator-(const Vec3& v) const;

  /**
    * The x, y, and z components of a 3-component vector.
    */
  float x;
  float y;
  float z;
};

#endif
