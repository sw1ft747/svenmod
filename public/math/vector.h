// Vector classes

#ifndef VECTOR_H
#define VECTOR_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Include dependencies
//-----------------------------------------------------------------------------

#include <math.h>
#include <float.h>

#include "floatbase.h"
#include "hl_sdk/cl_dll/in_defs.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define VectorExpand(v) (v).x, (v).y, (v).z

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

class Vector;
class Vector2D;

//-----------------------------------------------------------------------------
// 2D Vector
//-----------------------------------------------------------------------------

class Vector2D
{
public:
	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------

	inline Vector2D(void) { x = y = 0.f; }
	inline Vector2D(float X, float Y) { x = X; y = Y; }
	inline Vector2D(double X, double Y) { x = (float)X; y = (float)Y; }
	inline Vector2D(int X, int Y) { x = (float)X; y = (float)Y; }
	inline Vector2D(const Vector2D &vOther) { x = vOther.x; y = vOther.y; }
	inline Vector2D(float pArray[2]) { x = pArray[0]; y = pArray[1]; }

	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------

	inline vec_t operator[](int nIndex) const { return ((vec_t *)this)[nIndex]; }
	inline vec_t &operator[](int nIndex) { return ((vec_t *)this)[nIndex]; }

	inline bool operator==(const Vector2D &vOther) const { return x == vOther.x && y == vOther.y; }
	inline bool operator!=(const Vector2D &vOther) const { return x != vOther.x || y != vOther.y; }

	inline Vector2D &operator=(const Vector2D &vOther) { x = vOther.x; y = vOther.y; return *this; }
	inline Vector2D &operator+=(const Vector2D &vOther) { x += vOther.x; y += vOther.y; return *this; }
	inline Vector2D &operator+=(float flValue) { x += flValue; y += flValue; return *this; }
	inline Vector2D &operator-=(const Vector2D &vOther) { x -= vOther.x; y -= vOther.y; return *this; }
	inline Vector2D &operator-=(float flValue) { x -= flValue; y -= flValue; return *this; }
	inline Vector2D &operator*=(const Vector2D &vOther) { x *= vOther.x; y *= vOther.y; return *this; }
	inline Vector2D &operator*=(float flValue) { x *= flValue; y *= flValue; return *this; }
	inline Vector2D &operator/=(const Vector2D &vOther) { x /= vOther.x; y /= vOther.y; return *this; }
	inline Vector2D &operator/=(float flValue) { flValue = 1.0f / flValue; x *= flValue; y *= flValue; return *this; }

