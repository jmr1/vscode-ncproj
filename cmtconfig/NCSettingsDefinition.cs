using System;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System.Collections.Generic;
using System.IO;
using parser;
using Newtonsoft.Json.Linq;

namespace cmtconfig
{
    public class AuthorData
    {
        public string name { get; set; }
        public string surname { get; set; }
        public string position { get; set; }
        public string department { get; set; }
        public string phone1 { get; set; }
        public string phone2 { get; set; }
        public string email { get; set; }
        public string other { get; set; }
        public string document_header { get; set; }
        public string document_footer { get; set; }
        public string comment { get; set; }
        public bool working_time_at_end { get; set; }
    }

    public class MachineToolData
    {
        public Dictionary<string, bool> extras { get; set; }
        public List<string> axes { get; set; }
        [JsonConverter(typeof(StringEnumConverter))]
        public MachineToolType type { get; set; }
        public Kinematics kinematics { get; set; }
        public CncDefaultValues cnc_default_values { get; set; }
        public MachinePointsData machine_points_data { get; set; }
    }

    public enum Driver
    {
        fanuc_mill,
        fanuc_lathe_system_a,
        fanuc_lathe_system_b,
        fanuc_lathe_system_c,
        fanuc_millturn_system_a,
        fanuc_millturn_system_b,
        generic_mill,
        generic_lathe,
        haas_mill,
        haas_lathe,
        heidenhain,
        makino_mill
    }

    public class ListDriverConverter : JsonConverter
    {
        public override bool CanConvert(Type objectType)
        {
            return typeof(List<Driver>) == objectType;
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var jToken = JToken.Load(reader);

            if (existingValue == null)
                existingValue = new List<Driver>();

            var list = existingValue as List<Driver>;
            foreach (var item in jToken)
                list.Add((Driver)Enum.Parse(typeof(Driver), item.ToString()));

            return existingValue;
        }

        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            var list = (List<Driver>)value;

            writer.WriteStartObject();

            foreach (var item in list)
                writer.WriteValue(item.ToString());

