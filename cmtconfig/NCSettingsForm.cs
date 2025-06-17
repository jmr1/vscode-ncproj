using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Windows.Forms;
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

        private CNCMachineTools machineTools1;
        private CNCInitialParams initialParams1;
        private CNCKinematics kinematics1;
        private CNCProgramZero programZero1;
        private CNCMachinePoints machinePoints1;

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

            machineTools1 = new CNCMachineTools(ncSettingsDefaults, ref ncSettingsFile, language);
            kinematics1 = new CNCKinematics(ncSettingsDefaults, ref ncSettingsFile);
            initialParams1 = new CNCInitialParams(ncSettingsDefaults, ref ncSettingsFile);
            machinePoints1 = new CNCMachinePoints(ncSettingsDefaults, ref ncSettingsFile);
            programZero1 = new CNCProgramZero(ncSettingsDefaults, ref ncSettingsFile);
            
            initialParams1.Parent = this;

            InitializeComponent();
            Icon = Properties.Resources.nceditor;
            ShowInTaskbar = false;

            Controls.Add(machineTools1);
            Controls.Add(kinematics1);
            Controls.Add(initialParams1);
            Controls.Add(machinePoints1);
            Controls.Add(programZero1);

            SidePanel.Height = buttonMachineTools.Height;
            SidePanel.Top = buttonMachineTools.Top;
            machineTools1.BringToFront();
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
            get { return machineTools1.GetActiveDriver; }
        }

        public string GetActiveMachineTool
        {
            get { return machineTools1.GetActiveMachineTool; }
        }

        public CNCKinematics Kinematics1
        {
            get { return kinematics1; }
        }

        public CNCMachineTools MachineTools1
        {
            get { return machineTools1; }
        }

        public CNCMachinePoints MachinePoints1
        {
            get { return machinePoints1; }
        }

        public CNCInitialParams InitialParams1
        {
            get { return initialParams1; }
        }

        public CNCProgramZero ProgramZero1
        {
            get { return programZero1; }
        }

        private void button7_Click(object sender, EventArgs e)
        {
            ActiveForm.Close();
        }

        private void buttonMachineTools_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonMachineTools.Height;
            SidePanel.Top = buttonMachineTools.Top;
            machineTools1.BringToFront();
        }

        private void buttonKinematics_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonKinematics.Height;
            SidePanel.Top = buttonKinematics.Top;
            kinematics1.BringToFront();
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

        private void buttonInitialParams_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonInitialParams.Height;
            SidePanel.Top = buttonInitialParams.Top;
            initialParams1.BringToFront();
        }

        private void buttonMachinePoints_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonMachinePoints.Height;
            SidePanel.Top = buttonMachinePoints.Top;
            machinePoints1.BringToFront();
        }

        private void buttonProgramZero_Click(object sender, EventArgs e)
        {
            SidePanel.Height = buttonProgramZero.Height;
            SidePanel.Top = buttonProgramZero.Top;
            programZero1.BringToFront();
        }

        private void buttonDefault_Click(object sender, EventArgs e)
        {
            machineTools1.MakeDefault();
            kinematics1.MakeDefault();
            initialParams1.MakeDefault();
            machinePoints1.MakeDefault();
            programZero1.MakeDefault();
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
                    machineTools1.SetNCSettingsFile(ncSettingsFileLocal);
                    kinematics1.SetNCSettingsFile(ncSettingsFileLocal);
                    initialParams1.SetNCSettingsFile(ncSettingsFileLocal);
                    machinePoints1.SetNCSettingsFile(ncSettingsFileLocal);
                    programZero1.SetNCSettingsFile(ncSettingsFileLocal);
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

                    machineTools1.ReadData(ref ncSettingsFileLocal);
                    kinematics1.ReadData(ref ncSettingsFileLocal);
                    initialParams1.ReadData(ref ncSettingsFileLocal);
                    machinePoints1.ReadData(ref ncSettingsFileLocal);
                    programZero1.ReadData(ref ncSettingsFileLocal);

                    ncSettingsFileLocal.Write(saveFileDialog1.FileName);
                }
            }
        }
    }
}
