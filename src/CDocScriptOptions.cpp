#include "CDocI.h"

#define OUT1(a)    fprintf(fp, a)
#define OUT2(a, b) fprintf(fp, a, b)

/*--------------------------------------------------------------------*/

#define MAX_LEFT_MARGIN      252
#define MAX_RIGHT_MARGIN     255
#define MIN_LINES_PER_PAGE   8
#define MAX_PARAGRAPH_INDENT 16

#define CDOC_DEFAULT_LEFT_MARGIN      5
#define CDOC_DEFAULT_RIGHT_MARGIN     67
#define CDOC_DEFAULT_LINES_PER_PAGE   60
#define CDOC_DEFAULT_NUMBER_HEADERS   true
#define CDOC_DEFAULT_PARAGRAPH_INDENT 0
#define CDOC_DEFAULT_TITLE_PAGE       true
#define CDOC_DEFAULT_TITLE_PAGE_ALIGN CHALIGN_TYPE_RIGHT
#define CDOC_DEFAULT_INDEX            true
#define CDOC_DEFAULT_PAGE_NUMBERING   0
#define CDOC_DEFAULT_PROCESS_NAME     ""
#define CDOC_DEFAULT_REFERENCE_FILE   ""
#define CDOC_DEFAULT_ON_WARNING       CDOC_ON_WARNING_CONTINUE
#define CDOC_DEFAULT_ON_ERROR         CDOC_ON_ERROR_CONTINUE
#define CDOC_DEFAULT_SPELL_CHECK      false

/*--------------------------------------------------------------------*/

int         cdoc_left_margin      = CDOC_DEFAULT_LEFT_MARGIN;
int         cdoc_right_margin     = CDOC_DEFAULT_RIGHT_MARGIN;
int         cdoc_lines_per_page   = CDOC_DEFAULT_LINES_PER_PAGE;
int         cdoc_paragraph_indent = CDOC_DEFAULT_PARAGRAPH_INDENT;
int         cdoc_number_headers   = CDOC_DEFAULT_NUMBER_HEADERS;
int         cdoc_title_page       = CDOC_DEFAULT_TITLE_PAGE;
int         cdoc_title_page_align = CDOC_DEFAULT_TITLE_PAGE_ALIGN;
int         cdoc_index            = CDOC_DEFAULT_INDEX;
int         cdoc_page_numbering   = CDOC_DEFAULT_PAGE_NUMBERING;
std::string cdoc_process_name     = CDOC_DEFAULT_PROCESS_NAME;
std::string cdoc_reference_file   = CDOC_DEFAULT_REFERENCE_FILE;
int         cdoc_on_warning       = CDOC_DEFAULT_ON_WARNING;
int         cdoc_on_error         = CDOC_DEFAULT_ON_ERROR;
int         cdoc_spell_check      = CDOC_DEFAULT_SPELL_CHECK;
int         cdoc_spell_active     = CDOC_DEFAULT_SPELL_CHECK;

int         cdoc_header_number[CDOC_MAX_HEADERS];

/*----------*/

int         cdoc_save_left_margin      = CDOC_DEFAULT_LEFT_MARGIN;
int         cdoc_save_right_margin     = CDOC_DEFAULT_RIGHT_MARGIN;
int         cdoc_save_lines_per_page   = CDOC_DEFAULT_LINES_PER_PAGE;
int         cdoc_save_paragraph_indent = CDOC_DEFAULT_PARAGRAPH_INDENT;
int         cdoc_save_number_headers   = CDOC_DEFAULT_NUMBER_HEADERS;
int         cdoc_save_title_page       = CDOC_DEFAULT_TITLE_PAGE;
int         cdoc_save_title_page_align = CDOC_DEFAULT_TITLE_PAGE_ALIGN;
int         cdoc_save_index            = CDOC_DEFAULT_INDEX;
int         cdoc_save_page_numbering   = CDOC_DEFAULT_PAGE_NUMBERING;
std::string cdoc_save_process_name     = CDOC_DEFAULT_PROCESS_NAME;
std::string cdoc_save_reference_file   = CDOC_DEFAULT_REFERENCE_FILE;
int         cdoc_save_on_warning       = CDOC_DEFAULT_ON_WARNING;
int         cdoc_save_on_error         = CDOC_DEFAULT_ON_ERROR;
int         cdoc_save_spell_check      = CDOC_DEFAULT_SPELL_CHECK;
int         cdoc_save_spell_active     = CDOC_DEFAULT_SPELL_CHECK;

