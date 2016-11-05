#ifndef CDOC_SCRIPT_MACRO_I_H
#define CDOC_SCRIPT_MACRO_I_H

struct CDMacro;

/*---------------------------------------------------------------------*/

extern CDMacro      *CDocScriptCreateMacro
                      (const std::string &);
extern void          CDocScriptAddMacroLine
                      (CDMacro *, CDScriptLine *);
extern CDScriptLine *CDocScriptGetMacroLine
                      (CDMacro *, int);
extern CDMacro      *CDocScriptGetMacro
                      (const std::string &);
extern void          CDocScriptDeleteMacros
                      ();

#endif
