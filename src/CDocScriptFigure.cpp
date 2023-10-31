#include "CDocI.h"

/*------------------------------------------------------------------------*/

struct CDFigurePlace {
  CDFigure *figure;
  int       total_no_lines;
  int       figure_no_lines;
};

/*------------------------------------------------------------------------*/

typedef std::vector<CDFigure *>     CDFigureList;
typedef std::deque<CDFigurePlace *> CDFigurePlaceList;

static CDFigureList       figure_list;
static int                figure_number   = 0;
static CDFigure          *current_figure  = nullptr;
static CDFigurePlaceList  figure_top_list;

/*------------------------------------------------------------------------*/

static void CDocScriptOutputFigure
             (CDFigure *, int);

// Creates and Initialises a Figure Structure.
//
// The figure structure is saved internally so that it can be later referenced
// (by position in the input file).
CDFigure *
CDocScriptCreateFigure()
{
  /* Initialise Figure Globals */

  if (figure_list.empty())
    figure_number = 0;

  /* Set the Figure Number and set defaults for the
     other values */

  CDFigure *figure = new CDFigure;

  figure->depth       = 0;
  figure->frame       = reinterpret_cast<char *>(FRAME_RULE);
  figure->ident       = "";
  figure->indent      = 2;
  figure->place       = PLACE_TOP;
  figure->width       = WIDTH_PAGE;
  figure->number      = ++figure_number;
  figure->count       = -1;
  figure->caption     = "";
  figure->description = "";
  figure->page_no     = 0;
  figure->temp_file   = NULL;

  /* Add the structure to the Figure List */

  figure_list.push_back(figure);

  return figure;
}

// Get Figure at specified position in the input file. The position is the number
// of the figure i.e. the first figure is 1, the second is 2 etc.
//
// If the figure is not found NULL is returned.
CDFigure *
CDocScriptGetFigure(int no)
{
  CDFigure *figure;

  if (! figure_list.empty())
    figure = figure_list[no - 1];
  else
    figure = NULL;

  return figure;
}

// Start Figure Processing.
void
CDocScriptFigureBegin(CDFigure *figure)
{
  /* Start Outputting Text to Temporary File */

  figure->temp_file = CDocScriptStartTempFile("Figure");

  /* Set Figure Indent */

  cdoc_indent += figure->indent;

  /* Save as Current Figure */

  current_figure = figure;

  /* Turn off Formatting and Start Figure Font */

  CDocScriptFormattingOff();

  CDocScriptBeginFont("figfont");
}

// End Figure Processing and Output Figure.
void
CDocScriptFigureEnd(CDFigure *figure)
{
  /* Turn Formatting back on Switch back to Previous Font */

  CDocScriptFormattingOn();

  CDocScriptPreviousFont();

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(figure->temp_file);

  /* Calculate the Number of Lines Output */

  int figure_no_lines = figure->temp_file->no_lines;

  /* Output the Figure */

  CDocScriptOutputFigure(figure, figure_no_lines);

  /* Reset Current Figure */

  current_figure = NULL;
}

// Check if there are any figures waiting to be output at the top of the next page.
int
CDocScriptIsTopFigure()
{
  if (! figure_top_list.empty())
    return true;

  return false;
}

// Output the first figure waiting to be output at the top of the next page at
// the current position in the document.
void
CDocScriptOutputTopFigure()
{
  if (figure_top_list.empty())
    return;

  CDFigurePlace *figure_top = figure_top_list.front();

  if (figure_top != NULL) {
    CDocScriptSaveBodyState();

    figure_top_list.pop_front();

    figure_top->figure->place = PLACE_TOP_NOW;

    CDocScriptOutputFigure(figure_top->figure,
                           figure_top->figure_no_lines);

    figure_top->figure->place = PLACE_TOP;

    delete figure_top;

    CDocScriptRestoreBodyState();
  }
}

