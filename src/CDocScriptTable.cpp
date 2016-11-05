#include "CDocI.h"

/*---------------------------------------------------------------------*/

struct CDTableGrid {
  int     cell_no;
  int     row;
  int     column;
  int     rows;
  int     columns;
  int     width;
  char  **lines;
  int     no_lines;

 ~CDTableGrid() {
    for (int i = 0; i < no_lines; ++i)
      delete [] lines[i];

    delete [] lines;
  }
};

/*---------------------------------------------------------------------*/

struct CDocScriptTableGridSortProc {
  CDTableGrid *grid1;

  CDocScriptTableGridSortProc(CDTableGrid *grid) :
    grid1(grid) {
  }

  bool operator()(CDTableGrid *grid2) {
    if (grid1->column > grid2->column ||
        (grid1->column == grid2->column && grid1->row > grid2->row))
      return true;
    else
      return false;
  }
};

/*---------------------------------------------------------------------*/

typedef std::vector<CDTable *>       CDTableList;
typedef std::vector<CDTableRowDef *> CDTableRowDefList;

static CDTableList        table_list;
static int                table_number        = 0;
static CDTableRowDefList  row_definition_list;
static std::string        table_last_splitter;
static std::string        table_last_line;
static int                header_proc_skip    = false;
static int                row_output          = false;

/*---------------------------------------------------------------------*/

static void         CDocScriptOutputHeaderRow
                     (CDTable *);
static void         CDocScriptOutputTableRow
                     (CDTableRow *, int);
static void         CDocScriptTableSetLastLine
                     (const std::string &);
static void         CDocScriptTableSetLastSplitter
                     (const std::string &);
static CDTableCell *CDocScriptGetTableCell
                     (int, CDTableRow *);

// Output the List of Tables defined in the document.
extern void
CDocScriptOutputTableList()
{
  /* Output Table List as a Level 1 Header */

  if (cdoc_document.type == CDOC_STAIRS_DOCUMENT) {
    CDocScriptStartSection("List of Tables");

    CDocScriptStartHeader(1);

    char *temp_string = new char [128];

    sprintf(temp_string, "%d.0  List of Tables", CDocScriptGetHeader(1));

    CDocScriptWriteLeftJustifiedHeader(temp_string);

    delete [] temp_string;
  }
  else
    CDocScriptWriteCentreJustifiedPageHeader("List of Tables");

  CDocScriptSetTOCPage();

  CDocScriptSkipLine();

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("<ul>\n");

  int no_tables = table_list.size();

  for (int i = 1; i <= no_tables; i++) {
    /* Output Table Line */

    CDTable *table = table_list[i - 1];

    if (table->caption == "" && table->description == "")
      continue;

    char *temp_string;

    if (table->caption != "") {
      temp_string = new char [table->caption.size() + 128];

      sprintf(temp_string, "Table %d.  %s", table->count, table->caption.c_str());
    }
    else {
      temp_string = new char [128];

      sprintf(temp_string, "Table %d.", table->count);
    }

    if      (cdoc_page_numbering && CDocIsPagedOutput()) {
      CDocScriptWriteIndent(8);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
        CDocScriptWriteText("%s##cdoc_tbl_pg%d##B", ESCG, i);

      CDocScriptWriteText("%s", temp_string);

      int j = CDocStringDisplayLength(temp_string) + 8;

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

      CDocScriptWriteText("##cdoc_tbl_pg%d##", i);

      if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
        CDocScriptWriteText("%sE", ESCG);

      CDocScriptWriteText("\n");
    }
    else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
      CDocScriptWriteCommand("<li>%s</li>\n", CDocEncodeHtmlString(temp_string));
    }
    else {
      CDocScriptWriteIndent(8);

      CDocScriptWriteText("%s\n", CDocEncodeILeafString(temp_string));
    }

    delete [] temp_string;
  }

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    CDocScriptWriteCommand("</ul>\n");

  CDocScriptSkipLine();
}

// Set a Table's Page Number to the current Page Number.
extern void
CDocScriptSetTablePage(CDTable *table)
{
  if (cdoc_page_no != -1)
    table->page_no = cdoc_page_no;
  else
    table->page_no = cdoc_page_no_offset;
}

// Get the Page Number String associated with the Table whose numeric
// position is supplied.
extern std::string
CDocScriptGetTablePage(int no)
{
  std::string page_string;

  CDTable *table = CDocScriptGetTable(no);

  if (table != NULL)
    page_string = CStrUtil::toString(table->page_no);
  else
    page_string = "??";

  return page_string;
}

