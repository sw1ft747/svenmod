// Vector, QAngle, Quaternion

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

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#ifndef PITCH
#define PITCH 0 // up / down
#endif
#ifndef YAW
#define YAW 1 // left / right
#endif
#ifndef ROLL
#define ROLL 2 // fall over
#endif

#define VEC_DEG2RAD(a) (a) * (3.14159265358979323846f / 180.0f)
#define VEC_RAD2DEG(a) (a) * (180.0f / 3.14159265358979323846f)

#define VectorExpand(v) (v).x, (v).y, (v).z
#define Vector2DExpand(v) (v).x, (v).y
#define QuaternionExpand(q) (q).x, (q).y, (q).z, (q.w)

#define VEC_FLOAT_NAN_BITS 0x7FC00000
#define VEC_FLOAT_NAN Vec_BitsToFloat(VEC_FLOAT_NAN_BITS)

#define VEC_T_NAN VEC_FLOAT_NAN

//-----------------------------------------------------------------------------
// Forward declarations
//-----------------------------------------------------------------------------

typedef float vec_t;

class Vector;
class Vector2D;
class QAngle;
class Quaternion;

//-----------------------------------------------------------------------------
// Float operations
//-----------------------------------------------------------------------------

inline unsigned long &Vec_FloatToBits(vec_t &f)
{
	return *reinterpret_cast<unsigned long *>(&f);
}

inline vec_t Vec_BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t *>(&i);
}

inline bool Vec_IsFloatFinite(vec_t f)
{
	return ( Vec_FloatToBits(f) & 0x7F800000 ) != 0x7F800000;
}

inline float Vec_FloatMakeNegative(vec_t f)
{
	return Vec_BitsToFloat( Vec_FloatToBits(f) | 0x80000000 );
}

inline float Vec_FloatMakePositive(vec_t f)
{
	return Vec_BitsToFloat( Vec_FloatToBits(f) & 0x7FFFFFFF );
}

