#include "CDocI.h"

/*-----------------------------------------------------------------------*/

CDParagraph *cdoc_paragraph            = NULL;
bool         cdoc_in_paragraph         = false;
bool         cdoc_paragraph_done       = false;
bool         cdoc_outputting_paragraph = false;
bool         cdoc_paragraph_last_line  = false;

static char  highlight_string[128] = "";

/*-----------------------------------------------------------------------*/

static void CDocScriptOutputUnformattedParagraph();

// Initialise the Paragraph Control Structure ready to start processing
// IBM Script Text.
extern void
CDocScriptInitParagraphs()
{
  cdoc_paragraph = new CDParagraph;

  /*-------*/

  cdoc_in_paragraph   = false;
  cdoc_paragraph_done = false;
}

// Terminate the Paragraph Control Structure after processing of the
// IBM Script Text has finished.
extern void
CDocScriptTermParagraphs()
{
  CDocScriptTermParagraphHighlights();

  delete cdoc_paragraph;

  cdoc_paragraph = NULL;
}

// Start a new paragraph of the specified type containing the supplied str.
extern void
CDocScriptNewParagraph(const std::string &str, int type, int formatted, int justification)
{
  /* Set the flag to indicate we are in a paragraph
     and set the paragraph type */

  cdoc_in_paragraph = true;

  cdoc_paragraph->setType(type);
  cdoc_paragraph->setFormatted(formatted);
  cdoc_paragraph->setJustification(justification);

  /* Add the supplied text to an empty paragraph */

  cdoc_paragraph->setText("");

  CDocScriptAddStringToParagraph(str);

  /* Reset Highlight List and Warn user if any left */

  if (cdoc_paragraph->getNumHighlights() != 0) {
    CDocScriptWarning("Unterminated Highlighting on Previous Paragraph");

    CDocScriptTermParagraphHighlights();
  }
}

// Add the supplied str the the current paragraph text.
extern void
CDocScriptAddStringToParagraph(const std::string &str)
{
  /* Spell Check if Required */

  if (cdoc_spell_check && cdoc_spell_active)
    CSpellCheckString(str);

  cdoc_paragraph->addText(str);
}

// Start Highlighting at the specified Level and return the Highlight
// String for the Level.
extern const char *
CDocScriptStartParagraphHighlight(int level)
{
  cdoc_paragraph->addHighlight(level);

  if      (level == 0)
    return CDocStartNormal();
  else if (level == 1)
    return CDocStartUnderline();
  else if (level == 2)
    return CDocStartBold();
  else if (level == 3)
    return CDocStartBoldUnderline();
  else
    return CDocStartBold();
}

// End Highlighting at the specified Level and return the Highlight String
// to end the Level.
//
// The levels in pairs of CDocScriptStartParagraphHighlight() and
// CDocScriptEndParagraphHighlight() calls must match.
extern const char *
CDocScriptEndParagraphHighlight(int level)
{
  highlight_string[0] = '\0';

  if (cdoc_paragraph->getNumHighlights() == 0) {
    CDocScriptError("Invalid Level %d End Highlight", level);
    return "";
  }

  int highlight = cdoc_paragraph->getHighlight(cdoc_paragraph->getNumHighlights() - 1);

  if (highlight != level) {
    CDocScriptError("Invalid Level %d End Highlight", level);
    return "";
  }

  cdoc_paragraph->removeHighlight();

  if (cdoc_paragraph->getNumHighlights() > 0)
    highlight = cdoc_paragraph->getHighlight(cdoc_paragraph->getNumHighlights() - 1);
  else
    highlight = 0;

  int new_level = highlight;

  if      (level == 0)
    strcpy(highlight_string, CDocEndNormal());
  else if (level == 1)
    strcpy(highlight_string, CDocEndUnderline());
  else if (level == 2)
    strcpy(highlight_string, CDocEndBold());
  else if (level == 3)
    strcpy(highlight_string, CDocEndBoldUnderline());
  else
    strcpy(highlight_string, CDocEndBold());

  if      (new_level == 0)
    strcat(highlight_string, CDocStartNormal());
  else if (new_level == 1)
    strcat(highlight_string, CDocStartUnderline());
  else if (new_level == 2)
    strcat(highlight_string, CDocStartBold());
  else if (new_level == 3)
    strcat(highlight_string, CDocStartBoldUnderline());
  else
    strcat(highlight_string, CDocStartBold());

  return highlight_string;
}

