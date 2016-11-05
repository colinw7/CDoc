#include "CDocI.h"

/*----------------------------------------------------------------*/

struct CDTocEntry {
  std::string text;
  int         line_no;
  int         page_no;
};

/*----------------------------------------------------------------*/

typedef std::vector<CDTocEntry *> CDTocList;

static CDTocList table_of_contents_list;

// Add an Entry to the Table of Contents.
extern void
CDocScriptAddTOCEntry(const std::string &str)
{
  CDTocEntry *toc_entry = new CDTocEntry;

  toc_entry->text    = str;
  toc_entry->line_no = cdoc_input_line_no;
  toc_entry->page_no = 0;

  table_of_contents_list.push_back(toc_entry);
}

// Add a Blank Line to the Table of Contents.
extern void
CDocScriptAddTOCSkip()
{
  CDTocEntry *toc_entry = new CDTocEntry;

  toc_entry->text    = "";
  toc_entry->line_no = cdoc_input_line_no;
  toc_entry->page_no = 0;

  table_of_contents_list.push_back(toc_entry);
}

// Set all Table of Contents entries which match the current line number
// (there should only be one) to the current page number.
extern void
CDocScriptSetTOCPage()
{
  uint no_toc_entries = table_of_contents_list.size();

  for (uint i = 0; i < no_toc_entries; ++i) {
    CDTocEntry *toc_entry = table_of_contents_list[i];

    if (toc_entry->line_no == cdoc_input_line_no) {
      if (cdoc_page_no != -1)
        toc_entry->page_no = cdoc_page_no;
      else
        toc_entry->page_no = cdoc_page_no_offset;
    }
  }
}

// Get the Page Number String for the Nth Table of Contents entry.
extern std::string
CDocScriptGetTOCPage(int no)
{
  std::string page_string;

  page_string = "??";

  if (table_of_contents_list.empty())
    return page_string;

  CDTocEntry *toc_entry = table_of_contents_list[no - 1];

  if (toc_entry != NULL)
    page_string = CStrUtil::toString(toc_entry->page_no);

  return page_string;
}

// Output the Table of Contents.
extern void
CDocScriptOutputTableOfContents()
{
  /* Output Table of Contents Page */

  CDocScriptWriteCentreJustifiedPageHeader("Table of Contents");

  CDocScriptSkipLine();

  if (table_of_contents_list.empty())
    return;

  int j;

  uint no_toc_entries = table_of_contents_list.size();

  if      (cdoc_page_numbering && CDocIsPagedOutput()) {
    for (uint i = 0; i < no_toc_entries; ++i) {
      CDTocEntry *toc_entry = table_of_contents_list[i];

      /* Output Content Line */

      CDocScriptWriteIndent(8);

      if (toc_entry->text != "") {
        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
          CDocScriptWriteText("%s##cdoc_toc_pg%d##B", ESCG, i);

        CDocScriptWriteText("%s", toc_entry->text.c_str());

        j = CDocStringDisplayLength(toc_entry->text) + 8;

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC) {
          int  k;

          if (j + 6 <= cdoc_right_margin)
            k = cdoc_right_margin;
          else
            k = j + 6;

          CDocScriptWriteText(FILL_TMPL, j    , j + 1, ' ');
          CDocScriptWriteText(FILL_TMPL, j + 2, k - 4, '.');
          CDocScriptWriteText(FILL_TMPL, k - 3, k - 3, ' ');
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

        CDocScriptWriteText("##cdoc_toc_pg%d##", i);

        if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
          CDocScriptWriteText("%sE", ESCG);

        CDocScriptWriteText("\n");
      }
      else
        CDocScriptSkipLine();
    }
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML) {
    CDocScriptWriteCommand("<pre>\n");
    CDocScriptWriteCommand("\n");

    for (uint i = 0; i < no_toc_entries; ++i) {
      CDTocEntry *toc_entry = table_of_contents_list[i];

      /* Output Content Line */

      if (toc_entry->text != "")
        CDocScriptWriteCommand("        %s\n",
          CDocEncodeHtmlString(toc_entry->text.c_str()));
      else
        CDocScriptWriteCommand("\n");
    }

    CDocScriptWriteCommand("\n");
    CDocScriptWriteCommand("</pre>\n");
  }
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF) {
    CDocScriptWriteCommand("\n");

    for (uint i = 0; i < no_toc_entries; ++i) {
      CDTocEntry *toc_entry = table_of_contents_list[i];

      /* Output Content Line */

      if (toc_entry->text != "")
        CDocScriptWriteCommand("        %s\n",
          CDocEncodeILeafString(toc_entry->text.c_str()));
      else
        CDocScriptWriteCommand("\n");
    }

    CDocScriptWriteCommand("\n");
  }
  else {
    for (uint i = 0; i < no_toc_entries; ++i) {
      CDTocEntry *toc_entry = table_of_contents_list[i];

      /* Output Content Line */

      CDocScriptWriteIndent(8);

      if (toc_entry->text != "")
        CDocScriptWriteText("%s\n", toc_entry->text.c_str());
      else
        CDocScriptSkipLine();
    }
  }
}

// Delete resources used by the Table of Contents.
extern void
CDocScriptDeleteTOC()
{
  if (table_of_contents_list.empty())
    return;

  uint no_toc_entries = table_of_contents_list.size();

  for (uint i = 0; i < no_toc_entries; ++i) {
    CDTocEntry *toc_entry = table_of_contents_list[i];

    delete toc_entry;
  }

  table_of_contents_list.clear();
}
