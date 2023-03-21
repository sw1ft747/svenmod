#ifndef MPATTERNS_H
#define MPATTERNS_H

#ifdef _WIN32
#pragma once
#endif

#include "baselogic.h"

//-----------------------------------------------------------------------------
// Macro definitions
//-----------------------------------------------------------------------------

#define DEFINE_PATTERN(name, signature) struct pattern_inner_wrapper<get_pattern_length(signature), (unsigned char)0x2A> __pattern_##name(signature); \
	struct pattern_t *name = reinterpret_cast<struct pattern_t *>(&(__pattern_##name))

#define DEFINE_PATTERN_IGNORE_BYTE(name, signature, ignorebyte) struct pattern_inner_wrapper<get_pattern_length(signature), (unsigned char)ignorebyte> __pattern_##name(signature); \
	struct pattern_t *name = reinterpret_cast<struct pattern_t *>(&(__pattern_##name))

#define DEFINE_NULL_PATTERN(name) struct pattern_t *name = 0

#define EXTERN_PATTERN(name) extern struct pattern_t *name

#define REPLACE_PATTERN(dest, src) dest = src

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

static inline constexpr unsigned int get_pattern_length(const char *pszPattern)
{
	unsigned int nCount = 0;

	while (*pszPattern)
	{
		char symbol = *pszPattern;

		if (symbol != ' ')
		{
			++nCount;

			if (symbol != '?')
				++pszPattern;
		}

		++pszPattern;
	}

	return nCount;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

template<unsigned int bytesCount, unsigned char ignoreByte>
struct pattern_inner_wrapper
{
	constexpr pattern_inner_wrapper(const char *pszPattern) : signature(), length(bytesCount), ignorebyte(ignoreByte)
	{
		unsigned int nLength = 0;

		while (*pszPattern)
		{
			char symbol = *pszPattern;

			if (symbol != ' ')
			{
				if (symbol != '?')
				{
					char byte[3] = { 0 };

					byte[0] = pszPattern[0];
					byte[1] = pszPattern[1];

					//signature[nLength] = static_cast<unsigned char>(strtoul(byte, NULL, 16));
					signature[nLength] = static_cast<unsigned char>(hex_to_decimal_fast(byte));

					++pszPattern;
				}
				else
				{
					signature[nLength] = ignorebyte;
				}

				++nLength;
			}

			++pszPattern;
		}
	}

	unsigned int length;
	unsigned char ignorebyte;
	unsigned char signature[bytesCount];
};

struct pattern_t
{
	unsigned int length;
	unsigned char ignorebyte;
	unsigned char signature;
};

struct pattern_reg_t
{
	const char *name;
	pattern_t *pattern;
};

//-----------------------------------------------------------------------------
// Macro templates
//-----------------------------------------------------------------------------

/* Example of multiple patterns:

DEFINE_PATTERNS_3(build_number,
				  "Sven-Coop 5.22",
				  "A1 ? ? ? ? 83 EC ? 56",
				  "Sven-Coop 5.25",
				  "51 A1 ? ? ? ? 56 33 F6",
				  "Sven-Coop 5.26",
				  "A1 ? ? ? ? 56 ? F6");

int patternIndex;
void *pfnbuild_number = MemoryUtils()->FindPattern( SvenModAPI()->Modules()->Hardware, build_number, &patternIndex );

if ( pfnbuild_number != NULL )
{
	Msg("Found pattern of function \"build_number\" for version \"%s\"\n", GET_PATTERN_NAME_BY_INDEX(build_number, patternIndex));
}

*/

#define _DEFINE_PATTERN_STATIC_NAMED(name, signature) static struct pattern_inner_wrapper<get_pattern_length(signature), (unsigned char)0x2A> name(signature)
#define _PATTERN_GET_POINTER(name) reinterpret_cast<struct pattern_t *>(&(name))

#define _DEFINE_PATTERN_REG(name, pattern_name) { name, _PATTERN_GET_POINTER(pattern_name) }
#define _DEFINE_PATTERNS_REG_BEGIN(name, count) constexpr int name##__count = count - 1; pattern_reg_t name[count] = {
#define _DEFINE_PATTERNS_REG_END() { 0, 0 } }

#define _PATTERN_EXPAND(a) a

#define _PATTERN_CONCATENATE_(a, b) a##b
#define _PATTERN_CONCATENATE(a, b) _PATTERN_CONCATENATE_(a, b)
#define _PATTERN_DEDUCE_NAME(name, counter) _PATTERN_CONCATENATE(name##__pattern_, _PATTERN_EXPAND(counter))

#define DEFINE_PATTERNS_FUTURE(name) std::vector<std::future<void *>> name
#define EXTERN_PATTERNS(name) extern pattern_reg_t name[]

#define GET_PATTERNS_COUNT(name) name##__count
#define GET_PATTERN_NAME_BY_INDEX(pattern_name, idx) pattern_name[idx].name

// Uhh, macros are still pretty hard for me to automate the routine with __VA_ARGS__
#define _DEFINE_PATTERNS_1(name, pattern1, pattern1_str, signature1) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 2) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_2(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 3) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_3(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 4) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_4(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 5) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_5(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 6) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_6(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5, pattern6, pattern6_str, signature6) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5);_DEFINE_PATTERN_STATIC_NAMED(pattern6, signature6); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 7) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERN_REG(pattern6_str, pattern6), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_7(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5, pattern6, pattern6_str, signature6, pattern7, pattern7_str, signature7) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5);_DEFINE_PATTERN_STATIC_NAMED(pattern6, signature6);_DEFINE_PATTERN_STATIC_NAMED(pattern7, signature7); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 8) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERN_REG(pattern6_str, pattern6), \
	_DEFINE_PATTERN_REG(pattern7_str, pattern7), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_8(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5, pattern6, pattern6_str, signature6, pattern7, pattern7_str, signature7, pattern8, pattern8_str, signature8) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5);_DEFINE_PATTERN_STATIC_NAMED(pattern6, signature6);_DEFINE_PATTERN_STATIC_NAMED(pattern7, signature7);_DEFINE_PATTERN_STATIC_NAMED(pattern8, signature8); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 9) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERN_REG(pattern6_str, pattern6), \
	_DEFINE_PATTERN_REG(pattern7_str, pattern7), \
	_DEFINE_PATTERN_REG(pattern8_str, pattern8), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_9(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5, pattern6, pattern6_str, signature6, pattern7, pattern7_str, signature7, pattern8, pattern8_str, signature8, pattern9, pattern9_str, signature9) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5);_DEFINE_PATTERN_STATIC_NAMED(pattern6, signature6);_DEFINE_PATTERN_STATIC_NAMED(pattern7, signature7);_DEFINE_PATTERN_STATIC_NAMED(pattern8, signature8);_DEFINE_PATTERN_STATIC_NAMED(pattern9, signature9); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 10) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERN_REG(pattern6_str, pattern6), \
	_DEFINE_PATTERN_REG(pattern7_str, pattern7), \
	_DEFINE_PATTERN_REG(pattern8_str, pattern8), \
	_DEFINE_PATTERN_REG(pattern9_str, pattern9), \
	_DEFINE_PATTERNS_REG_END()

#define _DEFINE_PATTERNS_10(name, pattern1, pattern1_str, signature1, pattern2, pattern2_str, signature2, pattern3, pattern3_str, signature3, pattern4, pattern4_str, signature4, pattern5, pattern5_str, signature5, pattern6, pattern6_str, signature6, pattern7, pattern7_str, signature7, pattern8, pattern8_str, signature8, pattern9, pattern9_str, signature9, pattern10, pattern10_str, signature10) \
	_DEFINE_PATTERN_STATIC_NAMED(pattern1, signature1);_DEFINE_PATTERN_STATIC_NAMED(pattern2, signature2);_DEFINE_PATTERN_STATIC_NAMED(pattern3, signature3);_DEFINE_PATTERN_STATIC_NAMED(pattern4, signature4);_DEFINE_PATTERN_STATIC_NAMED(pattern5, signature5);_DEFINE_PATTERN_STATIC_NAMED(pattern6, signature6);_DEFINE_PATTERN_STATIC_NAMED(pattern7, signature7);_DEFINE_PATTERN_STATIC_NAMED(pattern8, signature8);_DEFINE_PATTERN_STATIC_NAMED(pattern9, signature9);_DEFINE_PATTERN_STATIC_NAMED(pattern10, signature10); \
	_DEFINE_PATTERNS_REG_BEGIN(name, 11) \
	_DEFINE_PATTERN_REG(pattern1_str, pattern1), \
	_DEFINE_PATTERN_REG(pattern2_str, pattern2), \
	_DEFINE_PATTERN_REG(pattern3_str, pattern3), \
	_DEFINE_PATTERN_REG(pattern4_str, pattern4), \
	_DEFINE_PATTERN_REG(pattern5_str, pattern5), \
	_DEFINE_PATTERN_REG(pattern6_str, pattern6), \
	_DEFINE_PATTERN_REG(pattern7_str, pattern7), \
	_DEFINE_PATTERN_REG(pattern8_str, pattern8), \
	_DEFINE_PATTERN_REG(pattern9_str, pattern9), \
	_DEFINE_PATTERN_REG(pattern10_str, pattern10), \
	_DEFINE_PATTERNS_REG_END()

// 1 pattern
#define DEFINE_PATTERNS_1(name, pattern1, signature1) _DEFINE_PATTERNS_1(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1)

// 2 patterns
#define DEFINE_PATTERNS_2(name, pattern1, signature1, pattern2, signature2) _DEFINE_PATTERNS_2(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2)

// 3 patterns
#define DEFINE_PATTERNS_3(name, pattern1, signature1, pattern2, signature2, pattern3, signature3) _DEFINE_PATTERNS_3(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3)

// 4 patterns
#define DEFINE_PATTERNS_4(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4) _DEFINE_PATTERNS_4(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4)

// 5 patterns
#define DEFINE_PATTERNS_5(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5) _DEFINE_PATTERNS_5(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5)

// 6 patterns
#define DEFINE_PATTERNS_6(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5, pattern6, signature6) _DEFINE_PATTERNS_6(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5, _PATTERN_DEDUCE_NAME(arg6, __COUNTER__), pattern6, signature6)

// 7 patterns
#define DEFINE_PATTERNS_7(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5, pattern6, signature6, pattern7, signature7) _DEFINE_PATTERNS_7(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5, _PATTERN_DEDUCE_NAME(arg6, __COUNTER__), pattern6, signature6, _PATTERN_DEDUCE_NAME(arg7, __COUNTER__), pattern7, signature7)

// 8 patterns
#define DEFINE_PATTERNS_8(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5, pattern6, signature6, pattern7, signature7, pattern8, signature8) _DEFINE_PATTERNS_8(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5, _PATTERN_DEDUCE_NAME(arg6, __COUNTER__), pattern6, signature6, _PATTERN_DEDUCE_NAME(arg7, __COUNTER__), pattern7, signature7, _PATTERN_DEDUCE_NAME(arg8, __COUNTER__), pattern8, signature8)

// 9 patterns
#define DEFINE_PATTERNS_9(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5, pattern6, signature6, pattern7, signature7, pattern8, signature8, pattern9, signature9) _DEFINE_PATTERNS_9(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5, _PATTERN_DEDUCE_NAME(arg6, __COUNTER__), pattern6, signature6, _PATTERN_DEDUCE_NAME(arg7, __COUNTER__), pattern7, signature7, _PATTERN_DEDUCE_NAME(arg8, __COUNTER__), pattern8, signature8, _PATTERN_DEDUCE_NAME(arg9, __COUNTER__), pattern9, signature9)

// 10 patterns
#define DEFINE_PATTERNS_10(name, pattern1, signature1, pattern2, signature2, pattern3, signature3, pattern4, signature4, pattern5, signature5, pattern6, signature6, pattern7, signature7, pattern8, signature8, pattern9, signature9, pattern10, signature10) _DEFINE_PATTERNS_10(name, _PATTERN_DEDUCE_NAME(arg1, __COUNTER__), pattern1, signature1, _PATTERN_DEDUCE_NAME(arg2, __COUNTER__), pattern2, signature2, _PATTERN_DEDUCE_NAME(arg3, __COUNTER__), pattern3, signature3, _PATTERN_DEDUCE_NAME(arg4, __COUNTER__), pattern4, signature4, _PATTERN_DEDUCE_NAME(arg5, __COUNTER__), pattern5, signature5, _PATTERN_DEDUCE_NAME(arg6, __COUNTER__), pattern6, signature6, _PATTERN_DEDUCE_NAME(arg7, __COUNTER__), pattern7, signature7, _PATTERN_DEDUCE_NAME(arg8, __COUNTER__), pattern8, signature8, _PATTERN_DEDUCE_NAME(arg9, __COUNTER__), pattern9, signature9, _PATTERN_DEDUCE_NAME(arg10, __COUNTER__), pattern10, signature10)

#endif // MPATTERNS_H