// Output the table defined by the contents of the supplied Table
// Structure.
extern void
CDocScriptOutputTable(CDTable *table)
{
  /* Start Sub-Section for Table */

  if (table->count != -1) {
    char *temp_string;

    if (table->caption != "") {
      temp_string = new char [table->caption.size() + 128];

      sprintf(temp_string, "  Table %d.  %s", table->count, table->caption.c_str());
    }
    else {
      temp_string = new char [128];

      sprintf(temp_string, "  Table %d.", table->count);
    }

    CDocScriptStartSubSection(temp_string);

    delete [] temp_string;
  }

  /* Set Number of Rows */

  int no_rows = table->row_list.size();

  /* Calculate Table Width */

  int min_table_width = 0;
  int max_table_width = 0;

  for (int i = 0; i <= no_rows + 1; i++) {
    CDTableRow *row;

    if      (i == 0)
      row = table->header_row;
    else if (i == no_rows + 1)
      row = table->footer_row;
    else
      row = table->row_list[i - 1];

    if (row == NULL)
      continue;

    if (row->row_definition->width > min_table_width)
      min_table_width = row->row_definition->width;

    if (row->row_definition->free_cells1 == 0) {
      if (row->row_definition->width > max_table_width)
        max_table_width = row->row_definition->width;
    }
  }

  if (max_table_width == 0) {
    if (min_table_width < (cdoc_right_margin - cdoc_left_margin + 1))
      table->full_width = cdoc_right_margin - cdoc_left_margin + 1;
    else
      table->full_width = min_table_width;
  }
  else
    table->full_width = max_table_width;

  /* Set Column Widths for each Row */

  for (int i = 0; i <= no_rows + 1; i++) {
    CDTableRow  *row;

    if      (i == 0)
      row = table->header_row;
    else if (i == no_rows + 1)
      row = table->footer_row;
    else
      row = table->row_list[i - 1];

    if (row == NULL)
      continue;

    delete [] row->cwidths;

    row->no_cwidths = row->row_definition->no_cwidths;
    row->cwidths    = new int [row->no_cwidths];

    for (int j = 0; j < row->no_cwidths; j++)
      row->cwidths[j] = row->row_definition->cwidths[j];

    if (row->row_definition->free_cells1 > 0) {
      int space = table->full_width - row->row_definition->width;

      int space1 = space/row->row_definition->free_cells2;
      int space2 = space - (space1*row->row_definition->free_cells2);

      for (int j = 0; j < row->no_cwidths; j++) {
        if (row->cwidths[j] < 0) {
          row->cwidths[j] = (-row->cwidths[j])*space1 + 1;

          if (space2 > 0) {
            row->cwidths[j]++;

            space2--;
          }
        }
      }
    }
  }

  /* Start Outputting Text to Temporary File */

  table->temp_file = CDocScriptStartTempFile("Table");

  /* Start Outputting Table to Temporary File */

  if (CDocIsPagedOutput()) {
    fputc(KEEP_MARK_C, cdoc_output_fp);
    fputc('\n'       , cdoc_output_fp);
  }

  row_output = false;

  if (table->header_row != NULL)
    CDocScriptOutputTableRow(table->header_row, table->full_width);

  /* Get first Table Note (if any) */

  CDTable::NoteList::iterator pn1 = table->note_list.begin();
  CDTable::NoteList::iterator pn2 = table->note_list.end  ();

  CDTableNote *note = NULL;

  if (pn1 != pn2)
    note = *pn1;

  /* Format Cell Text for Each Row */

  for (int i = 1; i <= no_rows; i++) {
    if (note != NULL && note->row_no < i) {
      char **lines;
      int    no_lines;

      char *str = new char [table->full_width + 16];

      if (! row_output)
        str[0] = BOX_TL__C;
      else
        str[0] = BOX_LPR_C;

      int j;

      for (j = 1; j < table->full_width - 1; j++) {
        if (j < int(table_last_line.size()) && table_last_line[j] == BOX_VRT_C)
          str[j] = BOX_BPR_C;
        else
          str[j] = BOX_HOR_C;
      }

      if (! row_output)
        str[j++] = BOX_TR__C;
      else
        str[j++] = BOX_RPR_C;

      str[j] = '\0';

      CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

      CDocScriptWriteText("%s", str);

      CDocScriptTableSetLastLine    (str);
      CDocScriptTableSetLastSplitter(str);

      CDocScriptWriteText("\n");

      row_output = true;

      if (CDocIsPagedOutput()) {
        fputc(KEEP_MARK_C, cdoc_output_fp);
        fputc('\n'       , cdoc_output_fp);
        fputc(KEEP_MARK_C, cdoc_output_fp);
        fputc('\n'       , cdoc_output_fp);
      }

      std::string text;

      CStrUtil::sprintf(text, "%sNote:  %s%s", CDocStartBold(), CDocEndBold(), note->text.c_str());

      CDocFormatStringInWidth(text, table->full_width - 2, CHALIGN_TYPE_LEFT, &lines, &no_lines);

      for (int j = 0; j < no_lines; j++) {
        strcpy(str, BOX_VRT_S);
        strcat(str, lines[j]);
        strcat(str, BOX_VRT_S);

        CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

        CDocScriptWriteText("%s", str);

        if (j == no_lines - 1)
          CDocScriptTableSetLastLine(str);

        CDocScriptWriteText("\n");
      }

      CDocFreeFormattedStrings(lines, no_lines);

      delete [] str;

      ++pn1;

      if (pn1 != pn2)
        note = *pn1;
      else
        note = NULL;
    }

    CDTableRow *row = table->row_list[i - 1];

    CDocScriptOutputTableRow(row, table->full_width);
  }

  if (table->footer_row != NULL)
    CDocScriptOutputTableRow(table->footer_row, table->full_width);

  /* Output closing Table Line */

  char *str = new char [table->full_width + 1];

  str[0] = BOX_BL__C;

  int j;

  for (j = 1; j < table->full_width - 1; j++) {
    if (j < int(table_last_line.size()) && table_last_line[j] == BOX_VRT_C)
      str[j] = BOX_BPR_C;
    else
      str[j] = BOX_HOR_C;
  }

  str[j++] = BOX_BR__C;

  str[j] = '\0';

  CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

  CDocScriptWriteText("%s", str);

  CDocScriptTableSetLastLine    (str);
  CDocScriptTableSetLastSplitter(str);

  CDocScriptWriteText("\n");

  row_output = false;

  if (CDocIsPagedOutput()) {
    fputc(KEEP_MARK_C, cdoc_output_fp);
    fputc('\n'       , cdoc_output_fp);
  }

  delete [] str;

  /* End Outputting Text to Temporary File */

  CDocScriptEndTempFile(table->temp_file);

  /* Calculate the Number of Lines Output */

  int no_lines;

  if (table->caption != "" || table->description != "")
    no_lines = table->temp_file->no_lines + 5;
  else
    no_lines = table->temp_file->no_lines + 2;

  /* If not enough room left on page (for paged output) then start new page */

  if (CDocIsPagedOutput())
    CDocScriptFindSpace(no_lines);

  if (cdoc_line_no == 0 && cdoc_char_no == 0)
    header_proc_skip = true;

  /* Turn off Formatting and swap to a Fixed Width Font */

  CDocScriptFormattingOff();

  CDocScriptBeginFont("xmpfont");

  CDocScriptSkipLine();

  /* Output contents of Temporary File */

  CDocScriptSetPagePostHeaderProc
    ((void (*)(const char *)) CDocScriptOutputHeaderRow, (char *) table);

  /* Output Text in Temporary File */

  CDocScriptOutputTempFile(table->temp_file);

  CDocScriptSetPagePostHeaderProc(NULL, NULL);

  delete table->temp_file;

  table->temp_file = NULL;

  /* Turn Formatting back on and swap back to previous Font */

  CDocScriptFormattingOn();

  CDocScriptPreviousFont();

  /* Write Table Caption + Description */

  if (table->caption != "" || table->description != "") {
    CDocScriptSkipLine();

    char *temp_string;

    if      (table->caption != "" && table->description != "") {
      temp_string = new char [table->caption.size() + table->description.size() + 128];

      sprintf(temp_string, "%sTable %d.  %s:  %s%s",
              CDocStartBold(), table->count, table->caption.c_str(), CDocEndBold(),
              table->description.c_str());
    }
    else if (table->caption != "") {
      temp_string = new char [table->caption.size() + 128];

      sprintf(temp_string, "%sTable %d.  %s%s",
              CDocStartBold(), table->count, table->caption.c_str(), CDocEndBold());
    }
    else if (table->description != "") {
      temp_string = new char [table->description.size() + 128];

      sprintf(temp_string, "%sTable %d.  %s%s",
              CDocStartBold(), table->count, CDocEndBold(),
              table->description.c_str());
    }

    CDocScriptWriteCentreJustifiedLine(temp_string, CDOC_NORMAL_FONT);

    delete [] temp_string;

    CDocScriptSkipLine();
  }

  /* Signal that we need space before the next paragraph */

  cdoc_paragraph_done = true;

  /* Clean up */

  for (int i = 0; i <= no_rows + 1; i++) {
    CDTableRow *row;

    if      (i == 0)
      row = table->header_row;
    else if (i == no_rows + 1)
      row = table->footer_row;
    else
      row = table->row_list[i - 1];

    if (row == NULL)
      continue;

    delete [] row->cwidths;

    row->cwidths = NULL;
  }

  CDocScriptTableSetLastLine    ("");
  CDocScriptTableSetLastSplitter("");
}

