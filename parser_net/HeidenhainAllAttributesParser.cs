using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace parser
{
    public class HeidenhainAllAttributesParser : AllAttributesParser
    {
        public HeidenhainAllAttributesParser(ref ParserSettings parserSettings, ref OtherSettings otherSettings)
        {
            var parserSettingsWrapper = (ParserSettingsWrapper)parserSettings;
            var otherSettingsWrapper = (OtherSettingsWrapper)otherSettings;

            pAllAttributesParser = 
                CreateHeidenhainAllAttributesParser(ref parserSettingsWrapper, ref otherSettingsWrapper);
        }
    }
}
