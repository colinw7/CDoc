#ifndef CDOC_SCRIPT_OPTIONS_I_H
#define CDOC_SCRIPT_OPTIONS_I_H

/*--------------------------------------------------------------------*/

#include "CDocScriptOptions.h"

/*--------------------------------------------------------------------*/

extern int         cdoc_left_margin;
extern int         cdoc_right_margin;
extern int         cdoc_lines_per_page;
extern int         cdoc_number_headers;
extern int         cdoc_paragraph_indent;
extern int         cdoc_title_page;
extern int         cdoc_title_page_align;
extern int         cdoc_index;
extern int         cdoc_page_numbering;
extern std::string cdoc_process_name;
extern std::string cdoc_reference_file;
extern int         cdoc_on_warning;
extern int         cdoc_on_error;
extern int         cdoc_spell_check;
extern int         cdoc_spell_active;

extern int         cdoc_header_number[];

/*----------*/

extern int         cdoc_save_left_margin;
extern int         cdoc_save_right_margin;
extern int         cdoc_save_lines_per_page;
extern int         cdoc_save_number_headers;
extern int         cdoc_save_paragraph_indent;
extern int         cdoc_save_title_page;
extern int         cdoc_save_title_page_align;
extern int         cdoc_save_index;
extern int         cdoc_save_page_numbering;
extern std::string cdoc_save_process_name;
extern std::string cdoc_save_reference_file;
extern int         cdoc_save_on_warning;
extern int         cdoc_save_on_error;
extern int         cdoc_save_spell_check;
extern int         cdoc_save_spell_active;

extern int         cdoc_save_header_number[];

/*--------------------------------------------------------------------*/

extern void  CDocScriptSaveOptions   ();
extern void  CDocScriptRestoreOptions();

#endif
