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

typedef enum _fieldtypes_t
{
	FIELD_TYPE_VOID = 0,		// No type or value

	FIELD_TYPE_FLOAT,			// Single precision floating-point
	FIELD_TYPE_DOUBLE,			// Double precision floating-point

	FIELD_TYPE_CSTRING,			// A zero terminated string

	FIELD_TYPE_INTEGER,			// Any integer or enum
	FIELD_TYPE_SHORT,			// 2 byte integer
	FIELD_TYPE_INTEGER64,		// 64bit integer

	FIELD_TYPE_BOOLEAN,			// boolean
	FIELD_TYPE_BYTE,			// a byte
	FIELD_TYPE_CHARACTER,		// a char

	FIELD_TYPE_UINT,			// unsigned integer
	FIELD_TYPE_UINT16,			// unsigned short integer
	FIELD_TYPE_UINT64,			// unsigned 64bit integer

	FIELD_TYPE_COLOR32,			// 8-bit per channel r,g,b,a (32bit color)

	FIELD_TYPE_VECTOR,			// Vector
	FIELD_TYPE_VECTOR2D,		// Vector2D
	FIELD_TYPE_QANGLE,			// QAngle
	FIELD_TYPE_QUATERNION,		// Quaternion

	FIELD_TYPE_TYPECOUNT,		// MUST BE LAST
	FIELD_TYPE_TYPEUNKNOWN = FIELD_TYPE_TYPECOUNT
} fieldtype_t;

template <typename T> struct FieldTypeDeducer { /* enum { FIELD_TYPE = FIELD_TYPEUNKNOWN }; */ };
#define DECLARE_DEDUCE_FIELDTYPE( fieldType, type ) template<> struct FieldTypeDeducer<type> { enum { FIELD_TYPE = fieldType }; };

DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_VOID, void);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_FLOAT, float);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_DOUBLE, double);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_CSTRING, const char *);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_CSTRING, char *);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_INTEGER, int);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_SHORT, short);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_INTEGER64, int64);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_BOOLEAN, bool);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_BYTE, unsigned char);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_CHARACTER, char);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_UINT, uint32);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_UINT16, uint16);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_UINT64, uint64);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_COLOR32, Color);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_COLOR32, const Color &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_VECTOR, Vector);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_VECTOR, const Vector &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_VECTOR2D, Vector2D);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_VECTOR2D, const Vector2D &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_QANGLE, QAngle);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_QANGLE, const QAngle &);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_QUATERNION, Quaternion);
DECLARE_DEDUCE_FIELDTYPE(FIELD_TYPE_QUATERNION, const Quaternion &);

#define DeduceFieldType(T) FieldTypeDeducer<T>::FIELD_TYPE

template <typename T>
inline constexpr int DeduceValueFieldType(T value)
{
	return FieldTypeDeducer<T>::FIELD_TYPE;
}

#endif // FIELDTYPES_H