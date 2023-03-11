#include <messagebuffer.h>

#include <hl_sdk/common/Platform.h>
#include <hl_sdk/common/protocol.h>
#include <dbg.h>
#include <sys.h>

CMessageBuffer::CMessageBuffer(void)
{
	Init();
}

CMessageBuffer::CMessageBuffer(int iBufferSize)
{
	m_pBuffer = &m_Buffer;

	m_Buffer.data = (byte *)malloc(iBufferSize * sizeof(byte));
	m_Buffer.maxsize = iBufferSize;
	m_Buffer.cursize = 0;

	m_Buffer.allowoverflow = 0;
	m_Buffer.overflowed = 0;

	m_iReadCount = 0;
	m_bBadRead = false;

	m_bBufferAllocated = true;
}

CMessageBuffer::CMessageBuffer(void *buffer, int iSize, bool bBufferFilled /* = false */)
{
	Init(buffer, iSize, bBufferFilled);
}

CMessageBuffer::CMessageBuffer(sizebuf_t *buffer)
{
	Init(buffer);
}

CMessageBuffer::CMessageBuffer(const sizebuf_t *buffer)
{
	Init(buffer);
}

CMessageBuffer::CMessageBuffer(sizebuf_t *buffer, int iReadCount, bool bBadRead /* = false */)
{
	Init(buffer, iReadCount, bBadRead);
}

CMessageBuffer::CMessageBuffer(const sizebuf_t *buffer, int iReadCount, bool bBadRead /* = false */)
{
	Init(buffer, iReadCount, bBadRead);
}

void CMessageBuffer::Init(void)
{
	m_pBuffer = &m_Buffer;

	m_Buffer.data = NULL;
	m_Buffer.maxsize = 0;
	m_Buffer.cursize = 0;

	m_Buffer.allowoverflow = 0;
	m_Buffer.overflowed = 0;

	m_iReadCount = 0;
	m_bBadRead = false;

	m_bBufferAllocated = false;
}

void CMessageBuffer::Init(void *buffer, int iSize, bool bBufferFilled /* = false */)
{
	m_pBuffer = &m_Buffer;

	m_Buffer.data = (byte *)buffer;
	m_Buffer.maxsize = iSize;
	m_Buffer.cursize = bBufferFilled ? iSize : 0;

	m_Buffer.allowoverflow = 0;
	m_Buffer.overflowed = 0;

	m_iReadCount = 0;
	m_bBadRead = false;

	m_bBufferAllocated = false;
}

void CMessageBuffer::Init(sizebuf_t *buffer)
{
	m_pBuffer = buffer;

	m_iReadCount = 0;
	m_bBadRead = false;

	m_bBufferAllocated = false;
}

void CMessageBuffer::Init(const sizebuf_t *buffer)
{
	m_pBuffer = const_cast<sizebuf_t *>(buffer);

	m_iReadCount = 0;
	m_bBadRead = false;

	m_bBufferAllocated = false;
}

void CMessageBuffer::Init(sizebuf_t *buffer, int iReadCount, bool bBadRead /* = false */)
{
	m_pBuffer = buffer;

	m_iReadCount = iReadCount;
	m_bBadRead = bBadRead;

	m_bBufferAllocated = false;
}

void CMessageBuffer::Init(const sizebuf_t *buffer, int iReadCount, bool bBadRead /* = false */)
{
	m_pBuffer = const_cast<sizebuf_t *>(buffer);

	m_iReadCount = iReadCount;
	m_bBadRead = bBadRead;

	m_bBufferAllocated = false;
}

CMessageBuffer::~CMessageBuffer()
{
	FreeBuffer();
}

const sizebuf_t *CMessageBuffer::GetBuffer() const
{
	return const_cast<const sizebuf_t *>(m_pBuffer);
}

bool CMessageBuffer::FreeBuffer(void)
{
	if (m_bBufferAllocated)
	{
		free( (void *)m_Buffer.data );
		m_bBufferAllocated = false;

		return true;
	}

	return false;
}

void CMessageBuffer::WriteChar(int c)
{
	byte *buf;

	buf = (byte *)GetSpace(1);
	buf[0] = c;
}

void CMessageBuffer::WriteByte(int c)
{
	byte *buf;

	buf = (byte *)GetSpace(1);
	buf[0] = c;
}

