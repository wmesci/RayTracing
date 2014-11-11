using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace RayTrack
{
    public partial class MainForm : Form
    {
        [DllImport("user32.dll", EntryPoint = "GetDCEx")]
        static extern IntPtr GetDC(IntPtr hwnd);

        [DllImport("gdi32.dll")]
        static extern IntPtr SelectObject(IntPtr hdc, IntPtr hObject);

        [DllImport("gdi32.dll")]
        static extern IntPtr DeleteObject(IntPtr hObject);

        [DllImport("gdi32.dll")]
        static extern int BitBlt(IntPtr hDestDC, int x, int y, int nWidth, int nHeight, IntPtr hSrcDC, int xSrc, int ySrc, int dwRop);


        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.Run(new MainForm());
        }

        private RayTracer RayTracer;
        private Bitmap bitmap;
        public MainForm()
        {
            InitializeComponent();
            this.SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.OptimizedDoubleBuffer, true);
            RayTracer = new RayTracer();
            bitmap = new Bitmap(this.ClientSize.Width, this.ClientSize.Height);
        }

        double height = 4;
        int maxdepth = 1;
        int n = 0;
        int start = 0;
        protected override void OnPaint(PaintEventArgs e)
        {
            n++;
            if (Environment.TickCount - start > 1000) 
            {
                Text = "FPS:" + n + "  MaxDepth=" + maxdepth;
                n = 0;
                start = Environment.TickCount;
            }

            var pos = new Vector();
            pos.X = (5 * Math.Cos(Environment.TickCount / 2000.0));
            pos.Y = height;
            pos.Z = (5 * Math.Sin(Environment.TickCount / 2000.0));
            RayTracer.DefaultScene.Camera = new Camera(pos, new Vector());

            RayTracer.Render(bitmap, RayTracer.DefaultScene, maxdepth);
            var hBitmap = bitmap.GetHbitmap(); 
            var det = e.Graphics.GetHdc();
            using (var g = Graphics.FromImage(bitmap))
            {
                var src = g.GetHdc();
                var hOldObject = SelectObject(src, hBitmap);
                BitBlt(det, 0, 0, bitmap.Width, bitmap.Height, src, 0, 0, 0x00CC0020);
                SelectObject(src, hOldObject); 
                g.ReleaseHdc(det);
            }
            e.Graphics.ReleaseHdc(det);
            DeleteObject(hBitmap);
            this.Invalidate();
        }

        protected override void OnKeyPress(KeyPressEventArgs e)
        {
            base.OnKeyPress(e);
            if (char.IsDigit(e.KeyChar))
            {
                maxdepth = int.Parse(e.KeyChar.ToString());
            }
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            base.OnKeyUp(e);
            switch (e.KeyCode) 
            {
                case Keys.Up:
                    height += 0.1f;
                    break;
                case Keys.Down:
                    height -= 0.1f;
                    break;
            }
        }
    }
}
