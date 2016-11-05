#include "CDocI.h"

// Return whether the current output format produces paged output.
extern int
CDocIsPagedOutput()
{
  if (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
      CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
      CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return true;
  else
    return false;
}

// Get Command to Start Normal Text.
extern const char *
CDocStartNormal()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC0m);
  else
    return("");
}

// Get Command to End Normal Text.
extern const char *
CDocEndNormal()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC0m);
  else
    return("");
}

// Get Command to Start Bold Text.
extern const char *
CDocStartBold()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC1m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033<b\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_BOLD_FONT);
  else
    return("");
}

// Get Command to End Bold Text.
extern const char *
CDocEndBold()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC0m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033</b\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_NORMAL_FONT);
  else
    return("");
}

// Get Command to Start Underlined Text.
extern const char *
CDocStartUnderline()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC4m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033<u\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_UNDERLINE_FONT);
  else
    return("");
}

// Get Command to End Underlined Text.
extern const char *
CDocEndUnderline()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC0m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033</u\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_NORMAL_FONT);
  else
    return("");
}

// Get Command to Start Bold Underlined Text.
extern const char *
CDocStartBoldUnderline()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC3m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033<b\033>\033<u\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_BOLD_UNDERLINE_FONT);
  else
    return("");
}

// Get Command to End Bold Underlined Text.
extern const char *
CDocEndBoldUnderline()
{
  if      (CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC   ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC ||
           CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
    return(ESC0m);
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
    return("\033</u\033>\033</b\033>");
  else if (CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
    return(ESC_ILEAF_NORMAL_FONT);
  else
    return("");
}
