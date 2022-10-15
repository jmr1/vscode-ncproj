using System;
using System.Linq;
using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;

namespace parser
{
    public enum BlockAccessType
    {
        ReadOnly,
        ReadWrite
    }

    public class Block
    {
        public int id { get; set; }
        public string name { get; set; }
        public List<string> code { get; set; }
        public List<string> description { get; set; }
    }

    public class Group
    {
        public string version { get; set; }
        public int id { get; set; }
        public string name { get; set; }
        public List<Block> blocks { get; set; }
        [JsonIgnore]
        public BlockAccessType blockAccessType; // not in json
    }

    public static class PredefinedBlocks
    {
        public static Dictionary<string, Group> Read(string path, string dot, BlockAccessType blockAccessType)
        {
            var groups = new Dictionary<string, Group>();
            try
            {
                var dirInfo = new DirectoryInfo(path);
                if (dirInfo.Exists) // will crash for empty directory without this check
                {
                    foreach (var file in dirInfo.GetFiles("*.json"))
                    {
                        var contents = File.ReadAllText(file.FullName);
                        var block = JsonConvert.DeserializeObject<Group>(contents);
                        block.blockAccessType = blockAccessType;
                        groups.Add(file.FullName, block);
                    }

                    foreach (var group in groups)
                        foreach (var block in group.Value.blocks)
                            for (var x = 0; x < block.code.Count; ++x)
                                block.code[x] = string.Format(block.code[x], dot);
                }
            }
            catch (Exception e)
            {
                throw;
            }

            return groups;
        }

        private static IEnumerable<string> GetFiles(string Path)
        {
            return Directory.EnumerateFiles(Path, "*.json", SearchOption.TopDirectoryOnly)
                     .Where(f => f.EndsWith(".json", StringComparison.OrdinalIgnoreCase));
        }

        public static void Write(Dictionary<string, Group> groups, string user_path)
        {
            foreach (var item in GetFiles(user_path))
            {
                if(!groups.ContainsKey(item))
                    File.Delete(item);
            }

            foreach (var item in groups)
            {
                if(item.Value.blockAccessType == BlockAccessType.ReadOnly)
                    continue;
                var json = JsonConvert.SerializeObject(item.Value, Formatting.Indented);
                File.WriteAllText(item.Key, json);
            }
        }
    }
}
