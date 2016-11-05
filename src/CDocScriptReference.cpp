#include "CDocI.h"

/*-----------------------------------------------------------------------*/

struct CDReference {
  int         type;
  std::string id;
  std::string str;
  std::string see_string;
  int         page_no;
  int         no;
  std::string command;
};

/*-----------------------------------------------------------------------*/

typedef std::vector<CDReference *> CDReferenceList;

static CDReferenceList reference_list;
static CDReferenceList reference_command_list;

static std::string reference_string;
static std::string page_reference_string;

/*-----------------------------------------------------------------------*/

static void         CDocScriptReadReferenceFile();
static CDReference *CDocScriptGetReference     (const std::string &, int);

// Initialise the reference control data ready for processing an IBM Script
// File and read any references defined in an external file.
extern void
CDocScriptInitReferences()
{
  /* Initialise the Reference List and Reference Command List */

  reference_list.clear();
  reference_command_list.clear();

  /* Read any Extra Reference Data from the Specified File */

  CDocScriptReadReferenceFile();
}

// Reads and Stores the commands in the Reference File (specified using
// CDocScriptSetReferenceListFile()) which allows cross referencing from on
// document to other information available in a number of formats when using
// CDoc to display the text.
static void
CDocScriptReadReferenceFile()
{
  /* If no Reference File has been specified then we have
     nothing to do */

  if (cdoc_reference_file == "")
    return;

  /* Open the Reference File */

  FILE *fp = fopen(cdoc_reference_file.c_str(), "r");

  /* If the Reference File cannot be read then issue a Warning
     and return (processing continues) */

  if (fp == NULL) {
    CDocScriptWarning("Reference File %s does not exist - Ignored",
                      cdoc_reference_file.c_str());
    return;
  }

  /* Read and Process Each Line from the File */

  char line[CDOC_MAX_LINE];

  while (fgets(line, CDOC_MAX_LINE, fp) != NULL) {
    /* Remove Trailing Newline Character */

    char *p1 = strchr(line, '\n');

    if (p1 != NULL)
      *p1 = '\0';

    /* Find the first TAB field separator(s) and zero them out so we are pointing
       at a string containing the second field */

    p1 = strchr(line, '\t');

    if (p1 != NULL) {
      *p1 = '\0';

      while (p1[1] == '\t') {
        p1++;

        *p1 = '\0';
      }
    }

    /* Find the second TAB field separator(s) and zero them out so we are pointing
       at a string containing the third field */

    char *p2 = strchr(p1 + 1, '\t');

    if (p2 != NULL) {
      *p2 = '\0';

      while (p2[1] == '\t') {
        p2++;

        *p2 = '\0';
      }
    }

    /* If we have first and second fields then the first field is the reference
       identifier and the second field is the text associated with this identifier. */

    if (p1 != NULL) {
      CDReference *reference = new CDReference;

      reference->type       = CDOC_REF;
      reference->id         = line;
      reference->str        = &p1[1];
      reference->page_no    = -1;
      reference->no         = reference_list.size() + 1;
      reference->see_string = "";

      /* If we have a third field then this is the command which is executed
         for this reference. */

      if (p2 != NULL)
        reference->command = &p2[1];
      else
        reference->command = "";

      /* Add reference to list */

      reference_list.push_back(reference);
    }
  }
}

// Add a reference of the specified type to the current list of references.
//
// Multiply defined References are Ignored.
extern void
CDocScriptAddReference(int type, const std::string &id, const std::string &str,
                       const std::string &see_string)
{
  CDReference *reference = CDocScriptGetReference(id, type);

  /* Ignore Multiple definition of Reference from Reference File */

  if (reference != NULL && reference->type == CDOC_REF)
    reference = NULL;

  if (reference == NULL) {
    reference = new CDReference;

    reference->type       = type;
    reference->id         = id;
    reference->str        = str;
    reference->see_string = see_string;
    reference->page_no    = 0;
    reference->no         = reference_list.size() + 1;
    reference->command    = "";

    reference_list.push_back(reference);
  }
  else {
    if      (type == HEADER_REF)
      CDocScriptError("Header Reference %s Multiply defined", id.c_str());
    else if (type == LIST_ITEM_REF)
      CDocScriptError("List Item Reference %s Multiply defined", id.c_str());
    else if (type == FIGURE_REF)
      CDocScriptError("Figure Reference %s Multiply defined", id.c_str());
    else if (type == TABLE_REF)
      CDocScriptError("Table Reference %s Multiply defined", id.c_str());
    else
      CDocScriptError("Reference %s Multiply defined", id.c_str());
  }
}

// Set the specified references page number to the current page number so that
// it can be later output when paged output is requested.
extern void
CDocScriptSetReferencePageNumber(int type, const std::string &id)
{
  CDReference *reference = CDocScriptGetReference(id, type);

  if (reference != NULL) {
    if (cdoc_page_no != -1)
      reference->page_no = cdoc_page_no;
    else
      reference->page_no = cdoc_page_no_offset;
  }
}

// Get the Reference Data associated with a Reference Identifier string.
static CDReference *
CDocScriptGetReference(const std::string &id, int type)
{
  CDReference *reference1 = NULL;

  /* Search List of Reference Structures for one with
     an Identifier matching the supplied one */

  uint no_references = reference_list.size();

  for (uint i = 1; i <= no_references; i++) {
    CDReference *reference = reference_list[i - 1];

    /* If found Match from Reference File save as default
       but continue to look for reference from text */

    if (reference->type == CDOC_REF && CStrUtil::casecmp(reference->id, id) == 0)
      reference1 = reference;

    /* Found Match so Return Reference Structure */

    if (reference->type == type && CStrUtil::casecmp(reference->id, id) == 0)
      return(reference);
  }

  return(reference1);
}

