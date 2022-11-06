#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

class Color
{
public:
	inline Color() { *(int *)&r = 0xFFFFFFFF; }
	inline Color(const Color &clr) { SetColor(clr); }
	inline Color(int r, int g, int b, int a) { SetColor(r, g, b, a); }
	inline Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) { SetColor(r, g, b, a); }
	inline Color(float r, float g, float b, float a) { SetColor(r, g, b, a); }

	inline void SetColor(const Color &clr)
	{
		*(int *)&r = *(int *)(&clr.r);
	}

	inline void SetColor(int r, int g, int b, int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}

	inline void SetColor(float r, float g, float b, float a)
	{
		this->r = int(255.f * r);
		this->g = int(255.f * g);
		this->b = int(255.f * b);
		this->a = int(255.f * a);
	}

	inline void GetColor(int &r, int &g, int &b, int &a)
	{
		r = this->r;
		g = this->g;
		b = this->b;
		a = this->a;
	}
	
	inline void GetColor(int *r, int *g, int *b, int *a)
	{
		if (r)
			*r = this->r;

		if (g)
			*g = this->g;

		if (b)
			*b = this->b;

		if (a)
			*a = this->a;
	}

	inline int GetColor()
	{
		int color = 0;

		unsigned char *pColorElement = (unsigned char *)&color;

		pColorElement[0] = r;
		pColorElement[1] = g;
		pColorElement[2] = b;
		pColorElement[3] = a;

		return color;
	}

	inline unsigned char &operator[](int id) { return ((unsigned char *)&r)[id]; }

public:
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

#endif // COLOR_H