using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using parser;

namespace cmtconfig
{
    public partial class CNCKinematics : UserControl
    {
        private readonly NCSettingsName ncSettingsName;
        private readonly NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;

        private string driverName = "";
        private string machineToolName = "";

        public CNCKinematics(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile)
        {
            InitializeComponent();

            dataGridView1.Columns[0].ReadOnly = true;

            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
        }

        void KinematicsLoad(object sender, EventArgs e)
        {
            labelMaxWorkingFeed.Text = ncSettingsName.kinematics_name["max_working_feed"];
            labelMaxWorkingFeedUnit.Text = ncSettingsName.kinematics_name["max_working_feed_unit"];
            labelMaxFastFeed.Text = ncSettingsName.kinematics_name["max_fast_feed"];
            labelMaxFastFeedUnit.Text = ncSettingsName.kinematics_name["max_fast_feed_unit"];
            labelMaximumSpindleSpeed.Text = ncSettingsName.kinematics_name["maximum_spindle_speed"];
            labelMaximumSpindleSpeedUnit.Text = ncSettingsName.kinematics_name["maximum_spindle_speed_unit"];
            labelToolsQuantity.Text = ncSettingsName.kinematics_name["numer_of_items_in_the_warehouse"];
            labelToolsQuantityUnit.Text = ncSettingsName.kinematics_name["numer_of_items_in_the_warehouse_unit"];
            labelToolExchangeTime.Text = ncSettingsName.kinematics_name["tool_exchange_time"];
            labelToolExchangeTimeUnit.Text = ncSettingsName.kinematics_name["tool_exchange_time_unit"];
            labelPalletExchangeTime.Text = ncSettingsName.kinematics_name["pallet_exchange_time"];
            labelPalletExchangeTimeUnit.Text = ncSettingsName.kinematics_name["pallet_exchange_time_unit"];
            labelToolMeasurementTime.Text = ncSettingsName.kinematics_name["tool_measurement_time"];
            labelToolMeasurementTimeUnit.Text = ncSettingsName.kinematics_name["tool_measurement_time_unit"];
            checkBoxDiameterProgramming.Text = ncSettingsName.kinematics_name["diameter_programming_2x"];
            checkBoxAutoMeasureAfterToolSelection.Text = ncSettingsName.kinematics_name["auto_measure_after_tool_selection"];
            labelPalletExchangeCode.Text = ncSettingsName.kinematics_name["pallet_exchange_code"];
            labelToolMeasurementCode.Text = ncSettingsName.kinematics_name["tool_measurement_code"];

            dataGridView1.Columns[0].HeaderText = ncSettingsName.kinematics_name["cartesian_system_axis"];
            dataGridView1.Columns[1].HeaderText = ncSettingsName.kinematics_name["range_min"];
            dataGridView1.Columns[2].HeaderText = ncSettingsName.kinematics_name["range_max"];

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
            SetDefaults(ncSettingsFileLocal.driver.ToString() == driverName && ncSettingsFileLocal.machine_tool == machineToolName &&
                ncSettingsFileLocal.kinematics.cartesian_system_axis.Count > 0 ?
                        ncSettingsFileLocal.kinematics : ncSettings.machine_tool[machineToolName].kinematics);
            reload_content(ncSettingsFileLocal.driver.ToString() == driverName && ncSettingsFileLocal.machine_tool == machineToolName &&
                ncSettingsFileLocal.extras.Count > 0 ?
                        ncSettingsFileLocal.extras : ncSettings.machine_tool[machineToolName].extras);
        }

        private void FillData(ref ComboBox comboBox, string name, string value)
        {
            var kinematics_list = ncSettings.kinematics_list;
            comboBox.Items.Clear();
            foreach (var item in kinematics_list[name])
                comboBox.Items.Add(new ComboboxItem(item, name, item));
            comboBox.SelectedIndex = comboBox.FindStringExact(value);
        }

