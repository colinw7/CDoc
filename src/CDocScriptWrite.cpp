#include "CDocI.h"

struct CDBodyState {
  int   font_type;
  int   colour;
  char *goto_page;
  int   indent;
  int   outputting_paragraph;
  int   formatting;
};

int  cdoc_page_no            = 0;
int  cdoc_page_no_offset     = 0;
int  cdoc_line_no            = 0;
int  cdoc_char_no            = 0;
int  cdoc_line_fiddle        = 0;
int  cdoc_page_header_output = false;
int  cdoc_last_indent        = -1;
int  cdoc_last_indent_page   = -1;

static char  box_chars[] = "-|+++++++++";

static int    current_font_type  = 0;
static int    current_colour     = 0;
static char  *current_goto_page  = NULL;
static int    output_header_skip = true;

typedef std::vector<CDBodyState *> CDBodyStateList;

static CDBodyStateList body_state_list;

static void  (*cdoc_script_pre_header_proc)(const char *)  = NULL;
static char   *cdoc_script_pre_header_data           = NULL;
static void  (*cdoc_script_post_header_proc)(const char *) = NULL;
static char   *cdoc_script_post_header_data          = NULL;
#if 0
static void  (*cdoc_script_pre_body_proc)(const char *)  = NULL;
static char   *cdoc_script_pre_body_data           = NULL;
static void  (*cdoc_script_post_body_proc)(const char *) = NULL;
static char   *cdoc_script_post_body_data          = NULL;
#endif
static void  (*cdoc_script_pre_footer_proc)(const char *)  = NULL;
static char   *cdoc_script_pre_footer_data           = NULL;
static void  (*cdoc_script_post_footer_proc)(const char *) = NULL;
static char   *cdoc_script_post_footer_data          = NULL;

// Find enough space to output a block of text of the specified number of lines.
//
// If there is enough room on the current page then we just return. If the number
// of lines is less than the number of lines on a page and there isn't enough
// room then we throw a page, output any top figures and try again.
extern int
CDocScriptFindSpace(int no_lines)
{
  int new_page = false;

  /* If outputting to temporary file signal we have the space */

  if (cdoc_page_no == -1)
    return true;

  /* Output the Header if not already done */

  if (! cdoc_page_header_output) {
    CDocScriptWritePageHeader();

    cdoc_line_no++;

    CDocScriptOutputFootnotes(false);

    if (cdoc_line_no == 1)
      new_page = true;
  }

  /* Check if enough room for number of lines and if not then
     start a new page and check again */

  int lines_per_page = CDocScriptGetLinesPerPage();

  if (no_lines > lines_per_page)
    fprintf(stderr, "Request for %d Lines when only %d Lines on a Page\n",
            no_lines, lines_per_page);

  while (no_lines                <= lines_per_page &&
         cdoc_line_no + no_lines >  lines_per_page) {
    new_page = true;

    CDocScriptStartNewPage();

    /* Output Top Figure if Any */

    if (CDocScriptIsTopFigure()) {
      CDocScriptOutputTopFigure();

      new_page = false;
    }
  }

  return new_page;
}

// Start a New Page in the Output Text and flush all top figures.
//
// This routine is called when a new page is required by the script command
// being processed and not when a new page is required because we have reached
// the end of the current one.
extern void
CDocScriptNewPage()
{
  CDocScriptStartNewPage();

  /* Output Top Figure if Any */

  while (CDocScriptIsTopFigure()) {
    CDocScriptOutputTopFigure();

    CDocScriptStartNewPage();
  }
}

// Start a New Page in the Output Text.
//
// In CDoc Output Format this adds the Page Number to the previous Page.
extern void
CDocScriptStartNewPage()
{
  /* Page No is -1 then we are outputting to a temporary
     file so flag the page with a control code and return */

  if (cdoc_page_no == -1) {
    if (cdoc_line_no != 0) {
      fputc(NEW_PAGE, cdoc_output_fp);
      fputc('\n'    , cdoc_output_fp);
    }

    return;
  }

  /* If already on new page then ignore */

  if (CDocInst->getOutputFormat() != CDOC_OUTPUT_HTML && cdoc_line_no == 0)
    return;

  /********************/

  CDocScriptOutputFootnotes(true);

  CDocScriptWritePageFooter();

  /********************/

  /* Output Page Throw Command for various Formats */

  CDocScriptSaveBodyState();

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".bp\n");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC)
    CDocScriptWriteCommand("%c\n", NEW_PAGE);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptEndAllFonts();
    CDocScriptWriteCommand("%c\n", NEW_PAGE);
    CDocScriptStartAllFonts();
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<hr>\n");

  CDocScriptRestoreBodyState();

  /* Increment Page No. and Reset Line and Character No. */

  cdoc_page_no++;

  cdoc_line_no = 0;
  cdoc_char_no = 0;

  cdoc_page_header_output = false;

  /* Indicate that a Paragraph has not been Output for
     this Section yet */

  cdoc_paragraph_done = false;
}

// Set the routine to be called before the header of a new page has been
// output.
extern void
CDocScriptSetPagePreHeaderProc(void (*proc)(const char *), char *data)
{
  cdoc_script_pre_header_proc = proc;
  cdoc_script_pre_header_data = data;
}

// Set the routine to be called after the header of a new page has been output.
extern void
CDocScriptSetPagePostHeaderProc(void (*proc)(const char *), char *data)
{
  cdoc_script_post_header_proc = proc;
  cdoc_script_post_header_data = data;
}

// Set the routine to be called before the footer of a page is output.
extern void
CDocScriptSetPagePreFooterProc(void (*proc)(const char *), char *data)
{
  cdoc_script_pre_footer_proc = proc;
  cdoc_script_pre_footer_data = data;
}

// Set the routine to be called after the footer of a page is output.
extern void
CDocScriptSetPagePostFooterProc(void (*proc)(const char *), char *data)
{
  cdoc_script_post_footer_proc = proc;
  cdoc_script_post_footer_data = data;
}