// End Highlighting for all currently active Highlights.

extern void
CDocScriptTermParagraphHighlights()
{
  cdoc_paragraph->clearHighlights();
}

// Set the Paragraph Prefix String's Text, Font and indentation. The Paragraph
// Prefix is used for Definition List Terms and Notes so that the Paragraph has an
// identifying string on its Left Hand Side.
extern void
CDocScriptSetParagraphPrefix(const std::string &str, int font, int indent)
{
  cdoc_paragraph->setPrefix(str, font, indent);
}

// Output the current paragraph text, and associated strings, formatted so that it
// fits between the left and right margins with the current indentation.
//
// This formatting is unnecessary for the Troff output formats as they will do this
// themselves. The formatting is still carried out to help these programs (??) and
// to produce readable output for these formats.
extern void
CDocScriptOutputParagraph()
{
  int  i;
  int  j;
  int  no;
  int  pos;
  int  width;
  int  indent1;
  char word[256];
  char last_char;
  int  left_margin;
  int  indent_paragraph;
  int  save_left_margin;

  /* If Paragraph is Unformatted, i.e. output as is, then
     Output it and Return */

  if (! cdoc_paragraph->getFormatted()) {
    if (cdoc_paragraph_done)
      CDocScriptSkipLine();

    /* If Paragraph has a line to be output before it then
       output it */

    if (cdoc_paragraph->getPreLine() != "") {
      CDocScriptWriteCommand("%s", cdoc_paragraph->getPreLine().c_str());

      cdoc_paragraph->setPreLine("");
    }

    /* If Paragraph is Centred, i.e. each line is centred
       between left and right margins then Output it and Return */

    if (cdoc_paragraph->getJustification() == CENTRE_JUSTIFICATION) {
      const char *p1;

      const std::string &text = cdoc_paragraph->getText();

      const char *p = text.c_str();

      while ((p1 = strchr(p, '\n')) != NULL) {
        std::string line = std::string(p[0], p1 - p);

        CDocScriptWriteCentreJustifiedLine(line.c_str(), CDOC_NORMAL_FONT);

        p = p1 + 1;
      }

      CDocScriptWriteCentreJustifiedLine(p, CDOC_NORMAL_FONT);
    }
    else
      CDocScriptOutputUnformattedParagraph();

    cdoc_paragraph->setPrefix("");

    cdoc_in_paragraph   = false;
    cdoc_paragraph_done = true;

    return;
  }

  /* Default is to indent Paragraph Text on First Line */

  indent_paragraph = true;

  /* If a Paragraph has been output in the Current Section
     then add a new line to separate from previous Paragraph */

  if (cdoc_paragraph_done) {
    if (CDocScriptGetCurrentListCompact())
      CDocScriptNewLine();
    else
      CDocScriptSkipLine();
  }

  /* Find Space for a least two lines */

  CDocScriptFindSpace(2);

  /* If Paragraph has a line to be output before it then
     output it */

  if (cdoc_paragraph->getPreLine() != "") {
    CDocScriptWriteCommand("%s", cdoc_paragraph->getPreLine().c_str());

    cdoc_paragraph->setPreLine("");
  }

  /* Save Initial Left Margin */

  save_left_margin = cdoc_left_margin;

  /* Set Paragraph Indentation and Left Margin for various
     types of Paragraphs */

  /* List Paragraph is not Indented and has its Left
     Margin at any parent List's Left Margin */

  if      (cdoc_paragraph->getType() == LIST_PARAGRAPH) {
    indent_paragraph = false;

    left_margin = CDocScriptGetPreviousListLeftMargin();

    if (left_margin != -1)
      cdoc_left_margin = left_margin;
  }

  /* Sub List Paragraph which is not Indented and has its Left
     at the current List's Left Margin */

  else if (cdoc_paragraph->getType() == SUB_LIST_PARAGRAPH1) {
    indent_paragraph = false;

    left_margin = CDocScriptGetCurrentListLeftMargin();

    if (left_margin != -1)
      cdoc_left_margin = left_margin;
  }

  /* Sub List Paragraph which is not Indented and has its Left
     Margin at any parent List's Left Margin */

  else if (cdoc_paragraph->getType() == SUB_LIST_PARAGRAPH2) {
    indent_paragraph = false;

    left_margin = CDocScriptGetPreviousListLeftMargin();

    if (left_margin != -1)
      cdoc_left_margin = left_margin;
  }

  /* Definition Paragraph is not Indented and has its Left
     Margin at any parent List's Left Margin */

  else if (cdoc_paragraph->getType() == DEFINITION_PARAGRAPH) {
    indent_paragraph = false;

    left_margin = CDocScriptGetPreviousListLeftMargin();

    if (left_margin != -1)
      cdoc_left_margin = left_margin;
  }

  /* Glossary Paragraph is not Indented */

  else if (cdoc_paragraph->getType() == GLOSSARY_PARAGRAPH) {
    indent_paragraph = false;
  }

  /* If Paragraph has a Header (e.g. Definition List Terms
     which are on a single line) then Output it */

  if (cdoc_paragraph->getHeader() != "") {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteText("%s", cdoc_paragraph->getHeader().c_str());

    cdoc_paragraph->setHeader("");
  }

  /* Initialise Current Character Position */

  pos = 0;

  /* If Paragraph has a Prefix (e.g. Definition List Terms
     which are on the same line as the Description, Notes
     etc.) then Output it in the specified font style ... */

  if (cdoc_paragraph->getPrefix() != "") {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteFontText(cdoc_paragraph->getPrefix().c_str(),
                            cdoc_paragraph->getPrefixFont());

    /* Troff : back up one line (to overwrite the line just written) */

    if     (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
      cdoc_line_no--;

      CDocScriptWriteText("\n");
      CDocScriptWriteCommand(".sp -1\n");

      cdoc_line_no++;

      CDocScriptWriteCommand(".in %.1lfm\n",
        CDocCharsToEms(cdoc_left_margin + cdoc_indent +
          CDocStringDisplayLength(cdoc_paragraph->getPrefix())));
    }
    else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
      cdoc_line_no--;

      CDocScriptWriteText("\n");
      CDocScriptWriteCommand(CDOC_SET_LINE_TMPL, -1);

      cdoc_line_no++;

      CDocScriptWriteCommand(CDOC_PARAGRAPH_BEGIN_TMPL,
                             cdoc_right_margin);

      cdoc_outputting_paragraph = true;

      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent +
        CDocStringDisplayLength(cdoc_paragraph->getPrefix()));
    }

    /* If we have a Definition Paragraph which has its term
       and description on one line then indent the left margin
       for lines after the first by the Definition Depth,
       otherwise indent the left margin by the length of the
       Paragraph Prefix */

    if (cdoc_paragraph->getType() == DEFINITION_PARAGRAPH &&
        cdoc_definition_list != NULL &&
        cdoc_definition_list->break_up == false) {
      cdoc_left_margin += cdoc_definition_list->depth;

      /* Set the Current Character Position */

      pos = CDocStringDisplayLength(cdoc_paragraph->getPrefix()) -
            cdoc_definition_list->depth;
    }
    else {
      if (cdoc_paragraph->getIndentPrefix())
        cdoc_left_margin += CDocStringDisplayLength(cdoc_paragraph->getPrefix());
      else
        pos = CDocStringDisplayLength(cdoc_paragraph->getPrefix());
    }

    /* Set the Paragraph Line Indent to Zero */

    indent1 = 0;

    cdoc_paragraph->setPrefix("");
  }

  /* ... otherwise set indent for first line dependant on whether
     first line is indented or not */

  else {
    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
      CDocScriptWriteCommand(CDOC_PARAGRAPH_BEGIN_TMPL,
                             cdoc_right_margin);

      cdoc_outputting_paragraph = true;
    }

    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
      if (indent_paragraph)
        CDocScriptWriteCommand
          (".ti +%.1lfm\n", CDocCharsToEms(2*cdoc_paragraph_indent));
    }
    else {
      if (indent_paragraph)
        CDocScriptWriteIndent(cdoc_paragraph_indent);
    }

    /* Set the Paragraph Line Indent to the required value if
       the first line is indented, otherwise set it to Zero */

    if (indent_paragraph)
      indent1 = cdoc_paragraph_indent;
    else
      indent1 = 0;
  }

  /* Process Paragraph Text */

  const char *text = cdoc_paragraph->getText().c_str();

  i         = 0;
  j         = 0;
  last_char = '\0';

  while (text[i] != '\0') {
    /* If we have an Escape Code in the Paragraph then
       Process it (Troff format only) and skip it as it
       is not counted as a character */

    if      ((no = CDocIsEscapeCode(&text[i])) != 0) {
      /* Replace Escape Code with inline Troff Command */

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
        static int  last_escape = -1;

        /* Terminate current word */

        word[j] = '\0';

        j = 0;

        /* Write any Previous separator character */

        if (last_char != '\0')
          CDocScriptWriteText("%c", last_char);

        /* Write current word */

        CDocScriptWriteText("%s", word);

        /* Start a new line */

        pos = 0;

        /* Output inline Troff Command dependant on
           current escape code and previous one */

        if      (text[i + 2] == '0') {
          if      (last_escape == 1)
            CDocScriptWriteCommand("\\fP");
          else if (last_escape == 3)
            CDocScriptWriteCommand("\\l\'|0\\(ul\'");
          else if (last_escape == 4)
            CDocScriptWriteCommand("\\l\'|0\\(ul\'\\fP");

          last_escape = -1;
        }
        else if (text[i + 2] == '1') {
          if (i > 0)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          last_escape = 1;
        }
        else if (text[i + 2] == '3') {
          if (i > 0)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("");

          last_escape = 3;
        }
        else if (text[i + 2] == '4') {
          if (i > 0)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          last_escape = 4;
        }
        else {
          if (i > 0)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          last_escape = 1;
        }

        /* Skip Escape Code */

        i += 4;

        /* Skip Space after Escape Code */

        while (text[i] == ' ')
          i++;

        last_char = '\0';
      }

      /* CDoc and Raw with Control Codes :
          Add Escape Code to Current Word */

      else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
               CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
        for (; no > 0; no--)
          word[j++] = text[i++];
      }

      /* Other formats : Ignore Escape Code */

      else {
        i += no;
      }
    }

    /* If we have a Page Reference then add to Current Word */

    else if ((no = CDocIsPageReference(&text[i])) != 0) {
      for (; no > 0; no--)
        word[j++] = text[i++];
    }

    /* If we have an inline Newline character then we want
       to break the paragraph and start a new line */

    else if (text[i] == '\n') {
      int  skip_lines;

      i++;

      /* Get Extra Newline Characters */

      skip_lines = 0;

      while (text[i] == '\n') {
        skip_lines++;

        i++;
      }

      /* Indicate if we are outputting the last line of the paragraph */

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
          cdoc_outputting_paragraph) {
        if (text[i] == '\0')
          cdoc_paragraph_last_line = true;
      }

      /* Terminate Current Word */

      word[j] = '\0';

      j = 0;

      /* Set Display Width */

      width = cdoc_right_margin - cdoc_left_margin -
              cdoc_indent - indent1;

      /* If Word does not fit on Current Line then output
         it on a new one, otherwise output the Word */

      if ((last_char != '\0' && pos + CDocStringDisplayLength(word) + 1 > width) ||
          (last_char == '\0' && pos + CDocStringDisplayLength(word)     > width)) {
        CDocScriptNewLine();

        if (CDocInst->getOutputFormat() != CDOC_OUTPUT_TROFF)
          CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

        CDocScriptWriteText("%s", word);
      }
      else {
        if (last_char != '\0')
          CDocScriptWriteText("%c", last_char);

        CDocScriptWriteText("%s", word);
      }

      /* Start a New Line */

      CDocScriptNewLine();

      /* Skip remaining new lines */

      while (skip_lines-- > 0)
        CDocScriptSkipLine();

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
          cdoc_outputting_paragraph) {
        if (text[i] != '\0') {
          CDocScriptWriteCommand(CDOC_PARAGRAPH_END_TMPL);

          CDocScriptWriteCommand(CDOC_PARAGRAPH_BEGIN_TMPL,
                                 cdoc_right_margin);
        }
      }

      /* Prepare to Write Text on New line */

      if (CDocInst->getOutputFormat() != CDOC_OUTPUT_TROFF) {
        if (text[i] != '\0')
          CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);
      }

      pos = 0;

      indent1 = 0;

      last_char = '\0';
    }

    /* If we have a character at which we can split the line
       (punctuation or space) then check against current
       word whether the line should be continued or a new
       line should be started */

    else if (CDocIsSplitter(&text[i])) {
      /* If not space then add to word (space is collapsed) */

      if (! isspace(text[i]))
        word[j++] = text[i];

      /* Terminate Word */

      word[j] = '\0';

      j = 0;

      /* Set Display Width */

      width = cdoc_right_margin - cdoc_left_margin -
              cdoc_indent - indent1;

      /* If word is an empty string then just output
         last space character and continue */

      if      (word[0] == '\0') {
        if (last_char != '\0') {
          CDocScriptWriteText("%c", last_char);

          pos++;
        }
      }

      /* If Word does not fit on current line then Output
         it on a new one, otherwise output previous space
         character and word */

      else if ((last_char != '\0' && pos + CDocStringDisplayLength(word) + 1 > width) ||
               (last_char == '\0' && pos + CDocStringDisplayLength(word)     > width)) {
        CDocScriptNewLine();

        if (CDocInst->getOutputFormat() != CDOC_OUTPUT_TROFF)
          CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

        CDocScriptWriteText("%s", word);

        pos = CDocStringDisplayLength(word);

        indent1 = 0;
      }
      else {
        if (last_char != '\0') {
          CDocScriptWriteText("%c", last_char);

          pos++;
        }

        CDocScriptWriteText("%s", word);

        pos += CDocStringDisplayLength(word);
      }

      /* Set Display Width */

      width = cdoc_right_margin - cdoc_left_margin -
              cdoc_indent - indent1;

      /* Save last Space Character */

      if (isspace(text[i]) && pos > 0 && pos < width)
        last_char = text[i];
      else
        last_char = '\0';

      /* Skip Splitter */

      i++;
    }

    /* Any other character is just added to the Current Word */

    else
      word[j++] = text[i++];
  }

  /* Indicate we are outputting the last line of the paragraph */

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      cdoc_outputting_paragraph)
    cdoc_paragraph_last_line = true;

  /* Terminate Current Word */

  word[j] = '\0';

  /* Set Display Width */

  width = cdoc_right_margin - cdoc_left_margin - cdoc_indent - indent1;

  /* If Word does not fit on current line then output it
     on a new line, otherwise output previous space and Word */

  if ((last_char != '\0' && pos + CDocStringDisplayLength(word) + 1 > width) ||
      (last_char == '\0' && pos + CDocStringDisplayLength(word)     > width)) {
    CDocScriptNewLine();

    if (CDocInst->getOutputFormat() != CDOC_OUTPUT_TROFF)
      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteText("%s", word);
  }
  else {
    if (last_char != '\0')
      CDocScriptWriteText("%c", last_char);

    CDocScriptWriteText("%s", word);
  }

  /* If we have a List then output a blank line between the
     Paragraphs if not compact, otherwise leave no blank line.
     If not a List then output a blank line between the
     Paragraphs */

  if (cdoc_paragraph->getType() == LIST_PARAGRAPH       ||
      cdoc_paragraph->getType() == DEFINITION_PARAGRAPH ||
      cdoc_paragraph->getType() == SUB_LIST_PARAGRAPH1  ||
      cdoc_paragraph->getType() == SUB_LIST_PARAGRAPH2  ||
      cdoc_paragraph->getType() == GLOSSARY_PARAGRAPH) {
    if (CDocScriptGetCurrentListCompact()) {
      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
        cdoc_line_no--;

        CDocScriptWriteText("\n");
        CDocScriptWriteCommand(CDOC_SET_LINE_TMPL, -1);

        cdoc_line_no++;
      }
    }
    else
      CDocScriptSkipLine();
  }
  else
    CDocScriptSkipLine();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      cdoc_outputting_paragraph) {
    cdoc_paragraph_last_line = false;

    CDocScriptWriteCommand(CDOC_PARAGRAPH_END_TMPL);

    cdoc_outputting_paragraph = false;
  }

  /* Save the List's Left Margin (for actual text) for the
     Definition and List Paragraphs */

  if      (cdoc_paragraph->getType() == DEFINITION_PARAGRAPH)
    CDocScriptSetCurrentListLeftMargin(cdoc_left_margin);
  else if (cdoc_paragraph->getType() == LIST_PARAGRAPH)
    CDocScriptSetCurrentListLeftMargin(cdoc_left_margin);

  /* Restore Left Margin */

  cdoc_left_margin = save_left_margin;

  cdoc_in_paragraph   = false;
  cdoc_paragraph_done = true;

  return;

}