inline float Vec_FloatNegate(vec_t f)
{
	return Vec_BitsToFloat( Vec_FloatToBits(f) ^ 0x80000000 );
}

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
	inline Vector2D &operator/=(float flValue) { flValue = 1.f / flValue; x *= flValue; y *= flValue; return *this; }

	inline Vector2D operator+(const Vector2D &vOther) const { return Vector2D(x + vOther.x, y + vOther.y); }
	inline Vector2D operator+(float flValue) const { return Vector2D(x + flValue, y + flValue); }
	inline Vector2D operator-(const Vector2D &vOther) const { return Vector2D(x - vOther.x, y - vOther.y); }
	inline Vector2D operator-(float flValue) const { return Vector2D(x - flValue, y - flValue); }
	inline Vector2D operator-() const { return Vector2D(-x, -y); }
	inline Vector2D operator*(const Vector2D &vOther) const { return Vector2D(x * vOther.x, y * vOther.y); }
	inline Vector2D operator*(float flValue) const { return Vector2D(x * flValue, y * flValue); }
	inline Vector2D operator/(const Vector2D &vOther) const { return Vector2D(x / vOther.x, y / vOther.y); }
	inline Vector2D operator/(float flValue) const { flValue = 1.f / flValue; return Vector2D(x * flValue, y * flValue); }

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return Vec_IsFloatFinite(x) && Vec_IsFloatFinite(y); }
	inline void Invalidate() { x = y = VEC_T_NAN; }

	inline void Zero() { x = y = 0.f; }
	inline void Negate() { x = -x; y = -y; }

	inline bool IsZero(float flTolerance = 0.01f) const
	{
		return x > -flTolerance && x < flTolerance && y > -flTolerance && y < flTolerance;
	}
	
	inline bool IsZeroFast() const
	{
		return *(unsigned long *)&x == 0 && *(unsigned long *)&y == 0;
	}

	inline bool WithinASquare(const Vector2D &vSquareMin, const Vector2D &vSquareMax) const
	{
		return x >= vSquareMin.x && x <= vSquareMax.x && y >= vSquareMin.y && y <= vSquareMax.y;
	}

	inline void Scale(float flScale) { x *= flScale; y *= flScale; }
	inline void Lerp(const Vector2D &vEnd, float dt) { x += (vEnd.x - x) * dt; y += (vEnd.y - y) * dt; }
	inline void CopyToArray(float pArray[2]) const { pArray[0] = x; pArray[1] = y; }

	inline Vector ToVector() const;

	inline float NormalizeInPlace() { float flLength = this->Length(); *this *= ( 1.f / ( flLength + FLT_EPSILON ) ); return flLength; }
	inline Vector2D Normalize() const { return *this * ( 1.f / ( this->Length() + FLT_EPSILON ) ); }

	inline Vector2D Project(const Vector2D &vOther) const { return vOther * ( Dot(vOther) / ( vOther.LengthSqr() + FLT_EPSILON ) ); }
	inline Vector2D Reject(const Vector2D &vOther) const { return *this - Project(vOther); }
	inline Vector2D Reflect(const Vector2D &vOther, float flFactor = 2.f) const { return *this - ( Project(vOther) * flFactor ); }

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
	inline Vector &operator/=(float flValue) { flValue = 1.f / flValue; x *= flValue; y *= flValue; z *= flValue; return *this; }

	inline Vector operator+(const Vector &vOther) const { return Vector(x + vOther.x, y + vOther.y, z + vOther.z); }
	inline Vector operator+(float flValue) const { return Vector(x + flValue, y + flValue, z + flValue); }
	inline Vector operator-(const Vector &vOther) const { return Vector(x - vOther.x, y - vOther.y, z - vOther.z); }
	inline Vector operator-(float flValue) const { return Vector(x - flValue, y - flValue, z - flValue); }
	inline Vector operator-() const { return Vector(-x, -y, -z); }
	inline Vector operator*(const Vector &vOther) const { return Vector(x * vOther.x, y * vOther.y, z * vOther.z); }
	inline Vector operator*(float flValue) const { return Vector(x * flValue, y * flValue, z * flValue); }
	inline Vector operator/(const Vector &vOther) const { return Vector(x / vOther.x, y / vOther.y, z / vOther.z); }
	inline Vector operator/(float flValue) const { flValue = 1.f / flValue; return Vector(x * flValue, y * flValue, z * flValue); }

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return Vec_IsFloatFinite(x) && Vec_IsFloatFinite(y) && Vec_IsFloatFinite(z); }
	inline void Invalidate() { x = y = z = VEC_T_NAN; }

	inline void Zero() { x = y = z = 0.f; }
	inline void Negate() { x = -x; y = -y; z = -z; }

	inline bool IsZero(float flTolerance = 0.01f) const
	{
		return x > -flTolerance && x < flTolerance && y > -flTolerance && y < flTolerance && z > -flTolerance && z < flTolerance;
	}

	inline bool IsZeroFast() const
	{
		return *(unsigned long *)&x == 0 && *(unsigned long *)&y == 0 && *(unsigned long *)&z == 0;
	}

	inline bool WithinABox(const Vector &vBoxMin, const Vector &vBoxMax) const
	{
		return x >= vBoxMin.x && x <= vBoxMax.x && y >= vBoxMin.y && y <= vBoxMax.y && z >= vBoxMin.z && z <= vBoxMax.z;
	}
	
	inline void Scale(float flScale) { x *= flScale; y *= flScale; z *= flScale; }
	inline void Lerp(const Vector &vEnd, float dt) { x += (vEnd.x - x) * dt; y += (vEnd.y - y) * dt; z += (vEnd.z - z) * dt; }
	inline void CopyToArray(float pArray[3]) const { pArray[0] = x; pArray[1] = y; pArray[2] = z; }

	inline Vector2D AsVector2D() { return Vector2D(x, y); }
	inline const Vector2D &AsVector2D() const { return *(Vector2D *)this; }

	inline float NormalizeInPlace() { float flLength = this->Length(); *this *= ( 1.f / ( flLength + FLT_EPSILON ) ); return flLength; }
	inline Vector Normalize() const { return *this * ( 1.f / ( this->Length() + FLT_EPSILON ) ); }

	inline Vector Project(const Vector &vOther) const { return vOther * ( Dot(vOther) / ( vOther.LengthSqr() + FLT_EPSILON ) ); }
	inline Vector Reject(const Vector &vOther) const { return *this - Project(vOther); }
	inline Vector Reflect(const Vector &vOther, float flFactor = 2.f) const { return *this - (Project(vOther) * flFactor); }

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
// QAngle
//-----------------------------------------------------------------------------

