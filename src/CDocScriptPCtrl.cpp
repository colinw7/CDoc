#include "CDocI.h"

/*------------------------------------------------------------------------*/

struct CDProcessCntrl {
  char **procs;
  int    no_procs;
};

/*------------------------------------------------------------------------*/

/* Process Specific Control Parameter Definition */

#define PSC_OFFSET(a) CDocOffset(CDProcessCntrl*,a)

static CDParameterData
psc_parameter_data[] =
{
  {"proc", PARM_STR_ARRAY, NULL, PSC_OFFSET(procs),},
  {NULL  , 0             , NULL, 0                ,},
};

/*------------------------------------------------------------------------*/

static bool           processing = true;
static CDProcessCntrl process_ctrl;

// Initialise the Process Control Structure ready for IBM Script processing.
extern void
CDocScriptInitPCtrl()
{
  processing = true;

  process_ctrl.procs    = NULL;
  process_ctrl.no_procs = 0;
}

// Start a Process Control Block checking whether the Process Name is valid and
// switching processing on/off accordingly.
extern void
CDocScriptStartPCtrl(CDColonCommand *colon_command)
{
  if (process_ctrl.procs == NULL) {
    CDocExtractParameterValues(colon_command->getParameters(),
                               colon_command->getValues(),
                               psc_parameter_data,
                               (char *) &process_ctrl);

    if (process_ctrl.procs != NULL) {
      processing = false;

      for (int i = 0; i < process_ctrl.no_procs; i++) {
        CStrUtil::toLower(process_ctrl.procs[i]);

        if (cdoc_process_name != "") {
          if (strcmp(process_ctrl.procs[i], cdoc_process_name.c_str()) == 0)
            processing = true;
        }
        else {
          if      (strcmp(process_ctrl.procs[i], "cdoccdoc") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_CDOC)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdoctroff") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_TROFF)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdocrawcc") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW_CC)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdocraw") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_RAW)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdochtml") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_HTML)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdocileaf") == 0 &&
                   CDocInst->getOutputFormat() == CDOC_OUTPUT_ILEAF)
            processing = true;
          else if (strcmp(process_ctrl.procs[i], "cdoc") == 0)
            processing = true;
        }
      }
    }
    else
      processing = true;
  }
  else
    CDocScriptError("Process Control not allowed inside another Process Control");
}

// End a Process Control Block end restart normal processing.
extern void
CDocScriptEndPCtrl()
{
  if (process_ctrl.procs != NULL) {
    for (int i = 0; i < process_ctrl.no_procs; i++)
      delete [] process_ctrl.procs[i];

    delete [] process_ctrl.procs;
  }

  process_ctrl.procs    = NULL;
  process_ctrl.no_procs = 0;

  processing = true;
}

// Return whether processing is currently on or off.
//
// This routine allows the processing flag to be internal to this file.
extern int
CDocScriptProcessing()
{
  return processing;
}