// Output the header row defined in the supplied Table Structure.
//
// This routine is used to output a new header row every time a new
// page is thrown.
static void
CDocScriptOutputHeaderRow(CDTable *table)
{
  if (header_proc_skip) {
    header_proc_skip = false;

    return;
  }

  CDocScriptSetPagePostHeaderProc(NULL, NULL);

  /*--------------*/

  row_output = false;

  CDocScriptTableSetLastLine    ("");
  CDocScriptTableSetLastSplitter("");

  if (table->header_row != NULL)
    CDocScriptOutputTableRow(table->header_row, table->full_width);

  /*--------------*/

  if (table_last_splitter != "") {
    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    for (uint i = 0; i < table_last_splitter.size(); i++) {
      if      (table_last_splitter[i] == BOX_TL__C)
        table_last_splitter[i] = BOX_LPR_C;
      else if (table_last_splitter[i] == BOX_TPR_C)
        table_last_splitter[i] = BOX_MID_C;
      else if (table_last_splitter[i] == BOX_TR__C)
        table_last_splitter[i] = BOX_RPR_C;
    }

    CDocScriptWriteText("%s", table_last_splitter.c_str());

    CDocScriptWriteText("\n");
  }

  /*--------------*/

  CDocScriptSetPagePostHeaderProc
    ((void (*)(const char *)) CDocScriptOutputHeaderRow, (char *) table);
}

