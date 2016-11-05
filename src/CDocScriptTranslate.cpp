#include "CDocI.h"

/*------------------------------------------------------------------------*/

static char cdoc_translate[256];

// Reset translations to their initial state i.e. all ASCII character codes
// map to themselves.
extern void
CDocScriptInitTranslations()
{
  for (int i = 0; i < 256; i++)
    cdoc_translate[i] = i;
}

// Parse Translation Definition String and update the Translation Table
// appropriately.
extern void
CDocScriptParseTranslation(const std::string &str)
{
  uint len = str.size();

  /* Skip Leading Blanks */

  uint i = 0;

  CStrUtil::skipSpace(str, &i);

  if (i >= len) {
    CDocScriptError("Invalid Translate Input Definition %s", str.c_str());
    return;
  }

  /****************/

  while (i < len) {
    /* Get From Character */

    int j = i;

    CStrUtil::skipNonSpace(str, &i);

    int len1 = i - j;

    int s;

    if      (len1 == 1)
      s = str[j];
    else if (len1 == 2 && isdigit(str[j]) && isdigit(str[j + 1]))
      s = (str[j] - '0')*10 + str[j + 1] - '0';
    else {
      CDocScriptError("Invalid Translate Input Definition %s", str.c_str());
      return;
    }

    CStrUtil::skipSpace(str, &i);

    /****************/

    /* Get To Character */

    j = i;

    CStrUtil::skipNonSpace(str, &i);

    len1 = i - j;

    int t;

    if      (len1 == 1)
      t = str[j];
    else if (len1 == 2 && isdigit(str[j]) && isdigit(str[j + 1]))
      t = (str[j] - '0')*10 + str[j + 1] - '0';
    else {
      CDocScriptError("Invalid Translate Input Definition %s", str.c_str());
      return;
    }

    CStrUtil::skipSpace(str, &i);

    /****************/

    cdoc_translate[s] = t;
  }
}

// Translate Line by replacing characters by their translated values.
extern void
CDocScriptTranslateLine(std::string &line)
{
  uint len = line.size();

  for (uint i = 0; i < len; ++i)
    line[i] = cdoc_translate[(int) line[i]];
}
