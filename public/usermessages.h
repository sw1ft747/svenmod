/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
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

#ifndef USERMESSAGES_H
#define USERMESSAGES_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// UserMessages namespace
//-----------------------------------------------------------------------------

namespace UserMessages
{
	// Parse user messages
	void		BeginRead(void *pBuffer, int iSize);

	int			ReadChar();
	int			ReadByte();
	int			ReadShort();
	int			ReadWord();
	int			ReadLong();
	float		ReadFloat();
	char		*ReadString();
	float		ReadCoord();
	float		ReadAngle();
	float		ReadHiresAngle();

	bool		ReadOK();
}

//-----------------------------------------------------------------------------
// BufferWriter
//-----------------------------------------------------------------------------

class BufferWriter
{
public:
	BufferWriter();
	BufferWriter(unsigned char *pBuffer, int bufferLen);
	void Init(unsigned char *pBuffer, int bufferLen);

	void WriteByte(unsigned char data);
	void WriteLong(int data);
	void WriteString(const char *str);

	bool HasOverflowed();
	int GetSpaceUsed();

protected:
	unsigned char *m_buffer;
	int m_remaining;
	bool m_overflow;
	int m_overallLength;
};

#endif // USERMESSAGES_H