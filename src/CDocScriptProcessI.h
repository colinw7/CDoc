#ifndef CDOC_SCRIPT_PROCESS_I_H
#define CDOC_SCRIPT_PROCESS_I_H

#include "CDocScriptProcess.h"

/*--------------------------------------------------------------------*/

#define ILEAF_NORMAL_FONT         "<F0>"
#define ILEAF_UNDERLINE_FONT      "<F10@Z7@Lam>"
#define ILEAF_BOLD_FONT           "<F6@Z7@Lam>"
#define ILEAF_BOLD_UNDERLINE_FONT "<F11@Z7@Lam>"

#define ESC_ILEAF_NORMAL_FONT         "\033<F0>"
#define ESC_ILEAF_UNDERLINE_FONT      "\033<F10@Z7@Lam>"
#define ESC_ILEAF_BOLD_FONT           "\033<F6@Z7@Lam>"
#define ESC_ILEAF_BOLD_UNDERLINE_FONT "\033<F11@Z7@Lam>"

/*--------------------------------------------------------------------*/

enum CDocType {
  CDOC_NO_DOCUMENT      = 0,
  CDOC_GENERAL_DOCUMENT = 1,
  CDOC_STAIRS_DOCUMENT  = 2,
  CDOC_MEMO_DOCUMENT    = 3
};

enum CDocPartType {
  CDOC_NO_PART           = 0,
  CDOC_FRONT_MATTER_PART = 1,
  CDOC_BODY_PART         = 2,
  CDOC_APPENDIX_PART     = 3,
  CDOC_BACK_MATTER_PART  = 4,
  CDOC_FRONT_SHEET_PART  = 5,
  CDOC_AMENDMENTS_PART   = 6,
  CDOC_MEMO_HEADER_PART  = 7
};

enum CDocSubPartType {
  CDOC_NO_SUB_PART         = 0,
  CDOC_TITLE_PAGE_SUB_PART = 1,
  CDOC_ABSTRACT_SUB_PART   = 2,
  CDOC_PREFACE_SUB_PART    = 3
};

struct CDDocument {
  std::string     security;
  std::string     language;
  CDocType        type;
  CDocPartType    part;
  CDocSubPartType sub_part;
};

/*--------------------------------------------------------------------*/

typedef std::vector<FILE *> FILEList;

extern CDDocument        cdoc_document;
extern int               cdoc_pass_no;
extern int               cdoc_input_line_no;
extern FILE             *cdoc_input_fp;
extern FILEList          cdoc_input_fp_list;
extern FILE             *cdoc_output_fp;
extern std::string       cdoc_current_section;
extern int               cdoc_indent;
extern int               cdoc_no_centred;
extern int               cdoc_no_uscore;
extern int               cdoc_no_cap;
extern int               cdoc_formatting;
extern CDDefinitionList *cdoc_definition_list;
extern CDGeneralList    *cdoc_general_list;
extern CDGlossaryList   *cdoc_glossary_list;
extern std::string       cdoc_date;
extern std::string       cdoc_footer;
extern std::string       cdoc_line;
extern int               cdoc_line_len;
extern int               cdoc_continuation_char;

/*--------------------------------------------------------------------*/

extern void  CDocScriptStartHeader   (int);
extern int   CDocScriptGetHeader     (int);
extern void  CDocScriptFormattingOn  ();
extern void  CDocScriptFormattingOff ();

#endif
