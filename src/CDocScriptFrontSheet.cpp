#include "CDocI.h"

/*----------------------------------------------------------------*/

struct CDFrontSheet {
  std::string       report;
  std::string       series;
  std::string       title;
  std::string       issue;
  std::string       typeref;
  std::string       author;
  std::string       tel;
  std::string       xtel;
  std::string       dept;
  std::string       date;
  int               no_circ;
  std::string       circ[20];
  int               in_summary;
  CDScriptTempFile *temp_file;
};

/*----------------------------------------------------------------*/

static CDFrontSheet front_sheet;

// Start a Front Sheet part of the document and Initialize the Front Sheet
// structure used to store the supplied data before formatting the output.
extern void
CDocScriptInitFrontSheet()
{
  cdoc_document.part     = CDOC_FRONT_SHEET_PART;
  cdoc_document.sub_part = CDOC_NO_SUB_PART;

  /* Initialise Front Sheet Structure Values */

  front_sheet.report  = "";
  front_sheet.series  = "";
  front_sheet.title   = "";
  front_sheet.issue   = "";
  front_sheet.typeref = "";
  front_sheet.author  = "";
  front_sheet.tel     = "";
  front_sheet.xtel    = "";
  front_sheet.dept    = "";
  front_sheet.date    = "";

  front_sheet.no_circ = 0;

  front_sheet.in_summary = false;
  front_sheet.temp_file  = NULL;
}

// Process a Colon Command found in the Front Sheet section.
extern int
CDocScriptProcessFrontSheetCommand(CDColonCommand *colon_command)
{
  /* Process Front Sheet Commands */

  /* Set Report Name */

  if      (strcmp(colon_command->getCommand().c_str(), "report") == 0) {
    front_sheet.report = colon_command->getText();

    CStrUtil::toUpper(front_sheet.report);
  }

  /* Set Report Series */

  else if (strcmp(colon_command->getCommand().c_str(), "series") == 0) {
    front_sheet.series = colon_command->getText();

    CStrUtil::toUpper(front_sheet.series);
  }

  /* Set Title (can be many lines long) */

  else if (strcmp(colon_command->getCommand().c_str(), "title") == 0) {
    /* Set title string if not set already, otherwise
       concatenate the str (with a Newline character)
       to the existing title str */

    if (front_sheet.title == "")
      front_sheet.title = colon_command->getText();
    else {
      front_sheet.title += "\n";
      front_sheet.title += colon_command->getText();
    }

    CStrUtil::toUpper(front_sheet.title);
  }

  /* Set the Issue */

  else if (strcmp(colon_command->getCommand().c_str(), "issue") == 0) {
    front_sheet.issue = colon_command->getText();

    CStrUtil::toUpper(front_sheet.issue);
  }

  /* Set the Typer's Reference */

  else if (strcmp(colon_command->getCommand().c_str(), "typeref") == 0) {
    front_sheet.typeref = colon_command->getText();
  }

  /* Set the Author */

  else if (strcmp(colon_command->getCommand().c_str(), "author") == 0) {
    front_sheet.author = colon_command->getText();
  }

  /* Set the Telephone Number */

  else if (strcmp(colon_command->getCommand().c_str(), "tel") == 0) {
    front_sheet.tel = colon_command->getText();
  }

  /* Set the External Telephone Number */

  else if (strcmp(colon_command->getCommand().c_str(), "xtel") == 0) {
    front_sheet.xtel = colon_command->getText();
  }

  /* Set the Department */

  else if (strcmp(colon_command->getCommand().c_str(), "dept") == 0) {
    front_sheet.dept = colon_command->getText();
  }

  /* Set the Date */

  else if (strcmp(colon_command->getCommand().c_str(), "date") == 0) {
    if (colon_command->getText() == "")
      front_sheet.date = cdoc_date;
    else
      front_sheet.date = colon_command->getText();
  }

  /* Ignore DNS */

  else if (strcmp(colon_command->getCommand().c_str(), "dns") == 0)
    ;

  /* Start the Summary as a New Paragraph */

  else if (strcmp(colon_command->getCommand().c_str(), "summary") == 0) {
    front_sheet.in_summary = true;

    /* Flush any existing paragraphs (shouldn't be any
       if correctly defined) */

    if (cdoc_in_paragraph)
      CDocScriptOutputParagraph();

    /* Start Outputting Text to Temporary File */

    front_sheet.temp_file = CDocScriptStartTempFile("Front Sheet");

    if (colon_command->getText() != "")
      CDocScriptNewParagraph(colon_command->getText(), NORMAL_PARAGRAPH,
                             true, LEFT_JUSTIFICATION);
  }

  /* Add a Circulation String (can be many lines long) */

  else if (strcmp(colon_command->getCommand().c_str(), "circ") == 0) {
    front_sheet.circ[front_sheet.no_circ++] = colon_command->getText();
  }

  /* End the Front Sheet */

  else if (strcmp(colon_command->getCommand().c_str(), "efronts") == 0) {
    CDocScriptOutputFrontSheet();

    cdoc_document.part     = CDOC_NO_PART;
    cdoc_document.sub_part = CDOC_NO_SUB_PART;
  }

  /* Start Amendments Part */

  else if (strcmp(colon_command->getCommand().c_str(), "amends") == 0) {
    CDocScriptOutputFrontSheet();

    CDocScriptInitAmendments();

    return true;
  }

  /* Start Body Part */

  else if (strcmp(colon_command->getCommand().c_str(), "body") == 0) {
    CDocScriptOutputFrontSheet();

    cdoc_document.part     = CDOC_BODY_PART;
    cdoc_document.sub_part = CDOC_NO_SUB_PART;
  }

  /* Process any other Command (In the Summary Section) as a
     Normal Colon Command */

  else {
    /* If In Summary then allow normal Script Commands to
       be processed */

    if (front_sheet.in_summary)
      return false;
    else
      CDocScriptWarning("Invalid Front Sheet Command - %s %s",
                        colon_command->getCommand().c_str(), colon_command->getText().c_str());
  }

  return true;
}

