#ifndef COLOR_H
#define COLOR_H

#ifdef _WIN32
#pragma once
#endif

class Color
{
public:
	inline Color(int r, int g, int b, int a) { SetColor(r, g, b, a); }

	inline void SetColor(int r, int g, int b, int a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
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