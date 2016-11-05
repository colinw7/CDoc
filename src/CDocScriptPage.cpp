#include <CDocI.h>
#include <unistd.h>

/*------------------------------------------------------------------------*/

static std::string cdoc_page_filename;
static FILE*       cdoc_page_save_fp  = NULL;

// Initialise Temporary File to which all output is directed when page
// numbering is on.
//
// This file is reprocessed to set any remaining unresolved page numbers and to
// output to the required output by CDocScriptPageNoTerm() when processing has
// finished.
extern void
CDocScriptPageNoInit()
{
  if (cdoc_page_filename != "")
    return;

  if (CDocIsPagedOutput()) {
    cdoc_page_filename = CDocInst->getTempFileName();
    cdoc_page_save_fp  = cdoc_output_fp;
    cdoc_output_fp     = fopen(cdoc_page_filename.c_str(), "w");

    if (cdoc_output_fp == NULL) {
      CDocScriptError("Failed to Open Temporary Page Number File '%s'",
                      cdoc_page_filename.c_str());

      cdoc_output_fp     = cdoc_page_save_fp;
      cdoc_page_save_fp  = NULL;
      cdoc_page_filename = "";

      cdoc_page_numbering = false;
    }
  }
}

// Reprocess the Temporary Page Number file to set remaining unresolved page
// numbers and to output to the required output file.
extern void
CDocScriptPageNoTerm()
{
  std::string temp_line;

  if (cdoc_page_filename == "")
    return;

  if (CDocIsPagedOutput()) {
    fclose(cdoc_output_fp);

    cdoc_output_fp = fopen(cdoc_page_filename.c_str(), "r");

    if (cdoc_output_fp != NULL) {
      char line[CDOC_MAX_LINE];

      while (fgets(line, CDOC_MAX_LINE, cdoc_output_fp) != NULL) {
        char *p;

        while ((p = strstr(line, "##cdoc_ref_pg")) != NULL ||
               (p = strstr(line, "##cdoc_toc_pg")) != NULL ||
               (p = strstr(line, "##cdoc_fig_pg")) != NULL ||
               (p = strstr(line, "##cdoc_tbl_pg")) != NULL ||
               (p = strstr(line, "##cdoc_ih1_pg")) != NULL ||
               (p = strstr(line, "##cdoc_ih2_pg")) != NULL ||
               (p = strstr(line, "##cdoc_ih3_pg")) != NULL) {
          char *p1 = &p[13];

          while (*p1 != '\0' && *p1 != '#')
            p1++;

          if (p1[0] == '#' && p1[1] == '#') {
            *p = '\0';

            temp_line = line;

            *p1 = '\0';

            if      (strncmp(&p[7], "ref", 3) == 0)
              temp_line += CDocScriptGetReferencePage(CStrUtil::toInteger(&p[13]));
            else if (strncmp(&p[7], "toc", 3) == 0)
              temp_line += CDocScriptGetTOCPage(CStrUtil::toInteger(&p[13]));
            else if (strncmp(&p[7], "fig", 3) == 0)
              temp_line += CDocScriptGetFigurePage(CStrUtil::toInteger(&p[13]));
            else if (strncmp(&p[7], "tbl", 3) == 0)
              temp_line += CDocScriptGetTablePage(CStrUtil::toInteger(&p[13]));
            else if (strncmp(&p[7], "ih1", 3) == 0)
              temp_line += CDocScriptGetIndex1Page(CStrUtil::toInteger(&p[13]));
            else if (strncmp(&p[7], "ih2", 3) == 0) {
              char *p2;

              if ((p2 = strchr(&p[13], '.')) != NULL) {
                *p2 = '\0';

                temp_line += CDocScriptGetIndex2Page(CStrUtil::toInteger(&p[13]),
                                                     CStrUtil::toInteger(&p2[1]));
              }
              else
                temp_line += "??";
            }
            else if (strncmp(&p[7], "ih3", 3) == 0) {
              char *p2;
              char *p3;

              if ((p2 = strchr(&p[13], '.')) != NULL && (p3 = strchr(p2 + 1, '.')) != NULL) {
                *p2 = '\0';
                *p3 = '\0';

                temp_line += CDocScriptGetIndex3Page(CStrUtil::toInteger(&p[13]),
                                                     CStrUtil::toInteger(p2 + 1),
                                                     CStrUtil::toInteger(p3 + 1));
              }
              else
                temp_line += "??";
            }

            temp_line += &p1[2];

            strcpy(line, temp_line.c_str());
          }
          else
            break;
        }

        fputs(line, cdoc_page_save_fp);
      }

      fclose(cdoc_output_fp);

      unlink(cdoc_page_filename.c_str());
    }
    else
      CDocScriptError("Failed to Reopen Temporary Page File '%s'",
                      cdoc_page_filename.c_str());

    cdoc_page_filename = "";

    cdoc_output_fp = cdoc_page_save_fp;
  }
}