// Output Front Sheet.
//
// Uses hard coded lookup of document series from first three characters of
// document report name.
//
// Should use lookup file to allow for future extension without the need to
// recompile.
extern void
CDocScriptOutputFrontSheet()
{
  char temp_string[CDOC_MAX_LINE];

  /* End Outputting Text to Temporary File */

  if (front_sheet.temp_file != NULL)
    CDocScriptEndTempFile(front_sheet.temp_file);

  front_sheet.in_summary = false;

  /* If Series not set then use first three characters of
     Report name */

  if (front_sheet.series == "" && front_sheet.report == "") {
    if      (strncmp(front_sheet.report.c_str(), "AAS", 3) == 0)
      front_sheet.series = "ACCEPTANCE TEST SPECIFICATION";
    else if (strncmp(front_sheet.report.c_str(), "ADN", 3) == 0)
      front_sheet.series = "ADR3 NOTE";
    else if (strncmp(front_sheet.report.c_str(), "CPG", 3) == 0)
      front_sheet.series = "COMPUTER PROGRAMMERS' GUIDE";
    else if (strncmp(front_sheet.report.c_str(), "CPR", 3) == 0)
      front_sheet.series = "COMPUTER PROGRAMMER'S REPORT";
    else if (strncmp(front_sheet.report.c_str(), "CSE", 3) == 0)
      front_sheet.series = "COMPUTING STANDARDS FOR ENGINNERING";
    else if (strncmp(front_sheet.report.c_str(), "CSN", 3) == 0)
      front_sheet.series = "COMPUTER SYSTEMS NOTE";
    else if (strncmp(front_sheet.report.c_str(), "CSR", 3) == 0)
      front_sheet.series = "COMPUTER SYSTEMS REPORT";
    else if (strncmp(front_sheet.report.c_str(), "CUG", 3) == 0)
      front_sheet.series = "COMPUTER USERS' GUIDE";
    else if (strncmp(front_sheet.report.c_str(), "CWE", 3) == 0)
      front_sheet.series = "COMPUTING WORK INSTRUCTIONS FOR ENG G";
    else if (strncmp(front_sheet.report.c_str(), "GIP", 3) == 0)
      front_sheet.series = "GENERAL INSTRUCTIONS TO PROGRAMMERS";
    else if (strncmp(front_sheet.report.c_str(), "POP", 3) == 0)
      front_sheet.series = "PROGRAM OPERATING PROCEDURE";
    else if (strncmp(front_sheet.report.c_str(), "SPS", 3) == 0)
      front_sheet.series = "SYSTEM PROPOSAL OR SPECIFICATION";
    else
      CDocScriptWarning("Unknown Front Sheet Report Type - %s", front_sheet.report.c_str());
  }

  /* Output Front Sheet in HTML Format */

  if       (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    cdoc_page_no++;

    /* Write Typeref */

    CDocScriptWriteCommand("<cite>%s</cite>\n", front_sheet.typeref.c_str());
    CDocScriptWriteCommand("<p>\n");

    CDocScriptWriteCommand("<table width='100%%' border>\n");

    /* Write Series, Report and Issue */

    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<td colspan=7>%s</td>\n", front_sheet.series.c_str());
    CDocScriptWriteCommand("<td colspan=2>%s</td>\n", front_sheet.report.c_str());
    CDocScriptWriteCommand("<td colspan=1>%s</td>\n", front_sheet.issue.c_str());
    CDocScriptWriteCommand("</tr>\n");

    /* Write Centred Line for each Title Line */

    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<td colspan=10 height=40>\n");

    const char *p1 = front_sheet.title.c_str();

    const char *p2;

    while ((p2 = strchr(p1, '\n')) != NULL) {
      std::string line = std::string(p1, p2 - p1);

      CDocScriptWriteCommand("%s\n", line.c_str());
      CDocScriptWriteCommand("<br>\n");

      p1 = p2 + 1;
    }

    CDocScriptWriteCommand("%s\n", p1);

    CDocScriptWriteCommand("</tr>\n");

    /* Write Author and Telephone Number */

    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<td colspan=7>%s</td>\n", front_sheet.author.c_str());
    CDocScriptWriteCommand("<td colspan=3>%s %s</td>\n", "Tel:", front_sheet.tel.c_str());
    CDocScriptWriteCommand("</tr>\n");

    CDocScriptWriteCommand("</table>\n");

    /* Write Summary Header */

    CDocScriptWriteCommand("<p>\n");
    CDocScriptWriteCommand("<table width='100%%' height=400 border>\n");
    CDocScriptWriteCommand("<tr>\n");
    CDocScriptWriteCommand("<td valign=top>\n");

    CDocScriptWriteCommand("<h3>Summary</h3>\n");

    CDocScriptWriteCommand("<br>\n");
  }

  /* Output Front Sheet in other Formats */

  else {
    /* Start New Page */

    CDocScriptNewPage();

    /* Write Typeref */

    CDocScriptWriteLine("%s", front_sheet.typeref.c_str());

    CDocScriptNewLine();

    /* Write Centred Line containing Series, Report and Issue */

    sprintf(temp_string, "%s    %s    Issue %s",
            front_sheet.series.c_str(), front_sheet.report.c_str(), front_sheet.issue.c_str());

    CDocScriptWriteCentreJustifiedLine(temp_string, CDOC_BOLD_FONT);

    CDocScriptNewLine();

    /* Write Centred Line for each Title Line */

    const char *p1 = front_sheet.title.c_str();

    const char *p2;

    while ((p2 = strchr(p1, '\n')) != NULL) {
      std::string line(p1, p2 - p1);

      CDocScriptWriteCentreJustifiedLine(line.c_str(), CDOC_BOLD_FONT);

      p1 = p2 + 1;
    }

    CDocScriptWriteCentreJustifiedLine(p1, CDOC_BOLD_FONT);

    CDocScriptNewLine();
    CDocScriptNewLine();

    /* Write Centred Line containing Author and Telephone Number */

    sprintf(temp_string, "Author: %s  Tel: %s",
            front_sheet.author.c_str(), front_sheet.tel.c_str());

    CDocScriptWriteCentreJustifiedLine(temp_string, CDOC_BOLD_FONT);

    CDocScriptNewLine();

    /* Write Summary Header */

    CDocScriptWriteLine("%sSummary%s", CDocStartBold(), CDocEndBold());

    CDocScriptNewLine();
  }

  /* Output Text in Temporary File */

  if (front_sheet.temp_file != NULL)
    CDocScriptOutputTempFile(front_sheet.temp_file);

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("</td>\n");
    CDocScriptWriteCommand("</tr>\n");
    CDocScriptWriteCommand("</table>\n");
    CDocScriptWriteCommand("<p>\n");
  }
  else
    CDocScriptNewLine();

  /* Clean Up */

  if (front_sheet.temp_file != NULL) {
    delete front_sheet.temp_file;

    front_sheet.temp_file = NULL;
  }
}
