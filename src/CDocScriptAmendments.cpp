#include <CDocI.h>
#include <sys/stat.h>

/*----------------------------------------------------------------*/

#define DUMMY_AMENDMENT  0
#define AMENDMENT_ISSUED 1
#define AMENDMENT_ADDED  2

struct CDAmendmentData {
  int   type;
  char *issue;
  char *amend;
  char *detail;
  char *author;
  char *date;
  char *approve;
  char *agree;
  char *text;
};

struct CDAmendment {
  CDAmendmentData data;

  CDAmendment() {
    data.type    = DUMMY_AMENDMENT;
    data.issue   = NULL;
    data.amend   = NULL;
    data.detail  = NULL;
    data.author  = NULL;
    data.date    = NULL;
    data.approve = NULL;
    data.agree   = NULL;
    data.text    = NULL;
  }

 ~CDAmendment() {
    delete [] data.issue  ;
    delete [] data.amend  ;
    delete [] data.detail ;
    delete [] data.author ;
    delete [] data.date   ;
    delete [] data.approve;
    delete [] data.agree  ;
    delete [] data.text   ;
  }
};

/*----------------------------------------------------------------*/

typedef std::vector<CDAmendment *> CDAmendmentList;

static CDAmendmentList   amendment_list;
static CDScriptTempFile *amendments_temp_file = NULL;

/*----------------------------------------------------------------*/

/* Ammendment Parameter Definition */

#define AMD_OFFSET(a) CDocOffset(CDAmendmentData*,a)

static CDParameterData
amd_parameter_data[] = {
  { "issue"  , PARM_NEW_STR, NULL, AMD_OFFSET(issue  ), },
  { "amend"  , PARM_NEW_STR, NULL, AMD_OFFSET(amend  ), },
  { "detail" , PARM_NEW_STR, NULL, AMD_OFFSET(detail ), },
  { "author" , PARM_NEW_STR, NULL, AMD_OFFSET(author ), },
  { "date"   , PARM_NEW_STR, NULL, AMD_OFFSET(date   ), },
  { "approve", PARM_NEW_STR, NULL, AMD_OFFSET(approve), },
  { "agree"  , PARM_NEW_STR, NULL, AMD_OFFSET(agree  ), },
  { NULL     , 0           , NULL, 0                  , },
};

/*----------------------------------------------------------------*/

static void   CDocScriptAmendmentWriteLine
               (CDAmendment *, int, int);
static void   CDocScriptAmendmentWriteText
               (char *);
static char  *CDocScriptAmendmentsGetText
               ();

// Initialise Amendments Control Variables ready to process amendments commands.
extern void
CDocScriptInitAmendments()
{
  cdoc_document.part     = CDOC_AMENDMENTS_PART;
  cdoc_document.sub_part = CDOC_NO_SUB_PART;

  /* Start Outputting Text to Temporary File */

  amendments_temp_file = CDocScriptStartTempFile("Amendments");

  cdoc_left_margin  = 0;
  cdoc_right_margin = 38;
  cdoc_indent       = 11;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_indent);
}

// Process a Colon Command found in the Amendments section.
extern void
CDocScriptProcessAmendmentsCommand(CDColonCommand *colon_command)
{
  /* Flush Paragraph */

  if (cdoc_in_paragraph)
    CDocScriptOutputParagraph();

  /* Get Text in Amendments Temporary File to add to previous Amendment */

  char *text = CDocScriptAmendmentsGetText();

  if (text != NULL) {
    CDAmendment *amendment = NULL;

    if (! amendment_list.empty())
      amendment = amendment_list.back();

    if (amendment == NULL || amendment->data.text != NULL) {
      amendment = new CDAmendment;

      amendment_list.push_back(amendment);
    }

    amendment->data.text = text;
  }

  /* If Issued or Amended Command then add Details to Amendments List */

  if      (strcmp(colon_command->getCommand().c_str(), "i") == 0 ||
           strcmp(colon_command->getCommand().c_str(), "a") == 0) {
    /* Create and Initialise Amendments Structure */

    CDAmendment *amendment = new CDAmendment;

    if (strcmp(colon_command->getCommand().c_str(), "i") == 0)
      amendment->data.type = AMENDMENT_ISSUED;
    else
      amendment->data.type = AMENDMENT_ADDED;

    amendment_list.push_back(amendment);

    /* Process Parameter/Value pairs to set the Amendments Structure */

    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               amd_parameter_data,
                               (char *) amendment);

    if (amendment->data.detail != NULL)
      CStrUtil::toUpper(amendment->data.detail);
  }

  /* If we have and End Amendments Command (either directly or
     indirectly) then Output current Amendments List */

  else if (strcmp(colon_command->getCommand().c_str(), "eamends" ) == 0 ||
           strcmp(colon_command->getCommand().c_str(), "body"    ) == 0 ||
           strcmp(colon_command->getCommand().c_str(), "appendix") == 0) {
    CDocScriptTermAmendments();

    /* Start new part as appropriate */

    if      (strcmp(colon_command->getCommand().c_str(), "body") == 0)
      cdoc_document.part = CDOC_BODY_PART;
    else if (strcmp(colon_command->getCommand().c_str(), "appendix") == 0)
      cdoc_document.part = CDOC_APPENDIX_PART;
  }
  else
    CDocScriptWarning("Invalid Amendments Command - %s %s",
                      colon_command->getCommand().c_str(), colon_command->getText().c_str());
}

