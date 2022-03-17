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
#include <math/mathlib.h>

#include <memory.h>

float anglemod(float a)
{
	a = (360.f / 65536.f) * ((int)(a * (65536.f / 360.f)) & 65535);
	return a;
}

void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (static_cast<float>(M_PI) / 180.0f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (static_cast<float>(M_PI) / 180.0f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (static_cast<float>(M_PI) / 180.0f);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}
	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void AngleVectorsTranspose(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (static_cast<float>(M_PI) / 180.0f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (static_cast<float>(M_PI) / 180.0f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (static_cast<float>(M_PI) / 180.0f);
	sr = sin(angle);
	cr = cos(angle);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = (sr * sp * cy + cr * -sy);
		forward->z = (cr * sp * cy + -sr * -sy);
	}
	if (right)
	{
		right->x = cp * sy;
		right->y = (sr * sp * sy + cr * cy);
		right->z = (cr * sp * sy + -sr * cy);
	}
	if (up)
	{
		up->x = -sp;
		up->y = sr * cp;
		up->z = cr * cp;
	}
}

void AngleMatrix(const float* angles, float matrix[3][4])
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (static_cast<float>(M_PI) / 180.0f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (static_cast<float>(M_PI) / 180.0f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (static_cast<float>(M_PI) / 180.0f);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr * sp * cy + cr * -sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[2][1] = sr * cp;
	matrix[0][2] = (cr * sp * cy + -sr * -sy);
	matrix[1][2] = (cr * sp * sy + -sr * cy);
	matrix[2][2] = cr * cp;
	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void AngleIMatrix(const Vector& angles, float matrix[3][4])
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	angle = angles[1] * (static_cast<float>(M_PI) / 180.0f);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[0] * (static_cast<float>(M_PI) / 180.0f);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[2] * (static_cast<float>(M_PI) / 180.0f);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp * cy;
	matrix[0][1] = cp * sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr * sp * cy + cr * -sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[1][2] = sr * cp;
	matrix[2][0] = (cr * sp * cy + -sr * -sy);
	matrix[2][1] = (cr * sp * sy + -sr * cy);
	matrix[2][2] = cr * cp;
	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

void NormalizeAngles(float* angles)
{
	int i;
	// Normalize angles
	for (i = 0; i < 3; i++)
	{
		if (angles[i] > 180.0f)
		{
			angles[i] -= 360.0f;
		}
		else if (angles[i] < -180.0f)
		{
			angles[i] += 360.0f;
		}
	}
}

/*
===================
InterpolateAngles

Interpolate Euler angles.
FIXME:  Use Quaternions to avoid discontinuities
Frac is 0.0 to 1.0 ( i.e., should probably be clamped, but doesn't have to be )
===================
*/
void InterpolateAngles(float* start, float* end, float* output, float frac)
{
	int i;
	float ang1, ang2;
	float d;

	NormalizeAngles(start);
	NormalizeAngles(end);

	for (i = 0; i < 3; i++)
	{
		ang1 = start[i];
		ang2 = end[i];

		d = ang2 - ang1;
		if (d > 180.f)
		{
			d -= 360.f;
		}
		else if (d < -180.f)
		{
			d += 360.f;
		}

		output[i] = ang1 + d * frac;
	}

	NormalizeAngles(output);
}


/*
===================
AngleBetweenVectors

===================
*/
float AngleBetweenVectors(const Vector& v1, const Vector& v2)
{
	float angle;
	float l1 = v1.Length();
	float l2 = v2.Length();

	if (0 == l1 || 0 == l2)
		return 0.0f;

	angle = acos(DotProduct(v1, v2)) / (l1 * l2);
	angle = (angle * 180.0f) / static_cast<float>(M_PI);

	return angle;
}

void VectorTransform(const Vector &in1, float in2[3][4], Vector &out)
{
	out[0] = DotProduct(in1, *reinterpret_cast<const Vector*>(in2[0])) + in2[0][3];
	out[1] = DotProduct(in1, *reinterpret_cast<const Vector*>(in2[1])) + in2[1][3];
	out[2] = DotProduct(in1, *reinterpret_cast<const Vector*>(in2[2])) + in2[2][3];
}

float VectorNormalize(Vector &v)
{
	return v.NormalizeInPlace();
}

int Q_log2(int val)
{
	int answer = 0;
	while ((val >>= 1) != 0)
		answer++;
	return answer;
}

void VectorMatrix(const Vector& forward, Vector& right, Vector& up)
{
	Vector tmp;

	if (forward[0] == 0.f && forward[1] == 0.f)
	{
		right[0] = 1.f;
		right[1] = 0.f;
		right[2] = 0.f;
		up[0] = -forward[2];
		up[1] = 0.f;
		up[2] = 0.f;
		return;
	}

	tmp[0] = 0.f;
	tmp[1] = 0.f;
	tmp[2] = 1.f;
	CrossProduct(forward, tmp, right);
	VectorNormalize(right);
	CrossProduct(right, forward, up);
	VectorNormalize(up);
}

void VectorAngles(const float* forward, float* angles)
{
	float tmp, yaw, pitch;

	if (forward[1] == 0.f && forward[0] == 0.f)
	{
		yaw = 0.f;
		if (forward[2] > 0.f)
			pitch = 90.f;
		else
			pitch = 270.f;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180.f / static_cast<float>(M_PI));
		if (yaw < 0.f)
			yaw += 360.f;

		tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(forward[2], tmp) * 180.f / static_cast<float>(M_PI));
		if (pitch < 0.f)
			pitch += 360.f;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0.f;
}

/*
================
ConcatRotations

================
*/

void ConcatRotations(float in1[3][3], float in2[3][3], float out[3][3])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
}

/*
================
ConcatTransforms

================
*/

void ConcatTransforms(float in1[3][4], float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] + in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] + in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] + in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] + in1[0][2] * in2[2][3] + in1[0][3];

	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] + in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] + in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] + in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] + in1[1][2] * in2[2][3] + in1[1][3];

	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] + in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] + in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] + in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] + in1[2][2] * in2[2][3] + in1[2][3];
}