class QAngle
{
public:
	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------

	inline QAngle(void) { x = y = z = 0.f; }
	inline QAngle(float X, float Y, float Z) { x = X; y = Y; z = Z; }
	inline QAngle(double X, double Y, double Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	inline QAngle(int X, int Y, int Z) { x = (float)X; y = (float)Y; z = (float)Z; }
	inline QAngle(const QAngle &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; }
	inline QAngle(const Vector &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; }
	inline QAngle(float pArray[3]) { x = pArray[0]; y = pArray[1]; z = pArray[2]; }
	
	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------

	inline vec_t operator[](int nIndex) const { return ((vec_t *)this)[nIndex]; }
	inline vec_t &operator[](int nIndex) { return ((vec_t *)this)[nIndex]; }

	inline bool operator==(const QAngle &vOther) const { return x == vOther.x && y == vOther.y && z == vOther.z; }
	inline bool operator!=(const QAngle &vOther) const { return x != vOther.x || y != vOther.y || z != vOther.z; }

	inline QAngle &operator=(const Vector &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; return *this; }
	inline QAngle &operator=(const QAngle &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; return *this; }
	inline QAngle &operator+=(const QAngle &vOther) { x += vOther.x; y += vOther.y; z += vOther.z; return *this; }
	inline QAngle &operator+=(float flValue) { x += flValue; y += flValue; z += flValue; return *this; }
	inline QAngle &operator-=(const QAngle &vOther) { x -= vOther.x; y -= vOther.y; z -= vOther.z; return *this; }
	inline QAngle &operator-=(float flValue) { x -= flValue; y -= flValue; z -= flValue; return *this; }
	inline QAngle &operator*=(const QAngle &vOther) { x *= vOther.x; y *= vOther.y; z *= vOther.z; return *this; }
	inline QAngle &operator*=(float flValue) { x *= flValue; y *= flValue; z *= flValue; return *this; }
	inline QAngle &operator/=(const QAngle &vOther) { x /= vOther.x; y /= vOther.y; z /= vOther.z; return *this; }
	inline QAngle &operator/=(float flValue) { flValue = 1.f / flValue; x *= flValue; y *= flValue; z *= flValue; return *this; }

	inline QAngle operator+(const QAngle &vOther) const { return QAngle(x + vOther.x, y + vOther.y, z + vOther.z); }
	inline QAngle operator+(float flValue) const { return QAngle(x + flValue, y + flValue, z + flValue); }
	inline QAngle operator-(const QAngle &vOther) const { return QAngle(x - vOther.x, y - vOther.y, z - vOther.z); }
	inline QAngle operator-(float flValue) const { return QAngle(x - flValue, y - flValue, z - flValue); }
	inline QAngle operator-() const { return QAngle(-x, -y, -z); }
	inline QAngle operator*(const QAngle &vOther) const { return QAngle(x * vOther.x, y * vOther.y, z * vOther.z); }
	inline QAngle operator*(float flValue) const { return QAngle(x * flValue, y * flValue, z * flValue); }
	inline QAngle operator/(const QAngle &vOther) const { return QAngle(x / vOther.x, y / vOther.y, z / vOther.z); }
	inline QAngle operator/(float flValue) const { flValue = 1.f / flValue; return QAngle(x * flValue, y * flValue, z * flValue); }

	operator Vector() { return &x; }
	operator const Vector &() const { return *(Vector *)this; }
	
	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return Vec_IsFloatFinite(x) && Vec_IsFloatFinite(y) && Vec_IsFloatFinite(z); }
	inline void Invalidate() { x = y = z = VEC_T_NAN; }

