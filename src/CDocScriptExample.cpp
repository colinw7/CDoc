#include "CDocI.h"

// Creates and Initialises an Example Structure.
extern CDExample *
CDocScriptCreateExample()
{
  CDExample *example = new CDExample;

  example->depth     = 0;
  example->temp_file = NULL;

  return example;
}

// Begin Example construction.
//
// Output is redirected to a temporary file before being inserted in the
// output so that it can be padded out to a specified depth if required.
extern void
CDocScriptExampleBegin(CDExample *example)
{
  /* Start Outputting Text to Temporary File */

  example->temp_file = CDocScriptStartTempFile("Example");

  /* Set Example Indent */

  cdoc_indent += 2;

  /* Turn off Formatting and Start Example Font */

  CDocScriptFormattingOff();

  CDocScriptBeginFont("xmpfont");
}

// End Example construction.
//
// The output in the temporary file is output onto the output file with
// extra blank lines to pad to a specified depth if required.
extern void
CDocScriptExampleEnd(CDExample *example)
{
  int no_lines;
  int new_page;

  /* Turn Formatting back on Switch back to Previous Font */

  CDocScriptFormattingOn();

  CDocScriptPreviousFont();

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(example->temp_file);

  /* Calculate the Number of Lines Output */

  no_lines = example->temp_file->no_lines;

  /* If outputting for CDoc or RAW and example does not fit on
     current page then start a new page (if example less than
     a third of the page height) */

  if (CDocIsPagedOutput()) {
    int  lines_per_page;

    lines_per_page = CDocScriptGetLinesPerPage();

    if (no_lines < lines_per_page/3) {
      if (cdoc_paragraph_done)
        new_page = CDocScriptFindSpace(no_lines + 1);
      else
        new_page = CDocScriptFindSpace(no_lines);
    }
    else
      new_page = false;
  }
  else
    new_page = false;

  /* Skip Line if Compact (as last line will not have new line character) */

  if (CDocScriptGetCurrentListCompact())
    CDocScriptSkipLine();

  if (! new_page && cdoc_paragraph_done)
    CDocScriptNewLine();

  /* Pad to Required Depth */

  while (example->depth > no_lines) {
    no_lines++;

    CDocScriptNewLine();
  }

  /* Output Text in Temporary File */

  CDocScriptOutputTempFile(example->temp_file);

  /* Clean Up */

  delete example->temp_file;
  delete example;
}
