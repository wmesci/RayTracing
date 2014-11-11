using System.Linq;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows.Media.Imaging;

namespace RayTracing
{

    /// <summary>
    /// 用于给平面赋以黑白相间的颜色
    /// </summary>
    public static class Materials
    {
        //仅用于X-Z平面.
        public static readonly Material CheckerBoard =
            new Material()
            {
                Diffuse = pos =>
                {
                    return (((int)(Math.Floor(pos.X) + Math.Floor(pos.Z))) % 2) == 0 ? new Color(0.8f, 0.8f, 0.8f) : new Color(0.1f, 0.1f, 0.1f);
                },
                Specular = pos => new Color(1, 1, 1),
                Refraction = pos => 0,
                Power = 50
            };

        public static readonly Material Shiny1 =
            new Material()
            {
                Diffuse = pos => new Color(0.5f, 0.2f, 0.2f),
                Specular = pos => new Color(.5f, .5f, .5f),
                Refraction = pos => 0,
                Power = 50
            };

        public static readonly Material Shiny2 =
            new Material()
            {
                Diffuse = pos => new Color(0.2f, 0.5f, 0.2f),
                Specular = pos => new Color(0.5f, 0.5f, 0.5f),
                Refraction = pos => 0,
                Power = 25
            };

        public static readonly Material Shiny3 =
            new Material()
            {
                Diffuse = pos => new Color(0.2f, 0.2f, 0.5f),
                Specular = pos => new Color(0.3f, 0.3f, 0.3f),
                Refraction = pos => 0f,
                Power = 15
            };
    }

    public struct Vector
    {
        public float X;
        public float Y;
        public float Z;

        public Vector(float x, float y, float z)
        {
            this = new Vector();
            X = x; Y = y; Z = z;
        }

        public float Length
        {
            get
            {
                return (float)Math.Sqrt(LengthSquared);
            }
        }

        public float LengthSquared
        {
            get
            {
                return this.X * this.X + this.Y * this.Y + this.Z * this.Z;
            }
        }

        public void Normalize()
        {
            float l = this.Length;
            X /= l;
            Y /= l;
            Z /= l;
        }

        public void Add(ref Vector v)
        {
            X += v.X;
            Y += v.Y;
            Z += v.Z;
        }

        public void Sub(ref Vector v)
        {
            X -= v.X;
            Y -= v.Y;
            Z -= v.Z;
        }

        public void Multiply(float value)
        {
            X *= value;
            Y *= value;
            Z *= value;
        }

        public void Multiply(ref Vector v)
        {
            X *= v.X;
            Y *= v.Y;
            Z *= v.Z;
        }

        public void Multiply(Vector v)
        {
            X *= v.X;
            Y *= v.Y;
            Z *= v.Z;
        }

        public void Div(float value)
        {
            X /= value;
            Y /= value;
            Z /= value;
        }

        public static float Dot(ref Vector v1, ref Vector v2) 
        {
            return v1.X * v2.X + v1.Y * v2.Y + v1.Z * v2.Z;
        }

        public static Vector Corss(ref Vector v1, ref Vector v2)
        {
            return new Vector(((v1.Y * v2.Z) - (v1.Z * v2.Y)),
                     ((v1.Z * v2.X) - (v1.X * v2.Z)),
                     ((v1.X * v2.Y) - (v1.Y * v2.X)));
        }

        public static Vector operator +(Vector V1, Vector V2)
        {
            return new Vector(V1.X + V2.X, V1.Y + V2.Y, V1.Z + V2.Z);
        }

        public static Vector operator -(Vector V1, Vector V2)
        {
            return new Vector(V1.X - V2.X, V1.Y - V2.Y, V1.Z - V2.Z);
        }

        public static Vector operator -(Vector V)
        {
            return new Vector(-V.X, -V.Y, -V.Z);
        }

        public static Vector operator *(Vector v, float n)
        {
            return new Vector(v.X * n, v.Y * n, v.Z * n);
        }

        public static Vector operator *(float n, Vector v)
        {
            return new Vector(v.X * n, v.Y * n, v.Z * n);
        }

        public static float operator *(Vector V1, Vector V2)
        {
            return (V1.X * V2.X) + (V1.Y * V2.Y) + (V1.Z * V2.Z);
        }

        public static bool operator ==(Vector v1, Vector v2)
        {
            return (v1.X == v2.X) && (v1.Y == v2.Y) && (v1.Z == v2.Z);
        }

        public static bool operator !=(Vector v1, Vector v2)
        {
            return (v1.X != v2.X) || (v1.Y != v2.Y) || (v1.Z != v2.Z);
        }
    }