// Write Text required at the top of page.
//
// Currently only outputs a blank line. May need to add any security
// classification ...
extern void
CDocScriptWritePageHeader()
{
  /* Don't output if we are outputting to a temporary file or
     a page header has already been output */

  if (cdoc_page_no == -1 || cdoc_page_header_output)
    return;

  CDocScriptSaveBodyState();

  /* Run the Pre-Header Proc if specified */

  if (cdoc_script_pre_header_proc != NULL)
    (*cdoc_script_pre_header_proc)(cdoc_script_pre_header_data);

  /* Signal Header has been Output for Current Page */

  cdoc_page_header_output = true;

  /* Write Page Header only for Paged Formats i.e. CDoc or Raw */

  if (CDocIsPagedOutput()) {
    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
      CDocScriptWriteCommand(CDOC_START_HEADER_TMPL);

    if (cdoc_document.type == CDOC_MEMO_DOCUMENT) {
      if (cdoc_page_no == 1)
        fputc('\n', cdoc_output_fp);
      else {
        /* Get Page Details */

        std::string reference, date;

        CDocScriptGetMemoDetails(reference, date);

        /* Make sure Page Details appear in consistent Font */

        CDocScriptBeginFont("xmpfont");

        /* Output Page Details */

        fputc('\n', cdoc_output_fp);

        for (int i = 0; i < cdoc_left_margin; i++)
          fputc(' ', cdoc_output_fp);

        fprintf(cdoc_output_fp, "%s\n", reference.c_str());

        for (int i = 0; i < cdoc_left_margin; i++)
          fputc(' ', cdoc_output_fp);

        fprintf(cdoc_output_fp, "%s\n", date.c_str());

        for (int i = 0; i < cdoc_left_margin; i++)
          fputc(' ', cdoc_output_fp);

        fprintf(cdoc_output_fp, "- %d -\n", cdoc_page_no);

        fputc('\n', cdoc_output_fp);
        fputc('\n', cdoc_output_fp);

        /* Restore Font */

        CDocScriptPreviousFont();
      }
    }
    else {
      if (cdoc_document.security != "") {
        /* Make sure Page Details appear in consistent Font */

        CDocScriptBeginFont("xmpfont");

        /* Centre the Security String */

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
          CDocScriptWriteCommand(CDOC_CENTRE_TMPL);

          CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_save_left_margin);

          fputc('\t', cdoc_output_fp);
        }
        else {
          int length = cdoc_document.security.size();

          int no_spaces = (cdoc_right_margin - cdoc_save_left_margin - length)/2;

          for (int i = 0; i < cdoc_save_left_margin + no_spaces; i++)
            fputc(' ', cdoc_output_fp);
        }

        fputs(cdoc_document.security.c_str(), cdoc_output_fp);

        fputc('\n', cdoc_output_fp);

        /* Restore Font */

        CDocScriptPreviousFont();
      }
      else
        fputc('\n', cdoc_output_fp);

      fputc('\n', cdoc_output_fp);
    }

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
      CDocScriptWriteCommand(CDOC_END_HEADER_TMPL);
  }

  /* Run the Post-Header Proc if specified */

  if (cdoc_script_post_header_proc != NULL)
    (*cdoc_script_post_header_proc)(cdoc_script_post_header_data);

  CDocScriptRestoreBodyState();
}

// Write Text required at the bottom of page.
//
// Pads out the Page to the required length and Writes the Page Number for
// CDoc, Raw with Control Codes and Raw Output.
//
// May need to add any security classification ...
extern void
CDocScriptWritePageFooter()
{
  CDocScriptSaveBodyState();

  if (cdoc_script_pre_footer_proc != NULL)
    (*cdoc_script_pre_footer_proc)(cdoc_script_pre_footer_data);

  /* Write Page Footer only for Paged Formats i.e. CDoc or Raw */

  if (CDocIsPagedOutput()) {
    int  i;
    int  lines_per_page;

    lines_per_page = CDocScriptGetLinesPerPage();

    /* Pad out to Required No Lines */

    for (i = cdoc_line_no; i < lines_per_page; i++)
      fputc('\n', cdoc_output_fp);

    /* Write Page Footer */

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
      CDocScriptWriteCommand(CDOC_START_FOOTER_TMPL);

    if (cdoc_document.type == CDOC_MEMO_DOCUMENT)
      fputc('\n', cdoc_output_fp);
    else {
      char  temp_string[256];

      /* Make sure Page Details appear in consistent Font */

      CDocScriptBeginFont("xmpfont");

      /* Write Centred Page Number */

      fputc('\n', cdoc_output_fp);

      if (cdoc_page_no != -1)
        sprintf(temp_string, "Page %d", cdoc_page_no);
      else
        sprintf(temp_string, "Page %d", cdoc_page_no_offset);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
        CDocScriptWriteCommand(CDOC_CENTRE_TMPL);

        CDocScriptWriteCommand(CDOC_INDENT_TMPL,
                               cdoc_save_left_margin);

        fputc('\t', cdoc_output_fp);
      }
      else {
        int  length;
        int  no_spaces;

        length = strlen(temp_string);

        no_spaces = (cdoc_right_margin -
                     cdoc_save_left_margin - length)/2;

        for (i = 0; i < cdoc_save_left_margin + no_spaces; i++)
          fputc(' ', cdoc_output_fp);
      }

      fputs(temp_string, cdoc_output_fp);

      fputc('\n', cdoc_output_fp);

      fputc('\n', cdoc_output_fp);

      /* Restore Font */

      CDocScriptPreviousFont();
    }

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
      CDocScriptWriteCommand(CDOC_END_FOOTER_TMPL);
  }

  if (cdoc_script_post_footer_proc != NULL)
    (*cdoc_script_post_footer_proc)(cdoc_script_post_footer_data);

  CDocScriptRestoreBodyState();
}

// Start a new section of the Script Output. This means a new page is started
// and a special tag written.
extern void
CDocScriptStartSection(const std::string &ref_string)
{
  /* Save Section as Current Section */

  cdoc_current_section = ref_string;

  /* Start New Page */

  CDocScriptNewPage();

  /* Output Section Marker for CDoc Formats */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
    CDocScriptWriteCommand(CDOC_SECTION_TMPL, ref_string.c_str());
}

// Start a new sub-section of the Script Output. This means a special tag
// is written.
extern void
CDocScriptStartSubSection(const std::string &str)
{
  /* Save Section as Current Section */

  cdoc_current_section = str;

  /* Output Sub-Section Marker for CDoc Formats */

  if       (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    /* Ignore Leading Spaces */

    int i = 0;

    CStrUtil::skipSpace(str, &i);

    CDocScriptWriteCommand(CDOC_SUB_SECTION_TMPL, &str[i]);
  }
}

// Start a New Line in the Output Text.
//
// The difference between this routine and the CDocScriptNewLine() routine is
// that CDoc can ignore the request if we are at the top of a new page.
extern void
CDocScriptSkipLine()
{
  if (cdoc_line_no != 0 || cdoc_char_no != 0)
    CDocScriptNewLine();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<br>\n");
}

// Start a New Line in the Output Text.
extern void
CDocScriptNewLine()
{
  /* Output Newline Character */

  CDocScriptFPutc('\n');
}