// Output a Row of the Table whose overall width is supplied.
static void
CDocScriptOutputTableRow(CDTableRow *row, int table_width)
{
  typedef std::list<CDTableGrid *> GridList;
  typedef std::vector<int *>       ArrangeList;

  int           i;
  int           j;
  int           k;
  int           c;
  int           len;
  CDTableCell  *cell;
  CDTableGrid  *grid;
  int           align;
  int           depth;
  int           concat;
  int           space1;
  int           space2;
  int          *arrange;
  int          *arrange1;
  int           highlight;
  GridList      grid_list;
  int           no_arrange;
  char        **save_lines;
  int          *grid_depth;
  int           no_arranges;
  int           arrange_row;
  char        **table_lines;
  ArrangeList   arrange_list;
  int           save_no_lines;
  int           max_row_lines;
  int           table_line_no;
  int           arrange_column;
  char          temp_string[256];

  /* If no Arrange List for this Row then create one which has each
     cell belonging to the corresponding column i.e. Cell 1 in
     Column 1 etc. */

  if (row->row_definition->arrange_list.empty()) {
    int *arrange1 = new int [row->no_cwidths];

    for (int i = 0; i < row->no_cwidths; i++)
      arrange1[i] = i + 1;

    arrange_list.push_back(arrange1);
  }

  /* If Row has Arrange List then ensure each Array of Cell Numbers
     is the Row's Number of Columns long. If it is too long it is
     clipped. If it is too short then it is padded with the last
     Cell Number */

  else {
    no_arranges = row->row_definition->arrange_list.size();

    for (i = 1; i <= no_arranges; i++) {
      no_arrange = row->row_definition->no_arrange_list[i - 1];
      arrange    = row->row_definition->arrange_list[i - 1];

      arrange1 = new int [row->no_cwidths];

      if (no_arrange < row->no_cwidths) {
        for (j = 0; j < no_arrange; j++)
          arrange1[j] = arrange[j];

        for ( ; j < row->no_cwidths; j++) {
          if (j == 0)
            arrange1[j] = 1;
          else
            arrange1[j] = arrange1[j - 1];
        }
      }
      else {
        for (j = 0; j < row->no_cwidths; j++)
          arrange1[j] = arrange[j];
      }

      arrange_list.push_back(arrange1);
    }
  }

  /* Create list of Table Grids in which the Cell Text will be formatted */

  no_arranges = arrange_list.size();

  grid_list.clear();

  arrange_row    = 1;
  arrange_column = 1;

  /* Search across the arrange columns and down the arrange rows for
     matching cell numbers to set the grid's number of rows and columns. */

  while (arrange_row    <= no_arranges &&
         arrange_column <= row->no_cwidths) {
    /* Get current Arrange Row */

    arrange = arrange_list[arrange_row - 1];

    /* Initialise the Grid to contain the arrange cell number, start at
       the current row and column and be 1 row deep and 1 column wide */

    grid = new CDTableGrid;

    grid->cell_no  = arrange[arrange_column - 1];
    grid->row      = arrange_row;
    grid->column   = arrange_column;
    grid->rows     = 1;
    grid->columns  = 1;
    grid->lines    = NULL;
    grid->no_lines = 0;

    /* Count Columns with the Grid's Cell Number */

    j = arrange_column;

    while (j < row->no_cwidths && arrange[j] == grid->cell_no) {
      grid->columns++;

      j++;
    }

    /* Count Rows with the Grid's Cell Number in the Grid's start Column */

    j = arrange_row + 1;

    while (j <= no_arranges) {
      arrange = arrange_list[j - 1];

      if (arrange[arrange_column - 1] != grid->cell_no)
        break;

      grid->rows++;

      j++;
    }

    /* Add the Grid to Grid List */

    GridList::iterator pg =
      find_if(grid_list.begin(), grid_list.end(), CDocScriptTableGridSortProc(grid));

    grid_list.insert(pg, grid);

    /* Blank out the Cell Numbers for this Grid */

    for (i = 0; i < grid->rows; i++) {
      arrange = arrange_list[grid->row + i - 1];

      for (j = 0; j < grid->columns; j++)
        arrange[grid->column + j - 1] = -1;
    }

    /* Set the next position in the arrange array from
       which the next Grid will be set up */

    arrange_row    = 1;
    arrange_column = 1;

    while (arrange_row    <= no_arranges &&
           arrange_column <= row->no_cwidths) {
      arrange = arrange_list[arrange_row - 1];

      while (arrange_column <= row->no_cwidths &&
             arrange[arrange_column - 1] == -1)
        arrange_column++;

      if (arrange_column > row->no_cwidths) {
        arrange_row    ++;
        arrange_column = 1;
      }
      else
        break;
    }
  }

  /* Set up Storage in which the Maximum Number of Text Lines in a
     Column is stored */

  grid_depth = new int [no_arranges];

  for (i = 0; i < no_arranges; i++)
    grid_depth[i] = 0;

  /* Format the Grid Cell's Text and update the Maximum Number of Text
     Lines in the Grid's Column */

  GridList::iterator pg1 = grid_list.begin();
  GridList::iterator pg2 = grid_list.end  ();

  for ( ; pg1 != pg2; ++pg1) {
    grid = *pg1;

    /* Set Grid's Width (Sum of All the Column Widths it contains) */

    grid->width = grid->columns - 1;

    for (j = 0; j < grid->columns; j++)
      grid->width += row->cwidths[grid->column + j - 1];

    /* Get Cell Definition for Grid */

    cell = CDocScriptGetTableCell(grid->cell_no, row);

    /* If Grid's Cell is not defined then set the Grid's text lines
       to NULL, update the Number of Text Lines in the Grid's Column
       (may need separator line) and move onto next Grid */

    if (cell == NULL || cell->text == "") {
      grid->lines    = NULL;
      grid->no_lines = 0;

      /* Update the Maximum Depth for a Grid Row */

      if (grid->rows == 1) {
        if (grid->row < no_arranges)
          space1 = 1;
        else
          space1 = 0;

        if (space1 > grid_depth[grid->row - 1])
          grid_depth[grid->row - 1] = space1;
      }

      continue;
    }

    /* Get Align for Cell */

    if      (cell->number <= row->row_definition->no_aligns)
      align = row->row_definition->aligns[cell->number - 1];
    else if (row->row_definition->no_aligns > 0)
      align = row->row_definition->aligns[row->row_definition->no_aligns - 1];
    else
      align = CHALIGN_TYPE_LEFT;

    /* Get Concatenation for Cell */

    if      (cell->number <= row->row_definition->no_concats)
      concat = row->row_definition->concats[cell->number - 1];
    else if (row->row_definition->no_concats > 0)
      concat = row->row_definition->concats[row->row_definition->no_concats - 1];
    else
      concat = true;

    /* If Text lines are Formatted in their concatenated form (which
       they already are in) then Format using the calculated width
       and alignment */

    if (concat)
      CDocFormatStringInWidth(cell->text, grid->width, align, &grid->lines, &grid->no_lines);

    /* If Text lines are Formatted in their unconcatenated form then
       split the line up using the break position array and format
       the individual lines before building the final line list */

    else {
      typedef std::vector<char *> LineList;

      char     **lines;
      int        no_lines;
      LineList   line_list;

      /* Create List to hold the List of formatted broken lines */

      /* If line has no break array then use whole line */

      if (cell->breaks == NULL) {
        CDocFormatStringInWidth(cell->text, grid->width, align, &lines, &no_lines);

        /* Add the lines to the line list */

        for (k = 0; k < no_lines; k++)
          line_list.push_back(CStrUtil::strdup(lines[k]));

        CDocFreeFormattedStrings(lines, no_lines);
      }

      /* If line has break array then break it into a number of
         lines at the specified character positions */

      else {
        int  l1;
        int  l2;

        for (j = 0; j < cell->no_breaks + 1; j++) {
          /* If first segment then extract up to
             first break point */

          if      (j == 0) {
            l2 = cell->breaks[j];

            strncpy(temp_string, cell->text.c_str(), l2);
            temp_string[l2] = '\0';
          }

          /* If last segment then extract from the next
             character after the final break point to the
             end of the text */

          else if (j == cell->no_breaks) {
            l1 = cell->breaks[j - 1] + 1;

            strcpy(temp_string, &cell->text.c_str()[l1]);
          }

          /* All other segments are extract from the next
             character after the break before to the break
             after */

          else {
            l1 = cell->breaks[j - 1] + 1;
            l2 = cell->breaks[j    ];

            strncpy(temp_string, &cell->text.c_str()[l1], l2 - l1);
            temp_string[l2 - l1] = '\0';
          }

          /* Formatted Extracted String so it fits in the
             required width and is aligned correctly */

          CDocFormatStringInWidth(temp_string, grid->width, align, &lines, &no_lines);

          /* Add the lines to the Line List */

          for (k = 0; k < no_lines; k++)
            line_list.push_back(CStrUtil::strdup(lines[k]));

          CDocFreeFormattedStrings(lines, no_lines);
        }
      }

      /* Set the Grid's Lines to the calculated line list */

      grid->no_lines = line_list.size();
      grid->lines    = new char * [grid->no_lines];

      for (j = 0; j < grid->no_lines; j++)
        grid->lines[j] = line_list[j];

      line_list.clear();
    }

    /* Update the Maximum Depth for a Grid Row */

    if (grid->rows == 1) {
      if (grid->row < no_arranges)
        space1 = grid->no_lines + 1;
      else
        space1 = grid->no_lines;

      if (space1 > grid_depth[grid->row - 1])
        grid_depth[grid->row - 1] = space1;
    }
  }

  /* Redo Grid Depth to Ensure Grids Spanning a Number of Rows Fit */

  pg1 = grid_list.begin();
  pg2 = grid_list.end  ();

  for ( ; pg1 != pg2; ++pg1) {
    grid = *pg1;

    if (grid->rows > 1) {
      if (grid->row < no_arranges)
        space1 = grid->no_lines + 1;
      else
        space1 = grid->no_lines;

      space2 = 0;

      for (j = 0; j < grid->rows; j++)
        space2 += grid_depth[grid->row + j - 1];

      space1 -= space2;

      while (space1 > 0) {
        for (j = 0; j < grid->rows; j++) {
          if (space1 > 0)
            grid_depth[grid->row + j - 1]++;

          space1--;
        }
      }
    }
  }

  /* Get the Maximum Number of Column Lines */

  max_row_lines = 0;

  for (i = 0; i < no_arranges; i++)
    max_row_lines += grid_depth[i];

  /* Change each Grid's Line Array to include any extra space
     using the cell's vertical align rule */

  pg1 = grid_list.begin();
  pg2 = grid_list.end  ();

  for ( ; pg1 != pg2; ++pg1) {
    grid = *pg1;

    /* Calculate New Grid Depth */

    depth = 0;

    for (j = 0; j < grid->rows; j++)
      depth += grid_depth[grid->row + j - 1];

    if (grid->row + grid->rows - 1 < no_arranges)
      depth--;

    /* Get Cell Definition for Grid */

    cell = CDocScriptGetTableCell(grid->cell_no, row);

    /* If no Cell text for this grid then Grid's lines are just
       blank lines of the required depth */

    if (cell == NULL || cell->text == "") {
      for (j = 0; j < grid->no_lines; j++)
        delete grid->lines[j];

      delete [] grid->lines;

      grid->lines = NULL;

      grid->no_lines = depth;

      if (grid->no_lines > 0)
        grid->lines = new char * [grid->no_lines];
      else
        grid->lines = NULL;

      for (j = 0; j < grid->no_lines; j++) {
        grid->lines[j] = new char [grid->width + 1];

        for (k = 0; k < grid->width; k++)
          grid->lines[j][k] = ' ';
        grid->lines[j][k] = '\0';
      }
    }

    /* If Cell text available then align the current lines using
       the cell's vertical alignment and add escape codes for their
       highlight type */

    else {
      if      (cell->number <= row->row_definition->no_valigns)
        align = row->row_definition->valigns[cell->number - 1];
      else if (row->row_definition->no_valigns > 0)
        align = row->row_definition->valigns
                 [row->row_definition->no_valigns - 1];
      else
        align = CVALIGN_TYPE_TOP;

      if      (cell->number <= row->row_definition->no_highlights)
        highlight = row->row_definition->highlights
                     [cell->number - 1];
      else if (row->row_definition->no_highlights > 0)
        highlight = row->row_definition->highlights
                     [row->row_definition->no_highlights - 1];
      else
        highlight = 0;

      if      (align == CVALIGN_TYPE_TOP)
        space1 = 0;
      else if (align == CVALIGN_TYPE_CENTRE)
        space1 = (depth - grid->no_lines)/2;
      else if (align == CVALIGN_TYPE_BOTTOM)
        space1 = depth - grid->no_lines;

      space2 = depth - grid->no_lines - space1;

      /* Save original Line Array */

      save_no_lines = grid->no_lines;
      save_lines    = grid->lines;

      /* Create New Line Array */

      grid->no_lines = depth;
      grid->lines    = new char * [grid->no_lines];

      /* Pad with required number of lines above */

      for (j = 0; j < space1; j++) {
        grid->lines[j] = new char [grid->width + 1];

        for (k = 0; k < grid->width; k++)
          grid->lines[j][k] = ' ';
        grid->lines[j][k] = '\0';
      }

      /* Add Original Lines with Highlighting */

      for (j = 0; j < save_no_lines; j++) {
        if (highlight != 1 && highlight != 2 && highlight != 3)
          grid->lines[space1 + j] = save_lines[j];
        else {
          grid->lines[space1 + j] =
            new char [strlen(save_lines[j]) + 9];

          if      (highlight == 1)
            strcpy(grid->lines[space1 + j], CDocStartUnderline());
          else if (highlight == 2)
            strcpy(grid->lines[space1 + j], CDocStartBold());
          else
            strcpy(grid->lines[space1 + j], CDocStartBoldUnderline());

          strcat(grid->lines[space1 + j], save_lines[j]);

          if      (highlight == 1)
            strcat(grid->lines[space1 + j], CDocEndUnderline());
          else if (highlight == 2)
            strcat(grid->lines[space1 + j], CDocEndBold());
          else
            strcat(grid->lines[space1 + j], CDocEndBoldUnderline());

          delete [] save_lines[j];
        }
      }

      /* Pad with required number of lines below */

      for (j = 0; j < space2; j++) {
        grid->lines[space1 + save_no_lines + j] =
          new char [grid->width + 1];

        for (k = 0; k < grid->width; k++)
          grid->lines[space1 + save_no_lines + j][k] = ' ';
        grid->lines[space1 + save_no_lines + j][k] = '\0';
      }

      delete [] save_lines;
    }
  }

  table_lines = new char * [max_row_lines + 1];

  for (i = 0; i < max_row_lines + 1; i++) {
    /* Allocate Each Table Line remembering may have Escape Codes */

    table_lines[i] = new char [table_width + 8*row->no_cwidths + 1];

    table_lines[i][0] = '\0';
  }

  pg1 = grid_list.begin();
  pg2 = grid_list.end  ();

  for ( ; pg1 != pg2; ++pg1) {
    grid = *pg1;

    table_line_no = 1;

    for (j = 0; j < grid->row - 1; j++)
      table_line_no += grid_depth[j];

    len = strlen(table_lines[table_line_no - 1]);

    if (len == 0)
      c = '\0';
    else
      c = table_lines[table_line_no - 1][len - 1];

    if (table_line_no == 1 && ! row_output) {
      if (c == '\0')
        strcat(table_lines[table_line_no - 1], BOX_TL__S);
      else
        strcat(table_lines[table_line_no - 1], BOX_TPR_S);
    }
    else {
      if      (c == '\0')
        strcat(table_lines[table_line_no - 1], BOX_LPR_S);
      else if (c >= BOX_HOR_C && c <= BOX_TL__C)
        strcat(table_lines[table_line_no - 1], BOX_TPR_S);
      else
        strcat(table_lines[table_line_no - 1], BOX_TL__S);
    }

    for (j = 0; j < grid->width; j++)
      strcat(table_lines[table_line_no - 1], BOX_HOR_S);

    for (j = 0; j < grid->no_lines; j++) {
      strcat(table_lines[table_line_no + j], BOX_VRT_S);
      strcat(table_lines[table_line_no + j], grid->lines[j]);
    }
  }

  if (! row_output)
    strcat(table_lines[0], BOX_TR__S);
  else
    strcat(table_lines[0], BOX_RPR_S);

  for (i = 1; i <= max_row_lines; i++) {
    len = strlen(table_lines[i]);

    if (len == 0)
      c = '\0';
    else
      c = table_lines[i][len - 1];

    if (c >= BOX_HOR_C && c <= BOX_TL__C)
      strcat(table_lines[i], BOX_RPR_S);
    else
      strcat(table_lines[i], BOX_VRT_S);
  }

  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC &&
      (row->row_definition->shade == SHADE_LIGHT  ||
       row->row_definition->shade == SHADE_MEDIUM ||
       row->row_definition->shade == SHADE_DARK   )) {
    if      (row->row_definition->shade == SHADE_LIGHT)
      CDocScriptWriteCommand(CDOC_SET_FILL_PIXMAP_TMPL, "25_foreground");
    else if (row->row_definition->shade == SHADE_MEDIUM)
      CDocScriptWriteCommand(CDOC_SET_FILL_PIXMAP_TMPL, "50_foreground");
    else if (row->row_definition->shade == SHADE_DARK)
      CDocScriptWriteCommand(CDOC_SET_FILL_PIXMAP_TMPL, "75_foreground");

    CDocScriptWriteCommand(CDOC_INDENT_TMPL, cdoc_left_margin + cdoc_indent);

    CDocScriptWriteCommand("%s i+5 5 i+%d, %d\n", CDOC_FILL_BOX_CMD,
      strlen(table_lines[0])*10 - 5, (max_row_lines + 1)*10 + 5);
  }

  for (i = 0; i <= max_row_lines; i++) {
    if (i == 0) {
      if (table_last_line != "") {
        for (uint j = 1; j < strlen(table_lines[i]) - 1; j++) {
          if (table_last_line[j] == BOX_VRT_C) {
            if      (table_lines[i][j] == BOX_TL__C)
              table_lines[i][j] = BOX_LPR_C;
            else if (table_lines[i][j] == BOX_TPR_C)
              table_lines[i][j] = BOX_MID_C;
            else if (table_lines[i][j] == BOX_HOR_C)
              table_lines[i][j] = BOX_BPR_C;
          }
        }
      }
    }
    else {
      for (uint j = 1; j < strlen(table_lines[i]) - 1; j++) {
        if (table_lines[i - 1][j] == BOX_VRT_C) {
          if      (table_lines[i][j] == BOX_TL__C)
            table_lines[i][j] = BOX_LPR_C;
          else if (table_lines[i][j] == BOX_TPR_C)
            table_lines[i][j] = BOX_MID_C;
          else if (table_lines[i][j] == BOX_HOR_C)
            table_lines[i][j] = BOX_BPR_C;
        }
      }
    }

    CDocScriptWriteIndent(cdoc_left_margin + cdoc_indent);

    CDocScriptWriteText("%s", table_lines[i]);

    CDocScriptWriteText("\n");

    if (i == 0) {
      if (CDocIsPagedOutput() && cdoc_page_no == -1) {
        fputc(KEEP_MARK_C, cdoc_output_fp);
        fputc('\n'       , cdoc_output_fp);
        fputc(KEEP_MARK_C, cdoc_output_fp);
        fputc('\n'       , cdoc_output_fp);
      }

      CDocScriptTableSetLastSplitter(table_lines[i]);
    }

    if (i == max_row_lines)
      CDocScriptTableSetLastLine(table_lines[i]);

    delete [] table_lines[i];
  }

  row_output = true;

  /* Clean up */

  no_arranges = arrange_list.size();

  for (i = 1; i <= no_arranges; i++) {
    arrange1 = arrange_list[i - 1];

    delete [] arrange1;
  }

  arrange_list.clear();

  delete [] table_lines;

  delete [] grid_depth;

  for_each(grid_list.begin(), grid_list.end(), CDeletePointer());

  grid_list.clear();
}