int         cdoc_save_header_number[CDOC_MAX_HEADERS];

/*--------------------------------------------------------------------*/

#define NUMBER_HEADERS_NO  0
#define NUMBER_HEADERS_YES 1

#define TITLE_PAGE_NO             0
#define TITLE_PAGE_JUSTIFY_LEFT   1
#define TITLE_PAGE_JUSTIFY_RIGHT  2
#define TITLE_PAGE_JUSTIFY_CENTRE 3

struct CDScriptOptionsData {
  int    left_margin;
  int    right_margin;
  int    lines_per_page;
  int    paragraph_indent;
  char  *number_headers;
  int    title_page;
  int    index;
  int    page_numbering;
  char  *process_name;
  char  *reference_file;
  int    on_warning;
  int    on_error;
  int    spell_check;
};

struct CDScriptOptions {
  int         left_margin;
  int         right_margin;
  int         lines_per_page;
  int         paragraph_indent;
  std::string number_headers;
  int         title_page;
  int         index;
  int         page_numbering;
  std::string process_name;
  std::string reference_file;
  int         on_warning;
  int         on_error;
  int         spell_check;
};

/*--------------------------------------------------------------------*/

#define OPT_OFF(a) CDocOffset(CDScriptOptionsData*,a)

static CDParameterChoiceData
number_headers_data[] = {
  {"yes", (char *) NUMBER_HEADERS_YES,},
  {"no" , (char *) NUMBER_HEADERS_NO ,},
  {""   , (char *) PARM_NEW_STR      ,},
  {NULL , (char *) 0                 ,},
};

static CDParameterChoiceData
title_page_data[] = {
  {"left"  , (char *) TITLE_PAGE_JUSTIFY_LEFT  ,},
  {"right" , (char *) TITLE_PAGE_JUSTIFY_RIGHT ,},
  {"centre", (char *) TITLE_PAGE_JUSTIFY_CENTRE,},
  {"center", (char *) TITLE_PAGE_JUSTIFY_CENTRE,},
  {"yes"   , (char *) TITLE_PAGE_JUSTIFY_RIGHT ,},
  {"no"    , (char *) TITLE_PAGE_NO            ,},
  {NULL    , (char *) 0                        ,},
};

static CDParameterChoiceData
on_warning_data[] = {
  {"continue", (char *) CDOC_ON_WARNING_CONTINUE,},
  {"ignore"  , (char *) CDOC_ON_WARNING_IGNORE  ,},
  {"prompt"  , (char *) CDOC_ON_WARNING_PROMPT  ,},
  {"exit"    , (char *) CDOC_ON_WARNING_EXIT    ,},
  {NULL      , (char *) 0                       ,},
};

static CDParameterChoiceData
on_error_data[] = {
  {"continue", (char *) CDOC_ON_ERROR_CONTINUE,},
  {"ignore"  , (char *) CDOC_ON_ERROR_IGNORE  ,},
  {"prompt"  , (char *) CDOC_ON_ERROR_PROMPT  ,},
  {"exit"    , (char *) CDOC_ON_ERROR_EXIT    ,},
  {NULL      , (char *) 0                     ,},
};

static CDOptionData
script_option_data[] = {
  {
    "left_margin",
    "l",
    PARM_CLENSTR,
    NULL,
    OPT_OFF(left_margin),
  },
  {
    "right_margin",
    "r",
    PARM_CLENSTR,
    NULL,
    OPT_OFF(right_margin),
  },
  {
    "page_length",
    "pagelen",
    PARM_LLENSTR,
    NULL,
    OPT_OFF(lines_per_page),
  },
  {
    "paragraph_indent",
    "pi",
    PARM_CLENSTR,
    NULL,
    OPT_OFF(paragraph_indent),
  },
  {
    "number_headers",
    "numhead",
    PARM_CHOICE,
    (char *) number_headers_data,
    OPT_OFF(number_headers),
  },
  {
    "title_page",
    "titlep",
    PARM_CHOICE,
    (char *) title_page_data,
    OPT_OFF(title_page),
  },
  {
    "index",
    "indx",
    PARM_BOOLEAN,
    (char *) NULL,
    OPT_OFF(index),
  },
  {
    "page_numbers",
    "pg",
    PARM_BOOLEAN,
    (char *) NULL,
    OPT_OFF(page_numbering),
  },
  {
    "process",
    "proc",
    PARM_STR,
    NULL,
    OPT_OFF(process_name),
  },
  {
    "reference_file",
    "ref_file",
    PARM_STR,
    NULL,
    OPT_OFF(reference_file),
  },
  {
    "on_warning",
    "onwarn",
    PARM_CHOICE,
    (char *) on_warning_data,
    OPT_OFF(on_warning),
  },
  {
    "on_error",
    "onerr",
    PARM_CHOICE,
    (char *) on_error_data,
    OPT_OFF(on_error),
  },
  {
    "spell_check",
    "spell",
    PARM_BOOLEAN,
    (char *) NULL,
    OPT_OFF(spell_check),
  },
  {
    NULL,
    NULL,
    0,
    (char *) NULL,
    0,
  },
};

