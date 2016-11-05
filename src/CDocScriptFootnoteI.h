#ifndef CDOC_SCRIPT_FOOTNOTE_I_H
#define CDOC_SCRIPT_FOOTNOTE_I_H

struct CDFootnote {
  int               number;
  std::string       ident;
  int               no_lines;
  CDScriptTempFile *temp_file;

  CDFootnote() :
   number(0), no_lines(0), temp_file(nullptr) {
  }

 ~CDFootnote();
};

extern CDFootnote *CDocScriptCreateFootnote ();
extern void        CDocScriptStartFootnote  (CDFootnote *);
extern void        CDocScriptEndFootnote    (CDFootnote *);
extern void        CDocScriptOutputFootnotes(int);
extern void        CDocScriptDeleteFootnotes();

#endif
