#include "CDocI.h"

/*----------------------------------------------------------------*/

struct CDMemoHeader {
  typedef std::vector<char *> ToList;
  typedef std::vector<char *> CList;

  char   from[64];
  char   ref[64];
  char   date[64];
  char   tel[64];
  char   location[64];
  char   function[64];
  ToList to_list;
  CList  c_list;
};

/*----------------------------------------------------------------*/

static CDMemoHeader memo_header;

/*----------------------------------------------------------------*/

static void CDocScriptOutputMemoLine1
             (const char *, const char *);
static void CDocScriptOutputMemoLine2
             (const char *, const char *, const char *, const char *);

// Start a Memo Header part of the document. Initialize the Memo Header structure
// used to store the supplied data before formatting the output.
extern void
CDocScriptInitMemoHeader()
{
  cdoc_document.part     = CDOC_MEMO_HEADER_PART;
  cdoc_document.sub_part = CDOC_NO_SUB_PART;

  /* Initialise the Memo Header Structure Values */

  strcpy(memo_header.from    , "");
  strcpy(memo_header.ref     , "");
  strcpy(memo_header.date    , "");
  strcpy(memo_header.tel     , "");
  strcpy(memo_header.location, "");
  strcpy(memo_header.function, "");
}

// Process a Colon Command found in the Memo Header section.
extern void
CDocScriptProcessMemoHeaderCommand(CDColonCommand *colon_command)
{
  /* Process Memo Header Commands */

  /* Set From */

  if      (strcmp(colon_command->getCommand().c_str(), "from") == 0) {
    strcpy(memo_header.from, colon_command->getText().c_str());
  }

  /* Set Reference */

  else if (strcmp(colon_command->getCommand().c_str(), "ref") == 0) {
    strcpy(memo_header.ref, colon_command->getText().c_str());
  }

  /* Set Date */

  else if (strcmp(colon_command->getCommand().c_str(), "date") == 0) {
    if (colon_command->getText()== "")
      strcpy(memo_header.date, cdoc_date.c_str());
    else
      strcpy(memo_header.date, colon_command->getText().c_str());
  }

  /* Set Telephone Number */

  else if (strcmp(colon_command->getCommand().c_str(), "tel") == 0) {
    strcpy(memo_header.tel, colon_command->getText().c_str());
  }

  /* Set Location */

  else if (strcmp(colon_command->getCommand().c_str(), "location") == 0) {
    strcpy(memo_header.location, colon_command->getText().c_str());
  }

  /* Set Function */

  else if (strcmp(colon_command->getCommand().c_str(), "function") == 0) {
    strcpy(memo_header.function, colon_command->getText().c_str());
  }

  /* Add To to List of To Lines */

  else if (strcmp(colon_command->getCommand().c_str(), "to") == 0) {
    memo_header.to_list.push_back(CStrUtil::strdup(colon_command->getText().c_str()));
  }

  /* Add Copies to List of Copies Lines */

  else if (strcmp(colon_command->getCommand().c_str(), "c") == 0) {
    memo_header.c_list.push_back(CStrUtil::strdup(colon_command->getText().c_str()));
  }

  /* End Memo Header */

  else if (strcmp(colon_command->getCommand().c_str(), "emh") == 0) {
    CDocScriptOutputMemoHeader();

    cdoc_document.part     = CDOC_NO_PART;
    cdoc_document.sub_part = CDOC_NO_SUB_PART;
  }
  else
    CDocScriptWarning("Invalid Memo Header Command - %s %s",
                      colon_command->getCommand().c_str(), colon_command->getText().c_str());
}

