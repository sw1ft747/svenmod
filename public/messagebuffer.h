#ifndef MESSAGEBUFFER_H
#define MESSAGEBUFFER_H

#ifdef _WIN32
#pragma once
#endif

#include "hl_sdk/common/sizebuf.h"
#include "hl_sdk/common/usercmd.h"

class CMessageBuffer
{
public:
	CMessageBuffer( void );
	CMessageBuffer( int iBufferSize );
	CMessageBuffer( void *buffer, int iSize, bool bBufferFilled = false );

	CMessageBuffer( sizebuf_t *buffer );
	CMessageBuffer( const sizebuf_t *buffer );

	CMessageBuffer( sizebuf_t *buffer, int iReadCount, bool bBadRead = false );
	CMessageBuffer( const sizebuf_t *buffer, int iReadCount, bool bBadRead = false );

	~CMessageBuffer();

public:
	void Init( void );
	void Init( void *buffer, int iSize, bool bBufferFilled = false );

	void Init( sizebuf_t *buffer );
	void Init( const sizebuf_t *buffer );

	void Init( sizebuf_t *buffer, int iReadCount, bool bBadRead = false );
	void Init( const sizebuf_t *buffer, int iReadCount, bool bBadRead = false );

public:
	const sizebuf_t *GetBuffer( void ) const;
	bool FreeBuffer( void );

	void WriteChar( int c );
	void WriteByte( int c);
	void WriteShort( int c );
	void WriteWord( int c );
	void WriteLong( int c );
	void WriteFloat( float f );
	void WriteBuf( void *buffer, int size );
	void WriteString( char *s );
	void WriteCoord( float f );
	void WriteAngle( float f );
	void WriteHiresAngle( float f );
	void WriteUsercmd( usercmd_t *from, usercmd_t *cmd );
	
	void BeginReading( void );
	int GetReadCount( void ) const;
	bool ReadOK( void ) const;

	int ReadChar( void ) const;
	int ReadByte( void ) const;
	int ReadShort( void ) const;
	int ReadWord( void ) const;
	int ReadLong( void ) const;
	float ReadFloat( void ) const;
	int ReadBuf( void *buffer, int size ) const;
	char *ReadString( void ) const;
	char *ReadStringLine( void ) const;
	float ReadCoord( void ) const;
	float ReadAngle( void ) const;
	float ReadHiresAngle( void ) const;
	void ReadDeltaUsercmd( usercmd_t *from, usercmd_t *cmd ) const;

private:
	void Clear( void );
	void *GetSpace( int length );
	void Write( void *data, int length );

private:
	sizebuf_t *m_pBuffer;
	sizebuf_t m_Buffer;

	mutable int m_iReadCount;
	mutable bool m_bBadRead;

	bool m_bBufferAllocated;
};

#endif // MESSAGEBUFFER_H