        private string ReadData(ComboBox combobox)
        {
            var comboboxItem = combobox.Items[combobox.SelectedIndex] as ComboboxItem;
            return comboboxItem.Value.ToString();
        }

        private void SetDefaults(Kinematics kinematics)
        {
            textBoxMaxWorkingFeed.Text = kinematics.max_working_feed.ToString();
            textBoxMaxFastFeed.Text = kinematics.max_fast_feed.ToString();
            textBoxMaximumSpindleSpeed.Text = kinematics.maximum_spindle_speed.ToString();
            textBoxToolsQuantity.Text = kinematics.numer_of_items_in_the_warehouse.ToString();
            textBoxToolExchangeTime.Text = kinematics.tool_exchange_time.ToString();
            textBoxPalletExchangeTime.Text = kinematics.pallet_exchange_time.ToString();
            textBoxToolMeasurementTime.Text = kinematics.tool_measurement_time.ToString();
            checkBoxDiameterProgramming.Checked = kinematics.diameter_programming_2x;
            checkBoxAutoMeasureAfterToolSelection.Checked = kinematics.auto_measure_after_tool_selection;

            FillData(ref comboBoxPalletExchangeCode, "pallet_exchange_code", kinematics.pallet_exchange_code);
            FillData(ref comboBoxToolMeasurementCode, "tool_measurement_code", kinematics.tool_measurement_code);
            textBoxPalletExchangeCode.Text = kinematics.pallet_exchange_code_value;
            textBoxToolMeasurementCode.Text = kinematics.tool_measurement_code_value;

            dataGridView1.Rows.Clear();

            foreach (var item in kinematics.cartesian_system_axis)
            {
                var row = new string[dataGridView1.ColumnCount];
                row[0] = item.Key;
                row[1] = item.Value.range_min.ToString();
                row[2] = item.Value.range_max.ToString();
                dataGridView1.Rows.Add(row);
            }
        }

        void comboBoxPalletExchangeCode_SelectedIndexChanged(object sender, EventArgs e)
        {
            var item = comboBoxPalletExchangeCode.SelectedItem as ComboboxItem;
            textBoxPalletExchangeCode.Enabled = !string.IsNullOrEmpty(item.Value.ToString());
        }

        void comboBoxToolMeasurementCode_SelectedIndexChanged(object sender, EventArgs e)
        {
            var item = comboBoxToolMeasurementCode.SelectedItem as ComboboxItem;
            textBoxToolMeasurementCode.Enabled = !string.IsNullOrEmpty(item.Value.ToString());
        }

        private void ShowPalletControls(bool show)
        {
            if (show)
            {
                labelPalletExchangeTime.Show();
                textBoxPalletExchangeTime.Show();
                labelPalletExchangeTimeUnit.Show();
                labelPalletExchangeCode.Show();
                comboBoxPalletExchangeCode.Show();
                textBoxPalletExchangeCode.Show();
            }
            else
            {
                labelPalletExchangeTime.Hide();
                textBoxPalletExchangeTime.Hide();
                labelPalletExchangeTimeUnit.Hide();
                labelPalletExchangeCode.Hide();
                comboBoxPalletExchangeCode.Hide();
                textBoxPalletExchangeCode.Hide();
            }
        }

        private void ShowMeasurementControls(bool show)
        {
            if (show)
            {
                labelToolMeasurementTime.Show();
                textBoxToolMeasurementTime.Show();
                labelToolMeasurementTimeUnit.Show();
                labelToolMeasurementCode.Show();
                comboBoxToolMeasurementCode.Show();
                textBoxToolMeasurementCode.Show();
                checkBoxAutoMeasureAfterToolSelection.Show();
            }
            else
            {
                labelToolMeasurementTime.Hide();
                textBoxToolMeasurementTime.Hide();
                labelToolMeasurementTimeUnit.Hide();
                labelToolMeasurementCode.Hide();
                comboBoxToolMeasurementCode.Hide();
                textBoxToolMeasurementCode.Hide();
                checkBoxAutoMeasureAfterToolSelection.Hide();
            }
        }

