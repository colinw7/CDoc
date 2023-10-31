#ifndef CDOC_SCRIPT_OPTIONS_H
#define CDOC_SCRIPT_OPTIONS_H

/*------------------------------------------------------------------*/

constexpr ulong CDOC_ON_WARNING_CONTINUE = 0;
constexpr ulong CDOC_ON_WARNING_IGNORE   = 1;
constexpr ulong CDOC_ON_WARNING_PROMPT   = 2;
constexpr ulong CDOC_ON_WARNING_EXIT     = 3;

constexpr ulong CDOC_ON_ERROR_CONTINUE = 0;
constexpr ulong CDOC_ON_ERROR_IGNORE   = 1;
constexpr ulong CDOC_ON_ERROR_PROMPT   = 2;
constexpr ulong CDOC_ON_ERROR_EXIT     = 3;

/*------------------------------------------------------------------*/

extern void  CDocScriptProcessOptions     (const char **, int *);
extern int   CDocScriptSetMargins         (int, int);
extern int   CDocScriptSetLinesPerPage    (int);
extern void  CDocScriptSetHeaderNumbering (int, int *, int);
extern int   CDocScriptSetParagraphIndent (int);
extern void  CDocScriptSetTitlePage       (int, int);
extern void  CDocScriptSetIndex           (int);
extern void  CDocScriptSetPageNumbering   (int);
extern void  CDocScriptSetProcessName     (char *);
extern void  CDocScriptSetReferenceFile   (char *);
extern void  CDocScriptSetOnWarning       (int);
extern void  CDocScriptSetOnError         (int);
extern void  CDocScriptSetSpellCheck      (int);
extern void  CDocScriptListOptions        (FILE *);

#endif