// Save the last table line so it can be used to determine the type of lines
// to be drawn around the table's grids.
static void
CDocScriptTableSetLastLine(const std::string &str)
{
  table_last_line = "";

  uint len = str.size();

  const char *p = str.c_str();

  uint i = 0;

  int no;

  while (i < len) {
    if      ((no = CDocIsEscapeCode(&p[i])) != 0)
      i += no;
    else if ((no = CDocIsPageReference(&p[i])) != 0) {
      i += no;

      table_last_line += '?';
      table_last_line += '?';
    }
    else
      table_last_line += p[i++];
  }
}

// Save the last table splitter line so it can be used to redraw the table
// header when a page throw is generated while outputting the table.
static void
CDocScriptTableSetLastSplitter(const std::string &str)
{
  table_last_splitter = str;
}

// Create and Store Table Structure ready to store a new Table Definition.
extern CDTable *
CDocScriptCreateTable()
{
  /* Initialise Table Globals */

  if (table_list.empty())
    table_number = 0;

  /* Set the Table Number and set defaults for the
     other values */

  CDTable *table = new CDTable;

  table->row_definition = NULL;
  table->refid          = "";
  table->id             = "";
  table->width          = WIDTH_PAGE;
  table->split          = false;
  table->rotate         = 0;
  table->number         = ++table_number;
  table->count          = -1;
  table->caption        = "";
  table->description    = "";
  table->header_row     = NULL;
  table->footer_row     = NULL;
  table->page_no        = 0;

  /* Add the Table to the Master Table List */

  table_list.push_back(table);

  return table;
}