        private void reload_content(IDictionary<string, bool> extras)
        {
            var machineToolType = ncSettings.machine_tool[machineToolName].type;

            if(machineToolType == MachineToolType.mill)
                checkBoxDiameterProgramming.Hide();
            else
                checkBoxDiameterProgramming.Show();

            if(extras.ContainsKey("pallet_changer") && extras["pallet_changer"])
                ShowPalletControls(true);
            else
                ShowPalletControls(false);

            if(extras.ContainsKey("cnc_probe") && extras["cnc_probe"])
                ShowMeasurementControls(true);
            else
                ShowMeasurementControls(false);
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
                    }
                    var ncSettingsForm = ParentForm as NCSettingsForm;
                    if (ncSettingsForm.MachineTools1.ExtrasChanged)
                    {
                        ShowPalletControls(ncSettingsForm.MachineTools1.PalletChangerPresent);
                        ShowMeasurementControls(ncSettingsForm.MachineTools1.CncProbePresent);
                        ncSettingsForm.MachineTools1.ExtrasChanged = false;
                    }
                }
            }

            base.WndProc(ref m);
        }

        public void MakeDefault()
        {
            HasActiveMachineToolChanged();
            SetDefaults(ncSettings.machine_tool[machineToolName].kinematics);
            reload_content(ncSettings.machine_tool[machineToolName].extras);
        }

        public void ReadData(ref NCSettingsFile ncSettingsFileLocal)
        {
            var kinematics = ncSettingsFileLocal.kinematics;

            kinematics.max_working_feed = int.Parse(textBoxMaxWorkingFeed.Text);
            kinematics.max_fast_feed = int.Parse(textBoxMaxFastFeed.Text);
            kinematics.maximum_spindle_speed = int.Parse(textBoxMaximumSpindleSpeed.Text);
            kinematics.numer_of_items_in_the_warehouse = int.Parse(textBoxToolsQuantity.Text);
            kinematics.tool_exchange_time = int.Parse(textBoxToolExchangeTime.Text);
            kinematics.diameter_programming_2x = checkBoxDiameterProgramming.Checked;

            var ncSettingsForm = ParentForm as NCSettingsForm;
            if (ncSettingsForm.MachineTools1.PalletChangerPresent)
            {
                kinematics.pallet_exchange_time = int.Parse(textBoxPalletExchangeTime.Text);
                kinematics.pallet_exchange_code = ReadData(comboBoxPalletExchangeCode);
                kinematics.pallet_exchange_code_value = textBoxPalletExchangeCode.Text;
            }
            else
            {
                kinematics.pallet_exchange_time = 0;
                kinematics.pallet_exchange_code = "";
                kinematics.pallet_exchange_code_value = "";
            }

            if (ncSettingsForm.MachineTools1.CncProbePresent)
            {
                kinematics.auto_measure_after_tool_selection = checkBoxAutoMeasureAfterToolSelection.Checked;
                kinematics.tool_measurement_time = int.Parse(textBoxToolMeasurementTime.Text);
                kinematics.tool_measurement_code = ReadData(comboBoxToolMeasurementCode);
                kinematics.tool_measurement_code_value = textBoxToolMeasurementCode.Text;
            }
            else
            {
                kinematics.auto_measure_after_tool_selection = false;
                kinematics.tool_measurement_time = 0;
                kinematics.tool_measurement_code = "";
                kinematics.tool_measurement_code_value = "";
            }

            kinematics.cartesian_system_axis.Clear();
            foreach (DataGridViewRow row in dataGridView1.Rows)
            {
                kinematics.cartesian_system_axis.Add(row.Cells[0].Value.ToString(), new AxisParameters()
                {
                    range_min = float.Parse(row.Cells[1].Value.ToString()),
                    range_max = float.Parse(row.Cells[2].Value.ToString()),
                });
            }
        }
    }
}