// Saves the current script options so they can be restored to their
// original values.
extern void
CDocScriptSaveOptions()
{
  cdoc_save_left_margin      = cdoc_left_margin;
  cdoc_save_right_margin     = cdoc_right_margin;
  cdoc_save_lines_per_page   = cdoc_lines_per_page;
  cdoc_save_paragraph_indent = cdoc_paragraph_indent;
  cdoc_save_number_headers   = cdoc_number_headers;
  cdoc_save_title_page       = cdoc_title_page;
  cdoc_save_title_page_align = cdoc_title_page_align;
  cdoc_save_index            = cdoc_index;
  cdoc_save_page_numbering   = cdoc_page_numbering;
  cdoc_save_process_name     = cdoc_process_name;
  cdoc_save_reference_file   = cdoc_reference_file;
  cdoc_save_on_warning       = cdoc_on_warning;
  cdoc_save_on_error         = cdoc_on_error;
  cdoc_save_spell_check      = cdoc_spell_check;
  cdoc_save_spell_active     = cdoc_spell_active;

  for (int i = 0; i < CDOC_MAX_HEADERS; i++)
    cdoc_save_header_number[i] = cdoc_header_number[i];
}

// Restore the current script options to the values they had when
// CDocScriptSaveOptions() was called.
extern void
CDocScriptRestoreOptions()
{
  cdoc_left_margin      = cdoc_save_left_margin;
  cdoc_right_margin     = cdoc_save_right_margin;
  cdoc_lines_per_page   = cdoc_save_lines_per_page;
  cdoc_paragraph_indent = cdoc_save_paragraph_indent;
  cdoc_number_headers   = cdoc_save_number_headers;
  cdoc_title_page       = cdoc_save_title_page;
  cdoc_title_page_align = cdoc_save_title_page_align;
  cdoc_index            = cdoc_save_index;
  cdoc_page_numbering   = cdoc_save_page_numbering;
  cdoc_process_name     = cdoc_save_process_name;
  cdoc_reference_file   = cdoc_save_reference_file;
  cdoc_on_warning       = cdoc_save_on_warning;
  cdoc_on_error         = cdoc_save_on_error;
  cdoc_spell_check      = cdoc_save_spell_check;
  cdoc_spell_active     = cdoc_save_spell_active;

  for (int i = 0; i < CDOC_MAX_HEADERS; i++)
    cdoc_header_number[i] = cdoc_save_header_number[i];
}