// Set a Table's Row Definition Structure to that matching the
// supplied Identifier.
extern void
CDocScriptSetTableRowDef(CDTable *table)
{
  if (table->refid != "") {
    table->row_definition = CDocScriptGetTableRowDef(table->refid);

    if (table->row_definition == NULL)
      CDocScriptError("Table Row Definition %s Not Found", table->refid.c_str());
  }
}

// Get the Nth Table.
extern CDTable *
CDocScriptGetTable(int no)
{
  CDTable *table = NULL;

  if (no >= 1 && no <= (int) table_list.size())
    table = (CDTable *) table_list[no - 1];

  return table;
}

// Delete all currently defined Table Structures.
//
// Used to free of all Table resources when processing of the input
// IBM Script File has finished.
extern void
CDocScriptDeleteTables()
{
  for_each(table_list.begin(), table_list.end(), CDeletePointer());

  table_list.clear();
}

// Create a Table Row Structure and add it to the supplied Table Structure.
extern CDTableRow *
CDocScriptCreateTableRow(CDTable *table)
{
  CDTableRow *row = new CDTableRow;

  row->table          = table;
  row->refid          = "";
  row->split          = false;
  row->row_definition = NULL;
  row->cell_number    = 0;
  row->no_cwidths     = 0;
  row->cwidths        = NULL;

  table->row_list.push_back(row);

  return row;
}

