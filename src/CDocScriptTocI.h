#ifndef CDOC_SCRIPT_TOC_I_H
#define CDOC_SCRIPT_TOC_I_H

extern void        CDocScriptAddTOCEntry           (const std::string &);
extern void        CDocScriptAddTOCSkip            ();
extern void        CDocScriptSetTOCPage            ();
extern std::string CDocScriptGetTOCPage            (int);
extern void        CDocScriptOutputTableOfContents ();
extern void        CDocScriptDeleteTOC             ();

#endif