// Process a List of CDoc Option Strings in order to set CDoc Options.
//
// This routine is meant to be called with command line options from a program
// which uses the CDoc subroutines internally.
//
// Options which are recognised as CDoc options are removed from the supplied
// option list. Other options are left as is.
//
// This routine is also used to parse an embedded CDoc option str in IBM
// Script source.
extern void
CDocScriptProcessOptions(const char **options, int *no_options)
{
  CDScriptOptionsData script_options;

  script_options.left_margin      = cdoc_left_margin;
  script_options.right_margin     = cdoc_right_margin;
  script_options.lines_per_page   = cdoc_lines_per_page;
  script_options.paragraph_indent = cdoc_paragraph_indent;
  script_options.index            = cdoc_index;
  script_options.page_numbering   = cdoc_page_numbering;
  script_options.process_name     = NULL;
  script_options.reference_file   = NULL;
  script_options.on_warning       = cdoc_on_warning;
  script_options.on_error         = cdoc_on_error;
  script_options.spell_check      = cdoc_spell_check;

  if (cdoc_number_headers)
    script_options.number_headers = (char *) NUMBER_HEADERS_YES;
  else
    script_options.number_headers = (char *) NUMBER_HEADERS_NO;

  if (cdoc_title_page) {
    if      (cdoc_title_page_align == CHALIGN_TYPE_LEFT)
      script_options.title_page = TITLE_PAGE_JUSTIFY_LEFT;
    else if (cdoc_title_page_align == CHALIGN_TYPE_RIGHT)
      script_options.title_page = TITLE_PAGE_JUSTIFY_RIGHT;
    else if (cdoc_title_page_align == CHALIGN_TYPE_CENTRE)
      script_options.title_page = TITLE_PAGE_JUSTIFY_CENTRE;
  }
  else
    script_options.title_page = TITLE_PAGE_NO;

  CDocExtractOptions((char **) options,
                     no_options,
                     script_option_data,
                     (char *) &script_options);

  if (script_options.left_margin < script_options.right_margin - 2) {
    if (script_options.left_margin >= 0 &&
        script_options.left_margin <= MAX_LEFT_MARGIN)
      cdoc_left_margin = script_options.left_margin;
    else
      CDocScriptWarning("Invalid Left Margin %d",
                        script_options.left_margin);

    if (script_options.right_margin >= 0 &&
        script_options.right_margin <= MAX_RIGHT_MARGIN)
      cdoc_right_margin = script_options.right_margin;
    else
      CDocScriptWarning("Invalid Right Margin %d",
                        script_options.right_margin);
  }
  else
    CDocScriptWarning("Invalid Left/Right Margin Combination %d -> %d",
                      script_options.left_margin,
                      script_options.right_margin);

  if (script_options.lines_per_page >= MIN_LINES_PER_PAGE)
    cdoc_lines_per_page = script_options.lines_per_page;
  else
    CDocScriptWarning("Too few Lines for Page %d",
                      script_options.lines_per_page);

  if (script_options.paragraph_indent >= 0 &&
      script_options.paragraph_indent <= MAX_PARAGRAPH_INDENT)
    cdoc_paragraph_indent = script_options.paragraph_indent;
  else
    CDocScriptWarning("Invalid Paragraph Indent %d",
                      script_options.paragraph_indent);

  if      (script_options.number_headers == (char *) NUMBER_HEADERS_NO)
    cdoc_number_headers = false;
  else if (script_options.number_headers == (char *) NUMBER_HEADERS_YES)
    cdoc_number_headers = true;
  else if (script_options.number_headers != NULL) {
    int temp_header_number[CDOC_MAX_HEADERS];

    int i = 0;

    int number_headers_valid = true;

    char *save_number_headers = CStrUtil::strdup(script_options.number_headers);

    char *p2 = script_options.number_headers;

    while (*p2 != '\0') {
      char *p1 = p2;

      while (*p2 != '\0' && isdigit(*p2))
        p2++;

      if (*p2 != '\0' && *p2 != '.') {
        CDocScriptError("Invalid Initial Header Level String %s",
                        save_number_headers);

        number_headers_valid = false;

        break;
      }

      if (*p2 == '.') {
        *p2 = '\0';

        p2++;
      }

      if (*p1 != '\0')
        temp_header_number[i] = CStrUtil::toInteger(p1);
      else
        temp_header_number[i] = 1;

      i++;
    }

    delete save_number_headers;

    if (number_headers_valid) {
      for (i = 0; i < CDOC_MAX_HEADERS; i++)
        cdoc_header_number[i] = temp_header_number[i];
    }
  }

  cdoc_index = script_options.index;

  cdoc_page_numbering = script_options.page_numbering;

  if      (script_options.title_page == TITLE_PAGE_NO) {
    cdoc_title_page       = false;
    cdoc_title_page_align = CHALIGN_TYPE_RIGHT;
  }
  else if (script_options.title_page == TITLE_PAGE_JUSTIFY_LEFT) {
    cdoc_title_page       = true;
    cdoc_title_page_align = CHALIGN_TYPE_LEFT;
  }
  else if (script_options.title_page == TITLE_PAGE_JUSTIFY_RIGHT) {
    cdoc_title_page       = true;
    cdoc_title_page_align = CHALIGN_TYPE_RIGHT;
  }
  else if (script_options.title_page == TITLE_PAGE_JUSTIFY_CENTRE) {
    cdoc_title_page       = true;
    cdoc_title_page_align = CHALIGN_TYPE_CENTRE;
  }

  if (script_options.process_name)
    cdoc_process_name = script_options.process_name;

  if (script_options.reference_file)
    cdoc_reference_file = script_options.reference_file;

  cdoc_on_warning  = script_options.on_warning;
  cdoc_on_error    = script_options.on_error;
  cdoc_spell_check = script_options.spell_check;
}