// Indent the current text output 'no_indent' spaces form the current left margin.
extern void
CDocScriptWriteIndent(int no_indent)
{
  bool changed = false;

  if (cdoc_char_no == 0) {
    if (no_indent != cdoc_last_indent || cdoc_page_no != cdoc_last_indent_page)
      changed = true;

    cdoc_last_indent      = no_indent;
    cdoc_last_indent_page = cdoc_page_no;
  }

  /* Set indent using the 'in' command for Troff using
     the 'CDocCharsToEms' function to calculate the number of
     em's to use for the specified indent */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
    CDocScriptWriteCommand(".in %.1lfm\n", CDocCharsToEms(no_indent));
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    if (cdoc_char_no == 0) {
      if (changed)
        CDocScriptWriteCommand(CDOC_INDENT_TMPL, no_indent);

      CDocScriptFPutc('\t');
    }
    else {
      for (int i = 0; i < no_indent; i++)
        CDocScriptFPutc(' ');
    }
  }

  /* Set indent using spaces for any other format */

  else {
    for (int i = 0; i < no_indent; i++)
      CDocScriptFPutc(' ');
  }
}

// Write the supplied str left, centre or right justified in the font style specified.
extern void
CDocScriptWriteJustifiedLine(const std::string &str, int align, int font_type)
{
  if      (align == CHALIGN_TYPE_LEFT)
    CDocScriptWriteLeftJustifiedLine(str, font_type);
  else if (align == CHALIGN_TYPE_CENTRE)
    CDocScriptWriteCentreJustifiedLine(str, font_type);
  else if (align == CHALIGN_TYPE_RIGHT)
    CDocScriptWriteRightJustifiedLine(str, font_type);
}

// Write the supplied str in the font style specified.
extern void
CDocScriptWriteLeftJustifiedLine(const std::string &str, int font_type)
{
  int  i;
  int  j;
  int  width;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=left><u>%s</u></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("<p align=left><b>%s</b></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=left><b><u>%s</u></b></p>\n", CDocEncodeHtmlString(str));
    else
      CDocScriptWriteCommand("<p align=left>%s</p>\n", CDocEncodeHtmlString(str));

    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"para\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteLeftJustifiedLine(lines[i], font_type);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    return;
  }

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    /* Indent to required position */

    CDocScriptWriteIndent(cdoc_left_margin);

    /* Write text in the appropriate font */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBoldUnderline(),
                          str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s\n", str.c_str());
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    /* Write text in the appropriate font */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand(".US \"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand(".BD \"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand(".BU \"");

    CDocScriptWriteText("%s", str.c_str());

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");

    CDocScriptWriteText("\n");
  }
  else {
    /* Indent to required position */

    CDocScriptWriteIndent(cdoc_left_margin);

    /* Write text with underline if required */

    CDocScriptWriteText("%s\n", str.c_str());

    if (font_type == CDOC_UNDERLINE_FONT || font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptDrawHLine(0, CDocStringDisplayLength(str));
  }
}

// Write the supplied str centred between the left and right margin in the
// font style specified.
extern void
CDocScriptWriteCentreJustifiedLine(const std::string &str, int font_type)
{
  int  i;
  int  j;
  int  width;
  int  length;
  int  no_spaces;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=center><u>%s</u></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("<p align=center><b>%s</b></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=center><b><u>%s</u></b></p>\n", CDocEncodeHtmlString(str));
    else
      CDocScriptWriteCommand("<p align=center>%s</p>\n", CDocEncodeHtmlString(str));

    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"para\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteCentreJustifiedLine(lines[i], font_type);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    return;
  }

  /* For Centred Text in Troff use 'ce' command with the
     previously defined macros for the font style */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteCommand(".ce\n");

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand(".US \"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand(".BD \"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand(".BU \"");

    CDocScriptWriteText("%s", str.c_str());

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");
  }

  /* For Centred Text in CDoc use the Centre Command */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptFindSpace(1);

    CDocScriptWriteCommand(CDOC_CENTRE_TMPL);

    CDocScriptWriteIndent(cdoc_left_margin);

    /* Output Text */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBoldUnderline(),
                          str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s\n", str.c_str());
  }

  /* For Centred Text in Raw Text with Control Codes manually
     centre text between left and right margins and use escape
     codes for the font style */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    /* Calculate extra spaces required to centre text between
       left and right margins */

    length = CDocStringDisplayLength(str);

    no_spaces = (cdoc_right_margin - cdoc_left_margin - length)/2;

    /* Indent to required position */

    CDocScriptWriteIndent(cdoc_left_margin + no_spaces);

    /* Output Text */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBoldUnderline(),
                          str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s\n", str.c_str());
  }

  /* For any other format centre text between left and right
     margins and just output text (underlined if specified) */

  else {
    length = CDocStringDisplayLength(str);

    no_spaces = (cdoc_right_margin - cdoc_left_margin - length)/2;

    CDocScriptWriteIndent(cdoc_left_margin + no_spaces);

    CDocScriptWriteText("%s\n", str.c_str());

    if (font_type == CDOC_UNDERLINE_FONT || font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptDrawHLine(0, CDocStringDisplayLength(str));
  }
}

// Write the supplied str centred so it is justified to the right margin in
// the font style specified.
extern void
CDocScriptWriteRightJustifiedLine(const std::string &str, int font_type)
{
  int  i;
  int  j;
  int  width;
  int  length;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=right><u>%s</u></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("<p align=right><b>%s</b></p>\n", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("<p align=right><b><u>%s</u></b></p>\n", CDocEncodeHtmlString(str));
    else
      CDocScriptWriteCommand("<p align=right>%s</p>\n", CDocEncodeHtmlString(str));

    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"para\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteRightJustifiedLine(lines[i], font_type);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    return;
  }

  /* For Right Justified Text in Troff use the previously
     defined macros dependant on font style */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand(".RU \"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand(".RB \"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand(".RE \"");
    else
      CDocScriptWriteCommand(".RN \"");

    CDocScriptWriteText("%s", str.c_str());

    CDocScriptWriteCommand("\"\n");
  }

  /* For Right Justified Text in CDoc use the Right Command */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptWriteCommand(CDOC_RIGHT_TMPL);

    /* Output Text */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartBoldUnderline(),
                          str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s\n", str.c_str());
  }

  /* For Right Justified Text in Raw Text with Control Codes
     manually right justify the text to the right margin using
     escape codes for the font style */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    /* Calculate indent required to right justify text
       to right margin */

    length = CDocStringDisplayLength(str);

    CDocScriptWriteIndent(cdoc_right_margin - length);

    /* Output Text */

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s\n", str.c_str());
  }

  /* For any other format manually right justify the text to
     the right margin and just output text (underlined if
     specified) */

  else {
    length = CDocStringDisplayLength(str);

    CDocScriptWriteIndent(cdoc_right_margin - length);

    CDocScriptWriteText("%s\n", str.c_str());

    if (font_type == CDOC_UNDERLINE_FONT || font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptDrawHLine(0, CDocStringDisplayLength(str));
  }
}

