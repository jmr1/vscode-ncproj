using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using parser;
using System.Threading;
using System.Globalization;

namespace cmtconfig
{
    public partial class NCSettingsForm : Form
    {
        private int mouseX = 0;
        private int mouseY = 0;
        private int mouseXclick;
        private int mouseYclick;
        private bool mouseDown;

        private ObrabiarkaCNC obrabiarkaCNC1;
        private Parametry parametry1;
        private Kinematyka kinematyka1;
        private Autor autor1;
        private ObszarRoboczy obszarRoboczy1;

        private readonly NCSettingsFile ncSettingsFile;

        public NCSettingsForm(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile, Language language)
        {
            switch (language)
            {
                case Language.English:
                    SetCulture("en-US");
                    break;
                case Language.Polish:
                    SetCulture("pl-PL");
                    break;
                default:
                    SetCulture("en-US");
                    break;
            }

            this.ncSettingsFile = ncSettingsFile;

            obrabiarkaCNC1 = new ObrabiarkaCNC(ncSettingsDefaults, ref ncSettingsFile, language);
            kinematyka1 = new Kinematyka(ncSettingsDefaults, ref ncSettingsFile);
            parametry1 = new Parametry(ncSettingsDefaults, ref ncSettingsFile);
            obszarRoboczy1 = new ObszarRoboczy(ncSettingsDefaults, ref ncSettingsFile);
            autor1 = new Autor(ncSettingsDefaults, ref ncSettingsFile);
            
            parametry1.Parent = this;

            InitializeComponent();
            Icon = Properties.Resources.nceditor;
            ShowInTaskbar = false;

            Controls.Add(obrabiarkaCNC1);
            Controls.Add(kinematyka1);
            Controls.Add(parametry1);
            Controls.Add(obszarRoboczy1);
            Controls.Add(autor1);

            SidePanel.Height = buttonObrabiarkaCNC.Height;
            SidePanel.Top = buttonObrabiarkaCNC.Top;
            obrabiarkaCNC1.BringToFront();
        }

        private IEnumerable<Control> GetChildren(Control control)
        {
            var controls = control.Controls.Cast<Control>();
            return controls.SelectMany(ctrl => GetChildren(ctrl)).Concat(controls);
        }

        private void SetCulture(string cultureName)
        {
            Thread.CurrentThread.CurrentCulture = Thread.CurrentThread.CurrentUICulture = CultureInfo.GetCultureInfo(cultureName);
            var resources = new ComponentResourceManager(GetType());
            GetChildren(this).ToList().ForEach(c => {
                resources.ApplyResources(c, c.Name);
            });
        }

        private void NCSettingsForm_Load(object sender, EventArgs e)
        {

        }

        public string GetActiveDriver
        {
            get { return obrabiarkaCNC1.GetActiveDriver; }
        }

        public string GetActiveMachineTool
        {
            get { return obrabiarkaCNC1.GetActiveMachineTool; }
        }

        public Kinematyka Kinematyka1
        {
            get { return kinematyka1; }
        }

        public ObrabiarkaCNC ObrabiarkaCNC1
        {
            get { return obrabiarkaCNC1; }
        }

        public ObszarRoboczy ObszarRoboczy1
        {
            get { return obszarRoboczy1; }
        }

        public Parametry Parametry1
        {
            get { return parametry1; }
        }

        public Autor Autor1
        {
            get { return autor1; }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            ActiveForm.Close();
        }

        private void buttonObrabiarkaCNC_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonObrabiarkaCNC.Height;
            SidePanel.Top = buttonObrabiarkaCNC.Top;
            obrabiarkaCNC1.BringToFront();
        }

        private void buttonKinematyka_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonKinematyka.Height;
            SidePanel.Top = buttonKinematyka.Top;
            kinematyka1.BringToFront();
        }

        private void panel2_MouseDown(object sender, MouseEventArgs e)
        {
            mouseDown = true;
            mouseXclick = e.X;
            mouseYclick = e.Y;
        }

        private void panel2_MouseUp(object sender, MouseEventArgs e)
        {
            mouseDown = false;
        }

        private void panel2_MouseMove(object sender, MouseEventArgs e)
        {
            if (mouseDown)
            {
                mouseX = this.Left + (e.X - mouseXclick);
                mouseY = this.Top + (e.Y - mouseYclick);

                this.SetDesktopLocation(mouseX, mouseY);
            }
        }

        private void buttonParametry_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonParametry.Height;
            SidePanel.Top = buttonParametry.Top;
            parametry1.BringToFront();
        }

        private void buttonObszarRoboczy_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonObszarRoboczy.Height;
            SidePanel.Top = buttonObszarRoboczy.Top;
            obszarRoboczy1.BringToFront();
        }

        private void buttonProgramista_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonProgramista.Height;
            SidePanel.Top = buttonProgramista.Top;
            autor1.BringToFront();
        }

        private void buttonDefault_Click(object sender, EventArgs e)
        {
            obrabiarkaCNC1.MakeDefault();
            kinematyka1.MakeDefault();
            parametry1.MakeDefault();
            obszarRoboczy1.MakeDefault();
            autor1.MakeDefault();
        }

        private void buttonOK_Click(object sender, EventArgs e)
        {
            ActiveForm.Close();
        }

        private void buttonImport_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (openFileDialog1.FileName != "")
                {
                    var ncSettingsFileLocal = NCSettingsFile.Read(openFileDialog1.FileName);
                    obrabiarkaCNC1.SetNCSettingsFile(ncSettingsFileLocal);
                    kinematyka1.SetNCSettingsFile(ncSettingsFileLocal);
                    parametry1.SetNCSettingsFile(ncSettingsFileLocal);
                    obszarRoboczy1.SetNCSettingsFile(ncSettingsFileLocal);
                    autor1.SetNCSettingsFile(ncSettingsFileLocal);
                }
            }
        }

        private void buttonExport_Click(object sender, EventArgs e)
        {
            if (saveFileDialog1.ShowDialog() == DialogResult.OK)
            {
                if (saveFileDialog1.FileName != "")
                {
                    NCSettingsFile ncSettingsFileLocal = ObjectCopy.Clone(ncSettingsFile);

                    obrabiarkaCNC1.ReadData(ref ncSettingsFileLocal);
                    kinematyka1.ReadData(ref ncSettingsFileLocal);
                    parametry1.ReadData(ref ncSettingsFileLocal);
                    obszarRoboczy1.ReadData(ref ncSettingsFileLocal);
                    autor1.ReadData(ref ncSettingsFileLocal);

                    ncSettingsFileLocal.Write(saveFileDialog1.FileName);
                }
            }
        }
    }
}
