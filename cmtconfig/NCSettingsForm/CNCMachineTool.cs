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
    public partial class CNCMachineTools : UserControl
    {
        private NCSettingsName ncSettingsName;
        private NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;
        private NCSettingsFile ncSettingsFileOverride = null;
        private bool extrasChanged = false;
        private Language language;

        public CNCMachineTools(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile, Language language)
        {
            InitializeComponent();

            comboBoxModel.SelectedIndex = 0;

            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
            this.language = language;
        }

        void MachineToolsLoad(object sender, EventArgs e)
        {
            labelModel.Hide();
            comboBoxModel.Hide();
            groupBoxExtras.Hide();
            Reload(ncSettingsFile);
        }

        public void SetNCSettingsFile(NCSettingsFile ncSettingsFileLocal)
        {
            ncSettingsFileOverride = ncSettingsFileLocal;
            Reload(ncSettingsFileLocal);
            ncSettingsFileOverride = null;
        }

        public void Reload(NCSettingsFile ncSettingsFileLocal)
        {
            comboBoxDriver.SelectedIndex = -1;
            SetDefaults(ncSettingsFileLocal.driver, ncSettingsFileLocal.machine_tool_id);
        }

        private void SetDefaults(Driver driver, string machine_tool_id)
        {
            if (comboBoxDriver.Items.Count <= 0)
            {
                foreach (var element in ncSettings.driver_list)
                {
                    switch (element)
                    {
                        case Driver.fanuc_mill:
                        case Driver.generic_mill:
                        case Driver.haas_mill:
                        case Driver.makino_mill:
                            comboBoxDriver.Items.Add(new ComboboxItem(ncSettingsName.driver_name[element], element.ToString(), element));
                            break;
                        case Driver.fanuc_lathe_system_a:
                        case Driver.fanuc_lathe_system_b:
                        case Driver.fanuc_lathe_system_c:
                        case Driver.generic_lathe:
                        case Driver.haas_lathe:
                            comboBoxDriver.Items.Add(new ComboboxItem(ncSettingsName.driver_name[element], element.ToString(), element));
                            break;
                        case Driver.heidenhain:
                            break;
                    }
                }
            }
            comboBoxDriver.SelectedIndex = comboBoxDriver.FindStringExact(ncSettingsName.driver_name[driver]);

            textBoxMachineID.Text = machine_tool_id;
        }

        private void FillComboBoxMachine(NCSettingsFile ncSettingsFileLocal, Driver driver)
        {
            var activeMachineTool = ncSettingsFileLocal.machine_tool;
            comboBoxMachine.Items.Clear();
            bool activeMachineToolPresent = false;
            foreach (var machine in ncSettings.machine_tool)
            {
                switch(driver)
                {
                    case Driver.fanuc_mill:
                    case Driver.generic_mill:
                    case Driver.haas_mill:
                    case Driver.makino_mill:
                        if (machine.Value.type != MachineToolType.mill)
                            continue;
                        break;
                    case Driver.fanuc_lathe_system_a:
                    case Driver.fanuc_lathe_system_b:
                    case Driver.fanuc_lathe_system_c:
                    case Driver.generic_lathe:
                        if (machine.Value.type != MachineToolType.lathe /*&& machine.Value.type != MachineToolType.millturn*/)
                            continue;
                        break;
                    case Driver.haas_lathe:
                        if (machine.Value.type != MachineToolType.lathe)
                            continue;
                        break;
                }

                comboBoxMachine.Items.Add(new ComboboxItem(ncSettingsName.machine_tool_name[machine.Key], machine.Key, machine.Value));
                if (activeMachineTool == machine.Key)
                    activeMachineToolPresent = true;
            }
            if (activeMachineToolPresent)
                comboBoxMachine.SelectedIndex = comboBoxMachine.FindStringExact(ncSettingsName.machine_tool_name[activeMachineTool]);
            else
                comboBoxMachine.SelectedIndex = 0;
        }

        private void comboBoxDriver_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxDriver.SelectedIndex == -1)
                return;

            var selected_item = comboBoxDriver.SelectedItem as ComboboxItem;
            var selectedDriver = (Driver)selected_item.Value;

            var ncSettingsFileTmp = ncSettingsFileOverride != null ? ncSettingsFileOverride : ncSettingsFile;
            FillComboBoxMachine(ncSettingsFileTmp, selectedDriver);
        }

        private void comboBoxMachine_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBoxMachine.SelectedIndex == -1)
                return;

            var ncSettingsFileTmp = ncSettingsFileOverride != null ? ncSettingsFileOverride : ncSettingsFile;

            var activeMachineTool = GetActiveMachineTool;
            listViewExtras.Clear();
            var selected_item = comboBoxMachine.SelectedItem as ComboboxItem;
            var machineToolsData = selected_item.Value as MachineToolData;
            var extras = ncSettingsFileTmp.extras.Count > 0 && activeMachineTool == ncSettingsFileTmp.machine_tool ? ncSettingsFileTmp.extras : machineToolsData.extras;
            foreach (var item in extras)
            {
                var lvi = listViewExtras.Items.Add(ncSettingsName.extras_name[item.Key]);
                lvi.Checked = item.Value;
                lvi.Tag = item;
            }

            pictureBox1.Image = Image.FromFile(@"img/" + activeMachineTool + ".png");

            var ncSettingsForm = ParentForm as NCSettingsForm;
            ncSettingsForm.Kinematics1.Reload(ncSettingsFileTmp);
            ncSettingsForm.InitialParams1.Reload(ncSettingsFileTmp);
            ncSettingsForm.MachinePoints1.Reload(ncSettingsFileTmp);
            ncSettingsForm.ProgramZero1.Reload(ncSettingsFileTmp);
        }

        public void MakeDefault()
        {
            comboBoxMachine.SelectedIndex = comboBoxMachine.FindStringExact(ncSettingsName.machine_tool_name[ncSettings.machine_tool_default]);
            SetDefaults(ncSettings.driver_default, ncSettings.machine_tool_id);
        }

        public string GetActiveDriver
        {
            get
            {
                var item = comboBoxDriver.SelectedItem as ComboboxItem;
                return item != null ? item.Name : "";
            }
        }

        public string GetActiveMachineTool
        {
            get
            {
                var item = comboBoxMachine.SelectedItem as ComboboxItem;
                return item != null ? item.Name : "";
            }
        }

        public bool ExtrasChanged
        {
            get
            {
                return extrasChanged;
            }
            set
            {
                extrasChanged = value;
            }
        }

        public void ReadData(ref NCSettingsFile ncSettingsFileLocal)
        {
            ncSettingsFileLocal.machine_tool = GetActiveMachineTool;

            var driver = comboBoxDriver.SelectedItem as ComboboxItem;
            ncSettingsFileLocal.driver = (Driver)driver.Value;

            var machine_selected_item = comboBoxMachine.SelectedItem as ComboboxItem;
            var machineToolsData = machine_selected_item.Value as MachineToolData;
            ncSettingsFileLocal.machine_tool_type = machineToolsData.type;

            ncSettingsFileLocal.machine_tool_id = textBoxMachineID.Text;

            ncSettingsFileLocal.extras.Clear();
            foreach(var item in listViewExtras.Items)
            {
                var lvi = (ListViewItem)item;
                var extras = (KeyValuePair<string, bool>)lvi.Tag;
                ncSettingsFileLocal.extras[extras.Key] = lvi.Checked;
            }

            // Set zero points of X and Y axes to zero as they aren't being used for lathe and millturn
            //var machineToolType = ncSettings.machine_tool[GetActiveMachineTool].type;
            //if (machineToolType == MachineToolType.lathe || machineToolType == MachineToolType.millturn)
            //{
            //    ncSettingsFile.zero_point["X"] = 0;
            //    ncSettingsFile.zero_point["Y"] = 0;
            //}
        }

        public bool PalletChangerPresent
        {
            get
            {
                foreach (var item in listViewExtras.Items)
                {
                    var lvi = (ListViewItem)item;
                    var extras = (KeyValuePair<string, bool>)lvi.Tag;
                    if(extras.Key == "pallet_changer")
                        return lvi.Checked;
                }
                return false;
            }
        }

        public bool CncProbePresent
        {
            get
            {
                foreach (var item in listViewExtras.Items)
                {
                    var lvi = (ListViewItem)item;
                    var extras = (KeyValuePair<string, bool>)lvi.Tag;
                    if (extras.Key == "cnc_probe")
                        return lvi.Checked;
                }
                return false;
            }
        }

        private void listViewExtras_ItemChecked(object sender, ItemCheckedEventArgs e)
        {
            var lvi = (ListViewItem)e.Item;
            if (lvi.Tag == null)
                return;

            var ncSettingsForm = ParentForm as NCSettingsForm;
            var kvp = (KeyValuePair<string, bool>)lvi.Tag;
            if (kvp.Key == "pallet_changer")
                extrasChanged = true;
            else if (kvp.Key == "cnc_probe")
                extrasChanged = true;
        }
    }

    public class ComboboxItem
    {
        public ComboboxItem(string text, string name, object value)
        {
            Text = text;
            Name = name;
            Value = value;
        }

        public string Text { get; set; }
        public string Name { get; set; }
        public object Value { get; set; }

        public override string ToString()
        {
            return Text;
        }
    }
}