// Write a Page Header aligned to the left, centre or right hand side of the page.
extern void
CDocScriptWriteJustifiedPageHeader(const std::string &str, int align)
{
  if      (align == CHALIGN_TYPE_LEFT)
    CDocScriptWriteLeftJustifiedPageHeader(str);
  else if (align == CHALIGN_TYPE_CENTRE)
    CDocScriptWriteCentreJustifiedPageHeader(str);
  else if (align == CHALIGN_TYPE_RIGHT)
    CDocScriptWriteRightJustifiedPageHeader(str);
}

// Write a Page Header aligned to the left side of the page.
extern void
CDocScriptWriteLeftJustifiedPageHeader(const std::string &str)
{
  /* Start a New Section */

  CDocScriptStartSection(str);

  /* Write the Header */

  CDocScriptWriteLeftJustifiedHeader(str);
}

// Write a Page Header aligned to the left side of the page.
extern void
CDocScriptWriteCentreJustifiedPageHeader(const std::string &str)
{
  /* Start a New Section */

  CDocScriptStartSection(str);

  /* Write the Header */

  CDocScriptWriteCentreJustifiedHeader(str);
}

// Write a Page Header aligned to the left side of the page.
extern void
CDocScriptWriteRightJustifiedPageHeader(const std::string &str)
{
  /* Start a New Section */

  CDocScriptStartSection(str);

  /* Write the Header */

  CDocScriptWriteRightJustifiedHeader(str);
}

// Write a Header aligned to the left, centre or right hand side of the page.
extern void
CDocScriptWriteJustifiedHeader(const std::string &str, int align)
{
  if      (align == CHALIGN_TYPE_LEFT)
    CDocScriptWriteLeftJustifiedHeader(str);
  else if (align == CHALIGN_TYPE_CENTRE)
    CDocScriptWriteCentreJustifiedHeader(str);
  else if (align == CHALIGN_TYPE_RIGHT)
    CDocScriptWriteRightJustifiedHeader(str);
}

// Write the supplied str as Page Header.
extern void
CDocScriptWriteLeftJustifiedHeader(const std::string &str)
{
  int    i;
  int    j;
  int    width;
  char  *temp_string;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<h3 align=left>%s</h3>", CDocEncodeHtmlString(str));
    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"header\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* Skip line after any previous paragraph */

  if (cdoc_paragraph_done)
    CDocScriptSkipLine();

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    output_header_skip = false;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteLeftJustifiedHeader(lines[i]);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    output_header_skip = true;

    goto WriteLeftJustifiedHeader_1;
  }

  /* Output the Header in Bold Underscored type (using macro) in Troff */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteCommand(".BU \"");
    CDocScriptWriteText("%s", str.c_str());
    CDocScriptWriteCommand("\"");
  }

  /* Output Header in Bold type in CDoc or Raw Text with Control Codes */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC)
    CDocScriptWriteLine("%s%s%s", CDocStartBold(), str.c_str(), CDocEndBold());

  /* Output Header manually centred and Underlined for
     any other Format */

  else {
    CDocScriptWriteLine("%s", str.c_str());

    temp_string = CStrUtil::strdup(str);

    for (i = 0; i < CDocStringDisplayLength(str); i++)
      temp_string[i] = '-';

    CDocScriptWriteLine("%s", temp_string);

    delete [] temp_string;
  }

WriteLeftJustifiedHeader_1:
  /* Output Newline after Header */

  if (output_header_skip)
    CDocScriptSkipLine();

  /* Set Paragraph Done flag to False as not Paragraph has
     been output for this new section yet */

  cdoc_paragraph_done = false;
}

// Write the supplied str as Page Header centred between the left and right margins.
extern void
CDocScriptWriteCentreJustifiedHeader(const std::string &str)
{
  int    i;
  int    j;
  int    width;
  int    length;
  int    no_spaces;
  char  *temp_string;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<h3 align=center>%s</h3>", CDocEncodeHtmlString(str));
    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"header\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* Skip line after any previous paragraph */

  if (cdoc_paragraph_done)
    CDocScriptSkipLine();

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    output_header_skip = false;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteCentreJustifiedHeader(lines[i]);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    output_header_skip = true;

    goto WriteCentreJustifiedHeader;
  }

  /* Output the Header centred in Bold Underscored type
     (using macro) in Troff */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteCommand(".ce\n");

    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteCommand(".BU \"");
    CDocScriptWriteText("%s", str.c_str());
    CDocScriptWriteCommand("\"");
  }

  /* Centred in Bold type for CDoc */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptFindSpace(1);

    CDocScriptWriteCommand(CDOC_CENTRE_TMPL);

    CDocScriptWriteIndent(cdoc_left_margin);

    CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
  }

  /* Output Header manually centred in Bold type for Raw Text
     with Control Codes */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    length = CDocStringDisplayLength(str);

    no_spaces = (cdoc_right_margin - cdoc_left_margin - length)/2;

    CDocScriptWriteIndent(no_spaces);

    CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
  }

  /* Output Header manually centred and Underlined for
     any other Format */

  else {
    length = CDocStringDisplayLength(str);

    no_spaces = (cdoc_right_margin - cdoc_left_margin - length)/2;

    CDocScriptWriteIndent(no_spaces);

    CDocScriptWriteText("%s\n", str.c_str());

    CDocScriptWriteIndent(no_spaces);

    temp_string = CStrUtil::strdup(str);

    for (i = 0; i < CDocStringDisplayLength(str); i++)
      temp_string[i] = '-';

    CDocScriptWriteText("%s\n", temp_string);

    delete [] temp_string;
  }

WriteCentreJustifiedHeader:
  /* Output Newline after Header */

  if (output_header_skip)
    CDocScriptSkipLine();

  /* Set Paragraph Done flag to False as not Paragraph has
     been output for this new section yet */

  cdoc_paragraph_done = false;
}

