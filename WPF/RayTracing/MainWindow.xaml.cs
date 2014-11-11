using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace RayTracing
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            CompositionTarget.Rendering += CompositionTarget_Rendering;
            bmp = new WriteableBitmap(800, 600, 96, 96, PixelFormats.Bgra32, null);
            tracer = new RayTracer();
            this.Background = new ImageBrush() { ImageSource = bmp };
        }

        WriteableBitmap bmp;
        RayTracer tracer;
        float height = 4;
        int maxdepth = 1;
        int n = 0;
        int start = 0;
        void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            n++;
            if (Environment.TickCount - start > 1000)
            {
                Title = "FPS:" + n + "  MaxDepth=" + maxdepth;
                n = 0;
                start = Environment.TickCount;
            }

            var pos = new Vector();
            pos.X = (float)(5 * Math.Cos(Environment.TickCount / 2000.0));
            pos.Y = height;
            pos.Z = (float)(5 * Math.Sin(Environment.TickCount / 2000.0));
            RayTracer.DefaultScene.Camera = new Camera(pos, new Vector());
            tracer.Render(bmp, RayTracer.DefaultScene, 5);
        }

        protected override void OnKeyUp(KeyEventArgs e)
        {
            base.OnKeyUp(e);
            switch (e.Key)
            {
                case Key.Up:
                    height += 0.1f;
                    break;
                case Key.Down:
                    height -= 0.1f;
                    break;
            }
        }
    }
}
