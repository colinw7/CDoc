#include "CDocI.h"

/*------------------------------------------------------------------------*/

typedef std::vector<CDLabel *> CDLabelList;

static CDLabelList label_list;

/*------------------------------------------------------------------------*/

static CDLabel *CDocScriptGetLabel(const std::string &);

// Creates and Stores a Label Control Structure for the supplied name.
extern void
CDocScriptAddLabel(const std::string &name)
{
  if (! cdoc_input_fp_list.empty()) {
    CDocScriptError("Labels not allowed in Imbedded Files");
    return;
  }

  CDLabel *label = CDocScriptGetLabel(name);

  if (label == NULL) {
    label = new CDLabel;

    label->name     = name;
    label->position = ftell(cdoc_input_fp);

    label_list.push_back(label);
  }
  else
    CDocScriptWarning("Multiply defined Label %s", name.c_str());
}

// Goto the specified Label by moving to the position recorded by CDocScriptAddLabel().
extern void
CDocScriptGotoLabel(const std::string &name)
{
  if (! cdoc_input_fp_list.empty()) {
    CDocScriptError("Gotos not allowed in Imbedded Files");
    return;
  }

  CDLabel *label = CDocScriptGetLabel(name);

  if (label == NULL)
    CDocScriptWarning("Label %s is not defined", name.c_str());
  else
    fseek(cdoc_input_fp, label->position, SEEK_SET);
}

// Get the label associated with a specified name.
static CDLabel *
CDocScriptGetLabel(const std::string &name)
{
  /* Search List of Label Structures for one with a Name matching the supplied one */

  uint no_labels = label_list.size();

  for (uint i = 1; i <= no_labels; i++) {
    CDLabel *label = label_list[i - 1];

    /* Found Match so Return Label Structure */

    if (label->name == name)
      return label;
  }

  return NULL;
}