// Get the text which is to be output when a cross reference is made.
//
// Should only produce a page number if reference is on different page to
// the text referred to.
extern std::string
CDocScriptGetParagraphReferenceText(CDCrossRef *cross_ref)
{
  CDReference *reference = CDocScriptGetReference(cross_ref->refid, cross_ref->type);

  /*-----*/

  if (reference == NULL) {
    if      (cross_ref->type == FIGURE_REF)
      CStrUtil::sprintf(reference_string, "-- Figure id \'%s\' unknown --", cross_ref->refid);
    else if (cross_ref->type == FOOTNOTE_REF)
      CStrUtil::sprintf(reference_string, "%s{00}%s", CDocStartBold(), CDocEndBold());
    else if (cross_ref->type == HEADER_REF)
      CStrUtil::sprintf(reference_string, "-- Heading id \'%s\' unknown --", cross_ref->refid);
    else if (cross_ref->type == LIST_ITEM_REF)
      CStrUtil::sprintf(reference_string, "-- LI \'%s\' --", cross_ref->refid);
    else if (cross_ref->type == TABLE_REF)
      CStrUtil::sprintf(reference_string, "-- Table id \'%s\' unknown --", cross_ref->refid);
    else
      CStrUtil::sprintf(reference_string, "-- id \'%s\' unknown --", cross_ref->refid);

    return reference_string;
  }

  /*-----*/

  std::string reference_string1;

  if      (cross_ref->type == LIST_ITEM_REF) {
    if (reference->str != "")
      reference_string1 = reference->str;
    else
      reference_string1 = reference->see_string;
  }
  else if (cross_ref->type == HEADER_REF) {
    CStrUtil::sprintf(reference_string, "\"%s\"", reference->str.c_str());

    reference_string1 = reference_string;
  }
  else
    reference_string1 = reference->str;

  /*-----*/

  if (cdoc_page_numbering && cross_ref->page &&
      CDocIsPagedOutput() && reference->page_no != -1) {
    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
      if (reference->page_no == 0)
        CStrUtil::sprintf(page_reference_string,
                          "%s##cdoc_ref_pg%d##B%s on page ##cdoc_ref_pg%d##%sE",
                          ESCG, reference->no, reference_string1.c_str(),
                          reference->no, ESCG);
      else
        CStrUtil::sprintf(page_reference_string, "%s%dB%s on page %d%sE",
                          ESCG, reference->page_no, reference_string1.c_str(),
                          reference->page_no, ESCG);
    }
    else {
      if (reference->page_no == 0)
        CStrUtil::sprintf(page_reference_string, "%s on page ##cdoc_ref_pg%d##",
                          reference_string1.c_str(), reference->no);
      else
        CStrUtil::sprintf(page_reference_string, "%s on page %d",
                          reference_string1.c_str(), reference->page_no);
    }

    reference_string1 = page_reference_string;
  }

  /*-----*/

  return reference_string1;
}

// Add a Reference to the Reference Command List if it appears the
// Reference Command File.
extern void
CDocScriptUpdateReferenceCommands(CDCrossRef *cross_ref)
{
  CDReference *reference = CDocScriptGetReference(cross_ref->refid, cross_ref->type);

  if (reference == NULL)
    return;

  /* If Reference has an associated Command (Specified in
     the Reference File) then save this Reference (if not
     already saved) */

  if (reference->command != "") {
    CDReferenceList::iterator p =
      find(reference_command_list.begin(), reference_command_list.end(), reference);

    if (p == reference_command_list.end())
      reference_command_list.push_back(reference);
  }
}

// Get Reference Text String
extern std::string
CDocScriptGetReferenceText(CDCrossRef *cross_ref)
{
  CDReference *reference = CDocScriptGetReference(cross_ref->refid, cross_ref->type);

  if      (reference->type == FIGURE_REF)
    return "figref_" + reference->id;
  else if (reference->type == FOOTNOTE_REF)
    return "fnref_" + reference->id;
  else if (reference->type == HEADER_REF)
    return "hdref_" + reference->id;
  else if (reference->type == LIST_ITEM_REF)
    return "liref_" + reference->id;
  else if (reference->type == TABLE_REF)
    return "tref_" + reference->id;
  else if (reference->type == INDEX_REF)
    return "iref_" + reference->id;
  else
    return "ref_" + reference->id;
}

// Get the Page String for a Reference i.e. on which Page the Command with the
// supplied Reference number was defined.
extern std::string
CDocScriptGetReferencePage(int reference_no)
{
  CDReference *reference = reference_list[reference_no - 1];

  /* Found Match so Return Reference Structure */

  if (reference != NULL)
    return CStrUtil::toString(reference->page_no);
  else
    return "??";
}

// Terminate Reference System when the end of the IBM Script File has been reached.
//
// Any Reference Commands are Output and allocated resources are freed off.
extern void
CDocScriptTermReferences()
{
  /* Add Commands from the Reference File to the end of
     the output so that the CDoc help display panel can
     add them to the top of the panel. */

  int no_commands = reference_command_list.size();

  for (int i = 1; i <= no_commands; i++) {
    CDReference *reference = reference_command_list[i - 1];

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
      CDocScriptWriteCommand(CDOC_COMMAND_TMPL, reference->str.c_str(),
                             reference->command.c_str());
  }

  /* Clean up */

  reference_command_list.clear();

  for_each(reference_list.begin(), reference_list.end(), CDeletePointer());

  reference_list.clear();

  /* Re-initialise to Initial Values */

  page_reference_string = "";
}