// Output the specified figure which will take up the specfied number of lines.
//
// If the figure is to be output at the top of a page and we are not already at the
// top of a page then it will be buffered for later output via CDocScriptOutputTopFigure().
static void
CDocScriptOutputFigure(CDFigure *figure, int figure_no_lines)
{
  if (figure->place == PLACE_TOP && CDocIsPagedOutput()) {
    if (cdoc_paragraph_done) {
      CDFigurePlace *figure_top = new CDFigurePlace;

      figure_top->figure          = figure;
      figure_top->figure_no_lines = figure_no_lines;

      figure_top_list.push_back(figure_top);

      return;
    }
    else
      figure->place = PLACE_TOP_NOW;
  }

  //---

  /* Calculate Total Number of Lines */

  int total_no_lines;

  if (figure->depth > figure_no_lines)
    total_no_lines = figure->depth;
  else
    total_no_lines = figure_no_lines;

  if (figure->frame != reinterpret_cast<char *>(FRAME_NONE))
    total_no_lines += 2;

  if (figure->caption != "" || figure->description != "")
    total_no_lines += 3;

  //---

  /* Find space for figure if not forcing output of top figure */

  if (figure->place != PLACE_TOP_NOW) {
    /* If outputting for CDoc or RAW and figure does not fit on
       current page then start a new page */

    if (CDocIsPagedOutput())
      CDocScriptFindSpace(total_no_lines);
  }

  /* Start Sub-Section for Figure */

  if (figure->count != -1) {
    char *temp_string;

    if (figure->caption != "") {
      temp_string = new char [figure->caption.size() + 128];

      sprintf(temp_string, "  Figure %d.  %s", figure->count, figure->caption.c_str());
    }
    else {
      temp_string = new char [128];

      sprintf(temp_string, "  Figure %d.", figure->count);
    }

    CDocScriptStartSubSection(temp_string);

    delete [] temp_string;
  }

  /* Set Figure's Page Number */

  if (figure->ident != "")
    CDocScriptSetReferencePageNumber(FIGURE_REF, figure->ident);

  CDocScriptSetFigurePage(figure);

  /* Add Blank Line if Previous Paragraph */

  if (cdoc_paragraph_done)
    CDocScriptNewLine();

  /* Output Initial Frame Line */

  int width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if      (figure->frame == reinterpret_cast<char *>(FRAME_RULE))
    CDocScriptDrawHLine(0, -2);
  else if (figure->frame == reinterpret_cast<char *>(FRAME_BOX))
    CDocScriptDrawHLine(0, -2);
  else if (figure->frame != reinterpret_cast<char *>(FRAME_NONE)) {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    int length = strlen(figure->frame);

    int i = 0;

    while (length > 0 && i + length <= width) {
      CDocScriptWriteText("%s", figure->frame);

      i += length;
    }

    length = width - i;

    for (i = 0; i < length; i++)
      CDocScriptWriteText("%c", figure->frame[i]);

    CDocScriptWriteText("\n");
  }

  /* Pad to Required Depth */

  while (figure->depth > figure_no_lines) {
    figure_no_lines++;

    CDocScriptNewLine();
  }

  /* Output Text in Temporary File */

  cdoc_line_fiddle = 0;

  CDocScriptOutputTempFile(figure->temp_file);

  /* Write Terminating Frame Line */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if      (figure->frame == reinterpret_cast<char *>(FRAME_RULE))
    CDocScriptDrawHLine(0, -2);
  else if (figure->frame == reinterpret_cast<char *>(FRAME_BOX)) {
    CDocScriptDrawHLine(0, -2);

    if (figure->depth > figure_no_lines) {
      CDocScriptDrawVLine( 0, -figure->depth + cdoc_line_fiddle);
      CDocScriptDrawVLine(-2, -figure->depth + cdoc_line_fiddle);
    }
    else {
      CDocScriptDrawVLine( 0, -figure_no_lines + cdoc_line_fiddle);
      CDocScriptDrawVLine(-2, -figure_no_lines + cdoc_line_fiddle);
    }
  }
  else if (figure->frame != reinterpret_cast<char *>(FRAME_NONE)) {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    int length = strlen(figure->frame);

    int i = 0;

    while (i + length <= width) {
      CDocScriptWriteText("%s", figure->frame);

      i += length;
    }

    length = width - i;

    for (i = 0; i < length; i++)
      CDocScriptWriteText("%c", figure->frame[i]);

    CDocScriptWriteText("\n");
  }

  /* Output centred Figure Text using Caption and
     Description if specified */

  if (figure->caption != "" || figure->description != "") {
    CDocScriptSkipLine();

    char *temp_string = nullptr;

    if      (figure->caption != "" && figure->description != "") {
      temp_string = new char [figure->caption.size() + figure->description.size() + 128];

      sprintf(temp_string, "%sFigure %d.  %s:  %s%s",
              CDocStartBold(), figure->count, figure->caption.c_str(), CDocEndBold(),
              figure->description.c_str());
    }
    else if (figure->caption != "") {
      temp_string = new char [figure->caption.size() + 128];

      sprintf(temp_string, "%sFigure %d.  %s%s",
              CDocStartBold(), figure->count, figure->caption.c_str(), CDocEndBold());
    }
    else if (figure->description != "") {
      temp_string = new char [figure->description.size() + 128];

      sprintf(temp_string, "%sFigure %d.  %s%s",
              CDocStartBold(), figure->count, CDocEndBold(),
              figure->description.c_str());
    }

    CDocScriptWriteCentreJustifiedLine(temp_string, CDOC_NORMAL_FONT);

    delete [] temp_string;
  }

  /* Signal that next paragraph should skip a line */

  cdoc_paragraph_done = true;

  /* Clean Up */

  delete figure->temp_file;

  figure->temp_file = nullptr;
}

