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
            bottomPanel.Dock = DockStyle.Right;

            autoscrollPanel.AutoScroll = true;
            autoscrollPanel.Dock = DockStyle.Left;

            mainPanel.Controls.Add(autoscrollPanel);
            mainPanel.Controls.Add(bottomPanel);
            this.Controls.Add(mainPanel);

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

            var images = new List<Image>();
            Func<PictureBox, PictureBox> setSmallPicBox = pb => {
                    pb.SizeMode = PictureBoxSizeMode.StretchImage;
                pb.Size = new Size(100, 100);
                return pb; };
            foreach (var f in files)
            {
                try
                {
                    var image = Image.FromFile(f);
                    images.Add(image);
                    addPictureBox(image, imagePanel, setSmallPicBox);

                } catch
                {
                    continue;
                }
            }

            bottomPanel.Controls.Clear();
            Func<PictureBox, PictureBox> left = pb => {
                pb.SizeMode = PictureBoxSizeMode.StretchImage;
                pb.Size = new Size(500, 500);
                pb.Anchor = AnchorStyles.Right;
                return pb; };
            Func<PictureBox, PictureBox> right = pb => {
                pb.SizeMode = PictureBoxSizeMode.StretchImage;
                pb.Size = new Size(500, 500);
                pb.Anchor = AnchorStyles.Left;
                return pb;
            };
            addPictureBox(createCombinedImage(images), bottomPanel, left);
            addPictureBox(createDepthMap(images), bottomPanel, right);
        }

        private Image createDepthMap(List<Image> images)
        {
            return images.First();
        }

        private Image createCombinedImage(List<Image> images)
        {
            return images.First();
        }

        private void addPictureBox(Image img, FlowLayoutPanel flowPanel, Func<PictureBox, PictureBox> setter)
        {
            var pb = new PictureBox();
            pb.Image = img;
            pb = setter(pb);
            flowPanel.Controls.Add(pb);
        }
    }
}
