#ifndef CDOC_SCRIPT_SYMBOL_I_H
#define CDOC_SCRIPT_SYMBOL_I_H

/*---------------------------------------------------------------------*/

struct CDSymbol {
  std::string symbol;
  std::string text;
};

/*---------------------------------------------------------------------*/

extern void        CDocScriptParseSymbol            (const std::string &);
extern void        CDocScriptParseDefineVariable    (const std::string &);
extern std::string CDocScriptReplaceSymbolsInString (const std::string &);
extern std::string CDocScriptGetSymbolText          (const std::string &);
extern void        CDocScriptDeleteSymbols          ();

#endif