// Create a Table Row Structure for the Table Footer and add it to the
// supplied Table Structure.
extern CDTableRow *
CDocScriptCreateTableFooterRow(CDTable *table)
{
  CDTableRow *row = new CDTableRow;

  row->table          = table;
  row->refid          = "";
  row->split          = false;
  row->row_definition = NULL;
  row->cell_number    = 0;
  row->no_cwidths     = 0;
  row->cwidths        = NULL;

  table->footer_row = row;

  return row;
}

// Create a Table Row Structure for the Table Header and add it to
// the supplied Table Structure.
extern CDTableRow *
CDocScriptCreateTableHeaderRow(CDTable *table)
{
  CDTableRow *row = new CDTableRow;

  row->table          = table;
  row->refid          = "";
  row->split          = false;
  row->row_definition = NULL;
  row->cell_number    = 0;
  row->no_cwidths     = 0;
  row->cwidths        = NULL;

  table->header_row = row;

  return row;
}

// Set a Table Row's Row Definition Structure to that matching the
// supplied Identifier.
extern void
CDocScriptSetTableRowRowDef(CDTableRow *row)
{
  CDTableRow *last_row;

  if (row->refid != "") {
    row->row_definition = CDocScriptGetTableRowDef(row->refid);

    if (row->row_definition == NULL)
      CDocScriptError("Invalid Row Definition Name %s", row->refid.c_str());
  }
  else {
    if (row->table->row_list.size() > 1) {
      last_row = row->table->row_list[row->table->row_list.size() - 2];

      if (last_row != NULL)
        row->row_definition = last_row->row_definition;
      else
        row->row_definition = row->table->row_definition;
    }
    else
      row->row_definition = row->table->row_definition;
  }
}

