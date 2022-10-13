namespace cmtconfig
{
    partial class CNCKinematics
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CNCKinematics));
            this.labelHeader = new System.Windows.Forms.Label();
            this.dataGridView1 = new System.Windows.Forms.DataGridView();
            this.Column1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.Column3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.labelMaximumSpindleSpeed = new System.Windows.Forms.Label();
            this.labelToolsQuantity = new System.Windows.Forms.Label();
            this.labelToolExchangeTime = new System.Windows.Forms.Label();
            this.labelPalletExchangeTime = new System.Windows.Forms.Label();
            this.labelToolMeasurementTime = new System.Windows.Forms.Label();
            this.textBoxMaximumSpindleSpeed = new System.Windows.Forms.TextBox();
            this.textBoxToolsQuantity = new System.Windows.Forms.TextBox();
            this.textBoxToolExchangeTime = new System.Windows.Forms.TextBox();
            this.textBoxPalletExchangeTime = new System.Windows.Forms.TextBox();
            this.textBoxToolMeasurementTime = new System.Windows.Forms.TextBox();
            this.labelMaximumSpindleSpeedUnit = new System.Windows.Forms.Label();
            this.labelToolsQuantityUnit = new System.Windows.Forms.Label();
            this.labelToolExchangeTimeUnit = new System.Windows.Forms.Label();
            this.labelPalletExchangeTimeUnit = new System.Windows.Forms.Label();
            this.labelToolMeasurementTimeUnit = new System.Windows.Forms.Label();
            this.checkBoxDiameterProgramming = new System.Windows.Forms.CheckBox();
            this.checkBoxAutoMeasureAfterToolSelection = new System.Windows.Forms.CheckBox();
            this.labelMaxFastFeedUnit = new System.Windows.Forms.Label();
            this.textBoxMaxFastFeed = new System.Windows.Forms.TextBox();
            this.labelMaxFastFeed = new System.Windows.Forms.Label();
            this.labelMaxWorkingFeedUnit = new System.Windows.Forms.Label();
            this.textBoxMaxWorkingFeed = new System.Windows.Forms.TextBox();
            this.labelMaxWorkingFeed = new System.Windows.Forms.Label();
            this.labelPalletExchangeCode = new System.Windows.Forms.Label();
            this.labelToolMeasurementCode = new System.Windows.Forms.Label();
            this.comboBoxPalletExchangeCode = new System.Windows.Forms.ComboBox();
            this.comboBoxToolMeasurementCode = new System.Windows.Forms.ComboBox();
            this.textBoxPalletExchangeCode = new System.Windows.Forms.TextBox();
            this.textBoxToolMeasurementCode = new System.Windows.Forms.TextBox();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.SuspendLayout();
            // 
            // labelHeader
            // 
            resources.ApplyResources(this.labelHeader, "labelHeader");
            this.labelHeader.Name = "labelHeader";
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridView1.AutoSizeRowsMode = System.Windows.Forms.DataGridViewAutoSizeRowsMode.DisplayedCells;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dataGridView1.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.Column1,
            this.Column2,
            this.Column3});
            resources.ApplyResources(this.dataGridView1, "dataGridView1");
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.RowHeadersVisible = false;
            // 
            // Column1
            // 
            resources.ApplyResources(this.Column1, "Column1");
            this.Column1.Name = "Column1";
            // 
            // Column2
            // 
            resources.ApplyResources(this.Column2, "Column2");
            this.Column2.Name = "Column2";
            // 
            // Column3
            // 
            resources.ApplyResources(this.Column3, "Column3");
            this.Column3.Name = "Column3";
            // 
            // labelMaximumSpindleSpeed
            // 
            resources.ApplyResources(this.labelMaximumSpindleSpeed, "labelMaximumSpindleSpeed");
            this.labelMaximumSpindleSpeed.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaximumSpindleSpeed.Name = "labelMaximumSpindleSpeed";
            // 
            // labelToolsQuantity
            // 
            resources.ApplyResources(this.labelToolsQuantity, "labelToolsQuantity");
            this.labelToolsQuantity.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolsQuantity.Name = "labelToolsQuantity";
            // 
            // labelToolExchangeTime
            // 
            resources.ApplyResources(this.labelToolExchangeTime, "labelToolExchangeTime");
            this.labelToolExchangeTime.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolExchangeTime.Name = "labelToolExchangeTime";
            // 
            // labelPalletExchangeTime
            // 
            resources.ApplyResources(this.labelPalletExchangeTime, "labelPalletExchangeTime");
            this.labelPalletExchangeTime.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelPalletExchangeTime.Name = "labelPalletExchangeTime";
            // 
            // labelToolMeasurementTime
            // 
            resources.ApplyResources(this.labelToolMeasurementTime, "labelToolMeasurementTime");
            this.labelToolMeasurementTime.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolMeasurementTime.Name = "labelToolMeasurementTime";
            // 
            // textBoxMaximumSpindleSpeed
            // 
            resources.ApplyResources(this.textBoxMaximumSpindleSpeed, "textBoxMaximumSpindleSpeed");
            this.textBoxMaximumSpindleSpeed.Name = "textBoxMaximumSpindleSpeed";
            // 
            // textBoxToolsQuantity
            // 
            resources.ApplyResources(this.textBoxToolsQuantity, "textBoxToolsQuantity");
            this.textBoxToolsQuantity.Name = "textBoxToolsQuantity";
            // 
            // textBoxToolExchangeTime
            // 
            resources.ApplyResources(this.textBoxToolExchangeTime, "textBoxToolExchangeTime");
            this.textBoxToolExchangeTime.Name = "textBoxToolExchangeTime";
            // 
            // textBoxPalletExchangeTime
            // 
            resources.ApplyResources(this.textBoxPalletExchangeTime, "textBoxPalletExchangeTime");
            this.textBoxPalletExchangeTime.Name = "textBoxPalletExchangeTime";
            // 
            // textBoxToolMeasurementTime
            // 
            resources.ApplyResources(this.textBoxToolMeasurementTime, "textBoxToolMeasurementTime");
            this.textBoxToolMeasurementTime.Name = "textBoxToolMeasurementTime";
            // 
            // labelMaximumSpindleSpeedUnit
            // 
            resources.ApplyResources(this.labelMaximumSpindleSpeedUnit, "labelMaximumSpindleSpeedUnit");
            this.labelMaximumSpindleSpeedUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaximumSpindleSpeedUnit.Name = "labelMaximumSpindleSpeedUnit";
            // 
            // labelToolsQuantityUnit
            // 
            resources.ApplyResources(this.labelToolsQuantityUnit, "labelToolsQuantityUnit");
            this.labelToolsQuantityUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolsQuantityUnit.Name = "labelToolsQuantityUnit";
            // 
            // labelToolExchangeTimeUnit
            // 
            resources.ApplyResources(this.labelToolExchangeTimeUnit, "labelToolExchangeTimeUnit");
            this.labelToolExchangeTimeUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolExchangeTimeUnit.Name = "labelToolExchangeTimeUnit";
            // 
            // labelPalletExchangeTimeUnit
            // 
            resources.ApplyResources(this.labelPalletExchangeTimeUnit, "labelPalletExchangeTimeUnit");
            this.labelPalletExchangeTimeUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelPalletExchangeTimeUnit.Name = "labelPalletExchangeTimeUnit";
            // 
            // labelToolMeasurementTimeUnit
            // 
            resources.ApplyResources(this.labelToolMeasurementTimeUnit, "labelToolMeasurementTimeUnit");
            this.labelToolMeasurementTimeUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelToolMeasurementTimeUnit.Name = "labelToolMeasurementTimeUnit";
            // 
            // checkBoxDiameterProgramming
            // 
            resources.ApplyResources(this.checkBoxDiameterProgramming, "checkBoxDiameterProgramming");
            this.checkBoxDiameterProgramming.ForeColor = System.Drawing.SystemColors.ControlText;
            this.checkBoxDiameterProgramming.Name = "checkBoxDiameterProgramming";
            this.checkBoxDiameterProgramming.UseVisualStyleBackColor = true;
            // 
            // checkBoxAutoMeasureAfterToolSelection
            // 
            resources.ApplyResources(this.checkBoxAutoMeasureAfterToolSelection, "checkBoxAutoMeasureAfterToolSelection");
            this.checkBoxAutoMeasureAfterToolSelection.ForeColor = System.Drawing.SystemColors.ControlText;
            this.checkBoxAutoMeasureAfterToolSelection.Name = "checkBoxAutoMeasureAfterToolSelection";
            this.checkBoxAutoMeasureAfterToolSelection.UseVisualStyleBackColor = true;
            // 
            // labelMaxFastFeedUnit
            // 
            resources.ApplyResources(this.labelMaxFastFeedUnit, "labelMaxFastFeedUnit");
            this.labelMaxFastFeedUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaxFastFeedUnit.Name = "labelMaxFastFeedUnit";
            // 
            // textBoxMaxFastFeed
            // 
            resources.ApplyResources(this.textBoxMaxFastFeed, "textBoxMaxFastFeed");
            this.textBoxMaxFastFeed.Name = "textBoxMaxFastFeed";
            // 
            // labelMaxFastFeed
            // 
            resources.ApplyResources(this.labelMaxFastFeed, "labelMaxFastFeed");
            this.labelMaxFastFeed.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaxFastFeed.Name = "labelMaxFastFeed";
            // 
            // labelMaxWorkingFeedUnit
            // 
            resources.ApplyResources(this.labelMaxWorkingFeedUnit, "labelMaxWorkingFeedUnit");
            this.labelMaxWorkingFeedUnit.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaxWorkingFeedUnit.Name = "labelMaxWorkingFeedUnit";
            // 
            // textBoxMaxWorkingFeed
            // 
            resources.ApplyResources(this.textBoxMaxWorkingFeed, "textBoxMaxWorkingFeed");
            this.textBoxMaxWorkingFeed.Name = "textBoxMaxWorkingFeed";
            // 
            // labelMaxWorkingFeed
            // 
            resources.ApplyResources(this.labelMaxWorkingFeed, "labelMaxWorkingFeed");
            this.labelMaxWorkingFeed.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelMaxWorkingFeed.Name = "labelMaxWorkingFeed";
            // 
            // labelPalletExchangeCode
            // 
            resources.ApplyResources(this.labelPalletExchangeCode, "labelPalletExchangeCode");
            this.labelPalletExchangeCode.Name = "labelPalletExchangeCode";
            // 
            // labelToolMeasurementCode
            // 
            resources.ApplyResources(this.labelToolMeasurementCode, "labelToolMeasurementCode");
            this.labelToolMeasurementCode.Name = "labelToolMeasurementCode";
            // 
            // comboBoxPalletExchangeCode
            // 
            this.comboBoxPalletExchangeCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxPalletExchangeCode.FormattingEnabled = true;
            resources.ApplyResources(this.comboBoxPalletExchangeCode, "comboBoxPalletExchangeCode");
            this.comboBoxPalletExchangeCode.Items.AddRange(new object[] {
            resources.GetString("comboBoxPalletExchangeCode.Items"),
            resources.GetString("comboBoxPalletExchangeCode.Items1"),
            resources.GetString("comboBoxPalletExchangeCode.Items2")});
            this.comboBoxPalletExchangeCode.Name = "comboBoxPalletExchangeCode";
            this.comboBoxPalletExchangeCode.SelectedIndexChanged += new System.EventHandler(this.comboBoxPalletExchangeCode_SelectedIndexChanged);
            // 
            // comboBoxToolMeasurementCode
            // 
            this.comboBoxToolMeasurementCode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxToolMeasurementCode.FormattingEnabled = true;
            resources.ApplyResources(this.comboBoxToolMeasurementCode, "comboBoxToolMeasurementCode");
            this.comboBoxToolMeasurementCode.Items.AddRange(new object[] {
            resources.GetString("comboBoxToolMeasurementCode.Items"),
            resources.GetString("comboBoxToolMeasurementCode.Items1"),
            resources.GetString("comboBoxToolMeasurementCode.Items2")});
            this.comboBoxToolMeasurementCode.Name = "comboBoxToolMeasurementCode";
            this.comboBoxToolMeasurementCode.SelectedIndexChanged += new System.EventHandler(this.comboBoxToolMeasurementCode_SelectedIndexChanged);
            // 
            // textBoxPalletExchangeCode
            // 
            resources.ApplyResources(this.textBoxPalletExchangeCode, "textBoxPalletExchangeCode");
            this.textBoxPalletExchangeCode.Name = "textBoxPalletExchangeCode";
            // 
            // textBoxToolMeasurementCode
            // 
            resources.ApplyResources(this.textBoxToolMeasurementCode, "textBoxToolMeasurementCode");
            this.textBoxToolMeasurementCode.Name = "textBoxToolMeasurementCode";
            // 
            // CNCKinematics
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.textBoxToolMeasurementCode);
            this.Controls.Add(this.textBoxPalletExchangeCode);
            this.Controls.Add(this.comboBoxToolMeasurementCode);
            this.Controls.Add(this.comboBoxPalletExchangeCode);
            this.Controls.Add(this.labelToolMeasurementCode);
            this.Controls.Add(this.labelPalletExchangeCode);
            this.Controls.Add(this.labelMaxWorkingFeedUnit);
            this.Controls.Add(this.textBoxMaxWorkingFeed);
            this.Controls.Add(this.labelMaxWorkingFeed);
            this.Controls.Add(this.labelMaxFastFeedUnit);
            this.Controls.Add(this.textBoxMaxFastFeed);
            this.Controls.Add(this.labelMaxFastFeed);
            this.Controls.Add(this.checkBoxAutoMeasureAfterToolSelection);
            this.Controls.Add(this.labelToolMeasurementTimeUnit);
            this.Controls.Add(this.labelPalletExchangeTimeUnit);
            this.Controls.Add(this.labelToolExchangeTimeUnit);
            this.Controls.Add(this.labelToolsQuantityUnit);
            this.Controls.Add(this.labelMaximumSpindleSpeedUnit);
            this.Controls.Add(this.textBoxToolMeasurementTime);
            this.Controls.Add(this.textBoxPalletExchangeTime);
            this.Controls.Add(this.textBoxToolExchangeTime);
            this.Controls.Add(this.textBoxToolsQuantity);
            this.Controls.Add(this.textBoxMaximumSpindleSpeed);
            this.Controls.Add(this.labelToolMeasurementTime);
            this.Controls.Add(this.labelPalletExchangeTime);
            this.Controls.Add(this.labelToolExchangeTime);
            this.Controls.Add(this.labelToolsQuantity);
            this.Controls.Add(this.labelMaximumSpindleSpeed);
            this.Controls.Add(this.dataGridView1);
            this.Controls.Add(this.labelHeader);
            this.Controls.Add(this.checkBoxDiameterProgramming);
            this.Name = "CNCKinematics";
            this.Load += new System.EventHandler(this.InitialParamsLoad);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelHeader;
        private System.Windows.Forms.DataGridView dataGridView1;
        private System.Windows.Forms.Label labelMaximumSpindleSpeed;
        private System.Windows.Forms.Label labelToolsQuantity;
        private System.Windows.Forms.Label labelToolExchangeTime;
        private System.Windows.Forms.Label labelPalletExchangeTime;
        private System.Windows.Forms.Label labelToolMeasurementTime;
        private System.Windows.Forms.TextBox textBoxMaximumSpindleSpeed;
        private System.Windows.Forms.TextBox textBoxToolsQuantity;
        private System.Windows.Forms.TextBox textBoxToolExchangeTime;
        private System.Windows.Forms.TextBox textBoxPalletExchangeTime;
        private System.Windows.Forms.TextBox textBoxToolMeasurementTime;
        private System.Windows.Forms.Label labelMaximumSpindleSpeedUnit;
        private System.Windows.Forms.Label labelToolsQuantityUnit;
        private System.Windows.Forms.Label labelToolExchangeTimeUnit;
        private System.Windows.Forms.Label labelPalletExchangeTimeUnit;
        private System.Windows.Forms.Label labelToolMeasurementTimeUnit;
        private System.Windows.Forms.CheckBox checkBoxDiameterProgramming;
        private System.Windows.Forms.CheckBox checkBoxAutoMeasureAfterToolSelection;
        private System.Windows.Forms.Label labelMaxFastFeedUnit;
        private System.Windows.Forms.TextBox textBoxMaxFastFeed;
        private System.Windows.Forms.Label labelMaxFastFeed;
        private System.Windows.Forms.Label labelMaxWorkingFeedUnit;
        private System.Windows.Forms.TextBox textBoxMaxWorkingFeed;
        private System.Windows.Forms.Label labelMaxWorkingFeed;
        private System.Windows.Forms.Label labelPalletExchangeCode;
        private System.Windows.Forms.Label labelToolMeasurementCode;
        private System.Windows.Forms.ComboBox comboBoxPalletExchangeCode;
        private System.Windows.Forms.ComboBox comboBoxToolMeasurementCode;
        private System.Windows.Forms.TextBox textBoxPalletExchangeCode;
        private System.Windows.Forms.TextBox textBoxToolMeasurementCode;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column1;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column2;
        private System.Windows.Forms.DataGridViewTextBoxColumn Column3;
    }
}
