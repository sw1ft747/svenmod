/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
// mathlib.h

#ifndef MATHLIB_H
#define MATHLIB_H

#ifdef _WIN32
#pragma once
#endif

#include <cmath>
#include "vector.h"

typedef vec_t vec5_t[5];

typedef short vec_s_t;
typedef vec_s_t vec3s_t[3];
typedef vec_s_t vec4s_t[4]; // x,y,z,w
typedef vec_s_t vec5s_t[5];

typedef int fixed4_t;
typedef int fixed8_t;
typedef int fixed16_t;
#ifndef M_PI
#define M_PI 3.14159265358979323846 // matches value in gcc v2 math.h
#endif

struct mplane_s;

const Vector vec3_origin;
const Vector g_vecZero;
extern int nanmask;

#define IS_NAN(x) !Vec_IsFloatFinite(x)
//#define IS_NAN(x) (((*(int*)&x) & nanmask) == nanmask)

//-----------------------------------------------------------------------------
// Vector stuff
//-----------------------------------------------------------------------------

inline void VectorCopy(const Vector &src, Vector &dest)
{
	dest.x = src.x;
	dest.y = src.y;
	dest.z = src.z;
}

inline void VectorAdd(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

inline void VectorSubtract(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

inline void VectorMultiply(const Vector &a, float scale, Vector &c)
{
	c.x = a.x * scale;
	c.y = a.y * scale;
	c.z = a.z * scale;
}

inline void VectorMultiply(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x * b.x;
	c.y = a.y * b.y;
	c.z = a.z * b.z;
}

inline void VectorScale(const Vector &in, float scale, Vector &result)
{
	VectorMultiply(in, scale, result);
}

inline void VectorDivide(const Vector &a, const Vector &b, Vector &c)
{
	c.x = a.x / b.x;
	c.y = a.y / b.y;
	c.z = a.z / b.z;
}

inline void VectorLerp(const Vector &src1, const Vector &src2, vec_t t, Vector &dest)
{
	dest.x = src1.x + (src2.x - src1.x) * t;
	dest.y = src1.y + (src2.y - src1.y) * t;
	dest.z = src1.z + (src2.z - src1.z) * t;
}

inline Vector VectorLerp(const Vector &src1, const Vector &src2, vec_t t)
{
	Vector result;
	VectorLerp(src1, src2, t, result);
	return result;
}

inline bool VectorCompare(const Vector &a, const Vector &b)
{
	return a == b;
}

inline void VectorInverse(Vector &v)
{
	v.Negate();
}

inline void VectorClear(Vector &v)
{
	v.Zero();
}

inline void VectorMA(const Vector &start, float scale, const Vector &direction, Vector &dest)
{
	dest.x = start.x + scale * direction.x;
	dest.y = start.y + scale * direction.y;
	dest.z = start.z + scale * direction.z;
}

inline void CrossProduct(const Vector &a, const Vector &b, Vector &result)
{
	result = a.Cross(b);
}

inline Vector CrossProduct(const Vector &a, const Vector &b)
{
	return a.Cross(b);
}

inline float DotProduct(float *a, float *b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

inline float DotProduct(const Vector &a, const Vector &b)
{
	return a.Dot(b);
}

inline float DotProduct(const Vector2D &a, const Vector2D &b)
{
	return a.Dot(b);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

// ToDo: export functions?

float VectorNormalize(Vector &v);

int Q_log2(int val);

void ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3]);
void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4]);

void FloorDivMod(double numer, double denom, int *quotient, int *rem);
fixed16_t Invert24To16(fixed16_t val);
int GreatestCommonDivisor(int i1, int i2);

void AngleVectors(const Vector &angles, Vector *forward, Vector *right, Vector *up);
void AngleVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up);
void AngleVectorsTranspose(const Vector &angles, Vector *forward, Vector *right, Vector *up);
#define AngleIVectors AngleVectorsTranspose

void AngleMatrix(const float *angles, float matrix[3][4]);
void AngleIMatrix(const Vector &angles, float matrix[3][4]);
void MatrixAngles(float matrix[3][4], float *angles);
void VectorTransform(const Vector &in1, float in2[3][4], Vector &out);
void VectorRotate(const Vector &in1, float in2[3][4], Vector &out);
void VectorITransform(const Vector &in1, float in2[3][4], Vector &out);
void VectorIRotate(const Vector &in1, float in2[3][4], Vector &out);

float NormalizeAngle(float angle);
float NormalizeAnglePositive(float angle);
void NormalizeAngles(float *angles);
void InterpolateAngles(float *start, float *end, float *output, float frac);
float AngleBetweenVectors(const Vector &v1, const Vector &v2);


void VectorMatrix(const Vector &forward, Vector &right, Vector &up);
void VectorAngles(const float *forward, float *angles);

void InvertMatrix(float in[3][4], float out[3][4]);

int BoxOnPlaneSide(const Vector &emins, const Vector &emaxs, struct mplane_s *plane);
float anglemod(float a);

void MatrixCopy(float in[3][4], float out[3][4]);
void QuaternionMatrix(vec4_t quaternion, float(*matrix)[4]);
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt);
void AngleQuaternion(float *angles, vec4_t quaternion);
void AngleQuaternion(const QAngle &in, Quaternion &out);
void QuaternionAngles(const Quaternion &in, QAngle &out);


#define BOX_ON_PLANE_SIDE(emins, emaxs, p)                                                                 \
	(((p)->type < 3) ? (                                                                                   \
						   ((p)->dist <= (emins)[(p)->type]) ? 1                                           \
															 : (                                           \
																   ((p)->dist >= (emaxs)[(p)->type]) ? 2   \
																									 : 3)) \
					 : BoxOnPlaneSide((emins), (emaxs), (p)))
					 
#endif