void CMessageBuffer::WriteShort(int c)
{
	byte *buf;

	buf = (byte *)GetSpace(2);
	buf[0] = c & 0xff;
	buf[1] = c >> 8;
}

void CMessageBuffer::WriteWord(int c)
{
	WriteShort(c);
}

void CMessageBuffer::WriteLong(int c)
{
	byte *buf;

	buf = (byte *)GetSpace(4);
	buf[0] = c & 0xff;
	buf[1] = (c >> 8) & 0xff;
	buf[2] = (c >> 16) & 0xff;
	buf[3] = c >> 24;
}

void CMessageBuffer::WriteFloat(float f)
{
	union
	{
		float	f;
		int		l;
	} dat;


	dat.f = f;

	Write(&dat.l, 4);
}

void CMessageBuffer::WriteBuf(void *buffer, int size)
{
	if ( !buffer )
		return;

	byte *data = (byte *)GetSpace(size);

	if ( m_pBuffer->overflowed )
		return;

	int buf_size = size;
	unsigned int buf_ptr_val = (unsigned int)&data[0];
	byte *buf = (byte *)buffer;

	if ( (unsigned long)size >= 8 )
	{
		if ( buf_ptr_val & 1 )
		{
			++buf_ptr_val;
			buf = (byte *)buffer + 1;
			buf_size = size - 1;
			*(byte *)(buf_ptr_val - 1) = *(byte *)buffer;
		}

		if ( buf_ptr_val & 2 )
		{
			unsigned short value = *(unsigned short *)buf;

			buf_ptr_val += 2;
			buf += 2;
			buf_size -= 2;

			*(unsigned short *)(buf_ptr_val - 2) = value;
		}

		if ( buf_ptr_val & 4 )
		{
			unsigned long *p = (unsigned long *)(buf_ptr_val + 4);
			*(p - 1) = *(unsigned long *)buf;

			memcpy( p, buf + 4, buf_size - 4 );
		}
		else
		{
			memcpy( (void *)buf_ptr_val, buf, buf_size );
		}
	}
	else
	{
		memcpy( (void *)buf_ptr_val, buf, buf_size );
	}
}

void CMessageBuffer::WriteString(char *s)
{
	if ( !s )
		Write((void *)(""), 1);
	else
		Write(s, strlen(s) + 1);
}

void CMessageBuffer::WriteCoord(float f)
{
	WriteLong( (int)(f * 8.f) );
}

void CMessageBuffer::WriteAngle(float f)
{
	WriteByte( (int)(f * 256.f / 360.f) & 0xFF );
}

void CMessageBuffer::WriteHiresAngle(float f)
{
	WriteShort( (int)(f * 65536.f / 360.f) & 0xFFFF );
}

void CMessageBuffer::WriteUsercmd(usercmd_t *from, usercmd_t *cmd)
{
	// ToDo
}

void CMessageBuffer::BeginReading(void)
{
	m_iReadCount = 0;
	m_bBadRead = false;
}

int CMessageBuffer::GetReadCount(void) const
{
	return m_iReadCount;
}

bool CMessageBuffer::ReadOK(void) const
{
	return !m_bBadRead;
}

int CMessageBuffer::ReadChar(void) const
{
	int	c;

	if (m_iReadCount + 1 > m_pBuffer->cursize)
	{
		m_bBadRead = true;
		return -1;
	}

	c = (signed char)m_pBuffer->data[m_iReadCount];
	m_iReadCount++;

	return c;
}

int CMessageBuffer::ReadByte(void) const
{
	int	c;

	if (m_iReadCount + 1 > m_pBuffer->cursize)
	{
		m_bBadRead = true;
		return -1;
	}

	c = (unsigned char)m_pBuffer->data[m_iReadCount];
	m_iReadCount++;

	return c;
}

int CMessageBuffer::ReadShort(void) const
{
	int	c;

	if (m_iReadCount + 2 > m_pBuffer->cursize)
	{
		m_bBadRead = true;
		return -1;
	}

	c = (short)(m_pBuffer->data[m_iReadCount] + (m_pBuffer->data[m_iReadCount + 1] << 8));

	m_iReadCount += 2;

	return c;
}

int CMessageBuffer::ReadWord(void) const
{
	return ReadShort();
}

