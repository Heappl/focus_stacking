using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace FocusStackExample
{
    class Filter
    {
        public int size;
        public int[] values;
        public int div;
        public Filter(int size, int[] values)
        {
            this.size = size;
            this.values = values;
            this.div = 0;
            foreach (var x in values)
                this.div += x;
            foreach (var x in values)
                if (x < 0)
                    this.div = 1;
        }

        public int getValue(int x, int y)
        {
            return values[y * size + x];
        }
    }

    class ByteImage
    {
        public int width;
        public int height;
        public int[] rgbs;

        int[] getRGB(Bitmap bmp)
        {
            var data = bmp.LockBits(new Rectangle(0, 0, bmp.Width, bmp.Height),
                System.Drawing.Imaging.ImageLockMode.ReadOnly,
                System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            try
            {
                var ptr = (IntPtr)((long)data.Scan0);
                var ret = new int[bmp.Width * bmp.Height];
                System.Runtime.InteropServices.Marshal.Copy(ptr, ret, 0, ret.Length);
                return ret;
            }
            finally
            {
                bmp.UnlockBits(data);
            }
        }

        Bitmap fromRgb(int[] rgbs, int width, int height)
        {
            var ret = new Bitmap(width, height);

            var data = ret.LockBits(new Rectangle(0, 0, width, height),
                System.Drawing.Imaging.ImageLockMode.WriteOnly,
                System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            try
            {
                var ptr = (IntPtr)((long)data.Scan0);
                System.Runtime.InteropServices.Marshal.Copy(rgbs, 0, ptr, rgbs.Length);
                return ret;
            }
            finally
            {
                ret.UnlockBits(data);
            }
        }

        public ByteImage(Image img)
        {
            this.rgbs = getRGB(new Bitmap(img));
            this.width = img.Size.Width;
            this.height = img.Size.Height;
        }
        public ByteImage(int width, int height)
        {
            this.rgbs = new int[width * height];
            this.width = width;
            this.height = height;
        }

        public Color getColor(int x, int y)
        {
            return Color.FromArgb(rgbs[y * width + x]);
        }

        public void setRgb(int x, int y, int red, int green, int blue)
        {
            rgbs[y * width + x] = Color.FromArgb(red, green, blue).ToArgb();
        }

        public Image toImage()
        {
            return fromRgb(rgbs, width, height);
        }
    }

    class FocusStacking
    {
        [DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr createFocusStack(int width, int height);

        [DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int addImage(IntPtr ctx, IntPtr img);

        [DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int createDepthOfField(IntPtr ctx, IntPtr dest);

        [DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int createInFocusImg(IntPtr ctx, IntPtr dest);

        [DllImport("FocusStackImpl.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int releaseFocusStack(IntPtr ctx);


        Filter gaussianFilter = new Filter(5, new int[]{1, 2, 4, 2, 1,
                                                        2, 4, 8, 4, 2,
                                                        4, 8, 16, 8, 4,
                                                        2, 4, 8, 4, 2,
                                                        1, 2, 4, 2, 1});
        Filter laplacianFilter = new Filter(3, new int[] { -1, -1, -1, -1, 8, -1, -1, -1, -1 });
        IntPtr impl = IntPtr.Zero;
        List<ByteImage> images = null;

        public FocusStacking(List<ByteImage> images)
        {
            this.impl = createFocusStack(images.First().width, images.First().height);
            this.images = images;
            foreach (var img in images)
            {
                var handle = GCHandle.Alloc(img.rgbs, GCHandleType.Pinned);
                try
                {
                    addImage(impl, handle.AddrOfPinnedObject());
                }
                finally
                {
                    if (handle.IsAllocated)
                    {
                        handle.Free();
                    }
                }
            }
        }

        ~FocusStacking()
        {
            releaseFocusStack(this.impl);
        }

        public Image createDepthMap()
        {
            var ret = new ByteImage(images.First().width, images.First().height);
            var handle = GCHandle.Alloc(ret.rgbs, GCHandleType.Pinned);
            try
            {
                IntPtr addr = handle.AddrOfPinnedObject();
                createDepthOfField(impl, addr);
                System.Runtime.InteropServices.Marshal.Copy(addr, ret.rgbs, 0, ret.rgbs.Length);
            }
            finally
            {
                if (handle.IsAllocated)
                {
                    handle.Free();
                }
            }
            return ret.toImage();
        }

        public Image createCombinedImage()
        {
            var ret = new ByteImage(images.First().width, images.First().height);
            var handle = GCHandle.Alloc(ret.rgbs, GCHandleType.Pinned);
            try
            {
                IntPtr addr = handle.AddrOfPinnedObject();
                createInFocusImg(impl, addr);
                System.Runtime.InteropServices.Marshal.Copy(addr, ret.rgbs, 0, ret.rgbs.Length);
            }
            finally
            {
                if (handle.IsAllocated)
                {
                    handle.Free();
                }
            }
            return ret.toImage();
        }
    }
}
