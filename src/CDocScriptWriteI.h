#ifndef CDOC_SCRIPT_WRITE_I_H
#define CDOC_SCRIPT_WRITE_I_H

#include "CDocScriptWriteI.h"

/*-----------------------------------------------------------------------*/

extern int  cdoc_page_no;
extern int  cdoc_page_no_offset;
extern int  cdoc_line_no;
extern int  cdoc_char_no;
extern int  cdoc_line_fiddle;
extern int  cdoc_page_header_output;
extern int  cdoc_last_indent;
extern int  cdoc_last_indent_page;

/*-----------------------------------------------------------------------*/

extern int    CDocScriptFindSpace
               (int);
extern void   CDocScriptNewPage
               ();
extern void   CDocScriptStartNewPage
               ();
extern void   CDocScriptSetPagePreHeaderProc
               (void (*)(const char *), char *);
extern void   CDocScriptSetPagePostHeaderProc
               (void (*)(const char *), char *);
extern void   CDocScriptSetPagePreFooterProc
               (void (*)(const char *), char *);
extern void   CDocScriptSetPagePostFooterProc
               (void (*)(const char *), char *);
extern void   CDocScriptWritePageHeader
               ();
extern void   CDocScriptWritePageFooter
               ();
extern void   CDocScriptStartSection
               (const std::string &);
extern void   CDocScriptStartSubSection
               (const std::string &);
extern void   CDocScriptSkipLine
               ();
extern void   CDocScriptNewLine
               ();
extern void   CDocScriptWriteIndent
               (int);
extern void   CDocScriptWriteJustifiedLine
               (const std::string &, int, int);
extern void   CDocScriptWriteLeftJustifiedLine
               (const std::string &, int);
extern void   CDocScriptWriteCentreJustifiedLine
               (const std::string &, int);
extern void   CDocScriptWriteRightJustifiedLine
               (const std::string &, int);
extern void   CDocScriptWriteJustifiedPageHeader
               (const std::string &, int);
extern void   CDocScriptWriteLeftJustifiedPageHeader
               (const std::string &);
extern void   CDocScriptWriteCentreJustifiedPageHeader
               (const std::string &);
extern void   CDocScriptWriteRightJustifiedPageHeader
               (const std::string &);
extern void   CDocScriptWriteJustifiedHeader
               (const std::string &, int);
extern void   CDocScriptWriteLeftJustifiedHeader
               (const std::string &);
extern void   CDocScriptWriteCentreJustifiedHeader
               (const std::string &);
extern void   CDocScriptWriteRightJustifiedHeader
               (const std::string &);
extern void   CDocScriptWriteUnderlinedText
               (const std::string &);
extern void   CDocScriptWriteFontText
               (const std::string &, int);
extern void   CDocScriptWriteLine
               (const std::string &, ...);
extern void   CDocScriptWriteText
               (const std::string &, ...);
extern void   CDocScriptWriteCommand
               (const std::string &, ...);
extern void   CDocScriptFPuts
               (const std::string &);
extern void   CDocScriptFPutc
               (int);
extern void   CDocScriptDrawHLine
               (int, int);
extern void   CDocScriptDrawVLine
               (int, int);
extern int    CDocScriptGetLinesPerPage
               ();
extern void   CDocScriptSaveBodyState
               ();
extern void   CDocScriptRestoreBodyState
               ();
extern char  *CDocEncodeHtmlString
               (const std::string &);
extern char  *CDocEncodeILeafString
               (const std::string &);

/*-----------------------------------------------------------------------*/

#endif