	inline Vector2D operator+(const Vector2D &vOther) const { return Vector2D(x + vOther.x, y + vOther.y); }
	inline Vector2D operator+(float flValue) const { return Vector2D(x + flValue, y + flValue); }
	inline Vector2D operator-(const Vector2D &vOther) const { return Vector2D(x - vOther.x, y - vOther.y); }
	inline Vector2D operator-(float flValue) const { return Vector2D(x - flValue, y - flValue); }
	inline Vector2D operator-() const { return Vector2D(-x, -y); }
	inline Vector2D operator*(const Vector2D &vOther) const { return Vector2D(x * vOther.x, y * vOther.y); }
	inline Vector2D operator*(float flValue) const { return Vector2D(x * flValue, y * flValue); }
	inline Vector2D operator/(const Vector2D &vOther) const { return Vector2D(x / vOther.x, y / vOther.y); }
	inline Vector2D operator/(float flValue) const { flValue = 1.0f / flValue; return Vector2D(x * flValue, y * flValue); }

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return IsFloatFinite(x) && IsFloatFinite(y); }
	inline void Invalidate() { x = y = VEC_T_NAN; }

	inline void Zero() { x = y = 0.0f; }
	inline void Negate() { x = -x; y = -y; }

	inline bool IsZero(float flTolerance = 0.01f) const
	{
		return x > -flTolerance && x < flTolerance && y > -flTolerance && y < flTolerance;
	}

	inline bool WithinASquare(const Vector2D &vSquareMin, const Vector2D &vSquareMax) const
	{
		return x >= vSquareMin.x && x <= vSquareMax.x && y >= vSquareMin.y && y <= vSquareMax.y;
	}

	inline void Scale(float flScale) { x *= flScale; y *= flScale; }
	inline void Lerp(const Vector2D &vEnd, float dt) { x += (vEnd.x - x) * dt; y += (vEnd.y - y) * dt; }
	inline void CopyToArray(float pArray[2]) const { pArray[0] = x; pArray[1] = y; }

	inline Vector ToVector() const;

	inline float NormalizeInPlace() { float flLength = Length(); *this *= (1.0f / (Length() + FLT_EPSILON)); return flLength; }
	inline Vector2D Normalize() const { return *this * (1.0f / (Length() + FLT_EPSILON)); }

	inline Vector2D Project(const Vector2D &vOther) const { return vOther * (Dot(vOther) / (vOther.LengthSqr() + FLT_EPSILON)); }
	inline Vector2D Reject(const Vector2D &vOther) const { return *this - Project(vOther); }
	inline Vector2D Reflect(const Vector2D &vOther, float flFactor = 2.0f) const { return *this - (Project(vOther) * flFactor); }

	inline vec_t Dot(const Vector2D &vOther) const { return x * vOther.x + y * vOther.y; }

	inline vec_t Length() const { return sqrtf(x * x + y * y); }
	inline vec_t LengthSqr() const { return x * x + y * y; }

	inline vec_t DistTo(const Vector2D &vOther) const { return (vOther - *this).Length(); }
	inline vec_t DistToSqr(const Vector2D &vOther) const { return (vOther - *this).LengthSqr(); }

	// Members
	vec_t x, y;
};

//-----------------------------------------------------------------------------
// 3D Vector
//-----------------------------------------------------------------------------

class Vector
{
public:
	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------