            writer.WriteEndObject();
        }
    }

    public class DictionaryDriverConverter : JsonConverter
    {
        public override void WriteJson(JsonWriter writer, object value, JsonSerializer serializer)
        {
            var dictionary = (Dictionary<Driver, string>)value;

            writer.WriteStartObject();

            foreach (KeyValuePair<Driver, string> pair in dictionary)
            {
                writer.WritePropertyName(pair.Key.ToString());
                writer.WriteValue(pair.Value);
            }

            writer.WriteEndObject();
        }

        public override object ReadJson(JsonReader reader, Type objectType, object existingValue, JsonSerializer serializer)
        {
            var jObject = JObject.Load(reader);

            if (existingValue == null)
                existingValue = new Dictionary<Driver, string>();

            var dict = existingValue as Dictionary<Driver, string>;
            foreach (var item in jObject)
                dict.Add((Driver)Enum.Parse(typeof(Driver), item.Key), item.Value.ToString());

            return existingValue;
        }

        public override bool CanConvert(Type objectType)
        {
            return typeof(IDictionary<Driver, string>) == objectType;
        }
    }

    public class NCSettings
    {
        public string version;
        [JsonConverter(typeof(StringEnumConverter))]
        public Driver driver_default;
        [JsonConverter(typeof(ListDriverConverter))]
        public List<Driver> driver_list;
        public string machine_tool_default;
        public string machine_tool_id;
        public Dictionary<string, List<string>> cnc_default_values_list { get; set; }
        public Dictionary<string, List<string>> kinematics_list { get; set; }
        public Dictionary<string, MachineToolData> machine_tool { get; set; }
        public AuthorData author;
    }

    public class NCSettingsName
    {
        public string version;
        [JsonConverter(typeof(DictionaryDriverConverter))]
        public Dictionary<Driver, string> driver_name { get; set; }
        public Dictionary<string, string> machine_tool_name { get; set; }
        public Dictionary<string, string> extras_name { get; set; }
        public Dictionary<string, string> kinematics_name { get; set; }
        public Dictionary<string, string> cnc_default_values_name { get; set; }
        public Dictionary<string, Dictionary<string, string>> cnc_default_values_list_name { get; set; }
        public Dictionary<string, string> machine_points_name { get; set; }
        public Dictionary<string, string> author_name { get; set; }
    }

    public class NCSettingsDefinition
    {
        public NCSettingsName ncSettingsName;
        public NCSettings ncSettings;

        public NCSettingsDefinition(string pathMachineToolsNames, string pathMachineTools)
        {
            try
            {
                string contents = File.ReadAllText(pathMachineToolsNames);
                ncSettingsName = JsonConvert.DeserializeObject<NCSettingsName>(contents);
    
                contents = File.ReadAllText(pathMachineTools);
                ncSettings = JsonConvert.DeserializeObject<NCSettings>(contents);
            }
            catch(JsonSerializationException e)
            {
#pragma warning disable CS4014
                //Program.SendTelemetry("exception", e.ToString());
#pragma warning restore CS4014
                //Program.log.Error(e.ToString());
            }
            catch(Exception e)
            {
#pragma warning disable CS4014
                //Program.SendTelemetry("exception", e.ToString());
#pragma warning restore CS4014
                //Program.log.Error(e.ToString());
            }
        }
    }

    public static class ObjectCopy
    {
        public static T Clone<T>(T source)
        {
            var serialized = JsonConvert.SerializeObject(source);
            return JsonConvert.DeserializeObject<T>(serialized);
        }
    }

    public class NCSettingsDefaults
    {
        public NCSettingsDefinition ncSettingsDefinition;

        public static explicit operator NCSettingsFile(NCSettingsDefaults ncSettingsDefaults)
        {
            var ncSettings = ncSettingsDefaults.ncSettingsDefinition.ncSettings;
            var machine_tool = ncSettings.machine_tool;

            var ncSettingsFile =                    new NCSettingsFile();
            ncSettingsFile.version =                ncSettings.version;
            ncSettingsFile.driver =                 ncSettings.driver_default;
            ncSettingsFile.machine_tool =           ObjectCopy.Clone(ncSettings.machine_tool_default);
            ncSettingsFile.machine_tool_type =      machine_tool[ncSettingsFile.machine_tool].type;
            ncSettingsFile.extras =                 ObjectCopy.Clone(machine_tool[ncSettingsFile.machine_tool].extras);
            ncSettingsFile.kinematics =             ObjectCopy.Clone(machine_tool[ncSettingsFile.machine_tool].kinematics);
            ncSettingsFile.cnc_default_values =     ObjectCopy.Clone(machine_tool[ncSettingsFile.machine_tool].cnc_default_values);
            ncSettingsFile.machine_points_data =    ObjectCopy.Clone(machine_tool[ncSettingsFile.machine_tool].machine_points_data);
            ncSettingsFile.author =                 ObjectCopy.Clone(ncSettings.author);
            ncSettingsFile.zero_point = new Dictionary<string, double>()
            {
                { "X", 0 },
                { "Y", 0 },
                { "Z", 0 }
            };

            return ncSettingsFile;
        }
    }

    public class NCSettingsFile
    {
        public string version;
        [JsonConverter(typeof(StringEnumConverter))]
        public Driver driver;
        public string machine_tool;
        public string machine_tool_id;
        [JsonConverter(typeof(StringEnumConverter))]
        public MachineToolType machine_tool_type;
        public Dictionary<string, bool> extras;
        public Kinematics kinematics { get; set; }
        public CncDefaultValues cnc_default_values { get; set; }
        public MachinePointsData machine_points_data;
        public AuthorData author { get; set; }
        public Dictionary<string, double> zero_point { get; set; }

        public void Write(string file_path)
        {
            version = "1.0";
            var json = JsonConvert.SerializeObject(this, Formatting.Indented);
            File.WriteAllText(file_path, json);
        }

        public static NCSettingsFile Read(string file_path)
        {
            var json = File.ReadAllText(file_path);
            return JsonConvert.DeserializeObject<NCSettingsFile>(json);
        }
    }
}
