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
    public partial class CNCInitialParams : UserControl
    {
        private readonly NCSettingsName ncSettingsName;
        private readonly NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;

        private string driverName = "";
        private string machineToolName = "";

        public CNCInitialParams(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile)
        {
            InitializeComponent();
            
            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
        }

        private void InitialParamsLoad(object sender, EventArgs e)
        {
            var cnc_default_values_name = ncSettingsName.cnc_default_values_name;
            labelDefaultMotion.Text = cnc_default_values_name["default_motion"];
            labelDefaultWorkPlane.Text = cnc_default_values_name["default_work_plane"];
            labelDefaultDriverUnits.Text = cnc_default_values_name["default_driver_units"];
            labelDefaultProgramming.Text = cnc_default_values_name["default_programming"];
            labelDefaultFeedMode.Text = cnc_default_values_name["default_feed_mode"];
            checkBoxFastMotionCancelCycle.Text = cnc_default_values_name["rapid_traverse_cancel_cycle"];
            checkBoxLinearInterpolationCancelCycle.Text = cnc_default_values_name["linear_interpolation_cancel_cycle"];
            checkBoxCircularInterpolationCWCancelCycle.Text = cnc_default_values_name["circular_interpolation_cw_cancel_cycle"];
            checkBoxCircularInterpolationCCWCancelCycle.Text = cnc_default_values_name["circular_interpolation_ccw_cancel_cycle"];
            checkBoxCycleCancelStartsFastMotion.Text = cnc_default_values_name["cycle_cancel_starts_rapid_traverse"];
            checkBoxOperatorTurnsOnRotation.Text = cnc_default_values_name["operator_turns_on_rotation"];
            labelDefaultRotation.Text = cnc_default_values_name["default_rotation"];
            labelDefaultRotationDirection.Text = cnc_default_values_name["default_rotation_direction"];
            checkBoxToolNumberExecutesExchange.Text = cnc_default_values_name["tool_number_executes_exchange"];
            checkBoxAutoFastMotionAfterToolExchange.Text = cnc_default_values_name["auto_rapid_traverse_after_tool_exchange"];
            labelDrillCycleZValue.Text = cnc_default_values_name["drill_cycle_z_value"];
            labelDrillCycleReturnValue.Text = cnc_default_values_name["drill_cycle_return_value"];

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
            SetDefaults(ncSettingsFileLocal.driver.ToString() == driverName && ncSettingsFileLocal.machine_tool == machineToolName ?
                ncSettingsFileLocal.cnc_default_values : ncSettings.machine_tool[machineToolName].cnc_default_values);
        }

        private void FillData(ref ComboBox comboBox, string name, string value)
        {
            var cnc_default_values_list = ncSettings.cnc_default_values_list;
            var cnc_default_values_list_name = ncSettingsName.cnc_default_values_list_name;
            comboBox.Items.Clear();
            foreach (var item in cnc_default_values_list[name])
                comboBox.Items.Add(new ComboboxItem(cnc_default_values_list_name[name][item], name, item));
            comboBox.SelectedIndex = comboBox.FindStringExact(cnc_default_values_list_name[name][value]);
        }

        private void SetDefaults(CncDefaultValues cnc_default_values)
        {
            FillData(ref comboBoxDefaultMotion, "default_motion", cnc_default_values.default_motion.ToString());
            FillData(ref comboBoxDefaultWorkPlane, "default_work_plane", cnc_default_values.default_work_plane.ToString());
            FillData(ref comboBoxDefaultDriverUnits, "default_driver_units", cnc_default_values.default_driver_units.ToString());
            FillData(ref comboBoxDefaultProgramming, "default_programming", cnc_default_values.default_programming.ToString());
            FillData(ref comboBoxDefaultFeedMode, "default_feed_mode", cnc_default_values.default_feed_mode.ToString());
            checkBoxFastMotionCancelCycle.Checked = cnc_default_values.rapid_traverse_cancel_cycle;
            checkBoxLinearInterpolationCancelCycle.Checked = cnc_default_values.linear_interpolation_cancel_cycle;
            checkBoxCircularInterpolationCWCancelCycle.Checked = cnc_default_values.circular_interpolation_cw_cancel_cycle;
            checkBoxCircularInterpolationCCWCancelCycle.Checked = cnc_default_values.circular_interpolation_ccw_cancel_cycle;
            checkBoxCycleCancelStartsFastMotion.Checked = cnc_default_values.cycle_cancel_starts_rapid_traverse;
            checkBoxOperatorTurnsOnRotation.Checked = cnc_default_values.operator_turns_on_rotation;
            textBoxDefaultRotation.Text = cnc_default_values.default_rotation.ToString();
            FillData(ref comboBoxDefaultRotationDirection, "default_rotation_direction", cnc_default_values.default_rotation_direction.ToString());
            checkBoxToolNumberExecutesExchange.Checked = cnc_default_values.tool_number_executes_exchange;
            checkBoxAutoFastMotionAfterToolExchange.Checked = cnc_default_values.auto_rapid_traverse_after_tool_exchange;
            FillData(ref comboBoxDrillCycleZValue, "drill_cycle_z_value", cnc_default_values.drill_cycle_z_value.ToString());
            FillData(ref comboBoxDrillCycleReturnValue, "drill_cycle_return_value", cnc_default_values.drill_cycle_return_value.ToString());

            if(checkBoxOperatorTurnsOnRotation.Checked)
            {
                textBoxDefaultRotation.Enabled = true;
                comboBoxDefaultRotationDirection.Enabled = true;
            }
            else
            {
                textBoxDefaultRotation.Enabled = false;
                comboBoxDefaultRotationDirection.Enabled = false;
            }
        }

        protected override void WndProc(ref Message m)
        {
            const int WM_WINDOWPOSCHANGING = 0x0046; // where is that defined in c# ???
            if (m.Msg == WM_WINDOWPOSCHANGING)
            {
                if (ParentForm.Controls.GetChildIndex(this) == 0) // reload datagrid if our form is brought to the front
                {
                    if(HasActiveMachineToolChanged())
                        SetCurrentOrDefaults(ncSettingsFile);
                }
            }

            base.WndProc(ref m);
        }

        public void MakeDefault()
        {
            HasActiveMachineToolChanged();
            SetDefaults(ncSettings.machine_tool[machineToolName].cnc_default_values);
        }

        private T ReadData<T>(ComboBox comboBox)
        {
            return (T)Enum.Parse(typeof(T), ((ComboboxItem)comboBox.Items[comboBox.SelectedIndex]).Value.ToString());
        }

        public void ReadData(ref NCSettingsFile ncSettingsFile)
        {
            var cnc_default_values = ncSettingsFile.cnc_default_values;

            cnc_default_values.default_motion = ReadData<Motion>(comboBoxDefaultMotion);
            cnc_default_values.default_work_plane = ReadData<WorkPlane>(comboBoxDefaultWorkPlane);
            cnc_default_values.default_driver_units = ReadData<DriverUnits>(comboBoxDefaultDriverUnits);
            cnc_default_values.default_programming = ReadData<ProgrammingType>(comboBoxDefaultProgramming);
            cnc_default_values.default_feed_mode = ReadData<FeedMode>(comboBoxDefaultFeedMode);
            cnc_default_values.rapid_traverse_cancel_cycle = checkBoxFastMotionCancelCycle.Checked;
            cnc_default_values.linear_interpolation_cancel_cycle = checkBoxLinearInterpolationCancelCycle.Checked;
            cnc_default_values.circular_interpolation_cw_cancel_cycle = checkBoxCircularInterpolationCWCancelCycle.Checked;
            cnc_default_values.circular_interpolation_ccw_cancel_cycle = checkBoxCircularInterpolationCCWCancelCycle.Checked;
            cnc_default_values.cycle_cancel_starts_rapid_traverse = checkBoxCycleCancelStartsFastMotion.Checked;
            cnc_default_values.operator_turns_on_rotation = checkBoxOperatorTurnsOnRotation.Checked;
            cnc_default_values.default_rotation = int.Parse(textBoxDefaultRotation.Text);
            cnc_default_values.default_rotation_direction = ReadData<RotationDirection>(comboBoxDefaultRotationDirection);
            cnc_default_values.tool_number_executes_exchange = checkBoxToolNumberExecutesExchange.Checked;
            cnc_default_values.auto_rapid_traverse_after_tool_exchange = checkBoxAutoFastMotionAfterToolExchange.Checked;
            cnc_default_values.drill_cycle_z_value = ReadData<DepthProgrammingType>(comboBoxDrillCycleZValue);
            cnc_default_values.drill_cycle_return_value = ReadData<DrillCycleReturnValue>(comboBoxDrillCycleReturnValue);
        }

        private void textBoxDefaultRotation_KeyPress(object sender, KeyPressEventArgs e)
        {
            if(!char.IsControl(e.KeyChar) && !Char.IsDigit(e.KeyChar))
            {
                e.Handled = true;
            }
        }

        void checkBoxOperatorTurnsOnRotation_CheckedChanged(object sender, EventArgs e)
        {
            if(checkBoxOperatorTurnsOnRotation.Checked)
            {
                textBoxDefaultRotation.Enabled = true;
                comboBoxDefaultRotationDirection.Enabled = true;
            }
            else
            {
                textBoxDefaultRotation.Enabled = false;
                comboBoxDefaultRotationDirection.Enabled = false;
            }
        }
    }
}
