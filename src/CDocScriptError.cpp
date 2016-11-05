#include "CDocI.h"

// Output a warning message after it has been formatted using 'printf' style
// format codes.
//
// The output of this routine can be switched off using the CDocSetScriptWarnings()
// routine.
extern void
CDocScriptWarning(const char *format, ...)
{
  if (cdoc_on_warning == CDOC_ON_WARNING_EXIT)
    exit(1);

  if (cdoc_on_warning == CDOC_ON_WARNING_CONTINUE ||
      cdoc_on_warning == CDOC_ON_WARNING_PROMPT) {
    va_list args;
    char    line[256];

    /*CONSTANTCONDITION*/
    va_start(args, format);

    vsprintf(line, format, args);

    va_end(args);

    if (cdoc_pass_no > 0)
      fprintf(stderr, "CDoc Warning : %s (Line %d, Pass %d)\n",
              line, cdoc_input_line_no, cdoc_pass_no);
    else
      fprintf(stderr, "CDoc Warning : %s\n", line);
  }

  if (cdoc_on_warning == CDOC_ON_WARNING_PROMPT) {
    int   i;
    char  line[256];

    printf("Continue (Y/n) : ");

    if (fgets(line, 256, stdin) != NULL)
      {
        i = 0;

        CStrUtil::skipSpace(line, &i);

        if (line[i] != '\0' && line[i] != 'y' && line[i] != 'Y')
          exit(1);
      }
    else
      exit(1);
  }
}

// Output an error message after it has been formatted using 'printf' style format codes.
extern void
CDocScriptError(const char *format, ...)
{
  if (cdoc_on_error == CDOC_ON_ERROR_EXIT)
    exit(1);

  if (cdoc_on_error == CDOC_ON_ERROR_CONTINUE ||
      cdoc_on_error == CDOC_ON_ERROR_PROMPT) {
    va_list  args;
    char     line[256];

    /*CONSTANTCONDITION*/
    va_start(args, format);

    vsprintf(line, format, args);

    va_end(args);

    if (cdoc_pass_no > 0)
      fprintf(stderr, "CDoc Error : %s (Line %d, Pass %d)\n",
              line, cdoc_input_line_no, cdoc_pass_no);
    else
      fprintf(stderr, "CDoc Error : %s\n", line);
  }

  if (cdoc_on_warning == CDOC_ON_ERROR_PROMPT) {
    int  i;
    char line[256];

    printf("Continue (Y/n) : ");

    if (fgets(line, 256, stdin) != NULL) {
      i = 0;

      CStrUtil::skipSpace(line, &i);

      if (line[i] != '\0' && line[i] != 'y' && line[i] != 'Y')
        exit(1);
    }
    else
      exit(1);
  }
}
