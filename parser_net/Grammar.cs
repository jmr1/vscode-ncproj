using Newtonsoft.Json;
using System.Collections.Generic;
using System.IO;

namespace parser
{
    using WordGrammarMap = Dictionary<string, WordGrammarWrapper>;

    public class FanucGrammar
    {
        public string version { get; set; }
        public List<string> operations { get; set; }
        public WordGrammarMap imperial { get; set; }
        public WordGrammarMap metric { get; set; }
    }

    public class Grammar
    {
        private string grammar_path;
        private readonly FanucGrammar fanuc_grammar = new FanucGrammar();

        public Grammar(string path)
        {
            grammar_path = path;
            string contents = File.ReadAllText(grammar_path);
            fanuc_grammar = JsonConvert.DeserializeObject<FanucGrammar>(contents);
        }

        public FanucGrammar GetFanucGrammar
        {
            get { return fanuc_grammar; }
        }
    }
}
