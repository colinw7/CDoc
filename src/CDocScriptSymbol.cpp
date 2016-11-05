#include "CDocI.h"

/*------------------------------------------------------------------------*/

typedef std::map<std::string,CDSymbol *> CDSymbolList;

static CDSymbolList symbol_list;

/*------------------------------------------------------------------------*/

static std::string CDocScriptGetGlobalSymbolText(const std::string &, int);
static void        CDocScriptAddSymbol          (const std::string &, const std::string &);

// Parse Symbol Definition String and add resultant Symbol definition to the Symbol List.
extern void
CDocScriptParseSymbol(const std::string &str)
{
  uint len = str.size();

  /* Skip Leading Blanks */

  uint i = 0;

  CStrUtil::skipSpace(str, &i);

  /****************/

  /* Get Symbol Name */

  uint j = i;

  while (i < len && isalnum(str[i]))
    i++;

  if (i == j) {
    CDocScriptError("Invalid Symbol Definition %s", str.c_str());
    return;
  }

  std::string symbol = str.substr(j, i - j);

  CStrUtil::skipSpace(str, &i);

  /****************/

  /* Check for and Skip Equals Sign */

  int equals = false;

  if (i < len && str[i] == '=') {
    equals = true;

    i++;

    CStrUtil::skipSpace(str, &i);
  }

  /****************/

  /* Get Symbol Text */

  std::string text;

  if (i < len && str[i] == '\'') {
    i++;

    j = 0;

    while (i < len) {
      if (equals && str[i + 0] == '\'') {
        if (i + 1 < len && str[i + 1] != '\'')
          break;

        i++;
      }

      text += str[i++];
    }

    if (! equals && j > 0 && text[j - 1] == '\'')
      text = text.substr(0, j - 1);
  }
  else {
    if (! equals) {
      CDocScriptError("Invalid Symbol Definition %s", str.c_str());

      return;
    }

    j = i;

    CStrUtil::skipNonSpace(str, &i);

    text = str.substr(j, i - j);
  }

  CDocScriptAddSymbol(symbol, text);
}

// Parse Define Variable String and add resultant Symbol definition to
// the Symbol List.
extern void
CDocScriptParseDefineVariable(const std::string &str)
{
  uint len = str.size();

  /* Skip Leading Blanks */

  uint i = 0;

  CStrUtil::skipSpace(str, &i);

  /****************/

  /* Get Symbol Name */

  uint j = i;

  while (i < len && isalnum(str[i]))
    i++;

  if (i == j) {
    CDocScriptError("Invalid Define Variable Definition %s", str.c_str());

    return;
  }

  std::string symbol = str.substr(j, i - j);

  CStrUtil::skipSpace(str, &i);

  /****************/

  /* Get Next Word */

  j = i;

  if (i < len && str[i] == '/') {
    i++;

    while (i < len && str[i] != '/')
      i++;
  }
  else
    CStrUtil::skipNonSpace(str, &i);

  if (i == j) {
    CDocScriptError("Invalid Define Variable Definition %s", str.c_str());

    return;
  }

  std::string word = str.substr(j, i - j);

  CStrUtil::skipSpace(str, &i);

  /****************/

  /* Process Symbol with Font */

  if      (CStrUtil::casecmp(word, "font") == 0) {
    CDocScriptWarning("Font Not Supported for Define Variable");

    /* Skip Next Word */

    j = i;

    CStrUtil::skipNonSpace(str, &i);

    if (i == j)
      CDocScriptWarning("Fontname missing for Define Variable");

    CStrUtil::skipSpace(str, &i);

    /* Get Next Word */

    j = i;

    if (i < len && str[i] == '/') {
      i++;

      while (i < len && str[i] != '/')
        i++;
    }
    else
      CStrUtil::skipNonSpace(str, &i);

    if (i == j) {
      CDocScriptError("Invalid Define Variable Definition %s", str.c_str());
      return;
    }

    word = str.substr(j, i - j);

    CStrUtil::skipSpace(str, &i);

    /* Skip Baseline Definition */

    if (CStrUtil::casecmp(word, "baseline") == 0) {
      /* Skip Next Word */

      j = i;

      CStrUtil::skipNonSpace(str, &i);

      if (i == j)
        CDocScriptWarning("Baseline missing for Define Variable");

      CStrUtil::skipSpace(str, &i);

      /* Get Next Word */

      j = i;

      if (i < len && str[i] == '/') {
        i++;

        while (i < len && str[i] != '/')
          i++;
      }
      else
        CStrUtil::skipNonSpace(str, &i);

      if (i == j) {
        CDocScriptError("Invalid Define Variable Definition %s", str.c_str());
        return;
      }

      word = str.substr(j, i - j);

      CStrUtil::skipSpace(str, &i);
    }
  }
  else if (CStrUtil::casecmp(word, "text") == 0) {
    /* Get Next Word */

    j = i;

    if (i < len && str[i] == '/') {
      i++;

      while (i < len && str[i] != '/')
        i++;
    }
    else
      CStrUtil::skipNonSpace(str, &i);

    if (i == j) {
      CDocScriptError("Invalid Define Variable Definition %s", str.c_str());
      return;
    }

    word = str.substr(j, i - j);

    CStrUtil::skipSpace(str, &i);
  }

  /****************/

  /* Get Symbol Text */

  if (word.empty() || word[0] != '/') {
    CDocScriptError("Invalid Define Variable Definition %s", str.c_str());
    return;
  }

  i = word.size() - 1;

  if (word[i - 1] == '/')
    word = word.substr(0, i - 1);

  CDocScriptAddSymbol(symbol, word.substr(1));
}

