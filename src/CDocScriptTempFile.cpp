#include "CDocI.h"

typedef std::vector<char *> KeepList;

static char     *keep_line = NULL;
static KeepList  keep_list;

// Start redirect of all output to a temporary file which will be post-processed
// before being added to the output file.
extern CDScriptTempFile *
CDocScriptStartTempFile(const std::string &command)
{
  CDScriptTempFile *temp_file = new CDScriptTempFile;

  temp_file->filename            = CDocInst->getTempFileName();
  temp_file->fp                  = fopen(temp_file->filename.c_str(), "w");
  temp_file->no_lines            = 0;
  temp_file->save_output_fp      = cdoc_output_fp;
  temp_file->save_page_no        = cdoc_page_no;
  temp_file->save_line_no        = cdoc_line_no;
  temp_file->save_char_no        = cdoc_char_no;
  temp_file->save_left_margin    = cdoc_left_margin;
  temp_file->save_right_margin   = cdoc_right_margin;
  temp_file->save_indent         = cdoc_indent;
  temp_file->save_in_paragraph   = cdoc_in_paragraph;
  temp_file->save_paragraph_done = cdoc_paragraph_done;

  /*-----------*/

  if (temp_file->fp != NULL)
    cdoc_output_fp = temp_file->fp;
  else
    CDocScriptError("Cannot open Temporary File '%s' for %s",
                    temp_file->filename.c_str(), command.c_str());

  cdoc_page_no         = -1;
  cdoc_line_no         = 0;
  cdoc_char_no         = 0;
  cdoc_indent          = 0;
  cdoc_in_paragraph    = false;
  cdoc_paragraph_done  = false;
  cdoc_page_no_offset += temp_file->save_page_no;

  return temp_file;
}

// End redirect of all output to a temporary file which will be post-processed
// before being added to the output file.
extern void
CDocScriptEndTempFile(CDScriptTempFile *temp_file)
{
  if (cdoc_in_paragraph)
    CDocScriptOutputParagraph();

  temp_file->no_lines = cdoc_line_no;

  /*-------------*/

  cdoc_output_fp       = temp_file->save_output_fp;
  cdoc_page_no         = temp_file->save_page_no;
  cdoc_line_no         = temp_file->save_line_no;
  cdoc_char_no         = temp_file->save_char_no;
  cdoc_left_margin     = temp_file->save_left_margin;
  cdoc_right_margin    = temp_file->save_right_margin;
  cdoc_indent          = temp_file->save_indent;
  cdoc_in_paragraph    = temp_file->save_in_paragraph;
  cdoc_paragraph_done  = temp_file->save_paragraph_done;
  cdoc_page_no_offset -= temp_file->save_page_no;

  if (temp_file->fp != NULL)
    fclose(temp_file->fp);

  temp_file->fp = NULL;
}

// Output lines stored in a temporary file (used by examples, figures, tables
// etc.) and ensure line number and page number are correct.
extern void
CDocScriptOutputTempFile(CDScriptTempFile *temp_file)
{
  if (temp_file->fp != NULL)
    fclose(temp_file->fp);

  temp_file->fp = fopen(temp_file->filename.c_str(), "r");

  if (temp_file->fp == NULL)
    return;

  /*-------------*/

  CDocScriptOutputTempFileInit();

  char line[CDOC_MAX_LINE];

  while (fgets(line, CDOC_MAX_LINE, temp_file->fp) != NULL) {
    char *p = strchr(line, '\n');

    if (p != NULL)
      *p = '\0';

    CDocScriptOutputTempFileLine(line);
  }

  CDocScriptOutputTempFileTerm();

  /*-------------*/

  fclose(temp_file->fp);

  temp_file->fp = NULL;
}

// Initialse Environment ready to output lines to a Temporary File where formatted
// text is buffered and outputted at a later point.
extern void
CDocScriptOutputTempFileInit()
{
  keep_line = NULL;

  keep_list.clear();
}

