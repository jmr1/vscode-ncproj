using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;
using parser;

namespace cmtconfig
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            const Language lang = Language.English;

            const string nc_settings_name_folder = "./conf/";
            const string nc_settings_path = "./conf/nc_settings.json";

            NCSettingsDefaults ncSettingsDefaults = new NCSettingsDefaults();

            string nc_settings_name_path = nc_settings_name_folder;
            switch (lang)
            {
                case Language.English:
                    nc_settings_name_path += "nc_settings_name_en.json";
                    break;
                case Language.Polish:
                    nc_settings_name_path += "nc_settings_name_pl.json";
                    break;
                default:
                    nc_settings_name_path += "nc_settings_name_en.json";
                    break;
            }
            ncSettingsDefaults.ncSettingsDefinition = new NCSettingsDefinition(nc_settings_name_path, nc_settings_path);

            var ncSettingsFile = (NCSettingsFile)ncSettingsDefaults;

            var ncSettingsForm =
                new NCSettingsForm(ncSettingsDefaults, ref ncSettingsFile, lang);

            Application.Run(ncSettingsForm);
        }
    }
}