void AngleQuaternion(float *angles, vec4_t quaternion)
{
	float angle;
	float sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5f;
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[1] * 0.5f;
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[0] * 0.5f;
	sr = sin(angle);
	cr = cos(angle);

	quaternion[0] = sr * cp * cy - cr * sp * sy; // X
	quaternion[1] = cr * sp * cy + sr * cp * sy; // Y
	quaternion[2] = cr * cp * sy - sr * sp * cy; // Z
	quaternion[3] = cr * cp * cy + sr * sp * sy; // W
}

/*
====================
QuaternionSlerp

====================
*/
void QuaternionSlerp(vec4_t p, vec4_t q, float t, vec4_t qt)
{
	int i;
	float omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i] - q[i]) * (p[i] - q[i]);
		b += (p[i] + q[i]) * (p[i] + q[i]);
	}
	if (a > b)
	{
		for (i = 0; i < 4; i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

	if ((1.0f + cosom) > 0.000001f)
	{
		if ((1.0f - cosom) > 0.000001f)
		{
			omega = acos(cosom);
			sinom = sin(omega);
			sclp = sin((1.0f - t) * omega) / sinom;
			sclq = sin(t * omega) / sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++)
		{
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sin((1.0f - t) * (0.5f * static_cast<float>(M_PI)));
		sclq = sin(t * (0.5f * static_cast<float>(M_PI)));
		for (i = 0; i < 3; i++)
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

/*
====================
QuaternionMatrix

====================
*/
void QuaternionMatrix(vec4_t quaternion, float (*matrix)[4])
{
	matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}

/*
====================
MatrixCopy

====================
*/
void MatrixCopy(float in[3][4], float out[3][4])
{
	memcpy(out, in, sizeof(float) * 3 * 4);
}