// Output the List of Figures in the Document.
void
CDocScriptOutputFigureList()
{
  int       i;
  int       j;
  CDFigure *figure;
  int       no_figures;
  char     *temp_string;

  /* Output Figure List as a Level 1 Header */

  if (cdoc_document.type == CDOC_STAIRS_DOCUMENT) {
    CDocScriptStartSection("List of Illustrations");

    CDocScriptStartHeader(1);

    temp_string = new char [128];

    sprintf(temp_string, "%d.0  List of Illustrations", CDocScriptGetHeader(1));

    CDocScriptWriteLeftJustifiedHeader(temp_string);

    delete [] temp_string;
  }
  else
    CDocScriptWriteCentreJustifiedPageHeader("List of Illustrations");

  CDocScriptSetTOCPage();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<pre>\n");

  CDocScriptSkipLine();

  no_figures = figure_list.size();

  for (i = 1; i <= no_figures; i++) {
    /* Output Figure Line */

    figure = figure_list[i - 1];

    if (figure->caption == "" && figure->description == "")
      continue;

    if (figure->caption != "") {
      temp_string = new char [figure->caption.size() + 128];

      sprintf(temp_string, "Figure %d.  %s",
              figure->count, figure->caption.c_str());
    }
    else {
      temp_string = new char [128];

      sprintf(temp_string, "Figure %d.", figure->count);
    }

    if      (cdoc_page_numbering && CDocIsPagedOutput()) {
      CDocScriptWriteIndent(8);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
        CDocScriptWriteText("%s##cdoc_fig_pg%d##B", ESCG, i);

      CDocScriptWriteText("%s", temp_string);

      j = CDocStringDisplayLength(temp_string) + 8;

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
        CDocScriptWriteText(FILL_TMPL, j                    , j + 1                , ' ');
        CDocScriptWriteText(FILL_TMPL, j + 2                , cdoc_right_margin - 4, '.');
        CDocScriptWriteText(FILL_TMPL, cdoc_right_margin - 3, cdoc_right_margin - 3, ' ');
      }
      else {
        if (j < cdoc_right_margin - 6) {
          CDocScriptWriteText("  ");

          j += 2;

          while (j < cdoc_right_margin - 3) {
            CDocScriptWriteText(".");

            j++;
          }
        }
        else {
          while (j < cdoc_right_margin - 3) {
            CDocScriptWriteText(" ");

            j++;
          }
        }

        CDocScriptWriteText(" ");
      }

      CDocScriptWriteText("##cdoc_fig_pg%d##", i);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
        CDocScriptWriteText("%sE", ESCG);

      CDocScriptWriteText("\n");
    }
    else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
      CDocScriptWriteCommand("        %s\n", CDocEncodeHtmlString(temp_string));
    else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
      CDocScriptWriteCommand("        %s\n", CDocEncodeILeafString(temp_string));
    else {
      CDocScriptWriteIndent(8);

      CDocScriptWriteText("%s\n", temp_string);
    }

    delete [] temp_string;
  }

  CDocScriptSkipLine();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("</pre>\n");
}

// Store the current page number in the figure structure for later output in
// the Figure List and Figure References.
void
CDocScriptSetFigurePage(CDFigure *figure)
{
  if (cdoc_page_no != -1)
    figure->page_no = cdoc_page_no;
  else
    figure->page_no = cdoc_page_no_offset;
}

// Get the Page Number String of the Figure which was the Nth to appear in the document.
std::string
CDocScriptGetFigurePage(int no)
{
  std::string page_string;

  auto *figure = CDocScriptGetFigure(no);

  if (figure)
    page_string = CStrUtil::toString(figure->page_no);
  else
    page_string = "??";

  return page_string;
}

// Delete all currently defined Figure Structures.
//
// Used to free of all Figure resources when processing of the input IBM
// Script File has finished.
void
CDocScriptDeleteFigures()
{
  for_each(figure_list.begin(), figure_list.end(), CDeletePointer());

  figure_list.clear();
}

// Set the Depth (in lines) of the Current Figure being processed.
//
// This is used to override the depth of a figure when it contains a X Image
// or X Bitmap.
void
CDocScriptSetCurrentFigureDepth(int depth)
{
  if (current_figure)
    current_figure->depth = depth;
}

//---------------

CDFigure::
~CDFigure()
{
  delete temp_file;
}
