#ifndef CDOC_SCRIPT_REFERENCE_I_H
#define CDOC_SCRIPT_REFERENCE_I_H

#define FIGURE_REF    1
#define FOOTNOTE_REF  2
#define HEADER_REF    3
#define LIST_ITEM_REF 4
#define TABLE_REF     5
#define CDOC_REF      6
#define INDEX_REF     100

struct CDCrossRef {
  int   type;
  char *refid;
  int   page;
};

extern void        CDocScriptInitReferences
                    ();
extern void        CDocScriptAddReference
                    (int, const std::string &, const std::string &, const std::string &);
extern void        CDocScriptSetReferencePageNumber
                    (int, const std::string &);
extern std::string CDocScriptGetParagraphReferenceText
                    (CDCrossRef *);
extern void        CDocScriptUpdateReferenceCommands
                    (CDCrossRef *);
extern std::string CDocScriptGetReferenceText
                    (CDCrossRef *);
extern std::string CDocScriptGetReferencePage
                    (int);
extern void        CDocScriptTermReferences
                    ();

#endif
