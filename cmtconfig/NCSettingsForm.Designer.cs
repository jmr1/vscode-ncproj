namespace cmtconfig
{
    partial class NCSettingsForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(NCSettingsForm));
            this.panel1 = new System.Windows.Forms.Panel();
            this.SidePanel = new System.Windows.Forms.Panel();
            this.buttonProgramZero = new System.Windows.Forms.Button();
            this.buttonMachinePoints = new System.Windows.Forms.Button();
            this.buttonInitialParams = new System.Windows.Forms.Button();
            this.buttonKinematics = new System.Windows.Forms.Button();
            this.buttonMachineTools = new System.Windows.Forms.Button();
            this.buttonDefault = new System.Windows.Forms.Button();
            this.buttonOK = new System.Windows.Forms.Button();
            this.buttonExport = new System.Windows.Forms.Button();
            this.buttonImport = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.saveFileDialog1 = new System.Windows.Forms.SaveFileDialog();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.panel1.Controls.Add(this.SidePanel);
            this.panel1.Controls.Add(this.buttonProgramZero);
            this.panel1.Controls.Add(this.buttonMachinePoints);
            this.panel1.Controls.Add(this.buttonInitialParams);
            this.panel1.Controls.Add(this.buttonKinematics);
            this.panel1.Controls.Add(this.buttonMachineTools);
            resources.ApplyResources(this.panel1, "panel1");
            this.panel1.Name = "panel1";
            // 
            // SidePanel
            // 
            this.SidePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            resources.ApplyResources(this.SidePanel, "SidePanel");
            this.SidePanel.Name = "SidePanel";
            // 
            // buttonProgramZero
            // 
            this.buttonProgramZero.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonProgramZero.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonProgramZero, "buttonProgramZero");
            this.buttonProgramZero.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonProgramZero.Name = "buttonProgramZero";
            this.buttonProgramZero.UseMnemonic = false;
            this.buttonProgramZero.UseVisualStyleBackColor = false;
            this.buttonProgramZero.Click += new System.EventHandler(this.buttonProgramZero_Click);
            // 
            // buttonMachinePoints
            // 
            this.buttonMachinePoints.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonMachinePoints.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonMachinePoints, "buttonMachinePoints");
            this.buttonMachinePoints.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonMachinePoints.Name = "buttonMachinePoints";
            this.buttonMachinePoints.UseMnemonic = false;
            this.buttonMachinePoints.UseVisualStyleBackColor = false;
            this.buttonMachinePoints.Click += new System.EventHandler(this.buttonMachinePoints_Click);
            // 
            // buttonInitialParams
            // 
            this.buttonInitialParams.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonInitialParams.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonInitialParams, "buttonInitialParams");
            this.buttonInitialParams.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonInitialParams.Name = "buttonInitialParams";
            this.buttonInitialParams.UseMnemonic = false;
            this.buttonInitialParams.UseVisualStyleBackColor = false;
            this.buttonInitialParams.Click += new System.EventHandler(this.buttonInitialParams_Click);
            // 
            // buttonKinematics
            // 
            this.buttonKinematics.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonKinematics.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonKinematics, "buttonKinematics");
            this.buttonKinematics.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonKinematics.Name = "buttonKinematics";
            this.buttonKinematics.UseMnemonic = false;
            this.buttonKinematics.UseVisualStyleBackColor = false;
            this.buttonKinematics.Click += new System.EventHandler(this.buttonKinematics_Click);
            // 
            // buttonMachineTools
            // 
            this.buttonMachineTools.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonMachineTools.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonMachineTools, "buttonMachineTools");
            this.buttonMachineTools.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonMachineTools.Name = "buttonMachineTools";
            this.buttonMachineTools.UseMnemonic = false;
            this.buttonMachineTools.UseVisualStyleBackColor = false;
            this.buttonMachineTools.Click += new System.EventHandler(this.buttonMachineTools_Click);
            // 
            // buttonDefault
            // 
            resources.ApplyResources(this.buttonDefault, "buttonDefault");
            this.buttonDefault.Name = "buttonDefault";
            this.buttonDefault.UseVisualStyleBackColor = true;
            this.buttonDefault.Click += new System.EventHandler(this.buttonDefault_Click);
            // 
            // buttonOK
            // 
            this.buttonOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            resources.ApplyResources(this.buttonOK, "buttonOK");
            this.buttonOK.Name = "buttonOK";
            this.buttonOK.UseVisualStyleBackColor = true;
            this.buttonOK.Click += new System.EventHandler(this.buttonOK_Click);
            // 
            // buttonExport
            // 
            resources.ApplyResources(this.buttonExport, "buttonExport");
            this.buttonExport.Name = "buttonExport";
            this.buttonExport.UseVisualStyleBackColor = true;
            this.buttonExport.Click += new System.EventHandler(this.buttonExport_Click);
            // 
            // buttonImport
            // 
            resources.ApplyResources(this.buttonImport, "buttonImport");
            this.buttonImport.Name = "buttonImport";
            this.buttonImport.UseVisualStyleBackColor = true;
            this.buttonImport.Click += new System.EventHandler(this.buttonImport_Click);
            // 
            // openFileDialog1
            // 
            resources.ApplyResources(this.openFileDialog1, "openFileDialog1");
            // 
            // saveFileDialog1
            // 
            resources.ApplyResources(this.saveFileDialog1, "saveFileDialog1");
            // 
            // NCSettingsForm
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Window;
            this.Controls.Add(this.buttonImport);
            this.Controls.Add(this.buttonExport);
            this.Controls.Add(this.buttonOK);
            this.Controls.Add(this.buttonDefault);
            this.Controls.Add(this.panel1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.Name = "NCSettingsForm";
            this.Load += new System.EventHandler(this.NCSettingsForm_Load);
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button buttonProgramZero;
        private System.Windows.Forms.Button buttonMachinePoints;
        private System.Windows.Forms.Button buttonInitialParams;
        private System.Windows.Forms.Button buttonMachineTools;
        private System.Windows.Forms.Panel SidePanel;
        private System.Windows.Forms.Button buttonKinematics;
        private System.Windows.Forms.Button buttonDefault;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Button buttonExport;
        private System.Windows.Forms.Button buttonImport;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    }
}