	inline Vector(void) { x = y = z = 0.f; }
	inline Vector(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline Vector(double X, double Y, double Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	inline Vector(int X, int Y, int Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	inline Vector(const Vector &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; }
	inline Vector(float pArray[3]) { x = pArray[0]; y = pArray[1]; z = pArray[2]; }
	
	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------

	inline vec_t operator[](int nIndex) const { return ((vec_t *)this)[nIndex]; }
	inline vec_t &operator[](int nIndex) { return ((vec_t *)this)[nIndex]; }

	inline bool operator==(const Vector &vOther) const { return x == vOther.x && y == vOther.y && z == vOther.z; }
	inline bool operator!=(const Vector &vOther) const { return x != vOther.x || y != vOther.y || z != vOther.z; }

	inline Vector &operator=(const Vector &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; return *this; }
	inline Vector &operator+=(const Vector &vOther) { x += vOther.x; y += vOther.y; z += vOther.z; return *this; }
	inline Vector &operator+=(float flValue) { x += flValue; y += flValue; z += flValue; return *this; }
	inline Vector &operator-=(const Vector &vOther) { x -= vOther.x; y -= vOther.y; z -= vOther.z; return *this; }
	inline Vector &operator-=(float flValue) { x -= flValue; y -= flValue; z -= flValue; return *this; }
	inline Vector &operator*=(const Vector &vOther) { x *= vOther.x; y *= vOther.y; z *= vOther.z; return *this; }
	inline Vector &operator*=(float flValue) { x *= flValue; y *= flValue; z *= flValue; return *this; }
	inline Vector &operator/=(const Vector &vOther) { x /= vOther.x; y /= vOther.y; z /= vOther.z; return *this; }
	inline Vector &operator/=(float flValue) { flValue = 1.0f / flValue; x *= flValue; y *= flValue; z *= flValue; return *this; }

	inline Vector operator+(const Vector &vOther) const { return Vector(x + vOther.x, y + vOther.y, z + vOther.z); }
	inline Vector operator+(float flValue) const { return Vector(x + flValue, y + flValue, z + flValue); }
	inline Vector operator-(const Vector &vOther) const { return Vector(x - vOther.x, y - vOther.y, z - vOther.z); }
	inline Vector operator-(float flValue) const { return Vector(x - flValue, y - flValue, z - flValue); }
	inline Vector operator-() const { return Vector(-x, -y, -z); }
	inline Vector operator*(const Vector &vOther) const { return Vector(x * vOther.x, y * vOther.y, z * vOther.z); }
	inline Vector operator*(float flValue) const { return Vector(x * flValue, y * flValue, z * flValue); }
	inline Vector operator/(const Vector &vOther) const { return Vector(x / vOther.x, y / vOther.y, z / vOther.z); }
	inline Vector operator/(float flValue) const { flValue = 1.0f / flValue; return Vector(x * flValue, y * flValue, z * flValue); }

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return IsFloatFinite(x) && IsFloatFinite(y) && IsFloatFinite(z); }
	inline void Invalidate() { x = y = z = VEC_T_NAN; }

	inline void Zero() { x = y = z = 0.0f; }
	inline void Negate() { x = -x; y = -y; z = -z; }

	inline bool IsZero(float flTolerance = 0.01f) const
	{
		return x > -flTolerance && x < flTolerance && y > -flTolerance && y < flTolerance && z > -flTolerance && z < flTolerance;
	}

	inline bool WithinABox(const Vector &vBoxMin, const Vector &vBoxMax) const
	{
		return x >= vBoxMin.x && x <= vBoxMax.x && y >= vBoxMin.y && y <= vBoxMax.y && z >= vBoxMin.z && z <= vBoxMax.z;
	}
	
	inline void Scale(float flScale) { x *= flScale; y *= flScale; z *= flScale; }
	inline void Lerp(const Vector &vEnd, float dt) { x += (vEnd.x - x) * dt; y += (vEnd.y - y) * dt; z += (vEnd.z - z) * dt; }
	inline void CopyToArray(float pArray[3]) const { pArray[0] = x; pArray[1] = y; pArray[2] = z; }

	inline Vector2D AsVector2D() const { return Vector2D(x, y); }
	inline Vector2D &AsVector2D() { return *(Vector2D *)this; }

	inline float NormalizeInPlace() { float flLength = Length(); *this *= (1.0f / (Length() + FLT_EPSILON)); return flLength; }
	inline Vector Normalize() const { return *this * (1.0f / (Length() + FLT_EPSILON)); }

	inline Vector Project(const Vector &vOther) const { return vOther * (Dot(vOther) / (vOther.LengthSqr() + FLT_EPSILON)); }
	inline Vector Reject(const Vector &vOther) const { return *this - Project(vOther); }
	inline Vector Reflect(const Vector &vOther, float flFactor = 2.0f) const { return *this - (Project(vOther) * flFactor); }

	inline Vector Cross(const Vector &vOther) const { return Vector(y * vOther.z - vOther.y * z, z * vOther.x - vOther.z * x, x * vOther.y - vOther.x * y); }
	inline vec_t Dot(const Vector &vOther) const { return x * vOther.x + y * vOther.y + z * vOther.z; }

	inline vec_t Length() const { return sqrtf(x * x + y * y + z * z); }
	inline vec_t LengthSqr() const { return x * x + y * y + z * z; }
	inline vec_t Length2D() const { return sqrtf(x * x + y * y); }
	inline vec_t Length2DSqr() const { return x * x + y * y; }

	inline vec_t DistTo(const Vector &vOther) const { return (vOther - *this).Length(); }
	inline vec_t DistToSqr(const Vector &vOther) const { return (vOther - *this).LengthSqr(); }

	// Members
	vec_t x, y, z;
};

//-----------------------------------------------------------------------------
// Vector2D implementations
//-----------------------------------------------------------------------------

inline Vector Vector2D::ToVector() const
{
	return Vector(x, y, 0.0f);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

typedef Vector2D vec2_t;
typedef Vector vec3_t;

#endif // VECTOR_H