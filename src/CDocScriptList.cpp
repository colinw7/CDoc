#include "CDocI.h"

/*------------------------------------------------------------------------*/

struct CDCurrentList {
  int    type;
  char  *list;
  int    left_margin;
  int    compact;
};

/*------------------------------------------------------------------------*/

typedef std::vector<CDDefinitionList *> CDDefinitionListList;
typedef std::vector<CDGlossaryList *>   CDGlossaryListList;
typedef std::vector<CDGeneralList *>    CDGeneralListList;
typedef std::vector<CDCurrentList *>    CDCurrentListList;

static CDDefinitionListList definition_list_list;
static CDGlossaryListList   glossary_list_list;
static CDGeneralListList    general_list_list;
static CDCurrentListList    current_list_list;

/*------------------------------------------------------------------------*/

static void  CDocScriptStartCurrentList (int, char *, int);

// Create and Store a Definition List Structure.
extern CDDefinitionList *
CDocScriptCreateDefinitionList()
{
  CDDefinitionList *definition_list = new CDDefinitionList;

  definition_list->heading_highlight = 3;
  definition_list->term_highlight    = 2;
  definition_list->depth             = 10;
  definition_list->break_up          = false;
  definition_list->compact           = false;
  definition_list->prefix_length     = 0;
  definition_list->term_flag         = false;

  definition_list_list.push_back(definition_list);

  return definition_list;
}

// Create and Store a Glossary List Structure.
extern CDGlossaryList *
CDocScriptCreateGlossaryList()
{
  CDGlossaryList *glossary_list = new CDGlossaryList;

  glossary_list->term_highlight = 2;
  glossary_list->compact        = false;

  glossary_list_list.push_back(glossary_list);

  return glossary_list;
}

// Create and Store a General List Structure.
extern CDGeneralList *
CDocScriptCreateGeneralList(int type)
{
  CDGeneralList *general_list = new CDGeneralList;

  general_list->type    = type;
  general_list->number  = 0;
  general_list->compact = false;

  general_list_list.push_back(general_list);

  return general_list;
}

// Start processing for a Definition List by starting a new current list structure.
extern void
CDocScriptStartDefinitionList(CDDefinitionList *definition_list)
{
  CDocScriptStartCurrentList(DEFINITION_LIST, (char *) definition_list,
                             definition_list->compact);
}

// Start processing for a Glossary List by starting a new current list structure.
extern void
CDocScriptStartGlossaryList(CDGlossaryList *glossary_list)
{
  CDocScriptStartCurrentList(GLOSSARY_LIST, (char *) glossary_list,
                             glossary_list->compact);
}

// Start processing for a General List by starting a new current list structure.
extern void
CDocScriptStartGeneralList(CDGeneralList *general_list)
{
  CDocScriptStartCurrentList(general_list->type, (char *) general_list,
                             general_list->compact);
}

// Start a new Current List.
static void
CDocScriptStartCurrentList(int type, char *list, int compact)
{
  CDCurrentList *current_list = new CDCurrentList;

  current_list->type        = type;
  current_list->list        = (char *) list;
  current_list->left_margin = cdoc_left_margin;
  current_list->compact     = compact;

  current_list_list.push_back(current_list);
}

// Count the number of General Lists currently active.
extern int
CDocScriptGetNoGeneralLists()
{
  int no = general_list_list.size();

  return no;
}

// Get the Nth currently active General List.
extern CDGeneralList *
CDocScriptGetNthGeneralList(int n)
{
  CDGeneralList *general_list = general_list_list[n - 1];

  return general_list;
}

// Get the currently active List's Left Margin.
extern int
CDocScriptGetCurrentListLeftMargin()
{
  if (current_list_list.empty())
    return -1;

  CDCurrentList *current_list = current_list_list.back();

  if (current_list == NULL)
    return -1;

  return current_list->left_margin;
}

// Get the List (previous to the currently active List) 's Left Margin.
extern int
CDocScriptGetPreviousListLeftMargin()
{
  if (current_list_list.size() <= 1)
    return -1;

  CDCurrentList *current_list =
    current_list_list[current_list_list.size() - 2];

  if (current_list == NULL)
    return -1;

  return current_list->left_margin;
}

// Return whether the currently active List is Compact.
extern int
CDocScriptGetCurrentListCompact()
{
  if (current_list_list.empty())
    return false;

  CDCurrentList *current_list = current_list_list.back();

  if (current_list == NULL)
    return false;

  return current_list->compact;
}

// Set the currently active List's Left Margin.
extern void
CDocScriptSetCurrentListLeftMargin(int left_margin)
{
  if (current_list_list.empty())
    return;

  CDCurrentList *current_list = current_list_list.back();

  if (current_list == NULL)
    return;

  current_list->left_margin = left_margin;
}

// Check if there is a currently active List.
extern int
CDocScriptIsCurrentList()
{
  if (current_list_list.empty())
    return false;

  CDCurrentList *current_list = current_list_list.back();

  if (current_list == NULL)
    return false;

  return true;
}

// Check if the currently active List is of the specified type.
extern int
CDocScriptIsCurrentListType(int type)
{
  if (current_list_list.empty())
    return false;

  CDCurrentList *current_list = current_list_list.back();

  return (current_list != NULL && current_list->type == type);
}

// Terminate the currently active Definition List and return the previously
// active Definition List (if any).
extern CDDefinitionList *
CDocScriptEndDefinitionList()
{
  CDDefinitionList *definition_list = definition_list_list.back();
  CDCurrentList    *current_list    = current_list_list   .back();

  if (definition_list != NULL && current_list != NULL &&
      current_list->type == DEFINITION_LIST) {
    definition_list_list.pop_back();
    current_list_list   .pop_back();

    delete definition_list;
    delete current_list   ;

    if (! definition_list_list.empty())
      definition_list = definition_list_list.back();
    else
      definition_list = NULL;
  }
  else
    CDocScriptError("Internal Error for Definition Lists");

  return definition_list;
}

// Terminate the currently active Glossary List and return the previously
// active Glossary List (if any).
extern CDGlossaryList *
CDocScriptEndGlossaryList()
{
  CDGlossaryList *glossary_list = glossary_list_list.back();
  CDCurrentList  *current_list  = current_list_list .back();

  if (glossary_list != NULL && current_list != NULL &&
      current_list->type == GLOSSARY_LIST) {
    glossary_list_list.pop_back();
    current_list_list .pop_back();

    delete glossary_list;
    delete current_list ;

    if (! glossary_list_list.empty())
      glossary_list = glossary_list_list.back();
    else
      glossary_list = NULL;
  }
  else
    CDocScriptError("Internal Error for Glossary Lists");

  return glossary_list;
}

// Terminate the currently active General List of the specified type and
// return the previously active General List (if any).
extern CDGeneralList *
CDocScriptEndGeneralList(int type)
{
  CDGeneralList *general_list = general_list_list.back();
  CDCurrentList *current_list = current_list_list.back();

  if (general_list != NULL && current_list != NULL && current_list->type == type) {
    general_list_list.pop_back();
    current_list_list.pop_back();

    delete general_list;
    delete current_list;

    if (! general_list_list.empty())
      general_list = general_list_list.back();
    else
      general_list = NULL;
  }
  else {
    if      (type == ORDERED_LIST)
      CDocScriptError("Internal Error for Ordered List");
    else if (type == SIMPLE_LIST)
      CDocScriptError("Internal Error for Simple List");
    else if (type == UNORDERED_LIST)
      CDocScriptError("Internal Error for Unordered List");
    else
      CDocScriptError("Internal Error for General List");
  }

  return general_list;
}