// Output the Memo Header.
//
// This code is far too complicated and needs to be cleaned up.
extern void
CDocScriptOutputMemoHeader()
{
  int    c_index   = 0;
  int    to_index  = 0;
  char  *c_string  = NULL;
  char  *to_string = NULL;

  /* Output Memo Header */

  /* Switch of Formatting and set Indent for Troff */

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteCommand(".nf\n");
    CDocScriptWriteCommand(".in %.1lfm\n", CDocCharsToEms(cdoc_left_margin));
  }

  /* Set Tab Stops */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".ta 3.6i\n");

  /* Output Title Line */

  CDocScriptOutputMemoLine1("internal memorandum",
                            "EDS c/o Rolls-Royce plc.");

  CDocScriptOutputMemoLine1(NULL, NULL);

  /* Output Location and Function Lines */

  CDocScriptOutputMemoLine1(NULL, memo_header.location);
  CDocScriptOutputMemoLine1(NULL, memo_header.function);

  CDocScriptOutputMemoLine1(NULL, NULL);

  /* Set Tab Stops */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".ta 2m +3.6i +1m\n");

  /* Get First To and Copies String */

  c_index  = 1;
  to_index = 1;

  if (to_index - 1 < int(memo_header.to_list.size()))
    to_string = memo_header.to_list[to_index - 1];
  else
    to_string = NULL;

  if (to_string && to_string[0] != '\0')
    to_index++;

  if (c_index - 1 < int(memo_header.c_list.size()))
    c_string = memo_header.c_list[c_index - 1];
  else
    c_string = NULL;

  if (c_string && c_string[0] != '\0')
    c_index++;

  /* Output From and To Line if To Specified, otherwise
     Output From and Copies Line if Copies Specified,
     otherwise just output From Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2("from", memo_header.from, "to", to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    CDocScriptOutputMemoLine2("from", memo_header.from, "c", c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine2("from", memo_header.from, NULL, NULL);

  /* Output To Line if To Specified, otherwise Output
     Copies Line if Copies Specified, otherwise Output
     Blank Line */

  if      (to_string != NULL) {
    CDocScriptOutputMemoLine2(NULL, NULL, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2(NULL, NULL, "c", c_string);
    else
      CDocScriptOutputMemoLine2(NULL, NULL, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine1(NULL, NULL);

  /* Output Reference and To Line if To Specified, otherwise
     Output Reference and Copies Line if Copies Specified,
     otherwise Output Reference Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2("ref", memo_header.ref, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2("ref", memo_header.ref, "c", c_string);
    else
      CDocScriptOutputMemoLine2("ref", memo_header.ref, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine2("ref", memo_header.ref, NULL, NULL);

  /* Output To Line if To Specified, otherwise Output
     Copies Line if Copies Specified, otherwise Output
     Blank Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2(NULL, NULL, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2(NULL, NULL, "c", c_string);
    else
      CDocScriptOutputMemoLine2(NULL, NULL, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine1(NULL, NULL);

  /* Output Date and To Line if To Specified, otherwise
     Output Date and Copies Line if Copies Specified,
     otherwise Output Date Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2("date", memo_header.date, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2("date", memo_header.date, "c", c_string);
    else
      CDocScriptOutputMemoLine2("date", memo_header.date, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine2("date", memo_header.date, NULL, NULL);

  /* Output To Line if To Specified, otherwise Output
     Copies Line if Copies Specified, otherwise Output
     Blank Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2(NULL, NULL, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2(NULL, NULL, "c", c_string);
    else
      CDocScriptOutputMemoLine2(NULL, NULL, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine1(NULL, NULL);

  /* Output Telephone Number and To Line if To Specified,
     otherwise Output Telephone Number and Copies Line if
     Copies Specified, otherwise Output Telephone Number Line */

  if      (to_string && to_string[0] != '\0') {
    CDocScriptOutputMemoLine2("tel", memo_header.tel, NULL, to_string);

    /* Get Next To String */

    if (to_index - 1 < int(memo_header.to_list.size()))
      to_string = memo_header.to_list[to_index - 1];
    else
      to_string = NULL;

    if (to_string && to_string[0] != '\0')
      to_index++;
  }
  else if (c_string && c_string[0] != '\0') {
    if (c_index == 2)
      CDocScriptOutputMemoLine2("tel", memo_header.tel, "c", c_string);
    else
      CDocScriptOutputMemoLine2("tel", memo_header.tel, NULL, c_string);

    /* Get Next Copies String */

    if (c_index - 1 < int(memo_header.c_list.size()))
      c_string = memo_header.c_list[c_index - 1];
    else
      c_string = NULL;

    if (c_string && c_string[0] != '\0')
      c_index++;
  }
  else
    CDocScriptOutputMemoLine2("tel", memo_header.tel, NULL, NULL);

  /* Output To Line if To Specified, otherwise Output
     Copies Line if Copies Specified, otherwise Output
     Blank Line */

  while (to_string && to_string[0] != '\0' && c_string && c_string[0] != '\0') {
    if      (to_string && to_string[0] != '\0') {
      CDocScriptOutputMemoLine2(NULL, NULL, NULL, to_string);

      /* Get Next To String */

      if (to_index - 1 < int(memo_header.to_list.size()))
        to_string = memo_header.to_list[to_index - 1];
      else
        to_string = NULL;

      if (to_string && to_string[0] != '\0')
        to_index++;
    }
    else if (c_string && c_string[0] != '\0') {
      if (c_index == 2)
        CDocScriptOutputMemoLine2(NULL, NULL, "c", c_string);
      else
        CDocScriptOutputMemoLine2(NULL, NULL, NULL, c_string);

      /* Get Next Copies String */

      if (c_index - 1 < int(memo_header.c_list.size()))
        c_string = memo_header.c_list[c_index - 1];
      else
        c_string = NULL;

      if (c_string && c_string[0] != '\0')
        c_index++;
    }
  }

  CDocScriptOutputMemoLine1(NULL, NULL);

  /* Start Formatting (for Troff) */

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".fi\n");
}

// Output the Memo Line Line consisting of two text values.
//
// Used for unlabelled text lines like:
//
//   (1)                    (2)
//   internal memorandum    EDS c/o Rolls-Royce plc.
//                          Engineering Systems
//                          B1-24a Moor Lane
static void
CDocScriptOutputMemoLine1(const char *string1, const char *string2)
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if      (string1 != NULL && string2 != NULL)
      CDocScriptWriteText("%s\t%s\n", string1, string2);
    else if (string1 != NULL)
      CDocScriptWriteText("%s\n", string1);
    else if (string2 != NULL)
      CDocScriptWriteText("\t%s\n", string2);
    else
      CDocScriptNewLine();
  }
  else if     (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    if (string1 != NULL && string2 != NULL) {
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%s\n", string1);

      CDocScriptWriteCommand(CDOC_SET_LINE_TMPL, -1);

      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("%s\n", string2);
    }
    else if (string1 != NULL) {
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%s\n", string1);
    }
    else if (string2 != NULL) {
      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("%s\n", string2);
    }
    else
      CDocScriptNewLine();
  }
  else {
    int  i;

    if      (string1 != NULL && string2 != NULL) {
      int  len;

      len = strlen(string1);

      CDocScriptWriteText("%s", string1);

      for (i = len; i < 32; i++)
        CDocScriptWriteText(" ");

      CDocScriptWriteText("%s\n", string2);
    }
    else if (string1 != NULL)
      CDocScriptWriteText("%s\n", string1);
    else if (string2 != NULL) {
      for (i = 0 ; i < 32; i++)
        CDocScriptWriteText(" ");

      CDocScriptWriteText("%s\n", string2);
    }
    else
      CDocScriptNewLine();
  }
}

