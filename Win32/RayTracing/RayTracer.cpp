#include "stdafx.h"
#include "RayTracer.h"

#include <ppl.h>
using namespace concurrency;

Color Shade(const Shape* thing, const Vector& pos, const Vector& norm, const Vector& reflect, const Scene& scene)
{
	Color ret;
	Ray ray;
	ray.Position = pos;
	for (unsigned int i = 0; i < scene.Lights.size(); i++)
	{
		const Light& light = scene.Lights[i];
		Vector ldis = light.Position - pos;
		auto len = ldis.Length();
		Vector livec = ldis / len;
		ray.Direction = livec;
		ray.MaxDistance = len;
		if (!scene.HitTest(ray))
		{
			auto illum = livec * norm;
			if (illum > 0)
			{
				auto lcolor = light.Color;
				lcolor *= illum;
				lcolor *= thing->Material.Diffuse(pos);
				ret += lcolor;
			}

			auto specular = livec * reflect;
			if (specular > 0)
			{
				auto scolor = light.Color;
				scolor *= pow(specular, thing->Material.Power(pos));
				scolor *= thing->Material.Specular(pos);
				ret += scolor;
			}
		}
	}
	return ret;
}

__forceinline float Fresnel(const Vector& wo, Vector& normal, float n, Vector* reflect, Vector* refract)
{
	float factor = 1;                        //菲涅尔系数
	auto cosa = wo * normal;
	if (cosa < 0)
	{
		cosa = -cosa;
		normal = -normal;
		if (n != 0)
			n = 1 / n;
	}

	*reflect =2 * cosa * normal - wo;//反射向量

	if (n != 0)
	{
		auto cosi = cosa;                           //cos(反射角/入射角)
		auto cost = 1 - (1 - cosi * cosi) / (n * n);//cos(折射角)
		if (cost > 0) //非全反射
		{
			cost = sqrt(cost);
			*refract = ((cosi - cost * n) * normal - wo).Normalize();

			auto fs = (cosi - n * cost) / (cosi + n * cost);
			fs *= fs;

			auto fp = (cost - n * cosi) / (cost + n * cosi);
			fp *= fp;

			factor = clamp((fs + fp) / 2, 0, 1);
		}

	}

	return factor;
}

Color Trace(const Ray& ray, const Scene& scene, int depth)
{
	HitTestResult hittest;
	if (!scene.HitTest(ray, &hittest)) return scene.Ambient;

	auto d = ray.Direction;
	auto pos = hittest.Distance * ray.Direction + ray.Position; //交点位置
	auto normal = hittest.Normal;                               //交点法向量
	auto cosa = normal * d;                                     //-Cos(入射角)

	Vector reflect, refract;
	auto f = Fresnel(-d, normal, hittest.Shape->Material.Refraction(pos), &reflect, &refract);

	Color result;
	if (cosa < 0)
		result = Shade(hittest.Shape, pos, normal, reflect, scene);
	if (depth <= 0) return result;
	depth--;

	//追踪反射光线
	if (f > 0)
		result += Trace({ pos, reflect, INFINITY}, scene, depth) * f * hittest.Shape->Material.Specular(pos);

	//追踪折射光线
	if (f < 1)
		result += Trace({ pos, refract, INFINITY }, scene, depth) * (1 - f) * hittest.Shape->Material.Transparency(pos);

	result += hittest.Shape->Material.Diffuse(pos) * scene.Ambient;

	return result;
}

void RayTracer::Render(void* buffer, int width, int height, int stride, int maxdepth)
{
	auto camera = scene->Camera;
	auto halfw = width / 2.0f;
	auto halfh = height / 2.0f;
	auto pfloath = 1 / (2.0f * height);
	auto pfloatw = 1 / (2.0f * width);

	auto proc = [&](int y)
	{
		auto prt = (unsigned int*)((char*)buffer + y * stride);
		auto cury = (halfh - y) * pfloath;
		auto dir = camera.Direction + cury * camera.Y;
		Ray ray = { camera.Position, Vector(), INFINITY };
		for (int x = 0; x < width; x++, prt++)
		{
			auto curx = (x - halfw) * pfloatw;
			ray.Direction = (dir + curx * camera.X).Normalize();
			ray.MaxDistance = INFINITY;
			*prt = Trace(ray, *scene, maxdepth).ToArgb();
		}
	};
	parallel_for(0, height, proc);
}

void SetDefaultScene(Scene* scene)
{
	Plane* p = new Plane();
	p->Normal = Vector(0, 1, 0);
	p->Offset = 0;
	p->Material.Diffuse = [](const Vector& pos)
	{ 
		if ((((int)pos.X + (int)pos.Z) & 1) == 0)
			return 0.8f;
		else
			return 0.1f;
	};
	p->Material.Specular = [](const Vector& pos) { return Color::White; };
	p->Material.Refraction = [](const Vector& pos) { return 0; };
	p->Material.Power = [](const Vector& pos) { return 50; };;
	scene->Children.push_back(p);

	Sphere* s1 = new Sphere();
	s1->Center = Vector(-1, 0.5f, 1.5f);
	s1->Radius = 0.5;
	s1->Material.Diffuse = [](const Vector& pos) { return Color(0.1f, 0.4f, 0.8f); };
	s1->Material.Specular = [](const Vector& pos) { return  0.05f; };
	s1->Material.Refraction = [](const Vector& pos) { return 0; };
	s1->Material.Power = [](const Vector& pos) { return 50; };
	scene->Children.push_back(s1);

	Sphere* s2 = new Sphere();
	s2->Center = Vector(-1, 1.8f, 1.5f);
	s2->Radius = 0.5;
	s2->Material.Diffuse = [](const Vector& pos) { return 0.0f; };
	s2->Material.Specular = [](const Vector& pos) { return 0.9f; };
	s2->Material.Refraction = [](const Vector& pos) { return 1.5f; };
	s2->Material.Transparency = [](const Vector& pos) { return  0.9f; };
	s2->Material.Power = [](const Vector& pos) { return 25; };
	scene->Children.push_back(s2);

	Sphere* s3 = new Sphere();
	s3->Center = Vector(0, 1, 0);
	s3->Radius = 1;
	s3->Material.Diffuse = [](const Vector& pos) { return 0.1f; };
	s3->Material.Specular = [](const Vector& pos) { return  0.8f; };
	s3->Material.Refraction = [](const Vector& pos) { return 0; };
	s3->Material.Power = [](const Vector& pos) { return 100; };
	scene->Children.push_back(s3);

	scene->Lights.push_back({ Vector(-2, 2.5f, 0), Color(0.9f, 0.4f, 0.4f) });
	scene->Lights.push_back({ Vector(1.5f, 2.5f, 1.5f), Color(0.4f, 0.9f, 0.4f) });
	scene->Lights.push_back({ Vector(1.5f, 2.5f, -1.5f), Color(0.4f, 0.4f, 0.9f) });

	scene->Camera = { Vector(6, 3, 0), Vector() };
	scene->Ambient = Color::Black;
}