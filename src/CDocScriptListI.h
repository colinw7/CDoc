#ifndef CDOC_SCRIPT_LIST_I_H
#define CDOC_SCRIPT_LIST_I_H

/*--------------------------------------------------------------------*/

#define NO_LIST         0
#define ORDERED_LIST    1
#define SIMPLE_LIST     2
#define UNORDERED_LIST  3
#define DEFINITION_LIST 4
#define GLOSSARY_LIST   5

struct CDDefinitionList {
  int  heading_highlight;
  int  term_highlight;
  int  depth;
  int  break_up;
  int  compact;
  int  prefix_length;
  int  term_flag;
};

struct CDGlossaryList {
  int  compact;
  int  term_highlight;
};

struct CDGeneralList {
  int  type;
  int  number;
  int  compact;
};

/*--------------------------------------------------------------------*/

extern CDDefinitionList  *CDocScriptCreateDefinitionList
                           ();
extern CDGlossaryList    *CDocScriptCreateGlossaryList
                           ();
extern CDGeneralList     *CDocScriptCreateGeneralList
                           (int);
extern void               CDocScriptStartDefinitionList
                           (CDDefinitionList *);
extern void               CDocScriptStartGlossaryList
                           (CDGlossaryList *);
extern void               CDocScriptStartGeneralList
                           (CDGeneralList *);
extern int                CDocScriptGetNoGeneralLists
                           ();
extern CDGeneralList     *CDocScriptGetNthGeneralList
                           (int);
extern int                CDocScriptGetCurrentListLeftMargin
                           ();
extern int                CDocScriptGetPreviousListLeftMargin
                           ();
extern int                CDocScriptGetCurrentListCompact
                           ();
extern void               CDocScriptSetCurrentListLeftMargin
                           (int);
extern int                CDocScriptIsCurrentList
                           ();
extern int                CDocScriptIsCurrentListType
                           (int);
extern CDDefinitionList  *CDocScriptEndDefinitionList
                           ();
extern CDGlossaryList    *CDocScriptEndGlossaryList
                           ();
extern CDGeneralList     *CDocScriptEndGeneralList
                           (int);

#endif
