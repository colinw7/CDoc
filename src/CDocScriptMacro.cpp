#include "CDocI.h"

/*------------------------------------------------------------------------*/

typedef std::vector<CDScriptLine *> CDScriptLineList;

struct CDMacro {
  std::string      name;
  CDScriptLineList line_list;

 ~CDMacro();
};

/*------------------------------------------------------------------------*/

typedef std::vector<CDMacro *> CDMacroList;

static CDMacroList macro_list;

// Create and Store a macro of a specified name.
//
// If a macro is defined more than once only the first will actually be registered.
extern CDMacro *
CDocScriptCreateMacro(const std::string &name)
{
  CDMacro *macro = CDocScriptGetMacro(name);

  if (macro == NULL) {
    macro = new CDMacro;

    macro->name = name;

    macro_list.push_back(macro);
  }
  else
    CDocScriptError("Multiply defined Macro %s", name.c_str());

  return macro;
}

// Add a parsed Script Line to the list of Lines associated with a Macro.
//
// The Script Line is not copied so it must not be deleted as would usually
// be the case.
extern void
CDocScriptAddMacroLine(CDMacro *macro, CDScriptLine *script_line)
{
  macro->line_list.push_back(script_line);
}

// Get the Nth Macro Script Line from the Macro Structure.
//
// Probably should call like CDocScriptGetNextMacroLine() with an internal
// counter for better data hiding.
extern CDScriptLine *
CDocScriptGetMacroLine(CDMacro *macro, int line_no)
{
  CDScriptLine *script_line = NULL;

  if (line_no <= (int) macro->line_list.size())
    script_line = macro->line_list[line_no - 1];

  return script_line;
}

// Get the Macro Structure associated with a Macro Name.
extern CDMacro *
CDocScriptGetMacro(const std::string &name)
{
  /* Search List of Macro Structures for one with a Name
     matching the supplied one */

  int no_macros = macro_list.size();

  for (int i = 1; i <= no_macros; i++) {
    CDMacro *macro = macro_list[i - 1];

    /* Found Match so Return Macro Structure */

    if (macro->name == name)
      return macro;
  }

  return NULL;
}

// Delete all resources allocated by Macros.
extern void
CDocScriptDeleteMacros()
{
  for_each(macro_list.begin(), macro_list.end(), CDeletePointer());

  macro_list.clear();
}

//---------

CDMacro::
~CDMacro()
{
  for_each(line_list.begin(), line_list.end(), CDeletePointer());

  line_list.clear();
}