// Output a line of text to a temporary file.
extern void
CDocScriptOutputTempFileLine(const char *line)
{
  int i;

  int lines_per_page = CDocScriptGetLinesPerPage();

  /* If new page character start new page */

  if      (line[0] == NEW_PAGE)
    CDocScriptNewPage();

  /* If Command Line then Output (don't count as text line) */

  else if (line[0] == CMD_MARK_C) {
    if (cdoc_page_no != -1 && ! cdoc_page_header_output)
      CDocScriptWritePageHeader();

/*
    if (line[1] == '\0') {
      cdoc_line_no++;

      CDocScriptOutputFootnotes(false);
    }
*/

    if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC && cdoc_page_no != -1) {
      if      (CDOC_IS_CMD(&line[1], CDOC_COMMAND_CMD)) {
        cdoc_line_no += 3;

        cdoc_line_fiddle += 2;

        CDocScriptOutputFootnotes(false);
      }
      else if (CDOC_IS_CMD(&line[1], CDOC_PARAGRAPH_BEGIN_CMD) ||
               CDOC_IS_CMD(&line[1], CDOC_PARAGRAPH_START_CMD))
        cdoc_outputting_paragraph = true;
      else if (CDOC_IS_CMD(&line[1], CDOC_PARAGRAPH_END_CMD ) ||
               CDOC_IS_CMD(&line[1], CDOC_PARAGRAPH_STOP_CMD))
        cdoc_outputting_paragraph = false;
      else if (CDOC_IS_CMD(&line[1], CDOC_SET_LINE_CMD)) {
        const char *p = &line[strlen(CDOC_SET_LINE_CMD) + 1];

        CStrUtil::skipSpace(&p);

        int no_lines = 0;

        if (*p != '\0')
          no_lines = CStrUtil::toInteger(p);

        if (no_lines == 0)
          return;

        cdoc_line_no += no_lines;
      }
      else if (CDOC_IS_CMD(&line[1], CDOC_BITMAP_CMD) ||
               CDOC_IS_CMD(&line[1], CDOC_XWD_CMD   ) ||
               CDOC_IS_CMD(&line[1], CDOC_XWD_Z_CMD ) ||
               CDOC_IS_CMD(&line[1], CDOC_IMAGE_CMD )) {
        const char *p;

        if      (CDOC_IS_CMD(&line[1], CDOC_BITMAP_CMD))
          p = &line[strlen(CDOC_BITMAP_CMD) + 1];
        else if (CDOC_IS_CMD(&line[1], CDOC_XWD_CMD))
          p = &line[strlen(CDOC_XWD_CMD   ) + 1];
        else if (CDOC_IS_CMD(&line[1], CDOC_XWD_Z_CMD))
          p = &line[strlen(CDOC_XWD_Z_CMD ) + 1];
        else
          p = &line[strlen(CDOC_IMAGE_CMD ) + 1];

        CStrUtil::skipSpace(&p);

        CStrUtil::skipNonSpace(&p);

        CStrUtil::skipSpace(&p);

        int no_lines = 0;

        if (*p != '\0') {
          std::vector<std::string> words;

          CStrUtil::addWords(p, words);

          if (words.size() > 0)
            no_lines = CStrUtil::toInteger(words[0]);
        }

        if (no_lines == 0)
          return;

        CDocScriptFindSpace(no_lines);

        cdoc_line_no += no_lines;

        cdoc_line_fiddle += no_lines - 1;

        CDocScriptOutputFootnotes(false);
      }
      else if (CDOC_IS_CMD(&line[1], CDOC_START_FONT_CMD)) {
        const char *p = &line[strlen(CDOC_START_FONT_CMD) + 1];

        CStrUtil::skipSpace(&p);

        CDocScriptBeginFont(p);

        return;
      }
      else if (CDOC_IS_CMD(&line[1], CDOC_END_FONT_CMD)) {
        CDocScriptPreviousFont();

        return;
      }
      else if (CDOC_IS_CMD(&line[1], CDOC_INDENT_CMD)) {
        const char *p = &line[strlen(CDOC_INDENT_CMD) + 1];

        CStrUtil::skipSpace(&p);

        cdoc_last_indent      = CStrUtil::toInteger(p);
        cdoc_last_indent_page = cdoc_page_no;
      }
    }

    if (cdoc_page_no == -1)
      fputc(CMD_MARK_C, cdoc_output_fp);

    fputs(&line[1], cdoc_output_fp);
    fputc('\n', cdoc_output_fp);
  }

  /* Update/Output Keep Line List */

  else if (line[0] == KEEP_MARK_C) {
    if (! keep_list.empty()) {
      char  *line;
      int    no_keep;
      int    new_page;

      new_page = false;

      no_keep = keep_list.size();

      if (keep_line != NULL) {
        if (no_keep                    <= lines_per_page &&
            cdoc_line_no + no_keep + 1 >= lines_per_page)
          new_page = true;
      }
      else {
        if (no_keep                <= lines_per_page &&
            cdoc_line_no + no_keep >= lines_per_page)
          new_page = true;
      }

      if (new_page) {
        if (keep_line != NULL) {
          for (uint j = 0; j < strlen(keep_line); j++) {
            if      (keep_line[j] == BOX_LPR_C)
              keep_line[j] = BOX_BL__C;
            else if (keep_line[j] == BOX_TPR_C)
              keep_line[j] = BOX_HOR_C;
            else if (keep_line[j] == BOX_MID_C)
              keep_line[j] = BOX_BPR_C;
            else if (keep_line[j] == BOX_RPR_C)
              keep_line[j] = BOX_BR__C;
          }

          CDocScriptFPuts(keep_line);
          CDocScriptFPutc('\n');

          delete [] keep_line;
        }

        CDocScriptNewPage();
      }
      else {
        if (keep_line != NULL) {
          CDocScriptFPuts(keep_line);
          CDocScriptFPutc('\n');

          delete [] keep_line;

          cdoc_paragraph_done = true;
        }
      }

      for (i = 1; i <= no_keep - 1; i++) {
        line = keep_list[i - 1];

        CDocScriptFPuts(line);
        CDocScriptFPutc('\n');

        delete [] line;

        cdoc_paragraph_done = true;
      }

      keep_line = keep_list.back();

      keep_list.clear();
    }
  }

  /* Otherwise just output line as normal */

  else {
    if (keep_list.empty()) {
      if (line[0] != '\n' || cdoc_line_no != 0) {
        CDocScriptFPuts(line);
        CDocScriptFPutc('\n');

        cdoc_paragraph_done = true;
      }
    }
    else
      keep_list.push_back(CStrUtil::strdup(line));
  }
}

// Terminate Environment where formatted text is buffered in a Temporary File
// for output at a later point.
extern void
CDocScriptOutputTempFileTerm()
{
  if (keep_line != NULL) {
    CDocScriptFPuts(keep_line);
    CDocScriptFPutc('\n');

    delete [] keep_line;

    cdoc_paragraph_done = true;

    keep_line = NULL;
  }

  char *line;

  int no = keep_list.size();

  for (int i = 1; i <= no; i++) {
    line = keep_list[i];

    CDocScriptFPuts(line);
    CDocScriptFPutc('\n');

    delete [] line;

    cdoc_paragraph_done = true;
  }

  keep_list.clear();
}

CDScriptTempFile::
~CDScriptTempFile()
{
  remove(filename.c_str());
}
