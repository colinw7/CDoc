#ifndef CDOC_SCRIPT_EXAMPLE_I_H
#define CDOC_SCRIPT_EXAMPLE_I_H

/*---------------------------------------------------------------------*/

struct CDExample {
  int               depth;
  CDScriptTempFile *temp_file;
};

/*---------------------------------------------------------------------*/

extern CDExample *CDocScriptCreateExample();
extern void       CDocScriptExampleBegin (CDExample *);
extern void       CDocScriptExampleEnd   (CDExample *);

#endif