// Write the supplied str as Page Header right justified between the
// left and right margins.
extern void
CDocScriptWriteRightJustifiedHeader(const std::string &str)
{
  int    i;
  int    j;
  int    width;
  int    length;
  int    no_spaces;
  char  *temp_string;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<h3 align=right>%s</h3>", CDocEncodeHtmlString(str));
    return;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("<\"header\">\n");

    CDocScriptWriteCommand("%s\n", CDocEncodeHtmlString(str));

    return;
  }

  /* Skip line after any previous paragraph */

  if (cdoc_paragraph_done)
    CDocScriptSkipLine();

  /* If line is longer than the width of the display area then
     split into smaller lines */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent;

  if (width <= 0) {
    CDocScriptError("Invalid Page Width %d - No room for Text", width);
    return;
  }

  if (CDocStringDisplayLength(str) > width) {
    char **lines;
    int    no_lines;

    output_header_skip = false;

    CDocFormatStringInWidth(str, width, CHALIGN_TYPE_LEFT, &lines, &no_lines);

    for (i = 0; i < no_lines; i++) {
      j = strlen(lines[i]) - 1;

      while (j >= 0 && isspace(lines[i][j]))
        lines[i][j--] = '\0';

      CDocScriptWriteRightJustifiedHeader(lines[i]);
    }

    CDocFreeFormattedStrings(lines, no_lines);

    output_header_skip = true;

    goto WriteRightJustifiedHeader;
  }

  /* Output the Header Right Justified in Bold Underscored type
     (using macro) in Troff */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteCommand(".RE \"");
    CDocScriptWriteText("%s", str.c_str());
    CDocScriptWriteCommand("\"\n");
  }

  /* Output Header manually right justified in Bold type in
     CDoc or Raw Text with Control Codes */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    length = CDocStringDisplayLength(str);

    no_spaces = cdoc_right_margin - cdoc_left_margin - length;

    CDocScriptWriteIndent(no_spaces);

    CDocScriptWriteText("%s%s%s\n", CDocStartBold(), str.c_str(), CDocEndBold());
  }

  /* Output Header manually centred and Underlined for
     any other Format */

  else {
    length = CDocStringDisplayLength(str);

    no_spaces = cdoc_right_margin - cdoc_left_margin - length;

    CDocScriptWriteIndent(no_spaces);

    CDocScriptWriteText("%s\n", str.c_str());

    CDocScriptWriteIndent(no_spaces);

    temp_string = CStrUtil::strdup(str);

    for (i = 0; i < CDocStringDisplayLength(str); i++)
      temp_string[i] = '-';

    CDocScriptWriteText("%s\n", temp_string);

    delete [] temp_string;
  }

WriteRightJustifiedHeader:
  /* Output Newline after Header */

  if (output_header_skip)
    CDocScriptSkipLine();

  /* Set Paragraph Done flag to False as not Paragraph has
     been output for this new section yet */

  cdoc_paragraph_done = false;
}

// Write the supplied str as underlined text.
extern void
CDocScriptWriteUnderlinedText(const std::string &str)
{
  /* For Underscored Text in Troff use previously defined 'US' macro */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteCommand(".US \"");
    CDocScriptWriteText("%s", str.c_str());
    CDocScriptWriteCommand("\"\n");
  }

  /* For Underscored Text in CDoc or Raw Text with Control Codes
     use escape codes */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteText("%s%s%s\n", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
  }

  /* For Underscored Text in HTML use 'u' command */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<u>%s</u>", CDocEncodeHtmlString(str));

  /* For Underscored Text in Interleaf use appropriate font */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    CDocScriptWriteCommand("%s%s%s", ILEAF_UNDERLINE_FONT,
                           CDocEncodeHtmlString(str), ILEAF_NORMAL_FONT);

  /* For Underscored Text in any other format draw simulate
     underline with hyphens */

  else {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteText("%s\n", str.c_str());

    CDocScriptDrawHLine(0, CDocStringDisplayLength(str));
  }
}

// Write the supplied str in the font style specified.
extern void
CDocScriptWriteFontText(const std::string &str, int font_type)
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s", CDocStartUnderline(), str.c_str(), CDocEndUnderline());
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteText("%s%s%s", CDocStartBold(), str.c_str(), CDocEndBold());
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteText("%s%s%s",
        CDocStartBoldUnderline(), str.c_str(), CDocEndBoldUnderline());
    else
      CDocScriptWriteText("%s", str.c_str());
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand(".US \"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand(".BD \"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand(".BU \"");

    CDocScriptWriteText("%s", str.c_str());

    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("\"");
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("\"");
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("<u>%s</u>", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("<b>%s</b>", CDocEncodeHtmlString(str));
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("<b><u>%s</u></b>", CDocEncodeHtmlString(str));
    else
      CDocScriptWriteCommand("%s", CDocEncodeHtmlString(str));
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    if      (font_type == CDOC_UNDERLINE_FONT)
      CDocScriptWriteCommand("%s%s%s", ILEAF_UNDERLINE_FONT,
                             CDocEncodeHtmlString(str), ILEAF_NORMAL_FONT);
    else if (font_type == CDOC_BOLD_FONT)
      CDocScriptWriteCommand("%s%s%s", ILEAF_BOLD_FONT,
                             CDocEncodeHtmlString(str), ILEAF_NORMAL_FONT);
    else if (font_type == CDOC_BOLD_UNDERLINE_FONT)
      CDocScriptWriteCommand("%s%s%s", ILEAF_BOLD_UNDERLINE_FONT,
                             CDocEncodeHtmlString(str), ILEAF_NORMAL_FONT);
    else
      CDocScriptWriteCommand("%s", CDocEncodeHtmlString(str));
  }
  else {
    CDocScriptWriteText("%s", str.c_str());
  }
}

// Write a line of text after it has been formatted using 'printf' style format codes.
//
// The text is written at the current indentation in untranslated form (i.e. as is)
// with a trailing new line.
// The only changes performed are those required to ensure the text is output in its
// original form by the final display program/device.
extern void
CDocScriptWriteLine(const std::string &format, ...)
{
  int     i;
  va_list args;
  char    line[256];

  /*CONSTANTCONDITION*/
  va_start(args, format);

  /* Output current indentation */

  CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

  /* Create actual line std::string from format and arguments */

  vsprintf(line, format.c_str(), args);

  /* If outputting in Troff format then check for leading dot
     or single quote characters (which are considered special
     by Troff) and Escape them with a preceding '\&', or an
     inline '\' character (which is considered special by Troff)
     and Escape it with another '\' character. */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    for (i = 0; i < int(strlen(line)); i++) {
      /* Check for leading dot or single quote characters */

      if (i == 0) {
        if (line[i] == '.' || line[i] == '\'') {
          fputc('\\', cdoc_output_fp);
          fputc('&' , cdoc_output_fp);
        }
      }

      /* Check for inline '\' character */

      else {
        if (line[i] == '\\')
          fputc('\\', cdoc_output_fp);
      }

      CDocScriptFPutc(line[i]);
    }

    CDocScriptFPutc('\n');
  }

  /* For other formats just output the line */

  else {
    CDocScriptFPuts(line);
    CDocScriptFPutc('\n');
  }

  va_end(args);
}

