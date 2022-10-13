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
            this.buttonProgramista = new System.Windows.Forms.Button();
            this.buttonObszarRoboczy = new System.Windows.Forms.Button();
            this.buttonParametry = new System.Windows.Forms.Button();
            this.buttonKinematyka = new System.Windows.Forms.Button();
            this.buttonObrabiarkaCNC = new System.Windows.Forms.Button();
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
            this.panel1.Controls.Add(this.buttonProgramista);
            this.panel1.Controls.Add(this.buttonObszarRoboczy);
            this.panel1.Controls.Add(this.buttonParametry);
            this.panel1.Controls.Add(this.buttonKinematyka);
            this.panel1.Controls.Add(this.buttonObrabiarkaCNC);
            resources.ApplyResources(this.panel1, "panel1");
            this.panel1.Name = "panel1";
            // 
            // SidePanel
            // 
            this.SidePanel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            resources.ApplyResources(this.SidePanel, "SidePanel");
            this.SidePanel.Name = "SidePanel";
            // 
            // buttonProgramista
            // 
            this.buttonProgramista.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonProgramista.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonProgramista, "buttonProgramista");
            this.buttonProgramista.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonProgramista.Name = "buttonProgramista";
            this.buttonProgramista.UseMnemonic = false;
            this.buttonProgramista.UseVisualStyleBackColor = false;
            this.buttonProgramista.Click += new System.EventHandler(this.buttonProgramista_Click);
            // 
            // buttonObszarRoboczy
            // 
            this.buttonObszarRoboczy.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonObszarRoboczy.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonObszarRoboczy, "buttonObszarRoboczy");
            this.buttonObszarRoboczy.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonObszarRoboczy.Name = "buttonObszarRoboczy";
            this.buttonObszarRoboczy.UseMnemonic = false;
            this.buttonObszarRoboczy.UseVisualStyleBackColor = false;
            this.buttonObszarRoboczy.Click += new System.EventHandler(this.buttonObszarRoboczy_Click);
            // 
            // buttonParametry
            // 
            this.buttonParametry.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonParametry.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonParametry, "buttonParametry");
            this.buttonParametry.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonParametry.Name = "buttonParametry";
            this.buttonParametry.UseMnemonic = false;
            this.buttonParametry.UseVisualStyleBackColor = false;
            this.buttonParametry.Click += new System.EventHandler(this.buttonParametry_Click);
            // 
            // buttonKinematyka
            // 
            this.buttonKinematyka.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonKinematyka.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonKinematyka, "buttonKinematyka");
            this.buttonKinematyka.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonKinematyka.Name = "buttonKinematyka";
            this.buttonKinematyka.UseMnemonic = false;
            this.buttonKinematyka.UseVisualStyleBackColor = false;
            this.buttonKinematyka.Click += new System.EventHandler(this.buttonKinematyka_Click);
            // 
            // buttonObrabiarkaCNC
            // 
            this.buttonObrabiarkaCNC.BackColor = System.Drawing.SystemColors.AppWorkspace;
            this.buttonObrabiarkaCNC.FlatAppearance.BorderSize = 0;
            resources.ApplyResources(this.buttonObrabiarkaCNC, "buttonObrabiarkaCNC");
            this.buttonObrabiarkaCNC.ForeColor = System.Drawing.Color.DarkSlateGray;
            this.buttonObrabiarkaCNC.Name = "buttonObrabiarkaCNC";
            this.buttonObrabiarkaCNC.UseMnemonic = false;
            this.buttonObrabiarkaCNC.UseVisualStyleBackColor = false;
            this.buttonObrabiarkaCNC.Click += new System.EventHandler(this.buttonObrabiarkaCNC_Click);
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
        private System.Windows.Forms.Button buttonProgramista;
        private System.Windows.Forms.Button buttonObszarRoboczy;
        private System.Windows.Forms.Button buttonParametry;
        private System.Windows.Forms.Button buttonObrabiarkaCNC;
        private System.Windows.Forms.Panel SidePanel;
        private System.Windows.Forms.Button buttonKinematyka;
        private System.Windows.Forms.Button buttonDefault;
        private System.Windows.Forms.Button buttonOK;
        private System.Windows.Forms.Button buttonExport;
        private System.Windows.Forms.Button buttonImport;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.SaveFileDialog saveFileDialog1;
    }
}