// Terminate Amendments Section outputing Buffered amendments text.
extern void
CDocScriptTermAmendments()
{
  /* Flush Paragraph */

  if (cdoc_in_paragraph)
    CDocScriptOutputParagraph();

  /* Get Text in Amendments Temporary File to add to previous Amendment */

  char *text = CDocScriptAmendmentsGetText();

  if (text != NULL) {
    CDAmendment *amendment = amendment_list.back();

    if (amendment == NULL || amendment->data.text != NULL) {
      amendment = new CDAmendment;

      amendment_list.push_back(amendment);
    }

    amendment->data.text = text;
  }

  /*------------*/

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(amendments_temp_file);

  delete amendments_temp_file;

  /*------------*/

  /* Write Header */

  CDocScriptWriteCentreJustifiedPageHeader("List of Amendments");

  CDocScriptSkipLine();

  int no_amendments = amendment_list.size();

  /* Output Amendments List */

  /* Set up Tab Stops */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".ta 0.2i 0.5i +2i +1i +1i\n");

  /* Output Column Headers */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteCommand(".BD \"\t\tDetails\tAuthor\tDate\tApprove\"\n");

    CDocScriptSkipLine();
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptWriteText(FILL_TMPL,  9,  9, ' ');

    CDocScriptWriteText("%sDetails%s", CDocStartUnderline(), CDocEndUnderline());

    CDocScriptWriteText(FILL_TMPL, 40, 40, ' ');

    CDocScriptWriteText("%sAuthor%s", CDocStartUnderline(), CDocEndUnderline());

    CDocScriptWriteText(FILL_TMPL, 51, 51, ' ');

    CDocScriptWriteText("%sDate%s", CDocStartUnderline(), CDocEndUnderline());

    CDocScriptWriteText(FILL_TMPL, 62, 62, ' ');

    CDocScriptWriteText("%sApprove%s", CDocStartUnderline(), CDocEndUnderline());

    CDocScriptWriteText("\n");

    CDocScriptSkipLine();
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    CDocScriptWriteLine("%s       %-30s %-10s %-10s %-20s%s", CDocStartUnderline(),
                        "Details", "Author", "Date", "Approve", CDocEndUnderline());

    CDocScriptSkipLine();
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<p>\n");
    CDocScriptWriteCommand("<table width='100%%' border>\n");
    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<th>&nbsp;</th>\n");
    CDocScriptWriteCommand("<th>Details</th>\n");
    CDocScriptWriteCommand("<th>Author</th>\n");
    CDocScriptWriteCommand("<th>Date</th>\n");
    CDocScriptWriteCommand("<th>Approve</th>\n");
    CDocScriptWriteCommand("</tr>\n");
  }
  else {
    CDocScriptWriteLine("       %-30s %-10s %-10s %-20s",
                        "Details", "Author", "Date", "Approve");

    CDocScriptSkipLine();
  }

  /* Output each Amendment Line */

  int issue_no = 0;
  int amend_no = 0;

  for (int i = 1; i <= no_amendments; i++) {
    CDAmendment *amendment = amendment_list[i - 1];

    if (amendment->data.type == DUMMY_AMENDMENT)
      CDocScriptAmendmentWriteText(amendment->data.text);
    else {
      if (amendment->data.type == AMENDMENT_ISSUED) {
        issue_no++;

        amend_no = 0;
      }
      else
        amend_no++;

      if ((issue_no > 0 || amend_no > 0) && CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML)
        CDocScriptSkipLine();

      CDocScriptAmendmentWriteLine(amendment, issue_no, amend_no);
    }
  }

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("</table>\n");
    CDocScriptWriteCommand("<p>\n");
  }

  /* Delete Amendments Structures and List */

  for (int i = 1; i <= no_amendments; i++) {
    CDAmendment *amendment = amendment_list[i - 1];

    delete amendment;
  }

  amendment_list.clear();

  /* End Amendments Document Part */

  cdoc_document.part     = CDOC_NO_PART;
  cdoc_document.sub_part = CDOC_NO_SUB_PART;
}

