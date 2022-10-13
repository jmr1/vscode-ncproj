using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;

namespace parser
{
    using CodeGroupsMap = Dictionary<string, List<CodeGroupValue>>;
    using CodeDescriptionMap = Dictionary<float, string>;

    public class CodeGroupValue
    {
        public uint code;
        public uint rest;

        public override string ToString()
        {
            return rest == 0 ? code.ToString() : string.Format("{0}.{1}", code, rest);
        }

        public static explicit operator CodeGroupValue(string value)
        {
            var dotIndex = value.IndexOf('.');
            uint code = uint.Parse(value.Substring(0, dotIndex != -1 ? dotIndex : value.Length));
            uint rest = 0;
            if (dotIndex != -1)
                rest = uint.Parse(value.Substring(dotIndex + 1));
            return new CodeGroupValue() { code = code, rest = rest };
        }

    };

    public class CodeGroupsCollection
    {
        public string version { get; set; }
        public CodeGroupsMap groups;
    }

    public class CodeDescriptionsCollection
    {
        public string version { get; set; }
        public CodeDescriptionMap descriptions;
    }

    public class CodeGroups
    {
        private CodeGroupsCollection codeGroupsCollection;
        private readonly CodeDescriptionsCollection codeDescriptionsCollection;

        public CodeGroups(string code_path, string code_desc_path)
        {
            string contents = File.ReadAllText(code_path);
            codeGroupsCollection = JsonConvert.DeserializeObject<CodeGroupsCollection>(contents);

            contents = File.ReadAllText(code_desc_path);
            codeDescriptionsCollection = JsonConvert.DeserializeObject<CodeDescriptionsCollection>(contents);
        }

        public CodeGroupsMap CodeGroupsMap
        {
            get { return codeGroupsCollection.groups; }
        }

        public CodeDescriptionMap CodeDescriptionMap
        {
            get { return codeDescriptionsCollection.descriptions; }
        }
    }
}
