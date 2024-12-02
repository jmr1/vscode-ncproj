using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace parser
{
    using CodeGroupsMap = Dictionary<string, List<CodeGroupValue>>;

    public class FanucAllAttributesParser : AllAttributesParser
    {
        public FanucAllAttributesParser(FanucGrammar fanuc_grammar,
                                        List<string> operations,
                                        CodeGroupsMap gCodeGroups,
                                        CodeGroupsMap mCodeGroups,
                                        ref ParserSettings parserSettings,
                                        ref OtherSettings otherSettings,
                                        FanucParserType fanuc_parser_type,
                                        bool instantiateWithoutNCSettings = false)
        {
            int cnt = 0;
            var gcode_values_length = new int[gCodeGroups.Keys.Count()];
            var gcode_values = new List<CodeGroupValueWrapper>();
            foreach (var item in gCodeGroups.Values.ToArray())
            {
                gcode_values.AddRange(item.Select(s => (CodeGroupValueWrapper)s).ToList());
                gcode_values_length[cnt] = item.Count();
                ++cnt;
            }

            cnt = 0;
            var mcode_values_length = new int[mCodeGroups.Keys.Count()];
            var mcode_values = new List<CodeGroupValueWrapper>();
            foreach (var item in mCodeGroups.Values.ToArray())
            {
                mcode_values.AddRange(item.Select(s => (CodeGroupValueWrapper)s).ToList());
                mcode_values_length[cnt] = item.Count();
                ++cnt;
            }

            var parserSettingsWrapper = (ParserSettingsWrapper)parserSettings;
            var otherSettingsWrapper = (OtherSettingsWrapper)otherSettings;

            pAllAttributesParser = 
                CreateFanucAllAttributesParser(fanuc_grammar.metric.Keys.ToArray(),
                                               fanuc_grammar.metric.Values.ToArray(),
                                               fanuc_grammar.metric.Count(),
                                               fanuc_grammar.imperial.Keys.ToArray(),
                                               fanuc_grammar.imperial.Values.ToArray(),
                                               fanuc_grammar.imperial.Count(),
                                               operations.ToArray(),
                                               operations.Count(),
                                               gCodeGroups.Keys.ToArray(),
                                               gcode_values.ToArray(),
                                               gCodeGroups.Count(),
                                               gcode_values_length,
                                               mCodeGroups.Keys.ToArray(),
                                               mcode_values.ToArray(),
                                               mCodeGroups.Count(),
                                               mcode_values_length,
                                               ref parserSettingsWrapper,
                                               ref otherSettingsWrapper,
                                               fanuc_parser_type,
                                               instantiateWithoutNCSettings);
        }
    }
}
