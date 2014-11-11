#pragma once
#include "Math.h"

struct Vector3
{
	float X, Y, Z;

	bool HasNaNs() const { return isnan(X) || isnan(Y) || isnan(Z); }

	Vector3()
		: X(0), Y(0), Z(0)
	{
	}

	Vector3(float x, float y, float z)
		: X(x), Y(y), Z(z)
	{
	}

	Vector3 operator+(const Vector3 &v) const
	{
		return Vector3(X + v.X, Y + v.Y, Z + v.Z);
	}

	Vector3& operator+=(const Vector3 &v)
	{
		X += v.X; Y += v.Y; Z += v.Z;
		return *this;
	}
	Vector3 operator-(const Vector3 &v) const
	{
		return Vector3(X - v.X, Y - v.Y, Z - v.Z);
	}

	Vector3& operator-=(const Vector3 &v)
	{
		X -= v.X; Y -= v.Y; Z -= v.Z;
		return *this;
	}
	Vector3 operator*(float f) const
	{
		return Vector3(f*X, f*Y, f*Z);
	}

	Vector3 &operator*=(float f)
	{
		X *= f; Y *= f; Z *= f;
		return *this;
	}
	Vector3 operator/(float f) const
	{
		float inv = 1.f / f;
		return Vector3(X * inv, Y * inv, Z * inv);
	}

	Vector3 &operator/=(float f)
	{
		float inv = 1.f / f;
		X *= inv; Y *= inv; Z *= inv;
		return *this;
	}
	Vector3 operator-() const
	{
		return Vector3(-X, -Y, -Z);
	}
	float operator[](int i) const
	{
		return (&X)[i];
	}

	float &operator[](int i)
	{
		return (&X)[i];
	}
	float LengthSquared() const
	{
		return X*X + Y*Y + Z*Z;
	}
	float Length() const
	{
		return sqrt(LengthSquared());
	}
	Vector3 Normalize() const { return *this / this->Length(); }

	inline static Vector3 Cross(const Vector3& p1, const Vector3& p2)
	{
		Vector3 result;
		result.X = p1.Y * p2.Z - p1.Z * p2.Y;
		result.Y = p1.Z * p2.X - p1.X * p2.Z;
		result.Z = p1.X * p2.Y - p1.Y * p2.X;
		return result;
	}

	inline static float Dot(const Vector3& p1, const Vector3& p2)
	{
		return p1 * p2;
	}

	inline float operator *(const Vector3& p) const
	{
		return X * p.X + Y * p.Y + Z * p.Z;
	}

	bool operator==(const Vector3 &v) const
	{
		return X == v.X && Y == v.Y && Z == v.Z;
	}
	bool operator!=(const Vector3 &v) const
	{
		return X != v.X || Y != v.Y || Z != v.Z;
	}
};

inline Vector3 operator *(float f, const Vector3& v)
{
	return v * f;
}


struct Vector4
{
	float X, Y, Z, W;

	bool HasNaNs() const { return isnan(X) || isnan(Y) || isnan(Z); }

	Vector4()
		: X(0), Y(0), Z(0)
	{
	}

	Vector4(float x, float y, float z)
		: X(x), Y(y), Z(z)
	{
	}

	Vector4 operator+(const Vector4 &v) const
	{
		Vector4 result;
		for (int i = 0; i < 4; i++)
			result[i] = (*this)[i] + v[i];
		return result;
	}

	Vector4& operator+=(const Vector4 &v)
	{
		for (int i = 0; i < 4; i++)
			(*this)[i] = (*this)[i] + v[i];
		return *this;
	}
	Vector4 operator-(const Vector4 &v) const
	{
		Vector4 result;
		for (int i = 0; i < 4; i++)
			result[i] = (*this)[i] - v[i];
		return result;
	}

	Vector4& operator-=(const Vector4 &v)
	{
		for (int i = 0; i < 4; i++)
			(*this)[i] = (*this)[i] - v[i];
		return *this;
	}
	Vector4 operator*(float f) const
	{
		Vector4 result;
		for (int i = 0; i < 4; i++)
			result[i] = (*this)[i] * f;
		return result;
	}

	Vector4 &operator*=(float f)
	{
		for (int i = 0; i < 4; i++)
			(*this)[i] = (*this)[i] * f;
		return *this;
	}
	Vector4 operator/(float f) const
	{
		return *this * (1.0f / f);
	}

	Vector4 &operator/=(float f)
	{
		return *this *= (1.0f / f);
	}
	Vector4 operator-() const
	{
		Vector4 result;
		for (int i = 0; i < 4; i++)
			result[i] = -(*this)[i];
		return result;
	}
	float operator[](int i) const
	{
		return (&X)[i];
	}

	float &operator[](int i)
	{
		return (&X)[i];
	}
	float LengthSquared() const
	{
		return *this * *this;
	}
	float Length() const
	{
		return sqrt(LengthSquared());
	}
	Vector4 Normalize() const { return *this / this->Length(); }

	inline static Vector4 Cross(const Vector4& p1, const Vector4& p2)
	{
		Vector4 result;
		result.X = p1.Y * p2.Z - p1.Z * p2.Y;
		result.Y = p1.Z * p2.X - p1.X * p2.Z;
		result.Z = p1.X * p2.Y - p1.Y * p2.X;
		return result;
	}

	inline static float Dot(const Vector4& p1, const Vector4& p2)
	{
		return p1 * p2;
	}

	inline float operator *(const Vector4& p) const
	{
		Vector4 result;
		for (int i = 0; i < 4; i++)
			result[i] = (*this)[i] * p[i];
		return result.X + result.Y + result.Z;
	}

	bool operator==(const Vector4 &v) const
	{
		return X == v.X && Y == v.Y && Z == v.Z;
	}
	bool operator!=(const Vector4 &v) const
	{
		return X != v.X || Y != v.Y || Z != v.Z;
	}
};

inline Vector4 operator *(float f, const Vector4& v)
{
	return v * f;
}

typedef Vector3 Vector;