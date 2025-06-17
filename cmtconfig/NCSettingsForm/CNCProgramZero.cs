using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace cmtconfig
{
    using ProgramZeroData = Dictionary<string, double>;
    public partial class CNCProgramZero : UserControl
    {
        private readonly NCSettingsName ncSettingsName;
        private readonly NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;

        private string driverName = "";
        private string machineToolName = "";

        private readonly ProgramZeroData defaultProgramZeroData = new ProgramZeroData { {"X", 0.0 }, { "Y", 0.0 }, { "Z", 0.0 } };

        public CNCProgramZero(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile)
        {
            InitializeComponent();

            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
        }
        
        void ProgramZeroLoad(object sender, EventArgs e)
        {
            Reload(ncSettingsFile);
        }

        public void SetNCSettingsFile(NCSettingsFile ncSettingsFileLocal)
        {
            Reload(ncSettingsFileLocal);
        }

        public void Reload(NCSettingsFile ncSettingsFileLocal)
        {
            HasActiveMachineToolChanged();
            SetCurrentOrDefaults(ncSettingsFileLocal);
        }

        private bool HasActiveMachineToolChanged()
        {
            var ncSettingsForm = ParentForm as NCSettingsForm;
            var activeMachineTool = ncSettingsForm.GetActiveMachineTool;
            var activeDriver = ncSettingsForm.GetActiveDriver;

            if (driverName == activeDriver && machineToolName == activeMachineTool)
                return false;

            machineToolName = activeMachineTool;
            driverName = activeDriver;

            return true;
        }

        private void SetCurrentOrDefaults(NCSettingsFile ncSettingsFileLocal)
        {
            if(ncSettingsFileLocal.driver.ToString() == driverName && ncSettingsFileLocal.machine_tool == machineToolName)
            {
                SetDefaults(ncSettingsFileLocal.machine_tool_type, ncSettingsFileLocal.kinematics, ncSettingsFileLocal.zero_point);
            }
            else
            {
                SetDefaults(ncSettings.machine_tool[machineToolName].type, ncSettings.machine_tool[machineToolName].kinematics, defaultProgramZeroData);
            }
        }

        private void SetDefaults(MachineToolType machineToolType, Kinematics kinematics, ProgramZeroData zeroPoint)
        {
            pictureBox1.Image = Image.FromFile(@"img/zero_point_" + machineToolType.ToString() + ".png");

            if (machineToolType == MachineToolType.lathe || machineToolType == MachineToolType.millturn)
            {
                labelX.Hide();
                textBoxX.Hide();
                labelXmin.Hide();
                labelXminValue.Hide();
                labelXArrows.Hide();
                labelXmax.Hide();
                labelXmaxValue.Hide();

                labelY.Hide();
                textBoxY.Hide();
                labelYmin.Hide();
                labelYminValue.Hide();
                labelYArrows.Hide();
                labelYmax.Hide();
                labelYmaxValue.Hide();
            }
            else
            {
                labelX.Show();
                textBoxX.Show();
                labelXmin.Show();
                labelXminValue.Show();
                labelXArrows.Show();
                labelXmax.Show();
                labelXmaxValue.Show();

                labelY.Show();
                textBoxY.Show();
                labelYmin.Show();
                labelYminValue.Show();
                labelYArrows.Show();
                labelYmax.Show();
                labelYmaxValue.Show();

                textBoxX.Text = zeroPoint["X"].ToString();
                textBoxY.Text = zeroPoint["Y"].ToString();

                labelXminValue.Text = kinematics.cartesian_system_axis["X"].range_min.ToString();
                labelXmaxValue.Text = kinematics.cartesian_system_axis["X"].range_max.ToString();
                labelYminValue.Text = kinematics.cartesian_system_axis["Y"].range_min.ToString();
                labelYmaxValue.Text = kinematics.cartesian_system_axis["Y"].range_max.ToString();
            }

            textBoxZ.Text = zeroPoint["Z"].ToString();

            labelZminValue.Text = kinematics.cartesian_system_axis["Z"].range_min.ToString();
            labelZmaxValue.Text = kinematics.cartesian_system_axis["Z"].range_max.ToString();
        }

        public void MakeDefault()
        {
            SetDefaults(ncSettings.machine_tool[machineToolName].type, ncSettings.machine_tool[machineToolName].kinematics, defaultProgramZeroData);
        }

        public void ReadData(ref NCSettingsFile ncSettingsFileLocal)
        {
            var zeroProgramData = ncSettingsFileLocal.zero_point;

            if (ncSettings.machine_tool[machineToolName].type == MachineToolType.lathe || ncSettings.machine_tool[machineToolName].type == MachineToolType.millturn)
            {
                zeroProgramData["X"] = 0;
                zeroProgramData["Y"] = 0;
            }
            else
            {
                zeroProgramData["X"] = Double.Parse(textBoxX.Text);
                zeroProgramData["Y"] = Double.Parse(textBoxY.Text);
            }

            zeroProgramData["Z"] = Double.Parse(textBoxZ.Text);
        }

        private void AllowDecimalsOnly(object sender, KeyPressEventArgs e)
        {
            if (!char.IsControl(e.KeyChar) && !Char.IsDigit(e.KeyChar) && e.KeyChar != '.' && e.KeyChar != '-')
            {
                e.Handled = true;
            }

            // only allow one decimal point
            if (e.KeyChar == '.' && (sender as TextBox).Text.IndexOf('.') > -1)
            {
                e.Handled = true;
            }

            // only allow one minus
            if (e.KeyChar == '-' && (sender as TextBox).Text.IndexOf('-') > -1)
            {
                e.Handled = true;
            }

            // only allow minus as a first character
            if (e.KeyChar == '-' && (sender as TextBox).SelectionStart != 0)
            {
                e.Handled = true;
            }
        }

        private void textBoxX_KeyPress(object sender, KeyPressEventArgs e)
        {
            AllowDecimalsOnly(sender, e);
        }

        private void textBoxY_KeyPress(object sender, KeyPressEventArgs e)
        {
            AllowDecimalsOnly(sender, e);
        }

        private void textBoxZ_KeyPress(object sender, KeyPressEventArgs e)
        {
            AllowDecimalsOnly(sender, e);
        }
    }
}