// Write some text after it has been formatted using 'printf' style format codes.
//
// The text is written at the current position and any inline Escape Codes are
// translated to commands used to generate the appropriate font style in
// the specified output format.
extern void
CDocScriptWriteText(const std::string &format, ...)
{
  int       no;
  char     *p1;
  char     *p2;
  va_list   args;
  char      line[256];

  /*CONSTANTCONDITION*/
  va_start(args, format);

  vsprintf(line, format.c_str(), args);

  va_end(args);

  /* If we are outputting in Troff format at the start of the
     line and the text starts with a '.', single quote, or '\'
     then precede it with '\&' to escape it */

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF && cdoc_char_no == 0) {
    if (line[0] == '.' || line[0] == '\'' || line[0] == '\\')
      fputs("\\&", cdoc_output_fp);
  }

  /* If we are outputting in Troff format replace Escape Codes
     with inline Troff Font Style Commands */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    static int  last_escape = -1;

    p1 = &line[0];

    /* Process each line ending in a Newline Character */

    while ((p2 = strchr(p1, '\n')) != NULL) {
      /* Blank out Newline Character */

      *p2 = '\0';

      /* Process Line */

      while (*p1 != '\0') {
        /* If we have an Escape Code then replace it with
           inline Troff Font Style Commands */

        if ((no = CDocIsEscapeCode(p1)) != 0) {
          /* Output inline Troff Command dependant on
             current escape code and previous one */

          if      (*(p1 + 2) == '0') {
            if      (last_escape == 1)
              CDocScriptWriteCommand("\\fP");
            else if (last_escape == 3)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'");
            else if (last_escape == 4)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'\\fP");

            last_escape = -1;
          }
          else if (*(p1 + 2) == '1') {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 1;
          }
          else if (*(p1 + 2) == '3') {
            CDocScriptFPutc('\n');

            last_escape = 3;
          }
          else if (*(p1 + 2) == '4') {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 4;
          }
          else {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 1;
          }

          /* Skip Escape Code */

          p1 += 4;
        }

        /* Otherwise just output character */

        else {
          CDocScriptFPutc(*p1);

          p1++;
        }
      }

      /* Output Newline */

      CDocScriptFPutc('\n');

      /* Next Line */

      p1 = p2 + 1;
    }

    /* Process any remaining characters */

    if (*p1 != '\0') {
      while (*p1 != '\0') {
        /* If we have an Escape Code then replace it with
           inline Troff Font Style Commands */

        if ((no = CDocIsEscapeCode(p1)) != 0) {
          /* Output inline Troff Command dependant on
             current escape code and previous one */

          if      (*(p1 + 2) == '0') {
            if      (last_escape == 1)
              CDocScriptWriteCommand("\\fP");
            else if (last_escape == 3)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'");
            else if (last_escape == 4)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'\\fP");

            last_escape = -1;
          }
          else if (*(p1 + 2) == '1') {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 1;
          }
          else if (*(p1 + 2) == '3') {
            CDocScriptFPutc('\n');

            last_escape = 3;
          }
          else if (*(p1 + 2) == '4') {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 4;
          }
          else {
            CDocScriptWriteText("\n");
            CDocScriptWriteCommand("\\fB");

            last_escape = 1;
          }

          /* Skip Escape Code */

          p1 += 4;
        }

        /* Otherwise just output character */

        else {
          CDocScriptFPutc(*p1);

          p1++;
        }
      }
    }
  }

  /* If we are Outputting in RAW format then remove Escape
     Codes from the Text */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW) {
    p1 = &line[0];

    /* Process each line ending in a Newline Character */

    while ((p2 = strchr(p1, '\n')) != NULL) {
      /* Blank out Newline Character */

      *p2 = '\0';

      /* Process Line */

      while (*p1 != '\0') {
        /* If we have an Escape Code then Skip it */

        if ((no = CDocIsEscapeCode(p1)) != 0)
          p1 += no;

        /* Otherwise just output the character */

        else {
          CDocScriptFPutc(*p1);

          p1++;
        }
      }

      /* Output Newline */

      CDocScriptFPutc('\n');

      /* Next Line */

      p1 = p2 + 1;
    }

    /* Process Rest of Line */

    if (*p1 != '\0') {
      while (*p1 != '\0') {
        /* If we have an Escape Code then Skip it */

        if ((no = CDocIsEscapeCode(p1)) != 0)
          p1 += no;

        /* Otherwise just output the character */

        else {
          CDocScriptFPutc(*p1);

          p1++;
        }
      }
    }
  }

  /* Any other format just output the line as is */

  else {
    p1 = &line[0];

    /* Output each line ending in a Newline Character */

    while ((p2 = strchr(p1, '\n')) != NULL) {
      /* Blank out Newline Character */

      *p2 = '\0';

      /* Output Line */

      CDocScriptFPuts(p1);
      CDocScriptFPutc('\n');

      /* Next Line */

      p1 = p2 + 1;
    }

    /* Output Remaining Characters */

    if (*p1 != '\0')
      CDocScriptFPuts(p1);
  }
}

// Write some text after it has been formatted using 'printf' style format codes.
//
// The text is written at the current position as is, i.e. with no translation of
// the characters in the text.
extern void
CDocScriptWriteCommand(const std::string &format, ...)
{
  char        *p1;
  char        *p2;
  va_list      args;
  static char *line = NULL;

  if (line == NULL)
    line = new char [4096];

  /*CONSTANTCONDITION*/
  va_start(args, format);

  vsprintf(line, format.c_str(), args);

  va_end(args);

/*
  if (line[0] == '\n') {
    cdoc_line_no++;

    CDocScriptOutputFootnotes(false);
  }
*/

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      CDOC_IS_CMD(line, CDOC_COMMAND_CMD)) {
    cdoc_line_no += 3;

    cdoc_line_fiddle += 2;

    CDocScriptOutputFootnotes(false);
  }

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      CDOC_IS_CMD(line, CDOC_SET_LINE_CMD)) {
    char  *p;
    int    no_lines;

    p = &line[strlen(CDOC_SET_LINE_CMD)];

    CStrUtil::skipSpace(&p);

    if (*p == '\0')
      no_lines = 0;
    else
      no_lines = CStrUtil::toInteger(p);

    if (no_lines == 0) {
      CDocScriptError("Invalid CDoc Set Line Command");
      return;
    }

    cdoc_line_no += no_lines;
  }

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      (CDOC_IS_CMD(line, CDOC_BITMAP_CMD) ||
       CDOC_IS_CMD(line, CDOC_XWD_CMD   ) ||
       CDOC_IS_CMD(line, CDOC_XWD_Z_CMD ) ||
       CDOC_IS_CMD(line, CDOC_IMAGE_CMD ))) {
    char  *p;
    int    no_lines;

    if      (CDOC_IS_CMD(line, CDOC_BITMAP_CMD))
      p = &line[strlen(CDOC_BITMAP_CMD)];
    else if (CDOC_IS_CMD(line, CDOC_XWD_CMD))
      p = &line[strlen(CDOC_XWD_CMD   )];
    else if (CDOC_IS_CMD(line, CDOC_XWD_Z_CMD))
      p = &line[strlen(CDOC_XWD_Z_CMD )];
    else
      p = &line[strlen(CDOC_IMAGE_CMD )];

    CStrUtil::skipSpace(&p);

    CStrUtil::skipNonSpace(&p);

    CStrUtil::skipSpace(&p);

    no_lines = 0;

    if (*p != '\0') {
      std::vector<std::string> words;

      CStrUtil::addWords(p, words);

      if (words.size() > 0)
        no_lines = CStrUtil::toInteger(words[0]);
    }

    if (no_lines == 0) {
      if (CDOC_IS_CMD(line, CDOC_BITMAP_CMD))
        CDocScriptError("Invalid CDoc Bitmap Command");
      else
        CDocScriptError("Invalid CDoc X Window Dump Command");

      return;
    }

    if (cdoc_page_no != -1) {
      CDocScriptFindSpace(no_lines);

      cdoc_line_no += no_lines;

      CDocScriptOutputFootnotes(false);
    }
    else {
      CDocScriptSetCurrentFigureDepth(no_lines);

      cdoc_line_no += no_lines;

      CDocScriptOutputFootnotes(false);
    }

    cdoc_line_fiddle += no_lines - 1;
  }

  p1 = &line[0];

  /* Output each line ending in a Newline Character */

  while ((p2 = strchr(p1, '\n')) != NULL) {
    /* Blank out Newline Character */

    *p2 = '\0';

    /* Output Command Marker if in Temporary File */

    if (cdoc_page_no == -1)
      fputc(CMD_MARK_C, cdoc_output_fp);

    /* Output Line */

    fputs(p1  , cdoc_output_fp);
    fputc('\n', cdoc_output_fp);

    /* Next Line */

    p1 = p2 + 1;
  }

  /* Output any remaining characters */

  if (*p1 != '\0') {
    /* Output Command Marker if in Temporary File */

    if (cdoc_page_no == -1)
      fputc(CMD_MARK_C, cdoc_output_fp);

    /* Output characters */

    fputs(p1, cdoc_output_fp);
  }
}

