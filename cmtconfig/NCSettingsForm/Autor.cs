using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace cmtconfig
{
    public partial class Autor : UserControl
    {
        private readonly NCSettingsName ncSettingsName;
        private readonly NCSettings ncSettings;
        private NCSettingsFile ncSettingsFile;

        public Autor(NCSettingsDefaults ncSettingsDefaults, ref NCSettingsFile ncSettingsFile)
        {
            InitializeComponent();

            this.ncSettingsName = ncSettingsDefaults.ncSettingsDefinition.ncSettingsName;
            this.ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            this.ncSettingsFile = ncSettingsFile;
        }
        
        void AutorLoad(object sender, EventArgs e)
        {
            labelHeader.Text = ncSettingsName.author_name["header"];
            labelName.Text = ncSettingsName.author_name["name"];
            labelSurname.Text = ncSettingsName.author_name["surname"];
            labelPosition.Text = ncSettingsName.author_name["position"];
            labelDepartment.Text = ncSettingsName.author_name["department"];
            lablePhone1.Text = ncSettingsName.author_name["phone1"];
            labelPhone2.Text = ncSettingsName.author_name["phone2"];
            labelEmail.Text = ncSettingsName.author_name["email"];
            labelOther.Text = ncSettingsName.author_name["other"];
            labelDocumentHeader.Text = ncSettingsName.author_name["document_header"];
            labelDocumentFooter.Text = ncSettingsName.author_name["document_footer"];
            labelDocumentInfo.Text = ncSettingsName.author_name["document_info"];
            labelComment.Text = ncSettingsName.author_name["comment"];
            checkBoxWorkingTimeAtTheEnd.Text = ncSettingsName.author_name["working_time_at_end"];

            labelDocumentHeader.Hide();
            textBoxDocumentHeader.Hide();
            labelDocumentFooter.Hide();
            textBoxDocumentFooter.Hide();
            labelDocumentInfo.Hide();
            checkBoxWorkingTimeAtTheEnd.Hide();

            Reload(ncSettingsFile);
        }

        public void SetNCSettingsFile(NCSettingsFile ncSettingsFileLocal)
        {
            Reload(ncSettingsFileLocal);
        }

        public void Reload(NCSettingsFile ncSettingsFileLocal)
        {
            SetDefaults(String.IsNullOrEmpty(ncSettingsFileLocal.machine_tool) ? ncSettings.author : ncSettingsFileLocal.author);
        }

        private void SetDefaults(AuthorData authorData)
        {
            textBoxName.Text =              authorData.name;
            textBoxSurname.Text =           authorData.surname;
            textBoxPosition.Text =          authorData.position;
            textBoxDepartment.Text =        authorData.department;
            textBoxPhone1.Text =            authorData.phone1;
            textBoxPhone2.Text =            authorData.phone2;
            textBoxEmail.Text =             authorData.email;
            textBoxOther.Text =             authorData.other;
            textBoxDocumentHeader.Text =    authorData.document_header;
            textBoxDocumentFooter.Text =    authorData.document_footer;
            textBoxComment.Text =           authorData.comment;
            checkBoxWorkingTimeAtTheEnd.Checked = authorData.working_time_at_end;
        }

        public void MakeDefault()
        {
            SetDefaults(ncSettings.author);
        }

        public void ReadData(ref NCSettingsFile ncSettingsFileLocal)
        {
            var authorParam = ncSettingsFileLocal.author;

            authorParam.name = textBoxName.Text;
            authorParam.surname = textBoxSurname.Text;
            authorParam.position = textBoxPosition.Text;
            authorParam.department = textBoxDepartment.Text;
            authorParam.phone1 = textBoxPhone1.Text;
            authorParam.phone2 = textBoxPhone2.Text;
            authorParam.email = textBoxEmail.Text;
            authorParam.other = textBoxOther.Text;
            authorParam.document_header = textBoxDocumentHeader.Text;
            authorParam.document_footer = textBoxDocumentFooter.Text;
            authorParam.comment = textBoxComment.Text;
            authorParam.working_time_at_end = checkBoxWorkingTimeAtTheEnd.Checked;
        }
    }
}