	inline void Zero() { x = y = z = 0.f; }
	inline void Negate() { x = -x; y = -y; z = -z; }

	inline void CopyToArray(float pArray[3]) const { pArray[0] = x; pArray[1] = y; pArray[2] = z; }

	inline Vector AsVector() { return Vector(x, y, z); }
	inline const Vector &AsVector() const { return *(Vector *)this; }

	Quaternion ToQuaternion() const;

	Vector GetForward() const;
	Vector GetRight() const;
	Vector GetUp() const;
	inline Vector GetLeft() const { return -this->GetRight(); };

	void Normalize();
	void NormalizePositive();

	inline vec_t Length() const { return sqrtf(x * x + y * y + z * z); }
	inline vec_t LengthSqr() const { return x * x + y * y + z * z; }

	// Members
	vec_t x, y, z;
};

//-----------------------------------------------------------------------------
// Quaternion
//-----------------------------------------------------------------------------

class Quaternion
{
public:
	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------

	inline Quaternion(void) { this->Identity(); }
	inline Quaternion(float X, float Y, float Z, float W) { x = X; y = Y; z = Z; w = W; }
	inline Quaternion(double X, double Y, double Z, double W) { x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; }
	inline Quaternion(int X, int Y, int Z, int W) { x = (float)X; y = (float)Y; z = (float)Z; w = (float)W; }
	inline Quaternion(float pArray[4]) { x = pArray[0]; y = pArray[1]; z = pArray[2]; w = pArray[4]; }
	inline Quaternion(const Vector &vecImaginaryPart, float flRealPart) { *(Vector *)this = vecImaginaryPart; w = flRealPart; }

	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------

	inline vec_t operator[](int nIndex) const { return ((vec_t *)this)[nIndex]; }
	inline vec_t &operator[](int nIndex) { return ((vec_t *)this)[nIndex]; }

	inline bool operator==(const Quaternion &vOther) const { return x == vOther.x && y == vOther.y && z == vOther.z && w == vOther.w; }
	inline bool operator!=(const Quaternion &vOther) const { return x != vOther.x || y != vOther.y || z != vOther.z || w != vOther.w; }

	inline Quaternion &operator=(const Quaternion &vOther) { x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w; return *this; }
	inline Quaternion &operator+=(const Quaternion &vOther) { x += vOther.x; y += vOther.y; z += vOther.z; w += vOther.w; return *this; }
	inline Quaternion &operator+=(float flValue) { x += flValue; y += flValue; z += flValue; w += flValue; return *this; }
	inline Quaternion &operator-=(const Quaternion &vOther) { x -= vOther.x; y -= vOther.y; z -= vOther.z; w -= vOther.w; return *this; }
	inline Quaternion &operator-=(float flValue) { x -= flValue; y -= flValue; z -= flValue; w -= flValue; return *this; }
	inline Quaternion &operator*=(const Quaternion &vOther) { *this = this->Multiply( vOther ); return *this; }
	inline Quaternion &operator*=(float flValue) { x *= flValue; y *= flValue; z *= flValue; w *= flValue; return *this; }
	inline Quaternion &operator/=(const Quaternion &vOther) { *this = this->Multiply( vOther.Invert() ); return *this; }
	inline Quaternion &operator/=(float flValue) { flValue = 1.f / flValue; x *= flValue; y *= flValue; z *= flValue; w *= flValue; return *this; }

