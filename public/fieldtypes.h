//======= Copyright 1996-2008, Valve Corporation, All rights reserved. ========//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef FIELDTYPES_H
#define FIELDTYPES_H

#ifdef _WIN32
#pragma once
#endif

#include "math/mathlib.h"
#include "steamtypes.h"
#include "color.h"

typedef enum _fieldtypes
{
	FIELD_VOID = 0,			// No type or value

	FIELD_FLOAT,			// Single precision floating-point
	FIELD_DOUBLE,			// Double precision floating-point

	FIELD_CSTRING,			// A zero terminated string

	FIELD_INTEGER,			// Any integer or enum
	FIELD_SHORT,			// 2 byte integer
	FIELD_INTEGER64,		// 64bit integer

	FIELD_BOOLEAN,			// boolean
	FIELD_BYTE,				// a byte
	FIELD_CHARACTER,		// a char

	FIELD_UINT,				// unsigned integer
	FIELD_UINT16,			// unsigned short integer
	FIELD_UINT64,			// unsigned 64bit integer

	FIELD_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)

	FIELD_VECTOR,			// Vector
	FIELD_VECTOR2D,			// Vector2D
	FIELD_QANGLE,			// QAngle
	FIELD_QUATERNION,		// Quaternion

	FIELD_TYPECOUNT,		// MUST BE LAST
	FIELD_TYPEUNKNOWN = FIELD_TYPECOUNT
} fieldtype_t;

template <typename T> struct FieldTypeDeducer { /* enum { FIELD_TYPE = FIELD_TYPEUNKNOWN }; */ };
#define DECLARE_DEDUCE_FIELDTYPE( fieldType, type ) template<> struct FieldTypeDeducer<type> { enum { FIELD_TYPE = fieldType }; };

DECLARE_DEDUCE_FIELDTYPE(FIELD_VOID, void);
DECLARE_DEDUCE_FIELDTYPE(FIELD_FLOAT, float);
DECLARE_DEDUCE_FIELDTYPE(FIELD_DOUBLE, double);
DECLARE_DEDUCE_FIELDTYPE(FIELD_CSTRING, const char *);
DECLARE_DEDUCE_FIELDTYPE(FIELD_CSTRING, char *);
DECLARE_DEDUCE_FIELDTYPE(FIELD_INTEGER, int);
DECLARE_DEDUCE_FIELDTYPE(FIELD_SHORT, short);
DECLARE_DEDUCE_FIELDTYPE(FIELD_INTEGER64, int64);
DECLARE_DEDUCE_FIELDTYPE(FIELD_BOOLEAN, bool);
DECLARE_DEDUCE_FIELDTYPE(FIELD_BYTE, unsigned char);
DECLARE_DEDUCE_FIELDTYPE(FIELD_CHARACTER, char);
DECLARE_DEDUCE_FIELDTYPE(FIELD_UINT, uint32);
DECLARE_DEDUCE_FIELDTYPE(FIELD_UINT16, uint16);
DECLARE_DEDUCE_FIELDTYPE(FIELD_UINT64, uint64);
DECLARE_DEDUCE_FIELDTYPE(FIELD_COLOR32, Color);
DECLARE_DEDUCE_FIELDTYPE(FIELD_COLOR32, const Color &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_VECTOR, Vector);
DECLARE_DEDUCE_FIELDTYPE(FIELD_VECTOR, const Vector &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_VECTOR2D, Vector2D);
DECLARE_DEDUCE_FIELDTYPE(FIELD_VECTOR2D, const Vector2D &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_QANGLE, struct QAngle); // ToDo: implement QAngle class
DECLARE_DEDUCE_FIELDTYPE(FIELD_QANGLE, const struct QAngle &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_QUATERNION, vec4_t); // ToDo: implement Quaternion class
DECLARE_DEDUCE_FIELDTYPE(FIELD_QUATERNION, const vec4_t &);

template <typename T>
inline constexpr int DeduceFieldType(T value)
{
	return FieldTypeDeducer<T>::FIELD_TYPE;
}

#endif // FIELDTYPES_H