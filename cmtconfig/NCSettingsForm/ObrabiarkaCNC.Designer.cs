namespace cmtconfig
{
    partial class ObrabiarkaCNC
    {
        /// <summary> 
        /// Wymagana zmienna projektanta.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Wyczyść wszystkie używane zasoby.
        /// </summary>
        /// <param name="disposing">prawda, jeżeli zarządzane zasoby powinny zostać zlikwidowane; Fałsz w przeciwnym wypadku.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Kod wygenerowany przez Projektanta składników

        /// <summary> 
        /// Wymagana metoda obsługi projektanta — nie należy modyfikować 
        /// zawartość tej metody z edytorem kodu.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ObrabiarkaCNC));
            this.labelHeader = new System.Windows.Forms.Label();
            this.comboBoxMachine = new System.Windows.Forms.ComboBox();
            this.comboBoxDriver = new System.Windows.Forms.ComboBox();
            this.labelDriver = new System.Windows.Forms.Label();
            this.labelModel = new System.Windows.Forms.Label();
            this.comboBoxModel = new System.Windows.Forms.ComboBox();
            this.labelMachine = new System.Windows.Forms.Label();
            this.listViewExtras = new System.Windows.Forms.ListView();
            this.groupBoxExtras = new System.Windows.Forms.GroupBox();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.labelMachineID = new System.Windows.Forms.Label();
            this.textBoxMachineID = new System.Windows.Forms.TextBox();
            this.groupBoxExtras.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // labelHeader
            // 
            resources.ApplyResources(this.labelHeader, "labelHeader");
            this.labelHeader.Name = "labelHeader";
            // 
            // comboBoxMachine
            // 
            this.comboBoxMachine.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxMachine, "comboBoxMachine");
            this.comboBoxMachine.FormattingEnabled = true;
            this.comboBoxMachine.Name = "comboBoxMachine";
            this.comboBoxMachine.SelectedIndexChanged += new System.EventHandler(this.comboBoxMachine_SelectedIndexChanged);
            // 
            // comboBoxDriver
            // 
            this.comboBoxDriver.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDriver, "comboBoxDriver");
            this.comboBoxDriver.FormattingEnabled = true;
            this.comboBoxDriver.Name = "comboBoxDriver";
            this.comboBoxDriver.SelectedIndexChanged += new System.EventHandler(this.comboBoxDriver_SelectedIndexChanged);
            // 
            // labelDriver
            // 
            resources.ApplyResources(this.labelDriver, "labelDriver");
            this.labelDriver.Name = "labelDriver";
            // 
            // labelModel
            // 
            resources.ApplyResources(this.labelModel, "labelModel");
            this.labelModel.Name = "labelModel";
            // 
            // comboBoxModel
            // 
            this.comboBoxModel.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxModel, "comboBoxModel");
            this.comboBoxModel.FormattingEnabled = true;
            this.comboBoxModel.Items.AddRange(new object[] {
            resources.GetString("comboBoxModel.Items"),
            resources.GetString("comboBoxModel.Items1")});
            this.comboBoxModel.Name = "comboBoxModel";
            // 
            // labelMachine
            // 
            resources.ApplyResources(this.labelMachine, "labelMachine");
            this.labelMachine.Name = "labelMachine";
            // 
            // listViewExtras
            // 
            this.listViewExtras.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.listViewExtras.CheckBoxes = true;
            resources.ApplyResources(this.listViewExtras, "listViewExtras");
            this.listViewExtras.ForeColor = System.Drawing.SystemColors.WindowText;
            this.listViewExtras.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.listViewExtras.HideSelection = false;
            this.listViewExtras.Name = "listViewExtras";
            this.listViewExtras.UseCompatibleStateImageBehavior = false;
            this.listViewExtras.View = System.Windows.Forms.View.List;
            this.listViewExtras.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.listViewExtras_ItemChecked);
            // 
            // groupBoxExtras
            // 
            this.groupBoxExtras.Controls.Add(this.listViewExtras);
            resources.ApplyResources(this.groupBoxExtras, "groupBoxExtras");
            this.groupBoxExtras.Name = "groupBoxExtras";
            this.groupBoxExtras.TabStop = false;
            // 
            // pictureBox1
            // 
            resources.ApplyResources(this.pictureBox1, "pictureBox1");
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.TabStop = false;
            // 
            // groupBox1
            // 
            resources.ApplyResources(this.groupBox1, "groupBox1");
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.TabStop = false;
            // 
            // labelMachineID
            // 
            resources.ApplyResources(this.labelMachineID, "labelMachineID");
            this.labelMachineID.Name = "labelMachineID";
            // 
            // textBoxMachineID
            // 
            resources.ApplyResources(this.textBoxMachineID, "textBoxMachineID");
            this.textBoxMachineID.Name = "textBoxMachineID";
            // 
            // ObrabiarkaCNC
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.textBoxMachineID);
            this.Controls.Add(this.labelMachineID);
            this.Controls.Add(this.groupBoxExtras);
            this.Controls.Add(this.labelMachine);
            this.Controls.Add(this.comboBoxModel);
            this.Controls.Add(this.labelModel);
            this.Controls.Add(this.labelDriver);
            this.Controls.Add(this.comboBoxDriver);
            this.Controls.Add(this.comboBoxMachine);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.labelHeader);
            this.Controls.Add(this.groupBox1);
            this.Name = "ObrabiarkaCNC";
            this.Load += new System.EventHandler(this.ObrabiarkaCNCLoad);
            this.groupBoxExtras.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelHeader;
        private System.Windows.Forms.ComboBox comboBoxMachine;
        private System.Windows.Forms.ComboBox comboBoxDriver;
        private System.Windows.Forms.Label labelDriver;
        private System.Windows.Forms.Label labelModel;
        private System.Windows.Forms.ComboBox comboBoxModel;
        private System.Windows.Forms.Label labelMachine;
        private System.Windows.Forms.ListView listViewExtras;
        private System.Windows.Forms.GroupBox groupBoxExtras;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label labelMachineID;
        private System.Windows.Forms.TextBox textBoxMachineID;
    }
}
