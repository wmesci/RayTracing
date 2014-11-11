#pragma once
#include "Vector.h"
#include "Color.h"
#include <limits> 
#include <functional>
#include <vector>

using namespace std;

struct Ray
{
	Vector Position;
	Vector Direction;
	mutable float MaxDistance;
};

template<typename T>
using Sampler = function<T(const Vector&)> ;

struct Material
{
	Sampler<Color> Diffuse;      //漫反射颜色
	Sampler<Color> Specular;     //镜面反射颜色
	Sampler<float> Power;        //镜面高光系数
	Sampler<float> Refraction;   //折射率
	Sampler<Color> Transparency; //透明度
};

struct HitTestResult;

class Shape
{
public:
	Material Material;
	virtual bool HitTest(const Ray& ray, HitTestResult* result) const = 0;
};

struct HitTestResult
{
	Shape* Shape;
	Vector Position;
	Vector Normal;
	float Distance;

	HitTestResult()
		: Shape(nullptr), Position(), Normal(), Distance((numeric_limits<float>::max)())
	{
	}
};

struct Camera
{
	Vector Position;
	Vector Direction;
	Vector Y;
	Vector X;

	Camera(const Vector& Position, const Vector& Taget)
		: Position(Position)
	{
		Vector forward = (Taget - Position).Normalize();
		Vector down(0, -1, 0);
		Vector right = Vector::Cross(forward, down).Normalize();
		right *= 2;
		Vector up = Vector::Cross(forward, right).Normalize();
		up *= 1.5f;

		this->Direction = forward;
		this->Y = up;
		this->X = right;
	}
};

struct Light
{
	Vector Position;
	Color Color;
};

class Sphere : public Shape
{
public:
	Vector Center;
	float Radius;

	virtual bool HitTest(const Ray& ray, HitTestResult* result) const override
	{
		Vector eo = Center - ray.Position;
		float v = eo * ray.Direction;
		auto disc = Radius * Radius - (eo * eo) + v * v;
		if (disc < 0) return false;
		disc = v - sqrt(disc);
		if (disc < 0.001f || disc >= ray.MaxDistance) return false;
		result->Shape = const_cast<Sphere*>(this);
		result->Normal = (disc * ray.Direction - eo).Normalize();
		result->Distance = disc;
		return true;
	}
};

class Plane : public Shape
{
public:
	Vector Normal;
	float Offset;

	virtual bool HitTest(const Ray& ray, HitTestResult* result) const override
	{
		auto denom = Normal * ray.Direction;
		if (denom > 0) return false;
		auto d = (Normal * ray.Position + Offset) / (-denom);
		if (d >= ray.MaxDistance) return false;
		result->Shape = const_cast<Plane*>(this);
		result->Normal = Normal;
		result->Distance = d;
		return true;
	}
};

class Triangle : public Shape
{
public:
	Vector Point1;
	Vector Point2;
	Vector Point3;

	Triangle(const Vector& Point1, const Vector& Point2, const Vector& Point3)
		: Point1(Point1), Point2(Point2), Point3(Point3)
	{
		auto n1 = Point2 - Point1;
		auto n2 = Point3 - Point1;
		normal = Vector::Cross((Point2 - Point1), (Point3 - Point1)).Normalize();
	}

	virtual bool HitTest(const Ray& ray, HitTestResult* result) const override
	{
		float eo;
		if (normal.Length() != 0 && (eo = ray.Direction * normal) < 0)
		{
			auto S = (Point1 - ray.Position) * normal / eo;
			if (S < 0.001f || S >= ray.MaxDistance)
				return false;

			auto V = S * ray.Direction + ray.Position;

			if (IsInner(V))
			{
				result->Shape = const_cast<Triangle*>(this);
				result->Normal = normal;
				result->Distance = S;
				return true;
			}
			return false;
		}
		return false;
	}

private:
	Vector normal;

	bool IsInner(const Vector& V) const
	{
		auto V1 = (Point1 - V).Normalize();
		auto V2 = (Point2 - V).Normalize();
		auto V3 = (Point3 - V).Normalize();
		auto a1 = acos(V1 * V2);
		auto a2 = acos(V2 * V3);
		auto a3 = acos(V3 * V1);
		auto a = a1 + a2 + a3 - 2 * pi;
		if (a <= 0.0001 && a >= -0.0001) return true;
		return false;
	}

};

class Scene
{
public:
	vector<Shape*> Children;
	vector<Light> Lights;
	Camera Camera;
	Color Ambient;

	Scene()
		: Camera(Vector(), Vector())
	{
	}

	inline bool HitTest(const Ray& ray, HitTestResult* result) const
	{
		bool hit = false;
		for (unsigned int i = 0; i < Children.size(); i++)
		{
			HitTestResult hittest;
			if (Children[i]->HitTest(ray, &hittest))
			{
				*result = hittest;
				ray.MaxDistance = hittest.Distance;
				hit = true;
			}
		}
		return hit;
	}

	inline bool HitTest(const Ray& ray) const
	{
		for (unsigned int i = 0; i < Children.size(); i++)
		{
			HitTestResult hit;
			if (Children[i]->HitTest(ray, &hit))
				return true;
		}
		return false;
	}

	~Scene()
	{
		for (auto s : Children)
			delete s;
	}
};

class RayTracer
{
protected:
	Scene* scene;
public:
	RayTracer(Scene* scene)
		: scene(scene)
	{
	}

	virtual void Render(void* buffer, int width, int height, int stride, int maxdepth);
};

void SetDefaultScene(Scene* scene);