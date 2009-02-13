/////////////////////////////////////////////////////////////////////////////
//
// 3D Math Primer for Games and Graphics Development
//
// Matrix4x3.h - Declarations for class Matrix4x3
//
// Visit gamemath.com for the latest version of this file.
//
// For more details, see Matrix4x3.cpp
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __MATRIX4X3_H_INCLUDED__
#define __MATRIX4X3_H_INCLUDED__

/*
class Vector3;
class EulerAngles;
class Quaternion;
class RotationMatrix;
*/
#include "Vector3.h"
#include "EulerAngles.h"
#include "Quaternion.h"
#include "RotationMatrix.h"

/**
 * class Matrix4x3
 *
 * Implement a 4x3 transformation matrix.  This class can represent
 * any 3D affine transformation.
 */

class Matrix4x3 {
public:

// ========== Public data

    /**
     * The values of the matrix.  Basically the upper 3x3 portion
     * contains a linear transformation, and the last row is the
     * translation portion.  See the Matrix4x3.cpp for more
     * details.
     */
    float m11, m12, m13;
    float m21, m22, m23;
    float m31, m32, m33;
    //float tx,  ty,  tz; // original
    Vector3 t;  // modification by zemm, let the translation part be Vector
                // which enables normal vector operations with it

// ========== Public operations

    /**
     * Constructor that setups as identity matrix
     */
    Matrix4x3() {
        identity();
    }

    /**
     * Set to identity matrix
     */
    void identity();

    /**
     * Access the translation portion of the matrix directly
     */
    void zeroTranslation();
    void setTranslation(const Vector3 &d);
    void setupTranslation(const Vector3 &d);

    /**
     * Setup the matrix to perform a specific transforms from parent <->
     * local space, assuming the local space is in the specified position
     * and orientation within the parent space.  The orientation may be
     * specified using either Euler angles, or a rotation matrix
     */
    void setupLocalToParent(const Vector3 &pos, const EulerAngles &orient);
    void setupLocalToParent(const Vector3 &pos, const RotationMatrix &orient);
    void setupParentToLocal(const Vector3 &pos, const EulerAngles &orient);
    void setupParentToLocal(const Vector3 &pos, const RotationMatrix &orient);

    /**
     * Matrix4x3::setupRotate
     *
     * Setup the matrix to perform a rotation about a cardinal axis
     *
     * The axis of rotation is specified using a 1-based index:
     *
     * 1 => rotate about the x-axis
     * 2 => rotate about the y-axis
     * 3 => rotate about the z-axis
     *
     * theta is the amount of rotation, in radians.  The left-hand rule is
     * used to define "positive" rotation.
     *
     * The translation portion is reset.
     *
     * See 8.2.2 for more info.
     */
    void setupRotate(int axis, float theta);

    /**
     * Setup the matrix to perform a rotation about an arbitrary axis
     */
    void setupRotate(const Vector3 &axis, float theta);

    /**
     * Setup the matrix to perform a rotation, given
     * the angular displacement in quaternion form
     */
    void fromQuaternion(const Quaternion &q);

    /**
     * Setup the matrix to perform scale on each axis
     */
    void setupScale(const Vector3 &s);

    /**
     * Setup the matrix to perform scale along an arbitrary axis
     */
    void setupScaleAlongAxis(const Vector3 &axis, float k);

    /**
     * Setup the matrix to perform a shear
     */
    void setupShear(int axis, float s, float t);

    /**
	   * Setup the matrix to perform a projection onto a plane passing
	   * through the origin
     */
    void setupProject(const Vector3 &n);

    /**
     * Setup the matrix to perform a reflection about a plane parallel
     * to a cardinal plane
     */
    void setupReflect(int axis, float k = 0.0f);

    /**
     * Setup the matrix to perform a reflection about an arbitrary plane
     * through the origin
     */
    void setupReflect(const Vector3 &n);

    /**
     * ----------------------------
     *  Additional methods by maze
     * ----------------------------
     *
     * ..to rather modify existing objects than create new
     *
     */

    /**
     * Transform given vector.
     * After operation, the given vector is transformed.
     *
     * @author  zemm
     * @param v vector to transform
     */
    void transform(Vector3 &v);

    /**
     * @TODO: Note to mazerts: this might not be needed!
     *
     * Transform given vector, place transformation to another.
     * After operation, the "to" vector is transformed.
     *
     * @author      zemm
     * @param from  source vector to translate from
     * @param to    target vector to change to hold the translation
     */
    void transform(Vector3 &from, Vector3 &to);

    /**
     * Concatonate this matrix with another.
     * After operation, this matrix is the concationation of the two.
     *
     * @author  zemm
     * @param m another matrix with which the catonation is done
     */
    void concatonate(Matrix4x3 &m);

    /**
     * getTranslation
     *
     * Return the translation row of the matrix in vector form
     */
    Vector3 getTranslation();

    // ===== end mazerts methods

};

/**
 * Operator* is used to transforms a point, and also concatonate matrices.
 * The order of multiplications from left to right is the same as
 * the order of transformations
 */
Vector3   operator*(const Vector3 &p, const Matrix4x3 &m);
Matrix4x3 operator*(const Matrix4x3 &a, const Matrix4x3 &b);

/**
 * Operator *= for conformance to C++ standards
 */
Vector3   &operator*=(Vector3 &p, const Matrix4x3 &m);
Matrix4x3 &operator*=(const Matrix4x3 &a, const Matrix4x3 &m);

/**
 * Compute the determinant of the 3x3 portion of the matrix
 */
float determinant(const Matrix4x3 &m);

/**
 * Compute the inverse of a matrix
 */
Matrix4x3 inverse(const Matrix4x3 &m);

/**
 * Extract the translation portion of the matrix
 */
Vector3 getTranslation(const Matrix4x3 &m);

/**
 * Extract the position/orientation from a local->parent matrix,
 * or a parent->local matrix
 */
Vector3 getPositionFromParentToLocalMatrix(const Matrix4x3 &m);
Vector3 getPositionFromLocalToParentMatrix(const Matrix4x3 &m);

/////////////////////////////////////////////////////////////////////////////
#endif // #ifndef __ROTATIONMATRIX_H_INCLUDED__