// Write an Amendments line from the data stored in the Amendments structure.
static void
CDocScriptAmendmentWriteLine(CDAmendment *amendment, int issue_no, int amend_no)
{
  char issue_string[16];

  if (amend_no > 0)
    sprintf(issue_string, "%d%c", issue_no, amend_no + 'A' - 1);
  else
    sprintf(issue_string, "%d%c", issue_no, ' ');

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteText("\t");

    if (amendment->data.issue != NULL)
      CDocScriptWriteText("%s", amendment->data.issue);
    else
      CDocScriptWriteText("%s", issue_string);

    CDocScriptWriteText("\t");

    if (amendment->data.detail != NULL)
      CDocScriptWriteText("%s", amendment->data.detail);
    else
      CDocScriptWriteText("");

    CDocScriptWriteText("\t");

    if (amendment->data.author != NULL)
      CDocScriptWriteText("%s", amendment->data.author);
    else
      CDocScriptWriteText("");

    CDocScriptWriteText("\t");

    if (amendment->data.date != NULL)
      CDocScriptWriteText("%s", amendment->data.date);
    else
      CDocScriptWriteText("");

    CDocScriptWriteText("\t");

    if (amendment->data.approve != NULL)
      CDocScriptWriteText("%s", amendment->data.approve);
    else
      CDocScriptWriteText("");

    CDocScriptWriteText("\n");
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptWriteText(FILL_TMPL,  4,  4, ' ');

    CDocScriptWriteText("%-4s",
      amendment->data.issue != NULL ? amendment->data.issue : issue_string);

    CDocScriptWriteText(FILL_TMPL,  9,  9, ' ');

    CDocScriptWriteText("%-30s",
      amendment->data.detail != NULL ? amendment->data.detail : "");

    CDocScriptWriteText(FILL_TMPL, 40, 40, ' ');

    CDocScriptWriteText("%-10s",
      amendment->data.author != NULL ? amendment->data.author : "");

    CDocScriptWriteText(FILL_TMPL, 51, 51, ' ');

    CDocScriptWriteText("%-10s",
      amendment->data.date != NULL ? amendment->data.date : "");

    CDocScriptWriteText(FILL_TMPL, 62, 62, ' ');

    CDocScriptWriteText("%-20s",
      amendment->data.approve != NULL ? amendment->data.approve : "");

    CDocScriptWriteText("\n");
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<td>%s</td>\n",
      amendment->data.issue   != NULL ? amendment->data.issue   : issue_string);
    CDocScriptWriteCommand("<td>%s</td>\n",
      amendment->data.detail  != NULL ? amendment->data.detail  : "&nbsp;"    );
    CDocScriptWriteCommand("<td>%s</td>\n",
      amendment->data.author  != NULL ? amendment->data.author  : "&nbsp;"    );
    CDocScriptWriteCommand("<td>%s</td>\n",
      amendment->data.date    != NULL ? amendment->data.date    : "&nbsp;"    );
    CDocScriptWriteCommand("<td>%s</td>\n",
      amendment->data.approve != NULL ? amendment->data.approve : "&nbsp;"    );
    CDocScriptWriteCommand("</tr>\n");
  }
  else {
    CDocScriptWriteLine("  %-4s %-30s %-10s %-10s %-20s",
      amendment->data.issue   != NULL ? amendment->data.issue   : issue_string,
      amendment->data.detail  != NULL ? amendment->data.detail  : "",
      amendment->data.author  != NULL ? amendment->data.author  : "",
      amendment->data.date    != NULL ? amendment->data.date    : "",
      amendment->data.approve != NULL ? amendment->data.approve : "");
  }

  if (amendment->data.text != NULL && amendment->data.text[0] != '\0')
    CDocScriptAmendmentWriteText(amendment->data.text);
}

// Write text associated with an amendment.
static void
CDocScriptAmendmentWriteText(char *text)
{
  char *p1;

  char *p = text;

  while ((p1 = strchr(p, '\n')) != NULL) {
    *p1 = '\0';

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
//    CDocScriptWriteText("\t\t");

      CDocScriptOutputTempFileLine(p);

//    CDocScriptWriteText("\n");
    }
    else {
//    CDocScriptWriteText("       ");

      CDocScriptOutputTempFileLine(p);
    }

    p = p1 + 1;
  }

  if (*p != '\0') {
    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
//    CDocScriptWriteText("\t\t");

      CDocScriptOutputTempFileLine(p);

//    CDocScriptWriteText("\n");
    }
    else {
//    CDocScriptWriteText("       ");

      CDocScriptOutputTempFileLine(p);
    }
  }

//CDocScriptSkipLine();
}

// Get any text which was defined between the amendments commands.
static char *
CDocScriptAmendmentsGetText()
{
  long        no_read;
  struct stat file_stat;

  char *text = NULL;

  /*-----------*/

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(amendments_temp_file);

  /*-----------*/

  int error = stat(amendments_temp_file->filename.c_str(), &file_stat);

  if (error == -1 || file_stat.st_size == 0)
    goto CDocScriptAmendmentsGetText_1;

  amendments_temp_file->fp = fopen(amendments_temp_file->filename.c_str(), "r");

  if (amendments_temp_file->fp == NULL)
    goto CDocScriptAmendmentsGetText_1;

  text = new char [file_stat.st_size + 1];

  no_read = fread(text, sizeof(char), file_stat.st_size + 1, amendments_temp_file->fp);

  text[no_read] = '\0';

  fclose(amendments_temp_file->fp);

  /*-----------*/

CDocScriptAmendmentsGetText_1:
  delete amendments_temp_file;

  /* Start Outputting Text to Temporary File */

  amendments_temp_file = CDocScriptStartTempFile("Amendments");

  cdoc_left_margin  = 0;
  cdoc_right_margin = 38;
  cdoc_indent       = 11;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_indent);

  return text;
}
