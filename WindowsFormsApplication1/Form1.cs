using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        FlowLayoutPanel bottomPanel = new FlowLayoutPanel();
        Panel autoscrollPanel = new Panel();
        public Form1()
        {
            FlowLayoutPanel mainPanel = new FlowLayoutPanel();

            mainPanel.Dock = DockStyle.Fill;
            mainPanel.AutoSize = true;
            mainPanel.AutoScroll = true;
            mainPanel.FlowDirection = FlowDirection.LeftToRight;

            bottomPanel.AutoSize = true;
            bottomPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            bottomPanel.FlowDirection = FlowDirection.TopDown;
            bottomPanel.Dock = DockStyle.Right;

            autoscrollPanel.AutoScroll = true;
            autoscrollPanel.Dock = DockStyle.Left;

            mainPanel.Controls.Add(autoscrollPanel);
            mainPanel.Controls.Add(bottomPanel);
            this.Controls.Add(mainPanel);
            this.WindowState = System.Windows.Forms.FormWindowState.Maximized;
            InitializeComponent();
        }

        private FlowLayoutPanel recreateFlowPanel()
        {
            autoscrollPanel.Controls.Clear();
            var flowPanel = new FlowLayoutPanel();
            autoscrollPanel.Controls.Add(flowPanel);
            flowPanel.AutoSize = true;
            flowPanel.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            flowPanel.FlowDirection = FlowDirection.TopDown;
            return flowPanel;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
        }

        private void clickedChooseStackFiles(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();

            DialogResult result = fbd.ShowDialog();

            var imagePanel = recreateFlowPanel();
            if (string.IsNullOrWhiteSpace(fbd.SelectedPath)) return;
            string[] files = Directory.GetFiles(fbd.SelectedPath);

            var images = new List<FocusStackExample.ByteImage>();
            Func<PictureBox, PictureBox> setSmallPicBox = pb => {
                    pb.SizeMode = PictureBoxSizeMode.StretchImage;
                pb.Size = new Size(150, 150);
                return pb; };
            foreach (var f in files)
            {
                try
                {
                    var image = Image.FromFile(f);
                    images.Add(new FocusStackExample.ByteImage(image));
                    addPictureBox(image, imagePanel, setSmallPicBox);

                } catch
                {
                    continue;
                }
            }

            bottomPanel.Controls.Clear();
            Panel p1 = new Panel();
            p1.AutoScroll = true;
            p1.Size = new Size(550, 550);
            Panel p2 = new Panel();
            p2.AutoScroll = true;
            p2.Size = new Size(550, 550);
            bottomPanel.Controls.Add(p1);
            bottomPanel.Controls.Add(p2);
            Func<PictureBox, PictureBox> bigImage = pb => {
                pb.SizeMode = PictureBoxSizeMode.AutoSize;
                return pb; };
            FocusStackExample.FocusStacking impl = new FocusStackExample.FocusStacking(images);
            addPictureBox(impl.createCombinedImage(), p1, bigImage);
            addPictureBox(impl.createDepthMap(), p2, bigImage);
        }

        private void addPictureBox(Image img, Panel flowPanel, Func<PictureBox, PictureBox> setter)
        {
            var pb = new PictureBox();
            pb.Image = img;
            pb = setter(pb);
            flowPanel.Controls.Add(pb);
        }
    }
}