    public struct Color
    {
        public float R;
        public float G;
        public float B;

        public Color(float r, float g, float b) { R = r; G = g; B = b; }

        public void Add(ref Color color) 
        {
            R += color.R;
            G += color.G;
            B += color.B;
        }

        public void Sub(ref Color color)
        {
            R -= color.R;
            G -= color.G;
            B -= color.B;
        }

        public void Multiply(float value) 
        {
            R *= value;
            G *= value;
            B *= value;
        }

        public void Multiply(ref Color color)
        {
            R *= color.R;
            G *= color.G;
            B *= color.B;
        }

        public void Multiply(Color color)
        {
            R *= color.R;
            G *= color.G;
            B *= color.B;
        }

        public void Div(float value)
        {
            R /= value;
            G /= value;
            B /= value;
        }

        public static readonly Color Black = new Color(0, 0, 0);

        public int ToArgb()
        {
            var r = (int)(Math.Min(1, Math.Max(0, R)) * 255);
            var g = (int)(Math.Min(1, Math.Max(0, G)) * 255);
            var b = (int)(Math.Min(1, Math.Max(0, B)) * 255);
            return ((0xFF * 256 + r) * 256 + g) * 256 + b;
        }
    }

    public struct Ray
    {
        public Ray(Vector pos, Vector dir) 
        {
            Position = pos;
            Direction = dir;
        }
        public Vector Position;
        public Vector Direction;
    }

    /// <summary>
    /// 求交测试的结果
    /// </summary>
    public class HitTestResult
    {
        public Shape Shape;
        public Ray Ray;
        public float Distance;
    }

    public class Material
    {
        public Func<Vector, Color> Diffuse;   //漫反射颜色
        public Func<Vector, Color> Specular;  //镜面反射颜色
        public float Power;                   //镜面高光系数
        public Func<Vector, float> Refraction;//折射率
        public Color Transparency;            //透明度（透射光衰减系数）
    }

    public class Camera
    {
        public Vector Position{ get; set;}
        public Vector Direction { get; set; }
        public Vector Y { get; set; }
        public Vector X { get; set; }

        public Camera(Vector Position, Vector Taget)
        {
            Vector forward = Taget - Position;
            forward.Normalize();
            Vector down = new Vector(0, -1, 0);
            Vector right = Vector.Corss(ref forward, ref down);
            right.Normalize();
            right *= 2;
            Vector up = Vector.Corss(ref forward, ref right);
            up.Normalize();
            up *= 1.5f;
            
            this.Position = Position;
            this.Direction = forward;
            this.Y = up;
            this.X = right;
        }
    }

    public class Light
    {
        public Vector Pos;
        public Color Color;
    }

    public class RayTracer
    {
        public bool Abort { get; set; }

        private Color Trace(ref Ray ray, Scene scene, int depth)
        {
            HitTestResult hittest = scene.HitTest(ref ray);
            if (hittest == null) return Color.Black;

            var d = hittest.Ray.Direction;
            var pos = hittest.Distance * hittest.Ray.Direction + hittest.Ray.Position; //交点位置
            var normal = hittest.Shape.GetNormal(pos);                                 //交点法向量
            var cosa = normal * d;                                                     //-Cos(入射角)
            var reflectDir = d - 2 * cosa * normal;                            //反射方向

            Color color;
            if (cosa < 0)
                color = GetNaturalColor(hittest.Shape, ref pos, ref normal, ref reflectDir, scene);
            else
                color = Color.Black;
            if (depth <= 0) return color;
            depth--;

            //追踪反射光线
            var ray1 = new Ray(pos, reflectDir);
            var rcolor1 = Trace(ref ray1, scene, depth);
            rcolor1.Multiply(hittest.Shape.Surface.Specular(pos));

            //追踪折射光线
            var n = hittest.Shape.Surface.Refraction(pos);// 折射率
            if (n > 0)
            {
                if (cosa > 0) n = 1 / n;

                var f = Math.Acos(-cosa);                      //入射角
                var t = Math.Asin(Math.Sqrt(1 - cosa * cosa)); //折射角

                //fresnel公式，计算反射比
                double s1 = Math.Sin(f - t);
                double s2 = Math.Sin(f + t);
                double t1 = Math.Tan(f - t);
                double t2 = Math.Tan(f + t);
                var factor = (float)(((s1 * s1) / (s2 * s2) + (t1 * t1) / (t2 * t2)) / 2);

                if (factor < 1 && factor >= 0) //等于1时发生了全反射
                {
                    rcolor1.Multiply(factor);

                    //折射定律矢量形式：n2 * e2 - n1 * e1 = (n2 * cos(t) - n1 * cos(f)) * normal
                    var dir = ((float)(n * Math.Cos(t) - Math.Cos(f)) * normal) * (1 / n);
                    dir.Normalize();
                    var ray2 = new Ray(pos, dir);

                    var rcolor2 = Trace(ref ray2, scene, depth);
                    rcolor2.Multiply(1 - factor);
                    color.Add(ref rcolor2);
                }
            }

            color.Add(ref rcolor1);

            return color;
        }

