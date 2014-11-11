using System.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace RayTracing
{
    public abstract class Shape
    {
        public string Name { get; set; }
        public Material Surface;
        public abstract HitTestResult HitTest(ref Ray ray);
        public abstract Vector GetNormal(Vector pos);
        public virtual double Reflectivity(Vector pos) { return 1; }
    }

    public class Sphere : Shape
    {
        /// <summary>
        /// 球心
        /// </summary>
        public Vector Center { get; set; }
        /// <summary>
        /// 半径
        /// </summary>
        public float Radius { get; set; }

        public override HitTestResult HitTest(ref Ray ray)
        {
            Vector eo = Center;
            eo.Sub(ref ray.Position);
            float v = Vector.Dot(ref eo, ref ray.Direction);
            var disc = Radius * Radius - Vector.Dot(ref eo, ref eo) + v * v;
            if (disc < 0) return null;
            disc = v - (float)Math.Sqrt(disc);
            if (disc < 0.001f) return null;
            return new HitTestResult()
            {
                Shape = this,
                Ray = ray,
                Distance = disc
            };
        }

        public override Vector GetNormal(Vector pos)
        {
            pos -= Center;
            pos.Normalize();
            return pos;
        }
    }

    public class Plane : Shape
    {
        public Vector Norm;
        public float Offset;

        public override HitTestResult HitTest(ref Ray ray)
        {
            float denom = Norm * ray.Direction;
            if (denom > 0) return null;
            return new HitTestResult()
            {
                Shape = this,
                Ray = ray,
                Distance = (Norm * ray.Position + Offset) / (-denom)
            };
        }

        public override Vector GetNormal(Vector pos)
        {
            return Norm;
        }
    }

    public class Triangle : Shape
    {
        public Vector Point1;
        public Vector Point2;
        public Vector Point3;
        private Vector Normal;

        public Triangle(Vector Point1, Vector Point2, Vector Point3)
        {
            this.Point1 = Point1;
            this.Point2 = Point2;
            this.Point3 = Point3;
            var n1 = Point2 - Point1;
            var n2 = Point3 - Point1;
            Normal = Vector.Corss(ref n1, ref n2);
            Normal.Normalize();
        }

        private bool IsInner(Vector V)
        {
            var V1 = Point1 - V;
            V1.Normalize();
            var V2 = Point2 - V;
            V2.Normalize();
            var V3 = Point3 - V;
            V3.Normalize();
            var a1 = Math.Acos(V1 * V2);
            var a2 = Math.Acos(V2 * V3);
            var a3 = Math.Acos(V3 * V1);
            var a = a1 + a2 + a3 - 2 * Math.PI;
            if (a <= 0.0001 && a >= -0.0001) return true;
            return false;
        }

        public override HitTestResult HitTest(ref Ray ray)
        {
            float eo;
            if (Normal.Length != 0 && (eo = ray.Direction * Normal) < 0)
            {
                var S = (Point1 - ray.Position) * Normal / eo;
                var V = S * ray.Direction + ray.Position;

                if (IsInner(V))
                {
                    return new HitTestResult()
                    {
                        Shape = this,
                        Ray = ray,
                        Distance = S
                    };
                }
                return null;
            }
            return null;
        }

        public override Vector GetNormal(Vector pos)
        {
            return Normal;
        }
    }

    public class Scene
    {
        public Shape[] Children;
        public Light[] Lights;
        public Camera Camera;
        public Color Ambient;

        public HitTestResult HitTest(ref Ray r)
        {
            HitTestResult result = null;
            for (int i = 0; i < Children.Length; i++) 
            {
                var hit = Children[i].HitTest(ref r);
                if (result == null || (hit != null && hit.Distance < result.Distance))
                    result = hit;
            }
            return result;
        }

        public bool ShaowTest(ref Ray r, float len)
        {
            for (int i = 0; i < Children.Length; i++)
            {
                var hit = Children[i].HitTest(ref r);
                if (hit != null && hit.Distance < len)
                    return true;
            }
            return false;
        }
    }
}