	inline Quaternion operator+(const Quaternion &vOther) const { return Quaternion(x + vOther.x, y + vOther.y, z + vOther.z, w + vOther.w); }
	inline Quaternion operator+(float flValue) const { return Quaternion(x + flValue, y + flValue, z + flValue, w + flValue); }
	inline Quaternion operator-(const Quaternion &vOther) const { return Quaternion(x - vOther.x, y - vOther.y, z - vOther.z, w - vOther.w); }
	inline Quaternion operator-(float flValue) const { return Quaternion(x - flValue, y - flValue, z - flValue, w - flValue); }
	inline Quaternion operator-() const { return Quaternion(-x, -y, -z, -w); }
	inline Quaternion operator*(const Quaternion &vOther) const { return this->Multiply( vOther ); }
	inline Quaternion operator*(float flValue) const { return Quaternion(x * flValue, y * flValue, z * flValue, w * flValue); }
	inline Quaternion operator/(const Quaternion &vOther) const { return this->Multiply( vOther.Invert() ); }
	inline Quaternion operator/(float flValue) const { flValue = 1.f / flValue; return Quaternion(x * flValue, y * flValue, z * flValue, w * flValue); }

	operator float *() { return &x; }
	operator const float *() const { return &x; }

	//-----------------------------------------------------------------------------
	// Methods
	//-----------------------------------------------------------------------------

	inline bool IsValid() const { return Vec_IsFloatFinite(x) && Vec_IsFloatFinite(y) && Vec_IsFloatFinite(z) && Vec_IsFloatFinite(w); }
	inline void Invalidate() { x = y = z = w = VEC_T_NAN; }

	inline void Zero() { x = y = z = w = 0.f; }
	inline void Identity() { x = y = z = 0.f; w = 1.f; }
	inline void Negate() { x = -x; y = -y; z = -z; w = -w; }

	inline bool IsZero(float flTolerance = 0.01f) const
	{
		return x > -flTolerance && x < flTolerance && y > -flTolerance && y < flTolerance && z > -flTolerance && z < flTolerance && w > -flTolerance && w < flTolerance;
	}

	inline bool IsZeroFast() const
	{
		return *(unsigned long *)&x == 0 && *(unsigned long *)&y == 0 && *(unsigned long *)&z == 0 && *(unsigned long *)&w == 0;
	}
	
	inline bool IsIdentity() const
	{
		return *(unsigned long *)&x == 0 && *(unsigned long *)&y == 0 && *(unsigned long *)&z == 0 && w == 1.f;
	}

	inline bool Equal(const Quaternion &q, float flTolerance = 0.00001f) const { return this->Dot(q) >= 1.f - flTolerance; }

	inline void Scale(float flScale) { x *= flScale; y *= flScale; z *= flScale; w *= flScale; }
	inline void CopyToArray(float pArray[4]) const { pArray[0] = x; pArray[1] = y; pArray[2] = z; pArray[3] = w; }

	QAngle ToQAngle() const;

	inline Vector &ImaginaryPart() { return *(Vector *)this; }
	inline const Vector &ImaginaryPart() const { return *(Vector *)this; }
	inline float &RealPart() { return w; }
	inline float RealPart() const { return w; }

	Vector GetForward() const;
	Vector GetRight() const;
	Vector GetUp() const;
	inline Vector GetLeft() const { return -this->GetRight(); };

	void AxisAngle(const Vector &axis, float angle);
	void ToAxisAngle(Vector &axis, float &angle) const;

	Quaternion Lerp(Quaternion &q, float t) const;
	Quaternion Slerp(Quaternion &q, float t) const;

	Quaternion Multiply(const Quaternion &q) const;
	
	Vector Rotate(const Vector &in) const;

	inline Quaternion Conjugate() const { return Quaternion(-x, -y, -z, w); }
	inline Quaternion Invert() const { return this->Conjugate() / this->NormSqr(); }

	inline float NormalizeInPlace() { float flNorm = this->Norm(); *this *= ( 1.f / ( flNorm + FLT_EPSILON ) ); return flNorm; }
	inline Quaternion Normalize() const { return *this * ( 1.f / ( this->Norm() + FLT_EPSILON ) ); }

