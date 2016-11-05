#ifndef CDOC_SCRIPT_TEMP_FILE_I_H
#define CDOC_SCRIPT_TEMP_FILE_I_H

struct CDScriptTempFile {
  std::string filename;
  FILE*       fp;
  int         no_lines;
  FILE*       save_output_fp;
  int         save_page_no;
  int         save_line_no;
  int         save_char_no;
  int         save_left_margin;
  int         save_right_margin;
  int         save_indent;
  int         save_in_paragraph;
  int         save_paragraph_done;

 ~CDScriptTempFile();
};

/*---------------------------------------------------------------------*/

extern CDScriptTempFile *CDocScriptStartTempFile
                          (const std::string &);
extern void              CDocScriptEndTempFile
                          (CDScriptTempFile *);
extern void              CDocScriptOutputTempFile
                          (CDScriptTempFile *);
extern void              CDocScriptOutputTempFileInit
                          ();
extern void              CDocScriptOutputTempFileLine
                          (const char *);
extern void              CDocScriptOutputTempFileTerm
                          ();

#endif