// Output String to File keeping count of pages, lines and characters output.
//
// All text (not including formatting commands for the various output formats)
// should come through this routine or CDocScriptFPutc() so an accurate count of
// pages, lines and characters can be kept.
extern void
CDocScriptFPuts(const std::string &str)
{
  int no;

  const char *p = str.c_str();

  while (*p != '\0') {
    if      ((no = CDocIsEscapeCode(p)) != 0) {
      if      (CDOC_IS_FONT_ESC(p))
        current_font_type = p[2] - '0';
      else if (CDOC_IS_FOREGROUND_ESC(p))
        current_colour = p[3] - '0';
      else if (CDOC_IS_GOTO_ESC(p)) {
        int   i;
        int   j;
        char  page_string[64];

        i = 2;
        j = 0;

        while (p[i] != '\0' && p[i] != 'B' && p[i] != 'E')
          page_string[j++] = p[i++];
        page_string[j] = '\0';

        if (current_goto_page != NULL)
          delete [] current_goto_page;

        if (p[i] == 'B')
          current_goto_page = CStrUtil::strdup(page_string);
        else
          current_goto_page = NULL;
      }

      while (no > 0) {
        fputc(*p++, cdoc_output_fp);

        no--;
      }
    }
    else if ((no = CDocIsPageReference(p)) != 0) {
      for (; no > 0; no--)
        fputc(*p++, cdoc_output_fp);
    }
    else
      CDocScriptFPutc(*p++);
  }
}

// Output Character to File keeping count of pages, lines and characters output.
//
// All text (not including formatting commands for the various output formats) should
// come through this routine or CDocScriptFPuts() so an accurate count of
// pages, lines and characters can be kept.
extern void
CDocScriptFPutc(int c)
{
  if (cdoc_page_no != -1 && ! cdoc_page_header_output) {
    CDocScriptWritePageHeader();

    cdoc_line_no++;

    CDocScriptOutputFootnotes(false);
  }

  if ((CDocInst->getDebug() & CDOC_DEBUG_OUTPUT) &&
      cdoc_char_no == 0 && cdoc_page_no != -1) {
    char  temp[32];

    sprintf(temp, "%6d> ", cdoc_line_no);

    fputs(temp, cdoc_output_fp);
  }

  if (c == '\n') {
    fputc('\n', cdoc_output_fp);

    cdoc_line_no++;

    if (CDocIsPagedOutput())
      CDocScriptFindSpace(1);

    cdoc_char_no = 0;

    CDocScriptOutputFootnotes(false);
  }
  else {
    if (cdoc_page_no != -1 && cdoc_char_no == 0) {
      int  rc;

      rc = CDocScriptGetRevisionControlChar();

      if     (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
        if (rc != ' ')
          CDocScriptWriteCommand(CDOC_REV_CNTRL_TMPL, rc);
      }
      else
        fputc(rc, cdoc_output_fp);
    }

    if (CDocInst->getOutputFormat() != CDOC_OUTPUT_CDOC) {
      if (c >= BOX_HOR_C && c <= BOX_TL__C)
        c = box_chars[c - BOX_HOR_C];
    }

    fputc(c, cdoc_output_fp);

    cdoc_char_no++;
  }
}

// Draw a Horizontal line of the specified width and indent for the current
// output format.
extern void
CDocScriptDrawHLine(int x1, int x2)
{
  int  i;

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    int  x11;
    int  x22;

    cdoc_line_no--;

    CDocScriptWriteText("\n");

    if (x1 > 0) x11 = (x1 + 1)*10; else x11 = x1*10;
    if (x2 > 0) x22 = (x2 + 1)*10; else x22 = x2*10;

    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_left_margin);

    if (x1 >= 0) {
      if (x2 >= 0)
        CDocScriptWriteCommand("%s i+%d -5 i+%d -5\n",
          CDOC_DRAW_LINE_CMD, x11, x22);
      else
        CDocScriptWriteCommand("%s i+%d -5 %d -5\n",
          CDOC_DRAW_LINE_CMD, x11, x22);
    }
    else {
      if (x2 >= 0)
        CDocScriptWriteCommand("%s %d -5 i+%d -5\n",
          CDOC_DRAW_LINE_CMD, x11, x22);
      else
        CDocScriptWriteCommand("%s %d -5 %d -5\n",
          CDOC_DRAW_LINE_CMD, x11, x22);
    }

    cdoc_line_no++;
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<hr>\n");
  }
  else {
    if (x1 < 0) x1 += cdoc_right_margin;
    if (x2 < 0) x2 += cdoc_right_margin;

    CDocScriptWriteIndent(cdoc_last_indent);

    for (i = 0; i < x1; i++)
      CDocScriptWriteText(" ");

    for (i = 0; i < x2 - x1 + 1; i++)
      CDocScriptWriteText("-");

    CDocScriptWriteText("\n");
  }
}