// Add a Table Node to a Table Structure.
extern void
CDocScriptAddTableNote(CDTable *table, const std::string &str)
{
  CDTableNote *table_note = new CDTableNote;

  table_note->text   = str;
  table_note->row_no = table->row_list.size();

  table->note_list.push_back(table_note);
}

// Create a Table Cell Structure and add it to the supplied Table Row.
extern CDTableCell *
CDocScriptCreateTableCell(CDTableRow *row)
{
  CDTableCell *cell = new CDTableCell;

  cell->row       = row;
  cell->number    = 0;
  cell->text      = "";
  cell->no_breaks = 0;
  cell->breaks    = NULL;

  row->cell_list.push_back(cell);

  return cell;
}

// Get the Nth Cell of a Table Row.
static CDTableCell *
CDocScriptGetTableCell(int cell_no, CDTableRow *row)
{
  int no_cells = row->cell_list.size();

  for (int i = 1; i <= no_cells; i++) {
    CDTableCell *cell = row->cell_list[i - 1];

    if (cell->number == cell_no)
      return cell;
  }

  return NULL;
}

// Create a Table Row Definition Structure ready to store a new Table
// Row Definition.
extern CDTableRowDef *
CDocScriptCreateTableRowDef()
{
  /* Allocate Row Definition Structure */

  CDTableRowDef *row_definition = new CDTableRowDef;

  row_definition->name            = "";
  row_definition->no_highlights   = 0;
  row_definition->highlights      = NULL;
  row_definition->no_aligns       = 0;
  row_definition->aligns          = NULL;
  row_definition->no_concats      = 0;
  row_definition->concats         = NULL;
  row_definition->no_valigns      = 0;
  row_definition->valigns         = NULL;
  row_definition->no_rotates      = 0;
  row_definition->rotates         = NULL;
  row_definition->no_mindepths    = 0;
  row_definition->mindepths       = NULL;
  row_definition->no_cwidths      = 0;
  row_definition->cwidths         = NULL;
  row_definition->shade           = SHADE_XLIGHT;
  row_definition->width           = 0;
  row_definition->free_cells1     = 0;
  row_definition->free_cells2     = 0;

  return row_definition;
}

// Store a fully defined Table Row Definition Structure in the List of Table
// Row Definition Structures.
//
// This is separate from the creation as the contents of the definition are
// checked before the structure is added allowing it to be rejected as invalid.
extern void
CDocScriptAddTableRowDef(CDTableRowDef *row_definition)
{
  /* Delete any Existing Row Definition of the Same Name */

  CDTableRowDef *row_definition1 = CDocScriptGetTableRowDef(row_definition->name);

  if (row_definition1 != NULL) {
    CDocScriptWarning("Row Definition %s Multiply Defined", row_definition->name.c_str());

    CDTableRowDefList::iterator p =
      find(row_definition_list.begin(), row_definition_list.end(), row_definition1);

    if (p != row_definition_list.end())
      row_definition_list.erase(p);

    delete row_definition1;
  }

  /* Add Row Definition to Master List */

  row_definition_list.push_back(row_definition);
}

// Get the Row Definition Structure associated with a Row Definition Name.
extern CDTableRowDef *
CDocScriptGetTableRowDef(const std::string &name)
{
  /* Search List of Row Definition Structures for one with
     a Name matching the supplied one */

  int no_row_definitions = row_definition_list.size();

  for (int i = 1; i <= no_row_definitions; i++) {
    CDTableRowDef *row_definition = row_definition_list[i - 1];

    /* Found Match so Return Row Definition Structure */

    if (row_definition->name == name)
      return row_definition;
  }

  return NULL;
}

// Delete all currently defined Table Row Definition Structures.
//
// Used to free of all Table Row Definition resources when processing of
// the input IBM Script File has finished.
extern void
CDocScriptDeleteTableRowDefs()
{
  for_each(row_definition_list.begin(), row_definition_list.end(), CDeletePointer());

  row_definition_list.clear();
}

//---------------------

CDTable::
~CDTable()
{
  delete header_row;
  delete footer_row;

  for_each(row_list.begin(), row_list.end(), CDeletePointer());

  row_list.clear();

  for_each(note_list.begin(), note_list.end(), CDeletePointer());

  note_list.clear();
}

CDTableRow::
~CDTableRow()
{
  for_each(cell_list.begin(), cell_list.end(), CDeletePointer());

  cell_list.size();

  delete [] cwidths;
}

CDTableCell::
~CDTableCell()
{
  delete [] breaks;
}

CDTableRowDef::
~CDTableRowDef()
{
  delete [] highlights;

  delete [] aligns;

  delete [] concats;

  delete [] valigns;

  delete [] rotates;

  delete [] mindepths;

  int no_arrange = arrange_list.size();

  for (int i = 1; i <= no_arrange; i++) {
    int *arrange = arrange_list[i - 1];

    delete [] arrange;
  }

  arrange_list.clear();
  no_arrange_list.clear();

  delete [] cwidths;
}
