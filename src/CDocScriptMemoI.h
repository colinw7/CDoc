#ifndef CDOC_SCRIPT_MEMO_I_H
#define CDOC_SCRIPT_MEMO_I_H

extern void  CDocScriptInitMemoHeader           ();
extern void  CDocScriptProcessMemoHeaderCommand (CDColonCommand *);
extern void  CDocScriptOutputMemoHeader         ();
extern void  CDocScriptOutputMemoSignature      ();
extern void  CDocScriptGetMemoDetails           (std::string &, std::string &);

#endif