// Output the Memo Line Line consisting of four text values.
//
// Used for labelled text lines like
//
// (1)    (2)                     (3) (4)
// from : Colin Williams          to: Fred Bloggs
// data : 25th Dec 1996           c : Nobody
static void
CDocScriptOutputMemoLine2(const char *string1, const char *string2, const char *string3,
                          const char *string4)
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if      (string1 != NULL && string2 != NULL &&
             string3 != NULL && string4 != NULL)
      CDocScriptWriteText("%s\t: %-24s\t%s\t: %s\n",
                          string1, string2, string3, string4);
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 != NULL)
      CDocScriptWriteText("%s\t: %-24s\t\t  %s\n",
                         string1, string2, string4);
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 == NULL)
      CDocScriptWriteText("%s\t: %s\n", string1, string2);
    else if (string1 == NULL && string2 == NULL &&
             string3 != NULL && string4 != NULL)
      CDocScriptWriteText("\t\t%s\t: %s\n", string3, string4);
    else if (string1 == NULL && string2 == NULL &&
             string3 == NULL && string4 != NULL)
      CDocScriptWriteText("\t\t\t  %s\n", string4);
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    if      (string1 != NULL && string2 != NULL &&
             string3 != NULL && string4 != NULL) {
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%-4s: %s\n", string1, string2);

      CDocScriptWriteCommand(CDOC_SET_LINE_TMPL, -1);

      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("%-4s: %s\n", string3, string4);
    }
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 != NULL) {
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%-4s: %s\n", string1, string2);

      CDocScriptWriteCommand(CDOC_SET_LINE_TMPL, -1);

      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("      %s\n", string4);
    }
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 == NULL) {
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%-4s: %s\n", string1, string2);
    }
    else if (string1 == NULL && string2 == NULL &&
             string3 != NULL && string4 != NULL) {
      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("%-4s: %s\n", string3, string4);
    }
    else if (string1 == NULL && string2 == NULL &&
             string3 == NULL && string4 != NULL) {
      CDocScriptWriteIndent(cdoc_right_margin - 25);

      CDocScriptWriteText("      %s\n", string4);
    }
  }
  else {
    if      (string1 != NULL && string2 != NULL &&
             string3 != NULL && string4 != NULL)
      CDocScriptWriteText("%-4s: %-24s  %-4s: %s\n",
                          string1, string2, string3, string4);
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 != NULL)
      CDocScriptWriteText("%-4s: %-24s        %s\n",
                          string1, string2, string4);
    else if (string1 != NULL && string2 != NULL &&
             string3 == NULL && string4 == NULL)
      CDocScriptWriteText("%s: %s\n", string1, string2);
    else if (string1 == NULL && string2 == NULL &&
             string3 != NULL && string4 != NULL)
      CDocScriptWriteText("                                %-4s: %s\n",
                          string3, string4);
    else if (string1 == NULL && string2 == NULL &&
             string3 == NULL && string4 != NULL)
      CDocScriptWriteText("                                      %s\n",
                          string4);
  }
}

// Output the Memo Signature (at the end of the Memo).
//
// Should check whether memo has been initialised and signature exists.
extern void
CDocScriptOutputMemoSignature()
{
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptWriteLine("%s", memo_header.from);
  CDocScriptNewLine();
}

// Get the Memo Reference and Date details for use in the Memo Page Header.
extern void
CDocScriptGetMemoDetails(std::string &reference, std::string &date)
{
  reference = memo_header.ref;
  date      = memo_header.date;
}