        private Color GetNaturalColor(Shape thing, ref Vector pos, ref Vector norm, ref Vector rd, Scene scene)
        {
            rd.Normalize();
            Color ret = Color.Black;
            var ray = new Ray();
            ray.Position = pos;
            for (int i = 0; i < scene.Lights.Length; i++) 
            {
                Light light = scene.Lights[i];
                Vector ldis = light.Pos - pos;
                var len = ldis.Length;
                Vector livec = ldis * (1f / len);
                ray.Direction = livec;
                if (!scene.ShaowTest(ref ray, len))
                {
                    float illum = livec * norm;
                    if (illum > 0)
                    {
                        var lcolor = light.Color;
                        lcolor.Multiply(illum);
                        lcolor.Multiply(thing.Surface.Diffuse(pos));
                        ret.Add(ref lcolor);
                    }

                    float specular = livec * rd;
                    if (specular > 0)
                    {
                        var scolor = light.Color;
                        scolor.Multiply((float)Math.Pow(specular, thing.Surface.Power));
                        scolor.Multiply(thing.Surface.Specular(pos));
                        ret.Add(ref scolor);
                    }
                }
            }
            return ret;
        }

        public unsafe void Render(WriteableBitmap bmp, Scene scene, int maxdepth)
        {
            var width = bmp.PixelWidth;
            var height = bmp.PixelHeight;
            var camera = scene.Camera;
            var halfw = width / 2;
            var halfh = height / 2;
            var pdoubleh = 1 / (2f * height);
            var pdoublew = 1 / (2f * width);
            Random rnd = new Random();
            bmp.Lock();
            int line = bmp.BackBufferStride / 4;
            IntPtr p = bmp.BackBuffer;

            Action<int> proc = y => 
            {
                int* prt = (int*)p + y * line;
                var cury = (halfh - y) * pdoubleh;
                var dir = camera.Direction + cury * camera.Y;
                var ray = new Ray() { Position = camera.Position };
                for (int x = 0; x < width; x++, prt++)
                {
                    var curx = (x - halfw) * pdoublew;
                    ray.Direction = dir + curx * camera.X;
                    ray.Direction.Normalize();
                    *prt = Trace(ref ray, scene, maxdepth).ToArgb();
                }
            };
            Parallel.For(0, height, proc);
            bmp.AddDirtyRect(new System.Windows.Int32Rect(0, 0, width, height));
            bmp.Unlock();
        }

        /// <summary>
        /// 默认场景的内容
        /// </summary>
        public static readonly Scene DefaultScene =
            new Scene()
            {
                Children = new Shape[] { 
                                new Plane() {
                                    Norm = new Vector(0,1,0),
                                    Offset = 0,
                                    Surface = Materials.CheckerBoard
                                },
                                new Sphere() {
                                    Center = new Vector(0,1,0),
                                    Radius = 1,
                                    Surface = Materials.Shiny3
                                },
                                new Sphere() {
                                    Center = new Vector(-1f,0.5f,1.5f),
                                    Radius = 0.5f,
                                    Surface = Materials.Shiny1
                                },
                                new Sphere() {
                                    Center = new Vector(-1f,1.8f,1.5f),
                                    Radius = 0.5f,
                                    Surface = Materials.Shiny2
                                },
                            },
                Lights = new Light[] { 
                                new Light() {
                                    Pos = new Vector(-2f,2.5f,0f),
                                    Color = new Color(0.9f,0.4f,0.4f)
                                },
                                new Light() {
                                    Pos = new Vector(1.5f,2.5f,1.5f),
                                    Color = new Color(0.4f,0.9f,0.4f)
                                },
                                new Light() {
                                    Pos = new Vector(1.5f,2.5f,-1.5f),
                                    Color = new Color(0.4f,0.4f,0.9f)
                                },
                                },
                Camera = new Camera(new Vector(5, 4, 0), new Vector()),
                Ambient = new Color(0.5f, 0.5f, 0.5f)
            };
    }
}