// Set the left and right margins of the text which will be output.
extern int
CDocScriptSetMargins(int left_margin, int right_margin)
{
  if (left_margin == -1)
    left_margin = cdoc_left_margin;

  if (right_margin == -1)
    right_margin = cdoc_right_margin;

  if (left_margin >= right_margin - 2)
    return false;

  if (left_margin  > 0           && left_margin  < MAX_LEFT_MARGIN &&
      right_margin > left_margin && right_margin < MAX_RIGHT_MARGIN) {
    cdoc_left_margin  = left_margin;
    cdoc_right_margin = right_margin;

    return true;
  }
  else
    return false;
}

// Set the Number of Lines on a page of the output file.
//
//
// The routine only affects conversion from IBM Script to CDoc, Raw with
// Control Codes or Raw output formats.
extern int
CDocScriptSetLinesPerPage(int lines_per_page)
{
  if (lines_per_page >= MIN_LINES_PER_PAGE) {
    cdoc_lines_per_page = lines_per_page;

    return true;
  }
  else
    return false;
}

// Specify whether headers are numbered when converting IBM Script.
//
// If the 'flag' variable is false (0) then the 'init_values' and
// 'no_init_values' are ignored.
//
// If no initial header values are required then pass 'init_values' as NULL
// and 'no_init_values' as 0.
extern void
CDocScriptSetHeaderNumbering(int flag, int *init_values, int no_init_values)
{
  if (flag) {
    cdoc_number_headers = true;

    if (no_init_values > CDOC_MAX_HEADERS)
      no_init_values = CDOC_MAX_HEADERS;

    for (int i = 0; i < no_init_values; i++) {
      if (init_values[i] > 0)
        cdoc_header_number[i] = init_values[i];
      else
        cdoc_header_number[i] = 1;
    }
  }
  else
    cdoc_number_headers = false;
}

// Specify the number of characters which the first line of a paragraph is
// indented.
//
// The IBM Script processor uses 0 (no indentation) but most documents will
// look better if indentation is used.
extern int
CDocScriptSetParagraphIndent(int indent)
{
  if (indent >= 0 && indent <= MAX_PARAGRAPH_INDENT) {
    cdoc_paragraph_indent = indent;

    return true;
  }
  else
    return false;
}

// Specify whether a Title Page is to be produced and, if it is, whether the
// text is aligned to the left, centre or right of the page.
//
// The default is to produce a right aligned title page.
extern void
CDocScriptSetTitlePage(int flag, int align)
{

  if (flag) {
    if (align == CHALIGN_TYPE_LEFT   ||
        align == CHALIGN_TYPE_CENTRE ||
        align == CHALIGN_TYPE_RIGHT)
      cdoc_title_page_align = align;

    cdoc_title_page = true;
  }
  else
    cdoc_title_page = false;

  return;

}

// Specify whether an Index is to be compiled or not.
//
// This must be set to true for the IBM Script tag ':index' to add an
// index to the document.
//
// The default is not to compile an index.
extern void
CDocScriptSetIndex(int flag)
{
  if (flag)
    cdoc_index = true;
  else
    cdoc_index = false;
}

// Specify whether the Page Numbers will be added to the Table of Contents,
// List of Illustrations, List of Figures, Index and Cross References.
//
// The default is for Page Numbers not be to used.
extern void
CDocScriptSetPageNumbering(int flag)
{
  if (flag)
    cdoc_page_numbering = true;
  else
    cdoc_page_numbering = false;
}

// Set the name under which a IBM Script File will be processed. The name is
// used to decide whether text enclosed in a ':psc' ... ':epsc' block
// should be processed or not.
//
// The process name is normally not set and the IBM Script File is processed
// using a name of 'CDoc' followed by the output format e.g. CDOCHTML for
// HTML output.
extern void
CDocScriptSetProcessName(const std::string &process_name)
{
  cdoc_process_name = process_name;
}

// Specify the dataset which contains information for references which
// are not resolved in the IBM Script input file.
extern void
CDocScriptSetReferenceFile(const std::string &filename)
{
  cdoc_reference_file = filename;
}

// Specify what action CDoc should take when a Warning Message is to be issued.
//
// This routine is useful if the user wishes to use an IBM Script containing
// unsupported commands (as this file will also be processed on the IBM) and
// doesn't wish to see these warning when using this file as a help dataset.
//
// Any unrecognised values for 'action' will be ignored.
extern void
CDocScriptSetOnWarning(int action)
{
  if (action == CDOC_ON_WARNING_CONTINUE ||
      action == CDOC_ON_WARNING_IGNORE   ||
      action == CDOC_ON_WARNING_PROMPT   ||
      action == CDOC_ON_WARNING_EXIT)
    cdoc_on_warning = action;
}

