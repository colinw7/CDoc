#include "CDocI.h"

/*-----------------------------------------------------------------*/

typedef std::vector<CDFootnote *> CDFootnoteList;

static CDFootnoteList footnote_list;
static int            footnote_no_lines = 0;
static int            last_footnote_no  = 0;
static int            footnote_number   = 0;

// Creates and Initialises a Footnote Structure.
//
// The footnote structure is saved internally so that it can be later
// referenced (by position in the input file).
extern CDFootnote *
CDocScriptCreateFootnote()
{
  /* Initialise Globals */

  if (footnote_list.empty()) {
    footnote_number   = 0;
    footnote_no_lines = 0;
  }

  /* Create Default Footnote Structure */

  CDFootnote *footnote = new CDFootnote;

  footnote->ident     = "";
  footnote->number    = ++footnote_number;
  footnote->no_lines  = 0;
  footnote->temp_file = nullptr;

  return footnote;
}

// Start Footnote Processing.
extern void
CDocScriptStartFootnote(CDFootnote *footnote)
{
  char temp_string[32];

  /* Add Footnote Reference to List or preceding paragraph depending on whether
     ident has been specified */

  sprintf(temp_string, "%s{%d}%s", CDocStartBold(), footnote->number, CDocEndBold());

  if (footnote->ident == "") {
    if (cdoc_in_paragraph)
      CDocScriptAddStringToParagraph(temp_string);
    else
      CDocScriptNewParagraph(temp_string, NORMAL_PARAGRAPH, true, LEFT_JUSTIFICATION);

    CDocScriptOutputParagraph();
  }
  else {
    if (cdoc_in_paragraph)
      CDocScriptOutputParagraph();
  }

  /* Start Outputting Text to Temporary File */

  footnote->temp_file = CDocScriptStartTempFile("Footnote");

  /* Footnote text is used to start a new paragraph starting
     with the footnote reference */

  CDocScriptNewParagraph(temp_string, NORMAL_PARAGRAPH, true, LEFT_JUSTIFICATION);

  CDocScriptAddStringToParagraph(" ");

  /* Store Footnote */

  footnote_list.push_back(footnote);
}

// End Footnote Processing.
extern void
CDocScriptEndFootnote(CDFootnote *footnote)
{
  int no_footnotes;

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(footnote->temp_file);

  /*----------------*/

  no_footnotes = footnote_list.size() - last_footnote_no;

  if (no_footnotes == 1)
    footnote->no_lines = footnote->temp_file->no_lines + 3;
  else
    footnote->no_lines = footnote->temp_file->no_lines + 1;

  footnote_no_lines += footnote->no_lines;
}

// Output the any footnotes which fit in remaining lines.
extern void
CDocScriptOutputFootnotes(int flush)
{
  static int outputting = false;

  CDFootnote *footnote;
  int         save_indent;
  int         no_footnotes;
  int         lines_per_page;

  if (outputting)
    return;

  if (footnote_list.empty() || footnote_no_lines == 0)
    return;

  lines_per_page = CDocScriptGetLinesPerPage();

  if (flush || (CDocIsPagedOutput() && cdoc_line_no + footnote_no_lines >= lines_per_page)) {
    outputting = true;

    CDocScriptSaveBodyState();

    /* Output at Left Margin */

    save_indent = cdoc_indent;
    cdoc_indent = 0;

    /* Pad out so Footnote is at the Bottom of the Page */

    while (cdoc_line_no + footnote_no_lines < lines_per_page - 1)
      CDocScriptNewLine();

    footnote_no_lines = 0;

    /* Output All Footnotes from Last Footnote Output */

    no_footnotes = footnote_list.size();

    for (int i = last_footnote_no + 1; i <= no_footnotes; i++) {
      footnote = footnote_list[i - 1];

      /* Add Separator for First Footnote */

      if (i == last_footnote_no + 1) {
        CDocScriptNewLine();

        CDocScriptDrawHLine(0, 16);

        CDocScriptNewLine();
      }
      else
        CDocScriptNewLine();

      CDocScriptOutputTempFile(footnote->temp_file);

      delete footnote->temp_file;

      footnote->temp_file = nullptr;
    }

    last_footnote_no = no_footnotes;

    /* Restore Indent */

    cdoc_indent = save_indent;

    CDocScriptRestoreBodyState();

    outputting = false;
  }
}

// Delete all resources used by Footnotes.
extern void
CDocScriptDeleteFootnotes()
{
  for_each(footnote_list.begin(), footnote_list.end(), CDeletePointer());

  footnote_list.clear();

  last_footnote_no = 0;
}

//---------

CDFootnote::
~CDFootnote()
{
  delete temp_file;
}