int CMessageBuffer::ReadLong(void) const
{
	int	c;

	if (m_iReadCount + 4 > m_pBuffer->cursize)
	{
		m_bBadRead = true;
		return -1;
	}

	c = m_pBuffer->data[m_iReadCount] + (m_pBuffer->data[m_iReadCount + 1] << 8) + (m_pBuffer->data[m_iReadCount + 2] << 16) + (m_pBuffer->data[m_iReadCount + 3] << 24);

	m_iReadCount += 4;

	return c;
}

float CMessageBuffer::ReadFloat(void) const
{
	union
	{
		byte	b[4];
		float	f;
		int	l;
	} dat;

	dat.b[0] = m_pBuffer->data[m_iReadCount];
	dat.b[1] = m_pBuffer->data[m_iReadCount + 1];
	dat.b[2] = m_pBuffer->data[m_iReadCount + 2];
	dat.b[3] = m_pBuffer->data[m_iReadCount + 3];
	m_iReadCount += 4;

	return dat.f;
}

int CMessageBuffer::ReadBuf(void *buffer, int size) const
{
	if (m_iReadCount + size > m_pBuffer->cursize)
	{
		m_bBadRead = true;
		return -1;
	}

	byte *data = (byte *)(m_pBuffer->data + m_iReadCount);

	int buf_size = size;
	byte *buf = (byte *)buffer;

	if ( (unsigned long)size >= 8 )
	{
		if ( (byte)((unsigned long)buffer & 0xFF) & 1 )
		{
			byte value = *data;

			buf = (byte *)buffer + 1;
			data++;
			buf_size = size - 1;

			*(byte *)buffer = value;
		}

		if ( (byte)((unsigned long)buf & 0xFF) & 2 )
		{
			unsigned short value = *(unsigned short *)data;

			buf += 2;
			data += 2;
			buf_size -= 2;

			*((unsigned short *)buf - 1) = value;
		}

		if ( (byte)((unsigned long)buf & 0xFF) & 4 )
		{
			unsigned long value = *(unsigned long *)data;

			buf += 4;
			data += 4;
			buf_size -= 4;

			*((unsigned long *)buf - 1) = value;
		}
	}

	memcpy(buf, data, buf_size);
	m_iReadCount += size;

	return 1;
}

char *CMessageBuffer::ReadString(void) const
{
	static char	string[2048];
	int		l, c;

	l = 0;
	do
	{
		c = ReadChar();
		if (c == -1 || c == 0)
			break;
		string[l] = c;
		l++;
	} while (l < M_ARRAYSIZE(string) - 1);

	string[l] = 0;

	return string;
}

char *CMessageBuffer::ReadStringLine(void) const
{
	static char	string[2048];
	int		l, c;

	l = 0;
	do
	{
		c = ReadChar();
		if (c == -1 || c == 0 || c == '\n')
			break;
		string[l] = c;
		l++;
	} while (l < M_ARRAYSIZE(string) - 1);

	string[l] = 0;

	return string;
}

float CMessageBuffer::ReadCoord(void) const
{
	return (float)ReadLong() * (1.f / 8.f);
}

float CMessageBuffer::ReadAngle(void) const
{
	return (float)ReadChar() * (360.f / 256.f);
}

float CMessageBuffer::ReadHiresAngle(void) const
{
	return (float)ReadShort() * (360.f / 65536.f);
}

void CMessageBuffer::ReadDeltaUsercmd(usercmd_t *from, usercmd_t *cmd) const
{
	// ToDo
}

void CMessageBuffer::Clear(void)
{
	m_pBuffer->cursize = 0;
	m_pBuffer->overflowed = false;
}

void *CMessageBuffer::GetSpace(int length)
{
	if ( m_pBuffer->cursize + length > m_pBuffer->maxsize )
	{
		if ( !m_pBuffer->allowoverflow )
			Sys_Error("[SvenMod] CMessageBuffer::GetSpace: overflow without allowoverflow set (%d)", m_pBuffer->maxsize);

		if ( length > m_pBuffer->maxsize )
			Sys_Error("[SvenMod] CMessageBuffer::GetSpace: %i is > full buffer size", length);

		Warning("[SvenMod] CMessageBuffer::GetSpace: overflow\n");
		Clear();

		m_pBuffer->overflowed = true;
	}

	void *data = m_pBuffer->data + m_pBuffer->cursize;
	m_pBuffer->cursize += length;

	return data;
}

void CMessageBuffer::Write(void *data, int length)
{
	memcpy( GetSpace(length), data, length );
}