// Specify what action CDoc should take when an Error Message is to be issued.
//
// This routine is useful if the user wishes to use an IBM Script containing
// unsupported commands (as this file will also be processed on the IBM) and
// doesn't wish to see these warning when using this file as a help dataset.
//
// Any unrecognised values for 'action' will be ignored.
extern void
CDocScriptSetOnError(int action)
{
  if (action == CDOC_ON_ERROR_CONTINUE ||
      action == CDOC_ON_ERROR_IGNORE   ||
      action == CDOC_ON_ERROR_PROMPT   ||
      action == CDOC_ON_ERROR_EXIT)
    cdoc_on_error = action;
}

// Specify whether Spell Checking should be performed on the words in
// the document.
extern void
CDocScriptSetSpellCheck(int flag)
{
  if (flag)
    cdoc_spell_check = true;
  else
    cdoc_spell_check = false;
}

// List the Script Processing Options (parsed by CDocScriptProcessOptions())
// to the specified File.
//
// This routine is meant to be used in a program that uses the
// CDocScriptProcessOptions() routine and wants to output a usage message.
extern void
CDocScriptListOptions(FILE *fp)
{
  if (fp == NULL)
    fp = stderr;

  OUT1("  Extra Options for Script Processing :-\n");
  OUT1("\n");
  OUT1("    -left_margin [-l] <chars>\n");
  OUT2("       : Set Left Margin (Default %d).\n", CDOC_DEFAULT_LEFT_MARGIN);
  OUT1("    -right_margin [-r] <chars>\n");
  OUT2("       : Set Right Margin (Default %d).\n", CDOC_DEFAULT_RIGHT_MARGIN);
  OUT1("    -page_length [-pagelen] <lines>\n");
  OUT2("       : Set Line Length (Default %d).\n", CDOC_DEFAULT_LINES_PER_PAGE);
  OUT1("    -paragraph_indent [-pi] <chars>\n");
  OUT2("       : Set Paragraph Indentation (Default %d).\n", CDOC_DEFAULT_PARAGRAPH_INDENT);
  OUT1("    -number_headers [-numhead] yes|no|<init>\n");
  OUT1("       : Set Header Numbering or Initialize Header's\n");
  OUT1("       : staring values (Default Yes).\n");
  OUT1("    -title_page [-titlep] left|right|centre|yes|no\n");
  OUT1("       : Specify whether title page should be output\n");
  OUT1("       : and if so whether the text should be left,\n");
  OUT1("       : right or centre justified.\n");
  OUT1("    -index [-indx] yes|no\n");
  OUT1("       : Specify whether an Index should be produced\n");
  OUT1("       : or not.\n");
  OUT1("    -page_numbers [-pg] yes|no\n");
  OUT1("       : Specify whether an Page Numbers should be\n");
  OUT1("       : used in Cross References or not.\n");
  OUT1("    -process [-proc] <str>\n");
  OUT1("       : Set current process name to select process\n");
  OUT1("       : specific options.\n");
  OUT1("    -reference_file [-ref_file] <filename>\n");
  OUT1("       : Set the reference filename to use to resolve\n");
  OUT1("       : undefined references.\n");
  OUT1("    -on_warning [-onwarn] continue|ignore|prompt|exit\n");
  OUT1("       : Specify how CDoc should react when a Warning\n");
  OUT1("       : is to be Output.\n");
  OUT1("       :\n");
  OUT1("       :   continue - Display Message and Continue\n");
  OUT1("       :   ignore   - Don't display Message and Continue\n");
  OUT1("       :   prompt   - Display Message and Prompt for Continue\n");
  OUT1("       :   exit     - Quit CDoc\n");
  OUT1("    -on_error [-onerr] continue|ignore|prompt|exit\n");
  OUT1("       : Specify how CDoc should react when an Error\n");
  OUT1("       : is to be Output.\n");
  OUT1("       :\n");
  OUT1("       :   continue - Display Message and Continue\n");
  OUT1("       :   ignore   - Don't display Message and Continue\n");
  OUT1("       :   prompt   - Display Message and Prompt for Continue\n");
  OUT1("       :   exit     - Quit CDoc\n");
  OUT1("    -spell_check [-spell] yes|no\n");
  OUT1("       : Specify whether spell checking of the words in\n");
  OUT1("       : the document should be performed.\n");
}