// Draw a Vertical line of the specified height at the specified x character
// coordinate for the current output format.
extern void
CDocScriptDrawVLine(int x, int height)
{
  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    int x1;

    if (x > 0) x1 = (x + 1)*10; else x1 = x*10;

    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_left_margin);

    if      (height > 0) {
      if (x >= 0)
        CDocScriptWriteCommand("%s i+%d -5 i+%d %d\n",
          CDOC_DRAW_LINE_CMD, x1, x1, (height + 1)*10 - 5);
      else
        CDocScriptWriteCommand("%s %d -5 %d %d\n",
          CDOC_DRAW_LINE_CMD, x1, x1, (height + 1)*10 - 5);
    }
    else if (height < 0) {
      if (x >= 0)
        CDocScriptWriteCommand("%s i+%d -5 i+%d %d\n",
          CDOC_DRAW_LINE_CMD, x1, x1, (height - 1)*10 - 5);
      else
        CDocScriptWriteCommand("%s %d -5 %d %d\n",
          CDOC_DRAW_LINE_CMD, x1, x1, (height - 1)*10 - 5);
    }
    else {
      if (x >= 0)
        CDocScriptWriteCommand("%s i+%d -5 i+%d 5\n",
          CDOC_DRAW_LINE_CMD, x1, x1);
      else
        CDocScriptWriteCommand("%s %d -5 %d 5\n",
          CDOC_DRAW_LINE_CMD, x1, x1);
    }
  }
}

// Get the number of lines available for text on the current page.
extern int
CDocScriptGetLinesPerPage()
{
  if (cdoc_document.type == CDOC_MEMO_DOCUMENT) {
    if (cdoc_page_no == 1)
      return(cdoc_lines_per_page - 2);
    else
      return(cdoc_lines_per_page - 7);
  }
  else
    return(cdoc_lines_per_page - 5);
}

// Save the current state when output to the body is suspended so that it can
// be restored after the footer and header have been output.
//
// Saves the Font Type, Colour, Goto Page, Indent and whether a Paragraph is
// Currently being output.
extern void
CDocScriptSaveBodyState()
{
  CDBodyState *body_state = new CDBodyState;

  body_state->font_type            = current_font_type;
  body_state->colour               = current_colour;
  body_state->goto_page            = current_goto_page;
  body_state->indent               = cdoc_last_indent;
  body_state->outputting_paragraph = cdoc_outputting_paragraph;
  body_state->formatting           = cdoc_formatting;

  body_state_list.push_back(body_state);

  /*----------*/

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    if (cdoc_outputting_paragraph) {
      if (cdoc_paragraph_last_line)
        CDocScriptWriteCommand(CDOC_PARAGRAPH_END_TMPL);
      else
        CDocScriptWriteCommand(CDOC_PARAGRAPH_STOP_TMPL);
    }

    if (current_goto_page != NULL)
      CDocScriptWriteCommand(CDOC_GOTO_END_TMPL);

    if (! cdoc_formatting)
      CDocScriptWriteCommand(CDOC_FORMAT_TMPL, "on");
  }

  current_font_type = 0;
  current_colour    = 0;
  current_goto_page = NULL;
  cdoc_last_indent  = -1;

  cdoc_outputting_paragraph = false;
}

// Restore the state saved by CDocScriptSaveBodyState().
//
// Resets the Font Type, Colour, Goto Page, Indent and whether a Paragraph is
// Currently being output.
extern void
CDocScriptRestoreBodyState()
{
  CDBodyState *body_state = body_state_list.back();

  body_state_list.pop_back();

  current_font_type         = body_state->font_type;
  current_colour            = body_state->colour;
  current_goto_page         = body_state->goto_page;
  cdoc_last_indent          = body_state->indent;
  cdoc_outputting_paragraph = body_state->outputting_paragraph;
  cdoc_formatting           = body_state->formatting;

  delete body_state;

  /*----------*/

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_last_indent);

    if (current_font_type != 0)
      CDocScriptWriteCommand(CDOC_FONT_START_TMPL, current_font_type);

    if (current_colour != 0)
      CDocScriptWriteCommand(CDOC_COLOUR_START_TMPL, current_colour);

    if (current_goto_page != NULL)
      CDocScriptWriteCommand(CDOC_GOTO_START_TMPL, current_goto_page);

    if (cdoc_outputting_paragraph)
      CDocScriptWriteCommand(CDOC_PARAGRAPH_START_TMPL,
                             cdoc_right_margin);

    if (! cdoc_formatting)
      CDocScriptWriteCommand(CDOC_FORMAT_TMPL, "off");
  }
}

extern char *
CDocEncodeHtmlString(const std::string &str)
{
  static char *buffer;
  static int   buffer_len;

  int i;
  int k;

  int len = str.size();

  if (5*len >= buffer_len) {
    buffer_len = 5*len + 1;

    if (buffer == NULL)
      buffer = new char [buffer_len];
    else {
      char *buffer1 = new char [buffer_len];

      strcpy(buffer1, buffer);

      delete [] buffer;

      buffer = buffer1;
    }
  }

  i = 0;
  k = 0;

  while (i < len) {
    if      (str[i] == ESC && (str[i + 1] == '<' || str[i + 1] == '>')) {
      i++;

      buffer[k++] = str[i++];
    }
    else if (str[i] == '<') {
      buffer[k++] = '&';
      buffer[k++] = 'l';
      buffer[k++] = 't';
      buffer[k++] = ';';

      i++;
    }
    else if (str[i] == '>') {
      buffer[k++] = '&';
      buffer[k++] = 'g';
      buffer[k++] = 't';
      buffer[k++] = ';';

      i++;
    }
    else
      buffer[k++] = str[i++];
  }

  buffer[k] = '\0';

  return(buffer);
}

extern char *
CDocEncodeILeafString(const std::string &str)
{
  static char *buffer;
  static int   buffer_len;

  int i;
  int k;

  int len = str.size();

  if (3*len >= buffer_len) {
    buffer_len = 3*len + 1;

    if (buffer == NULL)
      buffer = new char [buffer_len];
    else {
      char *buffer1 = new char [buffer_len];

      strcpy(buffer1, buffer);

      delete [] buffer;

      buffer = buffer1;
    }
  }

  i = 0;
  k = 0;

  while (i < len) {
    if      (str[i] == ESC && str[i + 1] == '<') {
      i++;

      buffer[k++] = str[i++];
    }
    else if (str[i] == '<') {
      buffer[k++] = '<';
      buffer[k++] = '<';

      i++;
    }
    else
      buffer[k++] = str[i++];
  }

  buffer[k] = '\0';

  return buffer;
}
