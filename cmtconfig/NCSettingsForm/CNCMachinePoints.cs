using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Globalization;
using parser;

namespace cmtconfig
{
    public partial class CNCMachinePoints : UserControl
    {
        private readonly NCSettingsName ncSettingsName;
        private readonly NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;

        private string driverName = "";
        private string machineToolName = "";

        public CNCMachinePoints(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile)
        {
            InitializeComponent();
            
            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
        }
        
        void MachinePointsLoad(object sender, EventArgs e)
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
            reload_content();
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
            SetDefaults(ncSettingsFileLocal.driver.ToString() == driverName && ncSettingsFileLocal.machine_tool == machineToolName &&
                ncSettingsFileLocal.machine_points_data.tool_exchange_point.Count > 0 ?
                ncSettingsFileLocal.machine_points_data : ncSettings.machine_tool[machineToolName].machine_points_data);
        }

        private void SetDefaults(MachinePointsData machinePointsDataParam)
        {
            groupBoxToolExchangePoint.Text = ncSettingsName.machine_points_name["tool_exchange_point"];
            groupBoxMachineStartingPosition.Text = ncSettingsName.machine_points_name["machine_base_point"];

            var tool_exchange_point = machinePointsDataParam.tool_exchange_point;
            var machine_base_point = machinePointsDataParam.machine_base_point;

            textBoxToolExchangePointX.Text = tool_exchange_point["X"].ToString();
            textBoxToolExchangePointY.Text = tool_exchange_point["Y"].ToString();
            textBoxToolExchangePointZ.Text = tool_exchange_point["Z"].ToString();
            textBoxMachineStartingPositionX.Text = machine_base_point["X"].ToString();
            textBoxMachineStartingPositionY.Text = machine_base_point["Y"].ToString();
            textBoxMachineStartingPositionZ.Text = machine_base_point["Z"].ToString();
        }

        void reload_content()
        {
            var machineToolData = ncSettings.machine_tool[machineToolName];
            pictureBox1.Image = Image.FromFile(@"img/" + machineToolData.type.ToString() + "_pkt_char.png");
        }

        protected override void WndProc(ref Message m)
        {
            const int WM_WINDOWPOSCHANGING = 0x0046; // where is that defined in c# ???
            if (m.Msg == WM_WINDOWPOSCHANGING)
            {
                if (ParentForm.Controls.GetChildIndex(this) == 0) // reload datagrid if our form is brought to the front
                {
                    if(HasActiveMachineToolChanged())
                    {
                        SetCurrentOrDefaults(ncSettingsFile);
                        reload_content();
                    }
                }
            }

            base.WndProc(ref m);
        }

        public void MakeDefault()
        {
            HasActiveMachineToolChanged();
            SetDefaults(ncSettings.machine_tool[machineToolName].machine_points_data);
            reload_content();
        }

        private void ReadData(Control control, IDictionary<string, double> dict, string name)
        {
            double value;
            if (!double.TryParse(control.Text, NumberStyles.Float | NumberStyles.AllowThousands, CultureInfo.InvariantCulture, out value))
                value = 0.0;
            dict[name] = value;
        }

        public void ReadData(ref NCSettingsFile ncSettingsFileLocal)
        {
            var tool_exchange_point = ncSettingsFileLocal.machine_points_data.tool_exchange_point;
            var machine_base_point = ncSettingsFileLocal.machine_points_data.machine_base_point;

            ReadData(textBoxToolExchangePointX, tool_exchange_point, "X");
            ReadData(textBoxToolExchangePointY, tool_exchange_point, "Y");
            ReadData(textBoxToolExchangePointZ, tool_exchange_point, "Z");
            ReadData(textBoxMachineStartingPositionX, machine_base_point, "X");
            ReadData(textBoxMachineStartingPositionY, machine_base_point, "Y");
            ReadData(textBoxMachineStartingPositionZ, machine_base_point, "Z");
        }

        private void ValidateInput(Control control)
        {
            string actualdata = string.Empty;
            char[] entereddata = control.Text.ToCharArray();
            foreach (char aChar in entereddata.AsEnumerable())
            {
                if(actualdata == string.Empty && aChar == '-')
                {
                    actualdata += aChar;
                    continue;
                }
                double parsedValue;
                if (double.TryParse(actualdata + aChar, NumberStyles.Float | NumberStyles.AllowThousands, CultureInfo.InvariantCulture, out parsedValue))
                    actualdata += aChar;
            }
            control.Text = actualdata;
        }

        private void textBoxToolExchangePointX_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxToolExchangePointX);
        }

        private void textBoxToolExchangePointY_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxToolExchangePointY);
        }

        private void textBoxToolExchangePointZ_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxToolExchangePointZ);
        }

        private void textBoxMachineStartingPositionX_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxMachineStartingPositionX);
        }

        private void textBoxMachineStartingPositionY_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxMachineStartingPositionY);
        }

        private void textBoxMachineStartingPositionZ_TextChanged(object sender, EventArgs e)
        {
            ValidateInput(textBoxMachineStartingPositionZ);
        }
    }
}