// Process the supplied String replacing any in-line '&' strings with the text
// which they define.
extern std::string
CDocScriptReplaceSymbolsInString(const std::string &str)
{
  std::string str1 = str;

  /* Keep Changing String until all Symbols (including Symbols in Symbols) are resolved. */

  bool changed = true;

  while (changed) {
    int len1 = str1.size();

    changed = false;

    std::string str2;

    int i = 0;

    while (i < len1) {
      /* If we have a potential Symbol */

      if (str1[i] == '&') {
        ++i;

        /* Get Symbol after Ampersand */

        std::string symbol;

        while (i < len1 && str1[i] != '.' && isalnum(str1[i]))
          symbol += str1[i++];

        /* Get Text for Symbol (if it is a Symbol) */

        std::string text;

        if (i < len1)
          text = CDocScriptGetGlobalSymbolText(symbol, str1[i]);
        else
          text = CDocScriptGetGlobalSymbolText(symbol, '\0');

        if (text != "") {
          str2 += text;

          changed = true;
        }

        /* Not an Symbol so just add text */

        else {
          str2 += "&";
          str2 += symbol;
        }
      }

      /* Not an Ampersand Command so just add text */

      else
        str2 += str1[i++];
    }

    /* If the line hasn't changed then terminate loop */

    if (! changed)
      break;

    /* Reset String */

    str1 = str2;
  }

  /* Replace any Control A Symbols with an Ampersand */

  int len1 = str1.size();

  int i = 0;

  while (i < len1) {
    if (str1[i] == AMP_MARK_C)
      str1[i] = '&';

    ++i;
  }

  return str1;
}

// Get the text associated with a Script builtin symbol or a user defined symbol name.
static std::string
CDocScriptGetGlobalSymbolText(const std::string &symbol, int next_char)
{
  std::string symbol_text = CDocScriptGetSymbolText(symbol);

  if (symbol_text != "")
    return symbol_text;

  /* Replace '&amp.' with the Control A character (this stops us creating a new
     symbol which is changed on the next pass) */

  if      (symbol == "amp" && next_char != '\0' &&
           (isalnum(next_char) || next_char == '@' ||
            next_char == '$'   || next_char == '#'))
    return AMP_MARK_S;

  /* Replace '&date.' with a date string */

  else if (symbol == "date")
    return cdoc_date;

  /* Replace '&gml.' with the ':' character */

  else if (symbol == "gml")
    return ":";

  /* Replace '&rbl.' with the ' ' character */

  else if (symbol == "rbl") {
    return " ";
  }

  /* Replace '&semi.' with the ';' character */

  else if (symbol == "semi")
    return ";";

  /* Replace '&time.' with a time string */

  else if (symbol == "time") {
    time_t current_time = time(&current_time);

    struct tm *tm = localtime(&current_time);

    static char time_string[32];

    strftime(time_string, 256, "%H:%M:%S", tm);

    return time_string;
  }

  return "";
}

// Add a parsed Symbol to the Symbol List.
//
// Multiple definitions of symbols cause the symbol to be replaced but this may
// cause problems as symbols are only stored on the first pass.
static void
CDocScriptAddSymbol(const std::string &symbol, const std::string &text)
{
  CDSymbolList::iterator p = symbol_list.find(symbol);

  if (p != symbol_list.end()) {
    CDSymbol *symbol1 = (*p).second;

    if (symbol1->text == text)
      return;

    CDocScriptWarning("Multiply defined Symbol %s", symbol.c_str());

    symbol1->text = text;

    return;
  }

  CDSymbol *symbol1 = new CDSymbol;

  symbol1->symbol = symbol;
  symbol1->text   = text;

  symbol_list[symbol1->symbol] = symbol1;
}

// Get the Text associated with a given Symbol Name.
extern std::string
CDocScriptGetSymbolText(const std::string &symbol)
{
  CDSymbolList::iterator p = symbol_list.find(symbol);

  if (p != symbol_list.end()) {
    CDSymbol *symbol1 = (*p).second;

    return symbol1->text;
  }
  else
    return "";
}

// Delete resources allocated by any defined Symbols.
extern void
CDocScriptDeleteSymbols()
{
  process_map_second(symbol_list, CDeletePointer());

  symbol_list.clear();
}
