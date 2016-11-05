#include "CDocI.h"

/*----------------------------------------------------------------*/

struct CDTitlePage {
  typedef std::vector<std::string> AddressList;

  std::string title;
  std::string docnum;
  std::string date;
  std::string author;
  AddressList address_list;

  CDTitlePage() {
    title  = "";
    docnum = "";
    date   = "";
    author = "";
  }
};

/*----------------------------------------------------------------*/

static CDTitlePage title_page;

/*----------------------------------------------------------------*/

static void  CDocScriptOutputTitlePage();

// Start a Title Page part of the document. Initialize the Title Page structure
// used to store the supplied data before formatting the output.
extern void
CDocScriptInitTitlePage()
{
  cdoc_document.part     = CDOC_FRONT_MATTER_PART;
  cdoc_document.sub_part = CDOC_TITLE_PAGE_SUB_PART;
}

// Process a Colon Command found in the Title Page section.
extern void
CDocScriptProcessTitlePageCommand(CDColonCommand *colon_command)
{
  /* Process Title Page Commands */

  /* Set Title (can be many lines long) */

  if      (colon_command->getCommand() == "title") {
    /* Set title string if not set already, otherwise concatenate the str
       (with a Newline character) to the existing title str */

    if (title_page.title == "")
      title_page.title = colon_command->getText();
    else {
      std::string new_title;

      new_title  = title_page.title;
      new_title += "\n";
      new_title += colon_command->getText();

      title_page.title = new_title;
    }

    CStrUtil::toUpper(title_page.title);
  }

  /* Set Document Number */

  else if (colon_command->getCommand() == "docnum") {
    title_page.docnum = colon_command->getText();
  }

  /* Set Date */

  else if (colon_command->getCommand() == "date") {
    if (colon_command->getText()[0] == '\0')
      title_page.date = cdoc_date;
    else
      title_page.date = colon_command->getText();
  }

  /* Set Author */

  else if (colon_command->getCommand() == "author") {
    title_page.author = colon_command->getText();
  }

  /* Start Address Section */

  else if (colon_command->getCommand() == "address")
    ;

  /* Add Address Line */

  else if (colon_command->getCommand() == "aline")
    title_page.address_list.push_back(colon_command->getText());

  /* End Address Section */

  else if (colon_command->getCommand() == "eaddress")
    ;

  /* End Title Page */

  else if (colon_command->getCommand() == "etitlep")
    cdoc_document.sub_part = CDOC_NO_SUB_PART;

  /* Any other command is Invalid (??) */

  else
    CDocScriptWarning("Invalid Title Page Command - %s %s",
                      colon_command->getCommand().c_str(), colon_command->getText().c_str());

  /* If we have received an End Title Page command (i.e. the
     document sub part has been changed from TITLE_PAGE_SUB_PART to
     NO_SUB_PART then output the contents of the Title Page
     Structure as Text */

  if (cdoc_document.sub_part != CDOC_TITLE_PAGE_SUB_PART)
    CDocScriptOutputTitlePage();
}

// Output the Title Page Definition.
static void
CDocScriptOutputTitlePage()
{
  /* Don't Output if Not Required */

  if (! cdoc_title_page)
    return;

  /* Start a New Section (i.e. a new page) */

  CDocScriptStartSection("Title Page");

  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output the Title Line(s) */

  int title_lines = 5;

  if (title_page.title != "") {
    if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
      CDocScriptWriteCommand(".ps +5\n");

      CDocScriptWriteJustifiedLine(title_page.title, cdoc_title_page_align,
                                   CDOC_BOLD_UNDERLINE_FONT);

      CDocScriptWriteCommand(".ps -5\n");

      title_lines--;
    }
    else {
      CDocScriptWriteJustifiedLine(title_page.title, cdoc_title_page_align,
                                   CDOC_BOLD_UNDERLINE_FONT);

      title_lines--;
    }
  }

  for (int i = 0; i < title_lines; i++)
    CDocScriptNewLine();

  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output the Document Number */

  if (title_page.docnum != "" && title_page.docnum[0] != '\0') {
    char temp_string[256];

    sprintf(temp_string, "Document Number %s", title_page.docnum.c_str());

    CDocScriptWriteJustifiedLine(temp_string, cdoc_title_page_align, CDOC_UNDERLINE_FONT);
  }
  else
    CDocScriptNewLine();

  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output the Date */

  if (title_page.date != "")
    CDocScriptWriteJustifiedLine(title_page.date, cdoc_title_page_align, CDOC_UNDERLINE_FONT);
  else
    CDocScriptNewLine();

  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output the Author */

  if (title_page.author != "")
    CDocScriptWriteJustifiedLine(title_page.author, cdoc_title_page_align, CDOC_UNDERLINE_FONT);
  else
    CDocScriptNewLine();

  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output the Address Lines */

  int no_lines = title_page.address_list.size();

  for (int i = 1; i <= no_lines; i++) {
    const std::string &line = title_page.address_list[i - 1];

    CDocScriptWriteJustifiedLine(line, cdoc_title_page_align, CDOC_UNDERLINE_FONT);
  }

  CDocScriptNewLine();
  CDocScriptNewLine();

  /* Output Security */

  if (cdoc_document.security != "" && cdoc_document.security[0] != '\0')
    CDocScriptWriteJustifiedLine(cdoc_document.security, cdoc_title_page_align,
                                 CDOC_UNDERLINE_FONT);

  /* Clean up */

  title_page.title  = "";
  title_page.docnum = "";
  title_page.date   = "";
  title_page.author = "";

  title_page.address_list.clear();
}