	inline vec_t Dot(const Quaternion &q) const { return x * q.x + y * q.y + z * q.z + w * q.w; };

	inline float Norm() const { return sqrtf(x * x + y * y + z * z + w * w); }
	inline float NormSqr() const { return x * x + y * y + z * z + w * w; }

	// Members
	vec_t x, y, z, w;
};

//-----------------------------------------------------------------------------
// Types
//-----------------------------------------------------------------------------

typedef Vector2D vec2_t;
typedef Vector vec3_t;
typedef vec_t vec4_t[4];

//-----------------------------------------------------------------------------
// Vector2D methods
//-----------------------------------------------------------------------------

inline Vector Vector2D::ToVector() const
{
	return Vector(x, y, 0.f);
}

//-----------------------------------------------------------------------------
// QAngle methods
//-----------------------------------------------------------------------------

inline Quaternion QAngle::ToQuaternion() const
{
	extern void AngleQuaternion(const QAngle &angles, Quaternion &out);

	Quaternion q;
	AngleQuaternion( *this, q );

	return q;
}

inline Vector QAngle::GetForward() const
{
	extern void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);

	Vector vecForward;

	AngleVectors( this->AsVector(), &vecForward, NULL, NULL );

	return vecForward;
}

inline Vector QAngle::GetRight() const
{
	extern void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);

	Vector vecRight;

	AngleVectors( this->AsVector(), NULL, &vecRight, NULL );

	return vecRight;
}

inline Vector QAngle::GetUp() const
{
	extern void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);

	Vector vecUp;

	AngleVectors( this->AsVector(), NULL, NULL, &vecUp );

	return vecUp;
}

inline void QAngle::Normalize()
{
	extern float NormalizeAngle(float angle);

	x = NormalizeAngle(x);
	y = NormalizeAngle(y);
	z = NormalizeAngle(z);
}
	
inline void QAngle::NormalizePositive()
{
	extern float NormalizeAnglePositive(float angle);

	x = NormalizeAnglePositive(x);
	y = NormalizeAnglePositive(y);
	z = NormalizeAnglePositive(z);
}

//-----------------------------------------------------------------------------
// Quaternion methods
//-----------------------------------------------------------------------------

inline QAngle Quaternion::ToQAngle() const
{
	extern void QuaternionAngles(const Quaternion &in, QAngle &out);

	QAngle angles;
	QuaternionAngles( *this, angles );

	return angles;
}

inline Vector Quaternion::GetForward() const
{
	return Vector(1.f - 2.f * y * y - 2.f * z * z,
				  2.f * x * y + 2.f * w * z,
				  2.f * x * z - 2.f * w * y);
}

inline Vector Quaternion::GetRight() const
{
	return Vector(-2.f * x * y - -2.f * w * z,
				  -1.f - -2.f * x * x - -2.f * z * z,
				  -2.f * y * z + -2.f * w * x);
}

inline Vector Quaternion::GetUp() const
{
	return Vector(2.f * x * z + 2.f * w * y,
				  2.f * y * z - 2.f * w * x,
				  1.f - 2.f * x * x - 2.f * y * y);
}

inline void Quaternion::AxisAngle(const Vector &axis, float angle)
{
	float sinHalfTheta = sinf(angle * 0.5f);

	x = axis.x * sinHalfTheta;
	y = axis.y * sinHalfTheta;
	z = axis.z * sinHalfTheta;
	w = cosf(angle * 0.5f);
}

inline void Quaternion::ToAxisAngle(Vector &axis, float &angle) const
{
	float sinHalfTheta;

	angle = acosf(w);

	sinHalfTheta = sinf(angle);

	angle *= 2.f;

	if ( sinHalfTheta != 0.f )
	{
		axis.x = x / sinHalfTheta;
		axis.y = y / sinHalfTheta;
		axis.z = z / sinHalfTheta;
	}
	else
	{
		axis.x = 1.f;
		axis.y = 0.f;
		axis.z = 0.f;
	}

	axis.NormalizeInPlace();
}