// Output the current paragraph text, and associated strings, in an
// unformatted manner i.e. as is.
static void
CDocScriptOutputUnformattedParagraph()
{
  int save_left_margin = cdoc_left_margin;

  if (cdoc_paragraph->getType() == SUB_LIST_PARAGRAPH1) {
    int left_margin = CDocScriptGetCurrentListLeftMargin();

    if (left_margin != -1)
      cdoc_left_margin = left_margin;
  }

  int length = 0;

  /* If Paragraph has a Prefix then indent by the prefix length
     plus the normal amount, otherwise just indent by the
     normal amount */

  if (cdoc_paragraph->getPrefix() != "") {
    length = CDocStringDisplayLength(cdoc_paragraph->getPrefix());

    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent + length);
  }
  else
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

  int i;
  int no;

  /* If we are outputting in Troff format replace Escape Codes
     with inline Troff Font Style Commands */

  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF) {
    int        escape_codes;
    static int last_escape = -1;

    /* Initialise Flag with Indicates whether we are
       currently at the start of a new line */

    int new_line = true;

    const char *p = cdoc_paragraph->getText().c_str();

    /* Process each line ending in a Newline Character */

    const char *p1;

    while ((p1 = strchr(p, '\n')) != NULL) {
      /* Blank out Newline Character */

      std::string line(p, p1 - p);

      /* Process Line */

      length = line.size();

      /* Set flag to indicate whether this line contains Escape Codes */

      escape_codes = false;

      for (i = 0; i < length; i++)
        if ((no = CDocIsEscapeCode(&line[i])) != 0) {
          escape_codes = true;

          break;
        }

      /* If we have Escape Codes then switch formatting
         back on (just for this line) so that the Troff
         Font Style commands (which may cause a new line)
         do not cause line to be broken up */

      if (escape_codes)
        CDocScriptWriteCommand(".fi\n");

      for (i = 0; i < length; i++) {
        /* If we have an Escape Code then replace it with
           inline Troff Font Style Commands */

        if ((no = CDocIsEscapeCode(&line[i])) != 0) {
          /* Output inline Troff Command dependant on
             current escape code and previous one */

          if      (line[i + 2] == '0') {
            if      (last_escape == 1)
              CDocScriptWriteCommand("\\fP");
            else if (last_escape == 3)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'");
            else if (last_escape == 4)
              CDocScriptWriteCommand("\\l\'|0\\(ul\'\\fP");

            new_line = false;

            last_escape = -1;
          }
          else if (line[i + 2] == '1') {
            if (! new_line)
              CDocScriptFPutc('\n');

            CDocScriptWriteCommand("\\fB");

            new_line = false;

            last_escape = 1;
          }
          else if (line[i + 2] == '3') {
            if (! new_line)
              CDocScriptFPutc('\n');

            new_line = true;

            last_escape = 3;
          }
          else if (line[i + 2] == '4') {
            if (! new_line)
              CDocScriptFPutc('\n');

            CDocScriptWriteCommand("\\fB");

            new_line = false;

            last_escape = 4;
          }
          else {
            if (! new_line)
              CDocScriptFPutc('\n');

            CDocScriptWriteCommand("\\fB");

            new_line = false;

            last_escape = 1;
          }

          /* Skip Escape Code */

          i += 3;
        }

        /* For any other characters check for leading dot or
           single quote characters (which are considered special
           by Troff) and Escape them with a preceding '\&', or an
           inline '\' character (which is considered special by
           Troff) and Escape it with another '\' character. */

        else {
          /* Check for leading dot or single quote characters */

          if (i == 0) {
            if (line[i] == '.' || line[i] == '\'') {
              CDocScriptFPutc('\\');
              CDocScriptFPutc('&');
            }
          }

          /* Check for inline '\' character */

          else {
            if (line[i] == '\\')
              CDocScriptFPutc('\\');
          }

          CDocScriptFPutc(line[i]);

          new_line = false;
        }
      }

      /* If we are not already on a new line then output a newline character */

      if (! new_line)
        CDocScriptFPutc('\n');

      p = p1 + 1;

      /* If we have inline Escape Codes then switch Formatting off again to prevent
         the next line being attached to this one */

      if (escape_codes)
        CDocScriptWriteCommand(".nf\n");
    }

    /* Initialise Flag with Indicates whether we are
       currently at the start of a new line */

    new_line = true;

    length = strlen(p);

    /* Set flag to indicate whether this line contains Escape Codes */

    escape_codes = false;

    for (i = 0; i < length; i++)
      if ((no = CDocIsEscapeCode(&p[i])) != 0) {
        escape_codes = true;

        break;
      }

    /* If we have Escape Codes then switch formatting back on (just
       for this line) so that the Troff Font Style commands (which
       may cause a new line) do not cause line to be broken up */

    if (escape_codes)
      CDocScriptWriteCommand(".fi\n");

    for (i = 0; i < length; i++) {
      /* If we have an Escape Code then replace it with
         inline Troff Font Style Commands */

      if ((no = CDocIsEscapeCode(&p[i])) != 0) {
        /* Output inline Troff Command dependant on
           current escape code and previous one */

        if      (p[i + 2] == '0') {
          if      (last_escape == 1)
            CDocScriptWriteCommand("\\fP");
          else if (last_escape == 3)
            CDocScriptWriteCommand("\\l\'|0\\(ul\'");
          else if (last_escape == 4)
            CDocScriptWriteCommand("\\l\'|0\\(ul\'\\fP");

          new_line = false;

          last_escape = -1;
        }
        else if (p[i + 2] == '1') {
          if (! new_line)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          new_line = false;

          last_escape = 1;
        }
        else if (p[i + 2] == '3') {
          if (! new_line)
            CDocScriptFPutc('\n');

          new_line = true;

          last_escape = 3;
        }
        else if (p[i + 2] == '4') {
          if (! new_line)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          new_line = false;

          last_escape = 4;
        }
        else {
          if (! new_line)
            CDocScriptFPutc('\n');

          CDocScriptWriteCommand("\\fB");

          new_line = false;

          last_escape = 1;
        }

        /* Skip Escape Code */

        i += 3;
      }

      /* For any other characters check for leading dot or
         single quote characters (which are considered special
         by Troff) and Escape them with a preceding '\&', or an
         inline '\' character (which is considered special by
         Troff) and Escape it with another '\' character. */

      else {
        /* Check for leading dot or single quote characters */

        if (i == 0) {
          if (p[i] == '.' || p[i] == '\'') {
            CDocScriptFPutc('\\');
            CDocScriptFPutc('&');
          }
        }

        /* Check for inline '\' character */

        else {
          if (p[i] == '\\')
            CDocScriptFPutc('\\');
        }

        CDocScriptFPutc(p[i]);

        new_line = false;
      }
    }

    /* If we are not already on a new line then output
       a newline character */

    if (! new_line)
      CDocScriptFPutc('\n');

    /* If we have inline Escape Codes then switch
       Formatting off again to prevent the next line
       being attached to this one */

    if (escape_codes)
      CDocScriptWriteCommand(".nf\n");
  }

  /* For HTML format replace any special characters */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    const char *p = cdoc_paragraph->getText().c_str();

    /* Process each line ending in a Newline Character */

    const char *p1;

    while ((p1 = strchr(p, '\n')) != NULL) {
      /* Blank out Newline Character */

      std::string line(p, p1 - p);

      /* Output Line */

      CDocScriptFPuts(CDocEncodeHtmlString(line.c_str()));
      CDocScriptFPutc('\n');

      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      /* Next Line */

      p = p1 + 1;
    }

    /* Output Line */

    CDocScriptFPuts(CDocEncodeHtmlString(p));
    CDocScriptFPutc('\n');
  }

  /* For Interleaf format replace any special characters */

  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    const char *p = cdoc_paragraph->getText().c_str();

    /* Process each line ending in a Newline Character */

    const char *p1;

    while ((p1 = strchr(p, '\n')) != NULL) {
      /* Blank out Newline Character */

      std::string line(p, p1 - p);

      /* Output Line */

      CDocScriptFPuts(CDocEncodeILeafString(line.c_str()));
      CDocScriptFPuts("<HR>");
      CDocScriptFPutc('\n');

      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      /* Next Line */

      p = p1 + 1;
    }

    /* Output Line */

    CDocScriptFPuts(CDocEncodeILeafString(p));
    CDocScriptFPuts("<HR>");
    CDocScriptFPutc('\n');
  }

  /* For any other format just Output the Lines (Escape
     Codes should not be present for Raw Output Format) */

  else {
    const char *p = cdoc_paragraph->getText().c_str();

    /* Process each line ending in a Newline Character */

    const char *p1;

    while ((p1 = strchr(p, '\n')) != NULL) {
      /* Blank out Newline Character */

      std::string line(p, p1 - p);

      /* Output Line */

      CDocScriptFPuts(line.c_str());
      CDocScriptFPutc('\n');

      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      /* Next Line */

      p = p1 + 1;
    }

    /* Output Line */

    CDocScriptFPuts(p);
    CDocScriptFPutc('\n');
  }

  cdoc_left_margin = save_left_margin;
}

//--------------

CDParagraph::
CDParagraph()
{
  type_           = NORMAL_PARAGRAPH;
  formatted_      = true;
  justification_  = LEFT_JUSTIFICATION;
  prefix_font_    = CDOC_NORMAL_FONT;
  indent_prefix_  = true;
  font_           = CDOC_NORMAL_FONT;
}

CDParagraph::
~CDParagraph()
{
}
