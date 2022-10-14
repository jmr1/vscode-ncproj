namespace cmtconfig
{
    partial class CNCInitialParams
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CNCInitialParams));
            this.labelHeader = new System.Windows.Forms.Label();
            this.labelDefaultMotion = new System.Windows.Forms.Label();
            this.labelDefaultWorkPlane = new System.Windows.Forms.Label();
            this.labelDefaultDriverUnits = new System.Windows.Forms.Label();
            this.labelDefaultProgramming = new System.Windows.Forms.Label();
            this.labelDefaultFeedMode = new System.Windows.Forms.Label();
            this.comboBoxDefaultMotion = new System.Windows.Forms.ComboBox();
            this.comboBoxDefaultWorkPlane = new System.Windows.Forms.ComboBox();
            this.comboBoxDefaultDriverUnits = new System.Windows.Forms.ComboBox();
            this.comboBoxDefaultProgramming = new System.Windows.Forms.ComboBox();
            this.comboBoxDefaultFeedMode = new System.Windows.Forms.ComboBox();
            this.textBoxDefaultRotation = new System.Windows.Forms.TextBox();
            this.labelDefaultRotation = new System.Windows.Forms.Label();
            this.labelDefaultRotationDirection = new System.Windows.Forms.Label();
            this.comboBoxDefaultRotationDirection = new System.Windows.Forms.ComboBox();
            this.labelDrillCycleZValue = new System.Windows.Forms.Label();
            this.comboBoxDrillCycleZValue = new System.Windows.Forms.ComboBox();
            this.labelDrillCycleReturnValue = new System.Windows.Forms.Label();
            this.comboBoxDrillCycleReturnValue = new System.Windows.Forms.ComboBox();
            this.checkBoxFastMotionCancelCycle = new System.Windows.Forms.CheckBox();
            this.checkBoxLinearInterpolationCancelCycle = new System.Windows.Forms.CheckBox();
            this.checkBoxCircularInterpolationCWCancelCycle = new System.Windows.Forms.CheckBox();
            this.checkBoxCircularInterpolationCCWCancelCycle = new System.Windows.Forms.CheckBox();
            this.checkBoxCycleCancelStartsFastMotion = new System.Windows.Forms.CheckBox();
            this.checkBoxOperatorTurnsOnRotation = new System.Windows.Forms.CheckBox();
            this.checkBoxToolNumberExecutesExchange = new System.Windows.Forms.CheckBox();
            this.checkBoxAutoFastMotionAfterToolExchange = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // labelHeader
            // 
            resources.ApplyResources(this.labelHeader, "labelHeader");
            this.labelHeader.Name = "labelHeader";
            // 
            // labelDefaultMotion
            // 
            resources.ApplyResources(this.labelDefaultMotion, "labelDefaultMotion");
            this.labelDefaultMotion.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultMotion.Name = "labelDefaultMotion";
            // 
            // labelDefaultWorkPlane
            // 
            resources.ApplyResources(this.labelDefaultWorkPlane, "labelDefaultWorkPlane");
            this.labelDefaultWorkPlane.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultWorkPlane.Name = "labelDefaultWorkPlane";
            // 
            // labelDefaultDriverUnits
            // 
            resources.ApplyResources(this.labelDefaultDriverUnits, "labelDefaultDriverUnits");
            this.labelDefaultDriverUnits.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultDriverUnits.Name = "labelDefaultDriverUnits";
            // 
            // labelDefaultProgramming
            // 
            resources.ApplyResources(this.labelDefaultProgramming, "labelDefaultProgramming");
            this.labelDefaultProgramming.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultProgramming.Name = "labelDefaultProgramming";
            // 
            // labelDefaultFeedMode
            // 
            resources.ApplyResources(this.labelDefaultFeedMode, "labelDefaultFeedMode");
            this.labelDefaultFeedMode.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultFeedMode.Name = "labelDefaultFeedMode";
            // 
            // comboBoxDefaultMotion
            // 
            this.comboBoxDefaultMotion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultMotion, "comboBoxDefaultMotion");
            this.comboBoxDefaultMotion.FormattingEnabled = true;
            this.comboBoxDefaultMotion.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultMotion.Items"),
            resources.GetString("comboBoxDefaultMotion.Items1")});
            this.comboBoxDefaultMotion.Name = "comboBoxDefaultMotion";
            this.comboBoxDefaultMotion.Tag = "";
            // 
            // comboBoxDefaultWorkPlane
            // 
            this.comboBoxDefaultWorkPlane.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultWorkPlane, "comboBoxDefaultWorkPlane");
            this.comboBoxDefaultWorkPlane.FormattingEnabled = true;
            this.comboBoxDefaultWorkPlane.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultWorkPlane.Items"),
            resources.GetString("comboBoxDefaultWorkPlane.Items1"),
            resources.GetString("comboBoxDefaultWorkPlane.Items2")});
            this.comboBoxDefaultWorkPlane.Name = "comboBoxDefaultWorkPlane";
            // 
            // comboBoxDefaultDriverUnits
            // 
            this.comboBoxDefaultDriverUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultDriverUnits, "comboBoxDefaultDriverUnits");
            this.comboBoxDefaultDriverUnits.FormattingEnabled = true;
            this.comboBoxDefaultDriverUnits.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultDriverUnits.Items"),
            resources.GetString("comboBoxDefaultDriverUnits.Items1")});
            this.comboBoxDefaultDriverUnits.Name = "comboBoxDefaultDriverUnits";
            // 
            // comboBoxDefaultProgramming
            // 
            this.comboBoxDefaultProgramming.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultProgramming, "comboBoxDefaultProgramming");
            this.comboBoxDefaultProgramming.FormattingEnabled = true;
            this.comboBoxDefaultProgramming.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultProgramming.Items"),
            resources.GetString("comboBoxDefaultProgramming.Items1")});
            this.comboBoxDefaultProgramming.Name = "comboBoxDefaultProgramming";
            // 
            // comboBoxDefaultFeedMode
            // 
            this.comboBoxDefaultFeedMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultFeedMode, "comboBoxDefaultFeedMode");
            this.comboBoxDefaultFeedMode.FormattingEnabled = true;
            this.comboBoxDefaultFeedMode.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultFeedMode.Items"),
            resources.GetString("comboBoxDefaultFeedMode.Items1")});
            this.comboBoxDefaultFeedMode.Name = "comboBoxDefaultFeedMode";
            // 
            // textBoxDefaultRotation
            // 
            resources.ApplyResources(this.textBoxDefaultRotation, "textBoxDefaultRotation");
            this.textBoxDefaultRotation.Name = "textBoxDefaultRotation";
            this.textBoxDefaultRotation.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBoxDefaultRotation_KeyPress);
            // 
            // labelDefaultRotation
            // 
            resources.ApplyResources(this.labelDefaultRotation, "labelDefaultRotation");
            this.labelDefaultRotation.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultRotation.Name = "labelDefaultRotation";
            // 
            // labelDefaultRotationDirection
            // 
            resources.ApplyResources(this.labelDefaultRotationDirection, "labelDefaultRotationDirection");
            this.labelDefaultRotationDirection.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDefaultRotationDirection.Name = "labelDefaultRotationDirection";
            // 
            // comboBoxDefaultRotationDirection
            // 
            this.comboBoxDefaultRotationDirection.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDefaultRotationDirection, "comboBoxDefaultRotationDirection");
            this.comboBoxDefaultRotationDirection.FormattingEnabled = true;
            this.comboBoxDefaultRotationDirection.Items.AddRange(new object[] {
            resources.GetString("comboBoxDefaultRotationDirection.Items"),
            resources.GetString("comboBoxDefaultRotationDirection.Items1")});
            this.comboBoxDefaultRotationDirection.Name = "comboBoxDefaultRotationDirection";
            // 
            // labelDrillCycleZValue
            // 
            resources.ApplyResources(this.labelDrillCycleZValue, "labelDrillCycleZValue");
            this.labelDrillCycleZValue.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDrillCycleZValue.Name = "labelDrillCycleZValue";
            // 
            // comboBoxDrillCycleZValue
            // 
            this.comboBoxDrillCycleZValue.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDrillCycleZValue, "comboBoxDrillCycleZValue");
            this.comboBoxDrillCycleZValue.FormattingEnabled = true;
            this.comboBoxDrillCycleZValue.Items.AddRange(new object[] {
            resources.GetString("comboBoxDrillCycleZValue.Items"),
            resources.GetString("comboBoxDrillCycleZValue.Items1")});
            this.comboBoxDrillCycleZValue.Name = "comboBoxDrillCycleZValue";
            // 
            // labelDrillCycleReturnValue
            // 
            resources.ApplyResources(this.labelDrillCycleReturnValue, "labelDrillCycleReturnValue");
            this.labelDrillCycleReturnValue.ForeColor = System.Drawing.SystemColors.ControlText;
            this.labelDrillCycleReturnValue.Name = "labelDrillCycleReturnValue";
            // 
            // comboBoxDrillCycleReturnValue
            // 
            this.comboBoxDrillCycleReturnValue.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            resources.ApplyResources(this.comboBoxDrillCycleReturnValue, "comboBoxDrillCycleReturnValue");
            this.comboBoxDrillCycleReturnValue.FormattingEnabled = true;
            this.comboBoxDrillCycleReturnValue.Items.AddRange(new object[] {
            resources.GetString("comboBoxDrillCycleReturnValue.Items"),
            resources.GetString("comboBoxDrillCycleReturnValue.Items1")});
            this.comboBoxDrillCycleReturnValue.Name = "comboBoxDrillCycleReturnValue";
            // 
            // checkBoxFastMotionCancelCycle
            // 
            resources.ApplyResources(this.checkBoxFastMotionCancelCycle, "checkBoxFastMotionCancelCycle");
            this.checkBoxFastMotionCancelCycle.Name = "checkBoxFastMotionCancelCycle";
            this.checkBoxFastMotionCancelCycle.UseVisualStyleBackColor = true;
            // 
            // checkBoxLinearInterpolationCancelCycle
            // 
            resources.ApplyResources(this.checkBoxLinearInterpolationCancelCycle, "checkBoxLinearInterpolationCancelCycle");
            this.checkBoxLinearInterpolationCancelCycle.Name = "checkBoxLinearInterpolationCancelCycle";
            this.checkBoxLinearInterpolationCancelCycle.UseVisualStyleBackColor = true;
            // 
            // checkBoxCircularInterpolationCWCancelCycle
            // 
            resources.ApplyResources(this.checkBoxCircularInterpolationCWCancelCycle, "checkBoxCircularInterpolationCWCancelCycle");
            this.checkBoxCircularInterpolationCWCancelCycle.Name = "checkBoxCircularInterpolationCWCancelCycle";
            this.checkBoxCircularInterpolationCWCancelCycle.UseVisualStyleBackColor = true;
            // 
            // checkBoxCircularInterpolationCCWCancelCycle
            // 
            resources.ApplyResources(this.checkBoxCircularInterpolationCCWCancelCycle, "checkBoxCircularInterpolationCCWCancelCycle");
            this.checkBoxCircularInterpolationCCWCancelCycle.Name = "checkBoxCircularInterpolationCCWCancelCycle";
            this.checkBoxCircularInterpolationCCWCancelCycle.UseVisualStyleBackColor = true;
            // 
            // checkBoxCycleCancelStartsFastMotion
            // 
            resources.ApplyResources(this.checkBoxCycleCancelStartsFastMotion, "checkBoxCycleCancelStartsFastMotion");
            this.checkBoxCycleCancelStartsFastMotion.Name = "checkBoxCycleCancelStartsFastMotion";
            this.checkBoxCycleCancelStartsFastMotion.UseVisualStyleBackColor = true;
            // 
            // checkBoxOperatorTurnsOnRotation
            // 
            resources.ApplyResources(this.checkBoxOperatorTurnsOnRotation, "checkBoxOperatorTurnsOnRotation");
            this.checkBoxOperatorTurnsOnRotation.Name = "checkBoxOperatorTurnsOnRotation";
            this.checkBoxOperatorTurnsOnRotation.UseVisualStyleBackColor = true;
            this.checkBoxOperatorTurnsOnRotation.CheckedChanged += new System.EventHandler(this.checkBoxOperatorTurnsOnRotation_CheckedChanged);
            // 
            // checkBoxToolNumberExecutesExchange
            // 
            resources.ApplyResources(this.checkBoxToolNumberExecutesExchange, "checkBoxToolNumberExecutesExchange");
            this.checkBoxToolNumberExecutesExchange.Name = "checkBoxToolNumberExecutesExchange";
            this.checkBoxToolNumberExecutesExchange.UseVisualStyleBackColor = true;
            // 
            // checkBoxAutoFastMotionAfterToolExchange
            // 
            resources.ApplyResources(this.checkBoxAutoFastMotionAfterToolExchange, "checkBoxAutoFastMotionAfterToolExchange");
            this.checkBoxAutoFastMotionAfterToolExchange.Name = "checkBoxAutoFastMotionAfterToolExchange";
            this.checkBoxAutoFastMotionAfterToolExchange.UseVisualStyleBackColor = true;
            // 
            // CNCInitialParams
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.checkBoxAutoFastMotionAfterToolExchange);
            this.Controls.Add(this.checkBoxToolNumberExecutesExchange);
            this.Controls.Add(this.checkBoxOperatorTurnsOnRotation);
            this.Controls.Add(this.checkBoxCycleCancelStartsFastMotion);
            this.Controls.Add(this.checkBoxCircularInterpolationCCWCancelCycle);
            this.Controls.Add(this.checkBoxCircularInterpolationCWCancelCycle);
            this.Controls.Add(this.checkBoxLinearInterpolationCancelCycle);
            this.Controls.Add(this.checkBoxFastMotionCancelCycle);
            this.Controls.Add(this.comboBoxDrillCycleReturnValue);
            this.Controls.Add(this.labelDrillCycleReturnValue);
            this.Controls.Add(this.comboBoxDrillCycleZValue);
            this.Controls.Add(this.labelDrillCycleZValue);
            this.Controls.Add(this.comboBoxDefaultRotationDirection);
            this.Controls.Add(this.labelDefaultRotationDirection);
            this.Controls.Add(this.labelDefaultRotation);
            this.Controls.Add(this.textBoxDefaultRotation);
            this.Controls.Add(this.comboBoxDefaultFeedMode);
            this.Controls.Add(this.comboBoxDefaultProgramming);
            this.Controls.Add(this.comboBoxDefaultDriverUnits);
            this.Controls.Add(this.comboBoxDefaultWorkPlane);
            this.Controls.Add(this.comboBoxDefaultMotion);
            this.Controls.Add(this.labelDefaultFeedMode);
            this.Controls.Add(this.labelDefaultProgramming);
            this.Controls.Add(this.labelDefaultDriverUnits);
            this.Controls.Add(this.labelDefaultWorkPlane);
            this.Controls.Add(this.labelDefaultMotion);
            this.Controls.Add(this.labelHeader);
            this.Name = "CNCInitialParams";
            this.Load += new System.EventHandler(this.InitialParamsLoad);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelHeader;
        private System.Windows.Forms.Label labelDefaultMotion;
        private System.Windows.Forms.Label labelDefaultWorkPlane;
        private System.Windows.Forms.Label labelDefaultDriverUnits;
        private System.Windows.Forms.Label labelDefaultProgramming;
        private System.Windows.Forms.Label labelDefaultFeedMode;
        private System.Windows.Forms.ComboBox comboBoxDefaultMotion;
        private System.Windows.Forms.ComboBox comboBoxDefaultWorkPlane;
        private System.Windows.Forms.ComboBox comboBoxDefaultDriverUnits;
        private System.Windows.Forms.ComboBox comboBoxDefaultProgramming;
        private System.Windows.Forms.ComboBox comboBoxDefaultFeedMode;
        private System.Windows.Forms.TextBox textBoxDefaultRotation;
        private System.Windows.Forms.Label labelDefaultRotation;
        private System.Windows.Forms.Label labelDefaultRotationDirection;
        private System.Windows.Forms.ComboBox comboBoxDefaultRotationDirection;
        private System.Windows.Forms.Label labelDrillCycleZValue;
        private System.Windows.Forms.ComboBox comboBoxDrillCycleZValue;
        private System.Windows.Forms.Label labelDrillCycleReturnValue;
        private System.Windows.Forms.ComboBox comboBoxDrillCycleReturnValue;
        private System.Windows.Forms.CheckBox checkBoxFastMotionCancelCycle;
        private System.Windows.Forms.CheckBox checkBoxLinearInterpolationCancelCycle;
        private System.Windows.Forms.CheckBox checkBoxCircularInterpolationCWCancelCycle;
        private System.Windows.Forms.CheckBox checkBoxCircularInterpolationCCWCancelCycle;
        private System.Windows.Forms.CheckBox checkBoxCycleCancelStartsFastMotion;
        private System.Windows.Forms.CheckBox checkBoxOperatorTurnsOnRotation;
        private System.Windows.Forms.CheckBox checkBoxToolNumberExecutesExchange;
        private System.Windows.Forms.CheckBox checkBoxAutoFastMotionAfterToolExchange;
    }
}