inline Quaternion Quaternion::Lerp(Quaternion &q, float t) const
{
	float t2 = 1.f - t;

	if ( this->Dot(q) < 0.f )
	{
		return Quaternion(-x * t2 + q.x * t,
						  -y * t2 + q.y * t,
						  -z * t2 + q.z * t,
						  -w * t2 + q.w * t);
	}

	return Quaternion(x * t2 + q.x * t,
					  y * t2 + q.y * t,
					  z * t2 + q.z * t,
					  w * t2 + q.w * t);
}

inline Quaternion Quaternion::Slerp(Quaternion &q, float t) const
{
	float cosHalfTheta = this->Dot(q);

	if ( cosHalfTheta < 0.f )
	{
		q.Negate();

		Vec_FloatNegate( cosHalfTheta );
	}

	if ( Vec_FloatMakePositive( cosHalfTheta ) >= 1.f )
	{
		return *this;
	}

	float theta = acosf(cosHalfTheta);
	float sinHalfTheta = sinf(theta);

	if ( Vec_FloatMakePositive(sinHalfTheta) < 0.001f )
	{
		return Quaternion(x * 0.5f + q.x * 0.5f,
						  y * 0.5f + q.y * 0.5f,
						  z * 0.5f + q.z * 0.5f,
						  w * 0.5f + q.w * 0.5f);
	}

	float ratioA = sinf( (1.f - t) * theta ) / sinHalfTheta;
	float ratioB = sinf( t * theta ) / sinHalfTheta;

	return Quaternion(x * ratioA + q.x * ratioB,
					  y * ratioA + q.y * ratioB,
					  z * ratioA + q.z * ratioB,
					  w * ratioA + q.w * ratioB);
}

inline Quaternion Quaternion::Multiply(const Quaternion &q) const
{
	return Quaternion(w * q.x + x * q.w + y * q.z - z * q.y,
					  w * q.y - x * q.z + y * q.w + z * q.x,
					  w * q.z + x * q.y - y * q.x + z * q.w,
					  w * q.w - x * q.x - y * q.y - z * q.z);
}

inline Vector Quaternion::Rotate(const Vector &in) const
{
#if 1
	Quaternion rotation;
	Quaternion conjugate = this->Conjugate();

	//rotation = this->Multiply( Quaternion(in, 0.f) );

	rotation.x = y * in.z - z * in.y + w * in.x;
	rotation.y = -x * in.z + z * in.x + w * in.y;
	rotation.z = x * in.y - y * in.x + w * in.z;
	rotation.w = -x * in.x - y * in.y - z * in.z;

	//return rotation.Multiply(conjugate).ImaginaryPart();

	return Vector(rotation.x * conjugate.w + rotation.y * conjugate.z - rotation.z * conjugate.y + rotation.w * conjugate.x,
				  -rotation.x * conjugate.z + rotation.y * conjugate.w + rotation.z * conjugate.x + rotation.w * conjugate.y,
				  rotation.x * conjugate.y - rotation.y * conjugate.x + rotation.z * conjugate.w + rotation.w * conjugate.z);
#else // slower
	float ww = w * w;
	float xx = x * x;
	float yy = y * y;
	float zz = z * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;

	return Vector(ww * in.x + 2 * wy * in.z - 2 * wz * in.y + xx * in.x + 2 * xy * in.y + 2 * xz * in.z - zz * in.x - yy * in.x,
				  2 * xy * in.x + yy * in.y + 2 * yz * in.z + 2 * wz * in.x - zz * in.y + ww * in.y - 2 * wx * in.z - xx * in.y,
				  2 * xz * in.x + 2 * yz * in.y + zz * in.z - 2 * wy * in.x - yy * in.z + 2 * wx * in.y - xx * in.z + ww * in.z);
#endif
}

#endif // VECTOR_H