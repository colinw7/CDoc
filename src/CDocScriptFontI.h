#ifndef CDOC_SCRIPT_FONT_I_H
#define CDOC_SCRIPT_FONT_I_H

/*---------------------------------------------------------------------*/

struct CDFont {
  std::string name;
  std::string def;
};

/*---------------------------------------------------------------------*/

extern void  CDocScriptDefineFont    (const std::string &, const std::string &);
extern void  CDocScriptBeginFont     (const std::string &);
extern void  CDocScriptPreviousFont  ();
extern int   CDocScriptIsValidFont   (const std::string &);
extern void  CDocScriptStartAllFonts ();
extern void  CDocScriptEndAllFonts   ();
extern void  CDocScriptDeleteFonts   ();

#endif
