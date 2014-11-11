#pragma once
#include "Math.h"

struct Color
{
public:
	float R, G, B;

public:
	Color() : R(0), G(0), B(0) { }

	Color(float r, float g, float b)
		: R(r), G(g), B(b)
	{
	}

	Color(float value)
		: R(value), G(value), B(value)
	{
	}

	Color(char r, char g, char b)
		: R(r / 255.0f), G(g / 255.0f), B(b / 255.0f)
	{
	}

	Color(int value)
		: R(((value >> 16) & 0xFF) / 255.0f), G(((value >> 8) & 0xFF) / 255.0f), B((value & 0xFF) / 255.0f)
	{
	}

	Color(unsigned int value)
		: R(((value >> 16) & 0xFF) / 255.0f), G(((value >> 8) & 0xFF) / 255.0f), B((value & 0xFF) / 255.0f)
	{
	}

	void operator +=(const Color& color)
	{
		this->R += color.R;
		this->G += color.G;
		this->B += color.B;
	}
	void operator -=(const Color& color)
	{
		this->R -= color.R;
		this->G -= color.G;
		this->B -= color.B;
	}
	void operator *=(float f)
	{
		this->R *= f;
		this->G *= f;
		this->B *= f;
	}
	void operator /=(float f)
	{
		f = 1.0f / f;
		this->R *= f;
		this->G *= f;
		this->B *= f;
	}

	Color operator +(const Color& color) const
	{
		return Color(R + color.R, G + color.G, B + color.B);
	}
	Color operator -(const Color& color) const
	{
		return Color(R - color.R, G - color.G, B - color.B);
	}
	Color operator *(float f) const
	{
		return Color(R * f, G * f, B * f);
	}
	Color operator /(float f) const
	{
		f = 1.0f / f;
		return Color(R * f, G * f, B * f);
	}

	Color operator *(const Color& color) const
	{
		return Color(R * color.R, G * color.G, B * color.B);
	}
	Color& operator *=(const Color& color)
	{
		R *= color.R;
		G *= color.G;
		B *= color.B;
		return *this;
	}

	bool operator ==(const Color& color) const
	{
		return R == color.R && G == color.G && B == color.B;
	}
	bool operator !=(const Color& color) const
	{
		return R != color.R || G != color.G || B != color.B;
	}

	void operator =(int rgb)
	{
		R = ((rgb >> 16) & 0xFF) / 255.0f;
		G = ((rgb >> 8) & 0xFF) / 255.0f;
		B = (rgb & 0xFF) / 255.0f;
	}
	void operator =(unsigned int rgb)
	{
		R = ((rgb >> 16) & 0xFF) / 255.0f;
		G = ((rgb >> 8) & 0xFF) / 255.0f;
		B = (rgb & 0xFF) / 255.0f;
	}

	unsigned int ToArgb() const
	{
		float r = R, g = G, b = B;
		if (r < 0) r = 0;
		if (g < 0) g = 0;
		if (b < 0) b = 0;
		auto maxf = max(r, max(g, b));
		if (maxf > 1)
		{
			maxf = 1 / maxf;
			r *= maxf;
			g *= maxf;
			b *= maxf;
		}
		return (((unsigned int) 255 * 256 + (unsigned int) (r * 255.0f)) * 256 + (unsigned int) (g * 255.0f)) * 256 + (unsigned int) (b * 255.0f);
	}

	unsigned int ToArgb(float gamma) const
	{
		return Color().ToArgb();
	}

	float ToGray() const
	{
		return max(0, min(1, R * 0.299f + G * 0.587f + B * 0.114f));
	}

	enum : unsigned int
	{
		Black = 0xFF000000,
		White = 0xFFFFFFFF,
		DarkGray = 0xFF404040,
		Gray = 0xFF808080,
		LightGray = 0xFFC0C0C0,
		Red = 0xFFFF0000,
		Green = 0xFF00FF00,
		Blue = 0xFF0000FF,
		SkyBlue = 0xFF6495ED
	};
};