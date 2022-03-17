// Float Base

#ifndef FLOATBASE_H
#define FLOATBASE_H

#ifdef _WIN32
#pragma once
#endif

#define FLOAT_NAN_BITS 0x7FC00000
#define FLOAT_NAN BitsToFloat(FLOAT_NAN_BITS)

#define VEC_T_NAN FLOAT_NAN

typedef float vec_t;

inline unsigned long &FloatToBits(vec_t &f)
{
	return *reinterpret_cast<unsigned long *>(&f);
}

inline vec_t BitsToFloat(unsigned long i)
{
	return *reinterpret_cast<vec_t *>(&i);
}

inline bool IsFloatFinite(vec_t f)
{
	return (FloatToBits(f) & 0x7F800000) != 0x7F800000;
}

inline float FloatMakeNegative(vec_t f)
{
	return BitsToFloat(FloatToBits(f) | 0x80000000);
}

inline float FloatMakePositive(vec_t f)
{
	return BitsToFloat(FloatToBits(f) & 0x7FFFFFFF);
}

inline float FloatNegate(vec_t f)
{
	return BitsToFloat(FloatToBits(f) ^ 0x80000000);
}

#endif