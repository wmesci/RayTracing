using System.Drawing;
using System.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows.Forms;

namespace RayTrack
{
    public abstract class Shape
    {
        public Material Surface;
        public abstract bool HitTest(ref Ray ray, ref HitTestResult result);
    }

    public class Sphere : Shape
    {
        public Vector Center;
        public double Radius;

        public override bool HitTest(ref Ray ray, ref HitTestResult result)
        {
            Vector eo = Center;
            eo.Sub(ref ray.Position);
            double v = Vector.Dot(ref eo, ref ray.Direction);
            var disc = Radius * Radius - Vector.Dot(ref eo, ref eo) + v * v;
            if (disc < 0) return false;
            disc = v - Math.Sqrt(disc);
            if (disc < 0.001f) return false;
            result.Shape = this;
            result.Position = ray.Position + ray.Direction * disc;
            result.Normal = result.Position - Center;
            result.Normal.Normalize();
            result.Distance = disc;
            return true;
        }
    }

    public class Plane : Shape
    {
        public Vector Normal;
        public double Offset;

        public override bool HitTest(ref Ray ray, ref HitTestResult result)
        {
            var denom = Normal * ray.Direction;
            if (denom > 0) return false;
            result.Shape = this;
            result.Distance = (Normal * ray.Position + Offset) / (-denom);
            result.Position = ray.Position + result.Distance * ray.Direction;
            result.Normal = Normal;
            return true;
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

        public override bool HitTest(ref Ray ray, ref HitTestResult result)
        {
            double eo;
            if (Normal.Length != 0 && (eo = ray.Direction * Normal) < 0)
            {
                var S = (Point1 - ray.Position) * Normal / eo;
                var V = S * ray.Direction + ray.Position;

                if (IsInner(V))
                {
                    result.Shape = this;
                    result.Normal = Normal;
                    result.Distance = S;
                    result.Position = ray.Position + ray.Direction * S;
                    return true;
                }
            }
            return false;
        }
    }

    public class Scene
    {
        public Shape[] Children;
        public Light[] Lights;
        public Camera Camera;

        public bool HitTest(ref Ray r, ref HitTestResult result)
        {
            bool hit = false;
            for (int i = 0; i < Children.Length; i++) 
            {
                HitTestResult hittest = new HitTestResult();
                hittest.Distance = double.MaxValue;
                if (Children[i].HitTest(ref r, ref hittest) && hittest.Distance < result.Distance)
                {
                    result = hittest;
                    hit = true;
                }
            }
            return hit;
        }

        public bool HitTest(ref Ray r, double len)
        {
            for (int i = 0; i < Children.Length; i++)
            {
                HitTestResult hittest = new HitTestResult();
                hittest.Distance = double.MaxValue;
                if (Children[i].HitTest(ref r, ref hittest) && hittest.Distance < len)
                    return true;
            }
            return false;
        }
    }
}